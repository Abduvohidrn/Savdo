#include "BozorNpc.h"

#include "AIController.h"
#include "BozorOvoz.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimSequence.h"
#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/ChildActorComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DetourCrowdAIController.h"
#include "Engine/StaticMesh.h"
#include "EngineUtils.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Misc/App.h"
#include "Misc/CommandLine.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"
#include "TimerManager.h"
#include "TwoBoneIK.h"
#include "UnrealClient.h"
#if WITH_EDITOR
#include "ShaderCompiler.h"
#endif

DEFINE_LOG_CATEGORY_STATIC(LogBozorNpc, Log, All);

namespace
{
	const FName SlotName(TEXT("DefaultSlot"));
	const FName MoveState(TEXT("Move"));

	FVector Flat(FVector V)
	{
		V.Z = 0.0;
		return V;
	}

	// Unity Mathf.Clamp: chegaralar teskari bo'lsa ham yiqilmaydi
	float SafeClamp(float X, float Lo, float Hi) { return FMath::Clamp(X, Lo, FMath::Max(Lo, Hi)); }
}

// ============================================================ korutin

bool FBozorCo::StepHandle(FHandle Co, float Dt)
{
	auto& P = Co.promise();
	// ichki korutin tugasa, ota o'sha kadrda davom etadi (Unity'dagi kabi); cheklov - cheksiz aylanishdan himoya
	for (int32 Guard = 0; Guard < 256; ++Guard)
	{
		if (Co.done()) return true;
		if (P.Child)
		{
			if (!StepHandle(P.Child, Dt)) return false;
			P.Child = nullptr;
		}
		else if (P.bWaiting)
		{
			// shu kadrda boshlangan kutish keyingi kadrdan sanaladi
			if (P.WaitFrame == GFrameCounter) return false;
			P.WaitFrame = GFrameCounter;
			P.Wait -= Dt;
			if (P.Wait > 0.f) return false;
			P.bWaiting = false;
		}
		Co.resume();
	}
	return false;
}

// ============================================================ menejer

ABozorNpcManager::ABozorNpcManager()
{
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
}

ABozorNpcManager* ABozorNpcManager::Get(const UObject* WorldContext)
{
	UWorld* W = WorldContext ? WorldContext->GetWorld() : nullptr;
	if (!W) return nullptr;
	for (TActorIterator<ABozorNpcManager> It(W); It; ++It) return *It;
	return nullptr;
}

void ABozorNpcManager::BeginPlay()
{
	Super::BeginPlay();
	StartTime = GetWorld()->GetTimeSeconds();
	for (int32 i = 0; i < Actions.Num(); ++i)
	{
		if (Actions[i].Anim) ByCat.FindOrAdd(Actions[i].Category).Add(i);
	}
	QueueCustomers.SetNum(Queues.Num());
	Popularity.SetNum(Queues.Num());
	// mashhur rastaga ko'proq odam keladi va navbat hosil bo'ladi
	for (float& P : Popularity) P = FMath::FRand() < 0.15f ? FMath::FRandRange(2.f, 3.f) : FMath::FRandRange(0.4f, 1.2f);
	SeatOccupant.SetNum(Seats.Num());
	bAutoTest = FParse::Param(FCommandLine::Get(), TEXT("BozorAutoTest"));
	NextShot = StartTime + 12.0;
	EnsureNavMesh();
	UE_LOG(LogBozorNpc, Display, TEXT("[BozorNpc] harakat=%d navbat=%d o'rindiq=%d nuqta=%d+%d"), Actions.Num(), Queues.Num(), Seats.Num(), WanderPoints.Num(), GatePoints.Num());
}

void ABozorNpcManager::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (bAutoTest) AutoTest(GetWorld()->GetTimeSeconds());
}

// Xaritada NavMesh bo'sh saqlangan bo'lsa (Dynamic rejimda o'yin uni o'zi qurmaydi), hech kim yurolmaydi: shu yerda quriladi
void ABozorNpcManager::EnsureNavMesh()
{
	UNavigationSystemV1* Nav = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (!Nav) return;
	for (const FVector& P : WanderPoints)
	{
		FNavLocation L;
		if (Nav->ProjectPointToNavigation(P, L, FVector(100.f, 100.f, 300.f))) return;
	}
	UE_LOG(LogBozorNpc, Warning, TEXT("[BozorNpc] NavMesh bo'sh: o'yin boshida quriladi"));
	Nav->Build();
}

const FBozorNpcAction* ABozorNpcManager::Pick(FName Category) const
{
	const TArray<int32>* L = ByCat.Find(Category);
	return L && L->Num() > 0 ? &Actions[(*L)[FMath::RandRange(0, L->Num() - 1)]] : nullptr;
}

FVector ABozorNpcManager::RandomWanderPoint() const
{
	return WanderPoints.Num() > 0 ? WanderPoints[FMath::RandRange(0, WanderPoints.Num() - 1)] : GetActorLocation();
}

FVector ABozorNpcManager::RandomGatePoint() const
{
	return GatePoints.Num() > 0 ? GatePoints[FMath::RandRange(0, GatePoints.Num() - 1)] : RandomWanderPoint();
}

// UE GetBestQueueForCustomer: yaqin va qisqa navbat, biroz tasodif bilan. Except - hozirgina ketgan navbat:
// o'sha rastaga (ikkinchi sotuvchisiga ham) qaytib bormaydi
int32 ABozorNpcManager::BestQueue(const FVector& From, int32 Except) const
{
	int32 Best = INDEX_NONE;
	float BestScore = MAX_flt;
	for (int32 q = 0; q < Queues.Num(); ++q)
	{
		const FBozorQueueSpot& Q = Queues[q];
		if (q == Except || !Q.Vendor || Q.Points.Num() == 0 || QueueCustomers[q].Num() >= Q.Points.Num()) continue;
		if (Except != INDEX_NONE && (Q.Vendor == Queues[Except].Vendor || (Q.Stall >= 0 && Q.Stall == Queues[Except].Stall))) continue;
		// masofa metrda (Unity bilan bir xil vazn)
		const float Score = FVector::Dist(From, Q.Points[0]) * 0.01f * FMath::FRandRange(0.4f, 1.6f) / Popularity[q] + QueueCustomers[q].Num() * 2.5f + FMath::FRandRange(0.f, 4.f);
		if (Score < BestScore)
		{
			BestScore = Score;
			Best = q;
		}
	}
	return Best;
}

int32 ABozorNpcManager::Join(int32 Q, ABozorNpc* C)
{
	TArray<TWeakObjectPtr<ABozorNpc>>& L = QueueCustomers[Q];
	L.RemoveAll([](const TWeakObjectPtr<ABozorNpc>& P) { return !P.IsValid(); });
	const int32 I = L.IndexOfByKey(C);
	if (I != INDEX_NONE) return I;
	if (L.Num() >= Queues[Q].Points.Num()) return -1;
	return L.Add(C);
}

// chiqib ketgan mijozdan keyingilar bir qadam oldinga siljiydi (UE: RefreshQueue)
void ABozorNpcManager::Leave(int32 Q, ABozorNpc* C)
{
	if (QueueCustomers.IsValidIndex(Q)) QueueCustomers[Q].Remove(C);
}

int32 ABozorNpcManager::IndexOf(int32 Q, const ABozorNpc* C) const
{
	return QueueCustomers.IsValidIndex(Q) ? QueueCustomers[Q].IndexOfByKey(C) : INDEX_NONE;
}

FVector ABozorNpcManager::Point(int32 Q, int32 I) const
{
	const TArray<FVector>& P = Queues[Q].Points;
	return P.Num() > 0 ? P[FMath::Clamp(I, 0, P.Num() - 1)] : GetActorLocation();
}

FVector ABozorNpcManager::LookAtLocation(int32 Q) const
{
	const FBozorQueueSpot& S = Queues[Q];
	return S.Vendor ? S.Vendor->GetActorLocation() : Point(Q, 0) + S.Forward * 100.f;
}

// hamrohlar navbat nuqtalarida turib qolmasin
bool ABozorNpcManager::NearAnyPoint(const FVector& P, float Radius) const
{
	for (const FBozorQueueSpot& Q : Queues)
	{
		for (const FVector& X : Q.Points)
		{
			if (FVector::DistSquared2D(X, P) < Radius * Radius) return true;
		}
	}
	return false;
}

int32 ABozorNpcManager::ReserveSeat(ABozorNpc* Npc, const FVector& From, float MaxDistance)
{
	int32 Best = INDEX_NONE;
	float Bd = MaxDistance;
	for (int32 i = 0; i < Seats.Num(); ++i)
	{
		if (SeatOccupant[i].IsValid()) continue;
		const float D = FVector::Dist2D(From, Seats[i].Position);
		if (D < Bd)
		{
			Bd = D;
			Best = i;
		}
	}
	if (Best != INDEX_NONE)
	{
		SeatOccupant[Best] = Npc;
		SitCount++;
	}
	return Best;
}

void ABozorNpcManager::ReleaseSeat(int32 Seat, ABozorNpc* Npc)
{
	if (SeatOccupant.IsValidIndex(Seat) && SeatOccupant[Seat].Get() == Npc) SeatOccupant[Seat] = nullptr;
}

// ============================================================ NPC

ABozorNpc::ABozorNpc(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UBozorNpcMesh>(ACharacter::MeshComponentName))
{
	PrimaryActorTick.bCanEverTick = true;
	AIControllerClass = ADetourCrowdAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	// burilishni harakat (yurganda) va TurnTo (turganda) boshqaradi, kontroller emas
	bUseControllerRotationYaw = false;
	GetCapsuleComponent()->InitCapsuleSize(34.f, 88.f);
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -88.f), FRotator(0.f, -90.f, 0.f));
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	UCharacterMovementComponent* M = GetCharacterMovement();
	M->bOrientRotationToMovement = true;
	M->RotationRate = FRotator(0.f, 360.f, 0.f);
	M->MaxWalkSpeed = WalkSpeed;
	// Unity agent.acceleration = 3 m/s2
	M->MaxAcceleration = 300.f;
	M->BrakingDecelerationWalking = 300.f;
	// AnimBP yurishni tezlanishdan biladi (ShouldMove)
	M->GetNavMovementProperties()->bUseAccelerationForPaths = true;
}

void ABozorNpc::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	LinkMetaHuman();   // muharrirda ham (o'yin boshlanmaganda) bitta gavda ko'rinsin
}

void ABozorNpc::LinkMetaHuman()
{
	TArray<USkeletalMeshComponent*> Parts;
	GetComponents(Parts);

	// Citizen garments and heads share the Citizen bind pose. Only the hidden
	// animation source uses the MetaHuman skeleton.
	const auto* CitizenPose = Parts.FindByPredicate([](const USkeletalMeshComponent* Part)
	{
		return Part->ComponentHasTag(TEXT("CitizenPose"));
	});
	if (CitizenPose)
	{
		// Unreal flattens leader chains. The retargeted mesh must evaluate its
		// own animation before the modular parts can follow it.
		(*CitizenPose)->SetLeaderPoseComponent(nullptr);
		for (USkeletalMeshComponent* Part : Parts)
		{
			if (Part->GetAttachParent() == *CitizenPose)
			{
				Part->SetLeaderPoseComponent(*CitizenPose);
			}
		}
		GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
		GetMesh()->SetVisibility(false, false);
		return;
	}

	// MetaHuman: kiyim tana (Mesh) pozasiga ergashadi; yuzning o'z ABP'si (ABP_Face) bor - u pozani o'zi nusxalaydi,
	// soch/qosh groom'lari ham shunga bog'langan (leader pose qilinsa ko'rinmaydi)
	for (USkeletalMeshComponent* P : Parts)
	{
		if (P != GetMesh() && !P->GetAnimClass()) P->SetLeaderPoseComponent(GetMesh());
	}
	// MetaHuman BP'si child actor sifatida (yuz, soch, LODSync o'zida): uning tanasi NPC tanasi pozasiga ergashadi,
	// NPC'ning o'z tanasi esa yashiriladi (animatsiya unda ishlayveradi)
	TArray<UChildActorComponent*> Kids;
	GetComponents(Kids);
	for (UChildActorComponent* K : Kids)
	{
		AActor* MH = K ? K->GetChildActor() : nullptr;
		if (!MH) continue;
		TArray<USkeletalMeshComponent*> MHParts;
		MH->GetComponents(MHParts);
		for (USkeletalMeshComponent* P : MHParts)
		{
			if (!P->GetAnimClass()) P->SetLeaderPoseComponent(GetMesh());
		}
		GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
		GetMesh()->SetVisibility(false, false);
	}
}

void ABozorNpc::BeginPlay()
{
	Super::BeginPlay();
	LinkMetaHuman();
	HomeRotation = GetActorQuat();
	// guruh eng sekin a'zosiga moslashadi: aks holda hamroh yetib ololmay qolardi
	if (NpcRole == EBozorRole::Hamroh && Leader) Leader->WalkSpeed = FMath::Min(Leader->WalkSpeed, MaxWalk * 0.85f);
	WalkSpeed = FMath::Clamp(WalkSpeed, MinWalk, MaxWalk);
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	if (NpcRole == EBozorRole::Sotuvchi || NpcRole == EBozorRole::Suhbatdosh)
	{
		// joyidan qimirlamaydi: boshqalar uni surib ketmasin
		GetCharacterMovement()->bOrientRotationToMovement = false;
	}
	// yuk yoki arava ustida paydo bo'lgan odam pastdagi yo'lakka tushiriladi
	const float Half = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	FVector Ground;
	if (NpcRole != EBozorRole::Sotuvchi && ProjectToNav(GetActorLocation() - FVector(0.f, 0.f, Half), Ground, 150.f)
		&& FMath::Abs(GetActorLocation().Z - Half - Ground.Z) > 20.f)
	{
		SetActorLocation(Ground + FVector(0.f, 0.f, Half + 2.f), false, nullptr, ETeleportType::TeleportPhysics);
	}
	if (NpcRole == EBozorRole::Hammol && bCart) AttachCart();
}

void ABozorNpc::EndPlay(const EEndPlayReason::Type Reason)
{
	Main.Reset();
	Super::EndPlay(Reason);
}

void ABozorNpc::StartRole()
{
	bStarted = true;
	// tasodifiy faza: yaqin NPC'lar bir xil harakatni bir vaqtda qilmasin (yurish - AnimBP, harakatlar - Do)
	Current = MoveState;
	if (NpcRole != EBozorRole::Sotuvchi && NpcRole != EBozorRole::Suhbatdosh) Mgr->Movers.Add(this);
	switch (NpcRole)
	{
	case EBozorRole::Sotuvchi: Main = VendorLoop(); break;
	case EBozorRole::Xaridor: Main = CustomerLoop(); break;
	case EBozorRole::Hamroh: Main = FollowerLoop(); break;
	case EBozorRole::Suhbatdosh: Main = TalkerLoop(); break;
	default: Main = WalkerLoop(); break;
	}
}

void ABozorNpc::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	CallCooldown -= DeltaSeconds;
	const double Now = GetWorld()->GetTimeSeconds();
	if (SettleAt > 0.0 && Now >= SettleAt) Settle();
	if (!bStarted)
	{
		Mgr = ABozorNpcManager::Get(this);
		if (Mgr && Mgr->HasActorBegunPlay() && (AI() || NpcRole == EBozorRole::Sotuvchi || NpcRole == EBozorRole::Suhbatdosh)) StartRole();
	}
	else if (Main.IsRunning())
	{
		Main.Step(DeltaSeconds);
	}
	if (Cart)
	{
		UpdateCart(DeltaSeconds);
	}
	else if (bTurning)
	{
		const FQuat Q = FMath::QInterpConstantTo(GetActorQuat(), TurnGoal, DeltaSeconds, FMath::DegreesToRadians(220.f));
		SetActorRotation(Q);
		if (Q.AngularDistance(TurnGoal) < FMath::DegreesToRadians(1.f))
		{
			SetActorRotation(TurnGoal);
			bTurning = false;
		}
	}
}

AAIController* ABozorNpc::AI() const
{
	return Cast<AAIController>(GetController());
}

bool ABozorNpc::ProjectToNav(const FVector& P, FVector& Out, float Extent) const
{
	UNavigationSystemV1* Nav = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	FNavLocation L;
	if (!Nav || !Nav->ProjectPointToNavigation(P, L, FVector(Extent, Extent, 200.f))) return false;
	Out = L.Location;
	return true;
}

// ================= animatsiya

// Faqat "Move" (tik turish/yurish - AnimBP'ning o'zi): o'ynayotgan harakat to'xtaydi. Allaqachon shu holatda bo'lsa hech narsa qilmaydi
void ABozorNpc::Play(FName State, float Fade, float Offset)
{
	if (State == Current) return;
	Current = State;
	SettleAt = 0.0;
	if (UAnimInstance* A = GetMesh()->GetAnimInstance()) A->StopSlotAnimation(Fade, SlotName);
}

// Kategoriyadan tasodifiy harakat; qaytaradi: davomiylik (sikl bo'lsa - [min, max] oralig'ida tasodifiy).
// Loop bo'lmagan klip tugagach montaj o'zi AnimBP'ga (tik turishga) qaytadi; o'tirganda - o'tirish sikliga
float ABozorNpc::Do(FName Category, float Min, float Max)
{
	const FBozorNpcAction* A = Mgr ? Mgr->Pick(Category) : nullptr;
	UAnimInstance* Inst = GetMesh()->GetAnimInstance();
	if (!A || !Inst)
	{
		PlayMove();
		return Min;
	}
	Current = NAME_None;
	const float Len = A->Anim->GetPlayLength();
	// o'tirish va turish tana surilishi bilan bir vaqtda boshlanadi va tugaydi - boshidan o'ynaydi
	const bool bChained = Category == TEXT("S_Down") || Category == TEXT("S_Up");
	// loop bo'lmagan klip ham biroz tasodifiy joydan: bir vaqtda boshlagan ikki odam bir xil qimirlamasin
	const float Offset = A->bLoop ? FMath::FRandRange(0.f, Len * 0.7f) : bChained ? 0.f : FMath::FRandRange(0.f, FMath::Min(0.3f, Len * 0.1f));
	Inst->PlaySlotAnimationAsDynamicMontage(A->Anim, SlotName, 0.3f, 0.25f, 1.f, A->bLoop ? 1000 : 1, -1.f, Offset);
	SettleAt = 0.0;
	if (!A->bLoop && !bChained && Category.ToString().StartsWith(TEXT("S_")))
	{
		SettleAt = GetWorld()->GetTimeSeconds() + FMath::Max(Len - Offset - 0.25f, 0.1f);
		SettleCategory = TEXT("S_Loop");
	}
	return A->bLoop ? FMath::FRandRange(Min, Max) : FMath::Clamp(Len, Min, Max);
}

// delay soniyadan keyin Do (bir vaqtda boshlagan odamlar bir xil klipni bir xil fazada o'ynamasin)
void ABozorNpc::DoLater(FName Category, float Duration, float Delay)
{
	FTimerHandle H;
	GetWorldTimerManager().SetTimer(H, FTimerDelegate::CreateWeakLambda(this, [this, Category, Duration, Delay]()
	{
		Do(Category, Duration - Delay, Duration - Delay);
	}), FMath::Max(Delay, 0.01f), false);
}

void ABozorNpc::Settle()
{
	SettleAt = 0.0;
	if (!SettleCategory.IsNone()) Do(SettleCategory);
	else PlayMove();
}

void ABozorNpc::StopMoving()
{
	if (AAIController* C = AI()) C->StopMovement();
}

bool ABozorNpc::MoveRequest(const FVector& Target, float Speed)
{
	AAIController* C = AI();
	if (!C || bSitting) return false;
	GetCharacterMovement()->MaxWalkSpeed = Speed;
	return C->MoveToLocation(Target, 10.f, false, true, true, false, nullptr, true) != EPathFollowingRequestResult::Failed;
}

void ABozorNpc::StartWalking(const FVector& Target, float Speed)
{
	bTurning = false;
	PlayMove();
	MoveRequest(Target, Speed);
}

void ABozorNpc::TurnTowards(const FVector& Forward)
{
	const FVector F = Flat(Forward);
	if (F.SizeSquared() < 1e-4) return;
	TurnGoal = F.Rotation().Quaternion();
	bTurning = true;
}

// ================= sotuvchi

FBozorCo ABozorNpc::VendorLoop()
{
	co_await FBozorWait{ FMath::FRandRange(0.f, 2.f) };
	while (true)
	{
		// savdo xaridorning DealWith korutinida boradi; salomlashish/xayrlashish tugashini ham kutadi
		if (Deal.IsValid() || GetWorld()->GetTimeSeconds() < BusyUntil)
		{
			co_await FBozorWait{ 0.f };
			continue;
		}
		if (CallCooldown <= 0.f && SomeoneInFront(350.f) && FMath::FRand() < 0.3f)
		{
			CallCooldown = FMath::FRandRange(8.f, 16.f);
			ABozorSotuvchiOvozi::Say(this, TEXT("chaqiriq"));
			co_await WaitOrDeal(Do(TEXT("V_Call"), 2.f, 4.f));
			continue;
		}
		const float R = FMath::FRand();
		co_await WaitOrDeal(Do(R < 0.5f ? TEXT("V_Idle") : R < 0.7f ? TEXT("V_Lean") : TEXT("V_Busy"), 5.f, 12.f));
	}
}

// harakat tugashini kutadi; xaridor kelsa (savdo yoki salomlashish) darhol to'xtaydi
FBozorCo ABozorNpc::WaitOrDeal(float T)
{
	while (T > 0.f && !Deal.IsValid() && GetWorld()->GetTimeSeconds() >= BusyUntil)
	{
		co_await FBozorWait{ 0.f };
		T -= GetWorld()->GetDeltaSeconds();
	}
}

bool ABozorNpc::SomeoneInFront(float Radius) const
{
	const FVector Fwd = HomeRotation.GetForwardVector();
	for (const TWeakObjectPtr<ABozorNpc>& M : Mgr->Movers)
	{
		if (!M.IsValid() || !M->IsMoving()) continue;
		const FVector D = Flat(M->GetActorLocation() - GetActorLocation());
		if (D.SizeSquared() < Radius * Radius && FVector::DotProduct(D.GetSafeNormal(), Fwd) > 0.3f) return true;
	}
	return false;
}

// xaridor rasta oldiga keldi: sotuvchi salomlashadi, keyin o'z ishiga qaytadi
void ABozorNpc::Greet()
{
	if (NpcRole != EBozorRole::Sotuvchi || Deal.IsValid()) return;
	BusyUntil = GetWorld()->GetTimeSeconds() + Do(TEXT("V_Greet"), 1.5f, 2.5f);
	ABozorSotuvchiOvozi::Say(this, TEXT("salom"));
}

bool ABozorNpc::TryBeginDeal(ABozorNpc* Customer)
{
	if (NpcRole != EBozorRole::Sotuvchi || Deal.IsValid()) return false;
	Deal = Customer;
	FaceTowards(Customer->GetActorLocation());
	return true;
}

// busy: xayrlashish harakati yana qancha o'ynashi kerak (shu payt VendorLoop uni kesmaydi)
void ABozorNpc::EndDeal(float Busy)
{
	Deal = nullptr;
	BusyUntil = GetWorld()->GetTimeSeconds() + Busy;
	TurnTowards(HomeRotation.GetForwardVector());
}

// ================= xaridor
// Rastani navbat orqali tanlaydi: oldi bo'sh bo'lsa to'g'ri boradi, aks holda navbatga turadi,
// oldinga siljib boradi va sabri tugasa boshqa rastaga ketadi.
FBozorCo ABozorNpc::CustomerLoop()
{
	co_await FBozorWait{ FMath::FRandRange(0.f, 4.f) };
	int32 Last = INDEX_NONE;
	while (true)
	{
		const int32 Q = Mgr->BestQueue(GetActorLocation(), Last);
		int32 Idx = Q != INDEX_NONE ? Mgr->Join(Q, this) : -1;
		if (Idx < 0)
		{
			co_await WalkTo(Mgr->RandomWanderPoint());
			continue;
		}
		Last = Q;
		Queue = Q;                                      // hamrohlar navbat nuqtalariga turmasin (FollowerLoop)
		const FBozorQueueSpot Spot = Mgr->Queues[Q];
		StallForward = Spot.Forward;
		// StartOnly: navbatga oxiridan kiradi
		if (Spot.bStartOnly && Idx > 0) co_await WalkTo(Mgr->EntryPoint(Q));

		float Patience = FMath::FRandRange(Spot.Patience.X, Spot.Patience.Y);
		bool bGaveUp = false;
		while (true)
		{
			Idx = Mgr->IndexOf(Q, this);
			const FVector P = Mgr->Point(Q, Idx);
			if (FVector::DistSquared2D(GetActorLocation(), P) > 15.f * 15.f) co_await WalkTo(P);
			co_await TurnTo(Idx == 0 ? Spot.Forward : Mgr->LookAtLocation(Q) - GetActorLocation());
			if (Idx == 0) break;
			// navbatda kutadi; oldingi odam ketsa siljiydi. bCanLeaveRandomly=false: sabr tugasa ham kutaveradi
			Do(TEXT("C_Wait"), 3.f, 6.f);
			while (Mgr->IndexOf(Q, this) == Idx && (Patience > 0.f || !Spot.bCanLeaveRandomly))
			{
				Patience -= 0.25f;
				co_await FBozorWait{ 0.25f };
			}
			if (Patience <= 0.f && Spot.bCanLeaveRandomly && Mgr->IndexOf(Q, this) == Idx)
			{
				bGaveUp = true;
				break;
			}
		}
		if (bGaveUp)
		{
			Mgr->Leave(Q, this);
			Queue = INDEX_NONE;
			continue;
		}

		bAtStall = true;
		ABozorNpc* Vendor = Spot.Vendor;
		if (FMath::FRand() < 0.4f) Vendor->Greet();      // sotuvchi salomlashadi
		co_await FBozorWait{ Do(TEXT("C_Browse"), 4.f, 10.f) };

		if (FMath::FRand() < 0.8f)
		{
			// sotuvchi boshqa joydagi xaridor bilan band bo'lsa, oldinda kutib turadi
			float Wait = FMath::Max(Patience, 8.f);
			if (!Vendor->TryBeginDeal(this)) Do(TEXT("C_Wait"), Wait, Wait);
			while (Wait > 0.f && !Vendor->TryBeginDeal(this) && Vendor->Deal.Get() != this)
			{
				Wait -= 0.25f;
				co_await FBozorWait{ 0.25f };
			}
			if (Vendor->Deal.Get() == this) co_await DealWith(Vendor);
		}
		bAtStall = false;
		Mgr->Leave(Q, this);
		Queue = INDEX_NONE;
		if (FMath::FRand() < 0.2f) co_await WalkTo(Mgr->RandomWanderPoint());
	}
}

// Savdo: navbatma-navbat gap (savdolashish) -> tortish -> pul -> rahmat; yoki "qimmat ekan" deb ketish
FBozorCo ABozorNpc::DealWith(ABozorNpc* V)
{
	Mgr->TradesStarted++;
	const int32 Turns = FMath::RandRange(3, 6);
	float Total = 0.f;
	const float Cap = FMath::FRandRange(15.f, 40.f);
	for (int32 i = 0; i < Turns && Total < Cap; ++i)
	{
		const float C = Do(TEXT("C_Talk"), 2.5f, 5.f);
		V->Do(TEXT("V_Listen"), C, C);
		co_await FBozorWait{ C };
		const float S = V->Do(TEXT("V_Talk"), 2.5f, 5.f);
		ABozorSotuvchiOvozi::Say(V, TEXT("savdo"), 0.6f);       // narx aytadi, savdolashadi (har gapida emas)
		Do(TEXT("C_Listen"), S, S);
		co_await FBozorWait{ S };
		Total += C + S;
	}
	// 80% savdolashadi x 75% sotib oladi = rastada to'xtaganlarning ~60% i xarid qiladi
	if (FMath::FRand() < 0.75f)
	{
		// qo'l balandligida, tananing oldida (aktor joyi - kapsula markazi, oyoqdan ~88 sm)
		const FVector VHand = V->GetActorLocation() + V->GetActorForwardVector() * 40.f + FVector(0.f, 0.f, 15.f);
		const FVector CHand = GetActorLocation() + GetActorForwardVector() * 35.f + FVector(0.f, 0.f, 15.f);
		const float W = V->Do(TEXT("V_Weigh"), 2.5f, 4.f);   // tarozida tortadi
		ABozorRastaOvozlari::PlayNear(this, { TEXT("tarozi_pip"), TEXT("tarozi_tosh") }, VHand, 300.f, TEXT("tarozi_pip"), V);
		Do(TEXT("C_Listen"), W, W);
		co_await FBozorWait{ W };
		const float H = V->Do(TEXT("V_Handout"), 2.f, 3.f);  // paketni uzatadi
		ABozorRastaOvozlari::Play(this, TEXT("paket"), VHand, V);
		Do(TEXT("C_Pay"), H, H);                               // pul beradi
		ABozorRastaOvozlari::Play(this, FMath::FRand() < 0.5f ? FName(TEXT("pul")) : FName(TEXT("tanga")), CHand, this);
		co_await FBozorWait{ H };
		TakeBag();
		Mgr->TradesBought++;
		const float Th = V->Do(TEXT("V_Thanks"), 1.5f, 2.5f); // "Bozoringizni bersin!" - "Yana keling!"
		ABozorSotuvchiOvozi::Say(V, TEXT("rahmat"));
		co_await FBozorWait{ 1.2f };
		V->EndDeal(Th - 1.2f);                                 // xaridor ketadi, sotuvchi ta'zimni oxirigacha qiladi
		co_return;
	}
	co_await FBozorWait{ Do(TEXT("C_Reject"), 1.5f, 3.f) };
	V->EndDeal();
}

void ABozorNpc::TakeBag()
{
	if (Bag || !Mgr || Mgr->BagMeshes.Num() == 0) return;
	UStaticMesh* M = Mgr->BagMeshes[FMath::RandRange(0, Mgr->BagMeshes.Num() - 1)];
	if (!M) return;
	Bag = NewObject<UStaticMeshComponent>(this);
	Bag->SetStaticMesh(M);
	Bag->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Bag->RegisterComponent();
	const float H = M->GetBoundingBox().GetSize().Z;
	if (H > 1.f) Bag->SetWorldScale3D(FVector(32.f / H));
	Bag->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("hand_r"));
	Bag->SetWorldRotation(GetActorRotation());
	Bag->SetWorldLocation(GetMesh()->GetSocketLocation(TEXT("hand_r")) - FVector(0.f, 0.f, 18.f));
}

// ================= hamroh
// Boshliq yurganda uning yonida yuradi. Tezlik = boshliq tezligi + orqada qolgan masofa: aks holda hamroh doim
// orqada qoladi va guruh qator bo'lib yuradi. Boshliq navbatda yoki rasta oldida tursa, navbat nuqtalariga
// tegmaydigan joyda (odatda uning orqasida, yo'lak tomonda) kutadi va vaqti-vaqti bilan harakatini almashtiradi.
FBozorCo ABozorNpc::FollowerLoop()
{
	co_await FBozorWait{ 0.f };
	const float Side = GroupSlot % 2 == 1 ? 1.f : -1.f;
	const float Row = static_cast<float>((GroupSlot - 1) / 2);
	double IdleUntil = 0.0, StuckUntil = 0.0;
	float BestTo = MAX_flt, NoProgress = 0.f;
	FVector LastSlot = GetActorLocation();
	FVector Spot = FVector::ZeroVector, SpotFor(1e9);   // kutish joyi va u tanlangan paytdagi boshliq joyi
	while (Leader)
	{
		const double Now = GetWorld()->GetTimeSeconds();
		const FVector Lv = Flat(Leader->GetVelocity());
		const bool bWalking = Lv.SizeSquared() > 20.f * 20.f;
		FVector Slot;
		if (bWalking || Leader->Queue == INDEX_NONE)
		{
			Slot = Leader->GetActorLocation() + Leader->GetActorRightVector() * Side * 60.f - Leader->GetActorForwardVector() * (15.f + Row * 60.f);
			FVector H;
			if (ProjectToNav(Slot, H, 80.f)) Slot = H;
		}
		else
		{
			// kutish joyi boshliq to'xtaganda bir marta tanlanadi: har tikda qayta hisoblansa, hamroh tebranib yurardi
			if (FVector::DistSquared2D(Leader->GetActorLocation(), SpotFor) > 30.f * 30.f)
			{
				Spot = WaitSpot(Side, Row);
				SpotFor = Leader->GetActorLocation();
			}
			Slot = Spot;
		}
		const float To = FVector::Dist2D(Slot, GetActorLocation());
		// joyiga yetib bo'lmayapti: turgan joyida kutadi (boshliq yonida 2.5 m, lekin 1.5 s yaqinlasha olmadi)
		if (bWalking || FVector::DistSquared2D(Slot, LastSlot) > 50.f * 50.f)
		{
			LastSlot = Slot;
			BestTo = MAX_flt;
			NoProgress = 0.f;
			StuckUntil = 0.0;
		}
		if (To < BestTo - 10.f)
		{
			BestTo = To;
			NoProgress = 0.f;
		}
		else
		{
			NoProgress += bWalking ? 0.1f : 0.25f;
		}
		if (!bWalking && To < 250.f && NoProgress > 1.5f) StuckUntil = DBL_MAX;
		const bool bUnreachable = !bWalking && Now < StuckUntil;
		if (bWalking)
		{
			// yo'lni boshliq ketayotgan tomonga oldindan olamiz; tezlikni esa hozirgi joydan orqada qolishga qarab
			const float Lag = FVector::DotProduct(Slot - GetActorLocation(), Lv.GetSafeNormal());
			const float Lo = FMath::Max(Lv.Size() * 0.6f, MinWalk);
			StartWalking(Slot + Lv * 0.4f, SafeClamp(Lv.Size() + 0.9f * Lag, Lo, FMath::Max(MinWalk, FMath::Min(Leader->WalkSpeed * 1.5f, MaxWalk))));
			IdleUntil = 0.0;
		}
		else if (To > 30.f && !bUnreachable)
		{
			StartWalking(Slot, SafeClamp(50.f + 0.6f * To, MinWalk, FMath::Max(MinWalk, FMath::Min(Leader->WalkSpeed * 1.3f, MaxWalk))));
			IdleUntil = 0.0;
		}
		else
		{
			if (IsMoving()) StopMoving();
			if (Now >= IdleUntil)
			{
				TurnTowards(Leader->Queue != INDEX_NONE && FMath::FRand() < 0.6f ? Leader->StallForward : Leader->GetActorLocation() - GetActorLocation());
				IdleUntil = Now + Do(TEXT("F_Idle"), 4.f, 9.f);
			}
		}
		co_await FBozorWait{ bWalking ? 0.1f : 0.25f };
	}
}

// Boshliq navbatda / rasta oldida: navbat nuqtalaridan uzoqroq joy
FVector ABozorNpc::WaitSpot(float Side, float Row) const
{
	const FVector F = Flat(Leader->StallForward).GetSafeNormal();
	const FVector R = FVector::CrossProduct(FVector::UpVector, F);
	const FVector Lp = Leader->GetActorLocation();
	const float Back = 85.f + Row * 55.f, Wide = 85.f * (1.f + Row);
	const FVector Offsets[] = { -F * Back + R * Side * 35.f, -F * Back - R * Side * 35.f, R * Side * Wide - F * 30.f, -R * Side * Wide - F * 30.f };
	for (const FVector& O : Offsets)
	{
		FVector H;
		if (!ProjectToNav(Lp + O, H, 30.f) || Mgr->NearAnyPoint(H, 60.f)) continue;
		return H;
	}
	return Lp + Offsets[0];
}

// ================= yo'lovchi va hammol

FBozorCo ABozorNpc::WalkerLoop()
{
	co_await FBozorWait{ FMath::FRandRange(0.f, 2.f) };
	while (true)
	{
		co_await WalkTo(NextWalkTarget());
		if (NpcRole != EBozorRole::Yolovchi) continue;
		const float R = FMath::FRand();
		if (R < 0.4f) co_await SitDown();                                                  // choyxonada bir oz o'tiradi
		else if (R < 0.55f) co_await FBozorWait{ Do(TEXT("P_Pause"), 3.f, 7.f) };          // telefon, choy, o'ylanish
	}
}

// Keyingi manzil: kamida 3 m narida. Hammolga - iloji bo'lsa oldinda (arava ortga yurmasin)
FVector ABozorNpc::NextWalkTarget() const
{
	for (int32 i = 0; i < 8; ++i)
	{
		const FVector P = FMath::FRand() < 0.3f ? Mgr->RandomGatePoint() : Mgr->RandomWanderPoint();
		const FVector To = Flat(P - GetActorLocation());
		if (To.SizeSquared() < 300.f * 300.f) continue;
		if (Cart && i < 6 && FVector::DotProduct(To.GetSafeNormal(), GetActorForwardVector()) < -0.2f) continue;
		return P;
	}
	return FMath::FRand() < 0.3f ? Mgr->RandomGatePoint() : Mgr->RandomWanderPoint();
}

// Bo'sh o'rindiqqa o'tirish: o'tiradi -> 15-40 s -> turadi
FBozorCo ABozorNpc::SitDown()
{
	const int32 Seat = Mgr->ReserveSeat(this, GetActorLocation(), 3000.f);
	if (Seat == INDEX_NONE) co_return;
	const FBozorSeat S = Mgr->Seats[Seat];
	const FVector Front = S.Position + S.Forward * 45.f;
	co_await WalkTo(Front);
	// yetib kelmadi (yo'l yo'q, vaqt tugadi): o'rindiqqa sakramaydi, boshqa ishiga qaytadi
	if (FVector::DistSquared2D(GetActorLocation(), Front) > 35.f * 35.f)
	{
		Mgr->ReleaseSeat(Seat, this);
		co_return;
	}
	co_await TurnTo(S.Forward);
	// klipda o'tirish orqaga siljish bilan berilgan, root motion yo'q: tanani o'tirish davomida o'rindiq ustiga silliq suramiz.
	// O'rindiq NavMesh'da emas: o'tirgan paytda harakat va to'qnashuv o'chiq
	bSitting = true;
	StopMoving();
	GetCharacterMovement()->DisableMovement();
	SetActorEnableCollision(false);
	const FVector Here = GetActorLocation();
	FVector On = S.Position + S.Forward * 12.f;
	On.Z = Here.Z;
	FVector Back = Front;
	Back.Z = Here.Z;
	co_await Slide(Here, On, Do(TEXT("S_Down"), 1.2f, 2.5f));
	float T = FMath::FRandRange(15.f, 40.f);
	while (T > 0.f)
	{
		const float D = Do(FMath::FRand() < 0.6f ? TEXT("S_Loop") : TEXT("S_Drink"), 5.f, 10.f);
		co_await FBozorWait{ FMath::Min(D, T) };
		T -= D;
	}
	co_await Slide(On, Back, Do(TEXT("S_Up"), 1.2f, 2.5f));
	SettleCategory = NAME_None;
	SetActorEnableCollision(true);
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	bSitting = false;
	Mgr->ReleaseSeat(Seat, this);
}

FBozorCo ABozorNpc::Slide(FVector From, FVector To, float Time)
{
	float T = 0.f;
	while (T < Time)
	{
		SetActorLocation(FMath::Lerp(From, To, FMath::SmoothStep(0.f, 1.f, T / Time)));
		co_await FBozorWait{ 0.f };
		T += GetWorld()->GetDeltaSeconds();
	}
	SetActorLocation(To);
}

float ABozorNpc::CapsuleHalf() const
{
	return GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
}

void ABozorNpc::AttachCart()
{
	if (!Mgr) Mgr = ABozorNpcManager::Get(this);
	UStaticMesh* M = Mgr ? Mgr->CartMesh.Get() : nullptr;
	if (!M) return;
	Cart = NewObject<UStaticMeshComponent>(this);
	Cart->SetStaticMesh(M);
	Cart->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Cart->SetCanEverAffectNavigation(false);
	Cart->SetupAttachment(GetRootComponent());
	Cart->SetUsingAbsoluteLocation(true);
	Cart->SetUsingAbsoluteRotation(true);
	Cart->SetUsingAbsoluteScale(true);
	Cart->RegisterComponent();
	// modelning pivoti g'ildirakli uchida, uzunligi -Y bo'ylab, dasta -Y uchida: +Y arava oldiga (g'ildiraklar oldinda,
	// dasta hammolga). Arava o'qlarida: X = model Y, Y = -model X
	const FBox B = M->GetBoundingBox();
	CartTurn = FQuat(FVector::UpVector, FMath::DegreesToRadians(-90.f));
	const float Length = FMath::Max(B.Max.Y - B.Min.Y, 1.f);
	// model 1.92 m: bozor aravachasi ~1.5 m (2-3 m li yo'laklarda katta arava qop va odamlarga tegmay yurolmasdi)
	CartScale = FMath::Min(1.f, 150.f / Length);
	const float S = CartScale, L = Length * S, H = (B.Max.Z - B.Min.Z) * S, CenterY = -(B.Min.X + B.Max.X) * 0.5f * S;
	// SM_Aravakash_Arava geometriyasi (Blender'da o'lchangan, 2026-10-21; uzunlik 1.92, balandlik 0.77, en 0.92 m):
	// g'ildirak o'qi g'ildirakli uchdan 23% da, r = balandlikning 39%; qora dasta qarama-qarshi uchda,
	// 97% balandlikda, eni +-60% (yarim enining)
	CartAxle = FVector(B.Max.Y * S - L * 0.23f, CenterY, B.Min.Z * S + H * 0.39f);
	CartGrip = FVector(B.Min.Y * S + 2.f, CenterY, B.Min.Z * S + H * 0.97f);
	CartGripHalf = (B.Max.X - B.Min.X) * 0.5f * S * 0.6f;
	// dastani qo'l balandligiga ko'taradi: arava g'ildirak o'qi atrofida egiladi, tirgovichi yerdan uziladi
	const float Hip = 95.f * GetMesh()->GetRelativeScale3D().Z;
	const float GripZ = Hip * 0.92f;
	CartReach = Hip * 0.45f;
	const FVector G = CartGrip - CartAxle;                              // (-x, 0, +z): dasta o'qdan orqada va yuqorida
	const float D = FVector2D(G.X, G.Z).Size(), Rest = FMath::Atan2(G.Z, -G.X);
	const float Lifted = FMath::Asin(FMath::Clamp((GripZ - CartAxle.Z) / D, -1.f, 1.f));
	const float Tilt = FMath::Max(0.f, Lifted - Rest);
	CartGripDist = D * FMath::Cos(Rest + Tilt);
	// egilish ishorasi: dastani ko'taradigan tomon
	const FQuat TiltA(FVector::RightVector, Tilt), TiltB(FVector::RightVector, -Tilt);
	CartTilt = TiltA.RotateVector(G).Z >= TiltB.RotateVector(G).Z ? TiltA : TiltB;
	// kapsula (agent) endi g'ildirak o'qi: hammolning tanasini UpdateCart o'zi joylaydi
	const float Half = CapsuleHalf();
	const FVector Feet = GetActorLocation() - FVector(0.f, 0.f, Half);
	CartAxis = Flat(GetActorForwardVector()).GetSafeNormal();
	CartHitch = Feet + CartAxis * CartReach;
	FVector Axle = CartHitch + CartAxis * CartGripDist, Hit;
	if (ProjectToNav(Axle, Hit, 100.f)) Axle = FVector(Hit.X, Hit.Y, Feet.Z);
	BodyPos = Feet;
	BodyRot = GetActorQuat();
	GetCharacterMovement()->bOrientRotationToMovement = false;
	SetActorLocation(Axle + FVector(0.f, 0.f, Half), false, nullptr, ETeleportType::TeleportPhysics);
	// tana va arava harakatdan keyin joylanadi, qo'l IK esa shundan keyin baholanadi
	AddTickPrerequisiteComponent(GetCharacterMovement());
	GetMesh()->PrimaryComponentTick.AddPrerequisite(this, PrimaryActorTick);
	if (UBozorNpcMesh* Body = Cast<UBozorNpcMesh>(GetMesh())) Body->bHandIK = true;
	UpdateCart(0.f);
}

// Tirkama (Unity LateUpdate): dasta osti (hitch) g'ildirak o'qidan doimiy masofada, uning orqasidan ergashadi. Arava o'qi -
// hitch'dan g'ildiraklarga; hammol dastaning orqasida, unga qarab turadi
void ABozorNpc::UpdateCart(float Dt)
{
	const FVector Axle = GetActorLocation() - FVector(0.f, 0.f, CapsuleHalf());
	const FVector PrevBody = BodyPos;
	AAIController* C = AI();
	UPathFollowingComponent* PF = C ? C->GetPathFollowingComponent() : nullptr;
	const bool bPath = PF && PF->GetStatus() != EPathFollowingStatus::Idle;
	// Manzil orqada qolgan (yo'l aravaning orqasiga ketadi): arava orqaga yurmaydi - aks holda hammol orqasi bilan yurib,
	// aravani tortib ketardi. G'ildirak o'qi joyida turadi, hammol dastadan ushlab yoy bo'ylab aylanib, aravani buradi
	if (bPath)
	{
		const FVector Want = Flat(SteeringTarget(PF, Axle) - Axle);
		const float Off = Want.SizeSquared() > 1.f ? FMath::RadiansToDegrees(FMath::Acos(FMath::Clamp(FVector::DotProduct(Want.GetSafeNormal(), CartAxis), -1.f, 1.f))) : 0.f;
		// ikki marta ketma-ket burib bo'lmasa (tor joy, hamma yo'l orqada) - oxirgi chora sifatida orqaga yuradi
		if (!bCartTurning && Off > 100.f && CartTurnFails < 2)
		{
			bCartTurning = true;
			CartTurns++;
			PF->PauseMove();
			CartTurnSign = FVector::CrossProduct(CartAxis, Want).Z >= 0.f ? 1.f : -1.f;
			CartTurnBest = Off;
			CartTurnStill = 0.f;
			CartTurnTries = 0;
		}
		if (bCartTurning)
		{
			CartHitch = Axle - FQuat(FVector::UpVector, FMath::DegreesToRadians(CartTurnSign * 35.f * Dt)).RotateVector(CartAxis) * CartGripDist;
			if (Off < 45.f)
			{
				bCartTurning = false;
				CartTurnFails = 0;
				PF->ResumeMove();
			}
			// yoy yo'lida rasta yoki odam bor (NavMesh hammolni qaytarib turibdi): 1.5 s burchak kamaymasa - teskari tomonga,
			// u ham bo'lmasa - bu manzildan voz kechadi. Aks holda hammol abadiy joyida qotib turardi
			else if (Off < CartTurnBest - 5.f)
			{
				CartTurnBest = Off;
				CartTurnStill = 0.f;
			}
			else if ((CartTurnStill += Dt) > 1.5f)
			{
				if (++CartTurnTries == 1)
				{
					CartTurnSign = -CartTurnSign;
					CartTurnBest = Off;
					CartTurnStill = 0.f;
				}
				else
				{
					bCartTurning = false;
					bCartTurnFailed = true;
					CartTurnFails++;
					CartGiveUps++;
					PF->ResumeMove();
				}
			}
		}
	}
	else
	{
		bCartTurning = false;
	}
	// tashxis: g'ildirak o'qi aravaning orqasiga yuryapti
	if (Dt > 0.f)
	{
		CartFrames++;
		CartBackFrames += FVector::DotProduct(Flat(GetVelocity()), CartAxis) < -20.f ? 1 : 0;
	}
	const FVector Back = Flat(CartHitch - Axle).SizeSquared() > 1.f ? Flat(CartHitch - Axle) : -CartAxis;
	FVector Axis = -Back.GetSafeNormal();
	FVector Body = Axle - Axis * (CartGripDist + CartReach);
	// hammol yuriladigan joyda (NavMesh) turadi: burilishda tashqariga surilib rasta ichiga kirib ketmasin
	FVector Hit;
	if (ProjectToNav(Body, Hit, 60.f))
	{
		const FVector A = Flat(Axle - Hit);
		Axis = A.SizeSquared() > 1.f ? A.GetSafeNormal() : -Back.GetSafeNormal();
		Body = Axle - Axis * (CartGripDist + CartReach);
	}
	Body.Z = Axle.Z;
	CartAxis = Axis;
	CartHitch = Axle - Axis * CartGripDist;
	// Odatda hammol aravaga qaraydi. Aravani aylantirayotganda yon tomonga yuradi - tanasi yurish yo'nalishi bilan arava
	// o'rtasiga buriladi. Arava ortga yursa (oxirgi chora) - o'girilib, aravani orqasidan tortib boradi (orqasi bilan yurmaydi)
	FVector Face = Axis;
	const FVector Step = Flat(Body - PrevBody);
	if (Dt > 0.f && Step.Size() > 20.f * Dt)
	{
		const FVector Sd = Step.GetSafeNormal();
		if (FVector::DotProduct(Sd, Axis) < -0.3f) Face = Sd;
		else if (bCartTurning && !(Axis + Sd).IsNearlyZero()) Face = (Axis + Sd).GetSafeNormal();
	}
	const FQuat FaceRot = Face.Rotation().Quaternion();
	BodyRot = Dt > 0.f ? FQuat::Slerp(BodyRot, FaceRot, FMath::Min(1.f, 6.f * Dt)) : FaceRot;
	BodyPos = Body;
	SetActorRotation(Axis.Rotation());
	GetMesh()->SetWorldLocationAndRotation(Body, BodyRot * FRotator(0.f, -90.f, 0.f).Quaternion());
	PlaceCart(Axle);
	// og'ir arava: keskin burilishda sekinlashadi (g'ildirak yonga sirpanmaydi - hammol aravani burib olishi kerak),
	// lekin yurish klipi ko'rsata oladigan tezlikdan sekin emas - aks holda oyoq sirpanadi
	if (bPath && !bCartTurning)
	{
		const FVector V = Flat(SteeringTarget(PF, Axle) - Axle);
		const float Turn = V.SizeSquared() > 1.f ? FMath::RadiansToDegrees(FMath::Acos(FMath::Clamp(FVector::DotProduct(V.GetSafeNormal(), Axis), -1.f, 1.f))) : 0.f;
		GetCharacterMovement()->MaxWalkSpeed = FMath::Max(MinWalk, WalkSpeed * FMath::Lerp(1.f, 0.5f, FMath::Clamp(Turn / 90.f, 0.f, 1.f)));
	}
}

// Unity agent.steeringTarget: yo'lning keyingi burilish nuqtasi (crowd rejimida PathFollowing faqat oxirgi nuqtani beradi)
FVector ABozorNpc::SteeringTarget(const UPathFollowingComponent* PF, const FVector& From) const
{
	const FNavPathSharedPtr Path = PF->GetPath();
	if (!Path.IsValid() || Path->GetPathPoints().Num() < 2) return PF->GetCurrentTargetLocation();
	const TArray<FNavPathPoint>& P = Path->GetPathPoints();
	// eng yaqin bo'lak, uning oxiri; unga juda yaqin bo'lsa - keyingisi
	int32 Best = 0;
	float BestD = MAX_flt;
	for (int32 i = 0; i + 1 < P.Num(); ++i)
	{
		const float D = FMath::PointDistToSegmentSquared(Flat(From), Flat(P[i].Location), Flat(P[i + 1].Location));
		if (D < BestD)
		{
			BestD = D;
			Best = i;
		}
	}
	int32 Next = Best + 1;
	if (Next + 1 < P.Num() && FVector::DistSquared2D(From, P[Next].Location) < 30.f * 30.f) Next++;
	return P[Next].Location;
}

// g'ildirak o'qi agent nuqtasida (yer + r), dastasi ko'tarilgan; qo'llar dastaning ikki chetida
void ABozorNpc::PlaceCart(const FVector& AxleGround)
{
	const FQuat Rot = CartAxis.Rotation().Quaternion() * CartTilt;
	const FVector Pivot = AxleGround + FVector(0.f, 0.f, CartAxle.Z) - Rot.RotateVector(CartAxle);
	Cart->SetWorldTransform(FTransform(Rot * CartTurn, Pivot, FVector(CartScale)));
	if (UBozorNpcMesh* Body = Cast<UBozorNpcMesh>(GetMesh()))
	{
		const FVector Grip = Pivot + Rot.RotateVector(CartGrip);
		const FVector Side = FVector::CrossProduct(FVector::UpVector, CartAxis) * CartGripHalf;
		Body->HandTarget[0] = Grip - Side;
		Body->HandTarget[1] = Grip + Side;
	}
}

// ================= qo'l IK

void UBozorNpcMesh::FinalizeBoneTransform()
{
	if (bHandIK) ApplyHandIK();
	Super::FinalizeBoneTransform();
}

// Yangi baholangan pozada yelka-tirsak-bilak zanjiri qo'lni nishonga yetkazadi (tirsak o'z tomoniga bukiladi);
// barmoqlar va boshqa bolalar suyaklari o'z lokal holatini saqlab ergashadi
void UBozorNpcMesh::ApplyHandIK()
{
	TArray<FTransform>& Cs = GetEditableComponentSpaceTransforms();
	const USkeletalMesh* M = GetSkeletalMeshAsset();
	if (!M || Cs.Num() == 0) return;
	const FReferenceSkeleton& Ref = M->GetRefSkeleton();
	static const FName Bones[2][3] = { { TEXT("upperarm_l"), TEXT("lowerarm_l"), TEXT("hand_l") }, { TEXT("upperarm_r"), TEXT("lowerarm_r"), TEXT("hand_r") } };
	const FTransform& ToWorld = GetComponentTransform();
	for (int32 Side = 0; Side < 2; ++Side)
	{
		const int32 U = GetBoneIndex(Bones[Side][0]), L = GetBoneIndex(Bones[Side][1]), H = GetBoneIndex(Bones[Side][2]);
		if (U == INDEX_NONE || L == INDEX_NONE || H == INDEX_NONE || H >= Cs.Num()) continue;
		const TArray<FTransform> Old = Cs;
		FTransform Ut = Cs[U], Lt = Cs[L], Ht = Cs[H];
		AnimationCore::SolveTwoBoneIK(Ut, Lt, Ht, Lt.GetLocation(), ToWorld.InverseTransformPosition(HandTarget[Side]), false, 1.0, 1.0);
		Cs[U] = Ut;
		Cs[L] = Lt;
		Cs[H] = Ht;
		TBitArray<> Changed(false, Cs.Num());
		Changed[U] = true;
		Changed[L] = true;
		Changed[H] = true;
		for (int32 i = U + 1; i < Cs.Num(); ++i)
		{
			const int32 P = Ref.GetParentIndex(i);
			if (i == L || i == H || P == INDEX_NONE || !Changed[P]) continue;
			Cs[i] = Old[i].GetRelativeTransform(Old[P]) * Cs[P];
			Changed[i] = true;
		}
	}
}

// ================= suhbatdosh
// Bir suhbatda bir vaqtda bitta odam gapiradi, qolganlari tinglaydi. Navbatni guruhning birinchi a'zosi boshqaradi.
FBozorCo ABozorNpc::TalkerLoop()
{
	if (TalkGroup >= 0) FaceTowards(LookTarget);
	co_await FBozorWait{ 0.f };                       // hamma a'zolar boshlansin
	TArray<ABozorNpc*> Members;
	if (TalkGroup >= 0)
	{
		for (TActorIterator<ABozorNpc> It(GetWorld()); It; ++It)
		{
			if (It->NpcRole == EBozorRole::Suhbatdosh && It->TalkGroup == TalkGroup) Members.Add(*It);
		}
	}
	if (Members.Num() == 0) Members.Add(this);
	Members.Sort([](const ABozorNpc& A, const ABozorNpc& B) { return A.Order < B.Order; });
	if (Members[0] != this) co_return;
	co_await FBozorWait{ FMath::FRandRange(0.f, 2.f) };
	int32 Speaker = FMath::RandRange(0, Members.Num() - 1);
	while (true)
	{
		ABozorNpc* S = Members[Speaker];
		const float D = S->Do(FMath::FRand() < 0.65f ? TEXT("Pair") : TEXT("C_Talk"), 3.f, 7.f);
		// tinglovchilar biroz kechikib: hammasi bir kadrda bir xil klipni boshlasa, bir xil qimirlardi
		for (ABozorNpc* M : Members)
		{
			if (M != S) M->DoLater(TEXT("C_Listen"), D, FMath::FRandRange(0.f, 0.8f));
		}
		co_await FBozorWait{ D };
		// gap ko'pincha boshqasiga o'tadi, ba'zan o'sha odam davom etadi
		if (Members.Num() > 1 && FMath::FRand() < 0.8f) Speaker = (Speaker + FMath::RandRange(1, Members.Num() - 1)) % Members.Num();
	}
}

// ================= yordamchilar

FBozorCo ABozorNpc::WalkTo(FVector Target)
{
	AAIController* C = AI();
	bTurning = false;
	PlayMove();
	// yurib bo'lmaydi: chaqiruvchi sikl kadr kutmasdan aylanib qolmasin
	if (!C || !MoveRequest(Target, WalkSpeed))
	{
		co_await FBozorWait{ 0.f };
		co_return;
	}
	float Timeout = 90.f, Still = 0.f;
	float Best = FVector::Dist2D(Target, GetActorLocation());
	bCartTurnFailed = false;
	while (Timeout > 0.f && C->GetMoveStatus() != EPathFollowingStatus::Idle)
	{
		co_await FBozorWait{ 0.f };
		const float Dt = GetWorld()->GetDeltaSeconds();
		Timeout -= Dt;
		// aravani orqaga burib bo'lmadi - boshqa manzil
		if (bCartTurnFailed)
		{
			bCartTurnFailed = false;
			break;
		}
		// 3 s manzilga yaqinlashmadi (yo'lni odamlar to'sib turibdi): bu manzildan voz kechadi. Arava burilyapti - bu tiqilish emas
		const float Dist = FVector::Dist2D(Target, GetActorLocation());
		if (Dist <= 50.f) CartTurnFails = 0;      // manzilga yetdi: keyingi safar yana burib ko'radi
		if (Dist < Best - 30.f || bCartTurning)
		{
			Best = FMath::Min(Best, Dist);
			Still = 0.f;
		}
		else if ((Still += Dt) > 3.f)
		{
			break;
		}
	}
	StopMoving();
}

FBozorCo ABozorNpc::TurnTo(FVector Forward)
{
	TurnTowards(Forward);
	while (bTurning) co_await FBozorWait{ 0.f };
}

// ============================================================ avtomatik sinov

void ABozorNpcManager::LogStats(const TCHAR* Tag)
{
	int32 Chars = 0, Moving = 0, InQueue = 0, Trading = 0, AtStall = 0, Sitting = 0;
	for (TActorIterator<ABozorNpc> It(GetWorld()); It; ++It)
	{
		Chars++;
		Moving += It->IsMoving() ? 1 : 0;
		InQueue += It->Queue != INDEX_NONE ? 1 : 0;
		AtStall += It->bAtStall ? 1 : 0;
		Trading += It->NpcRole == EBozorRole::Sotuvchi && It->Deal.IsValid() ? 1 : 0;
		Sitting += It->bSitting ? 1 : 0;
	}
	// tashxis: tana qaysi tomonga qaragan (oyoq uchi: foot -> ball) - yuruvchilarda yurish tomoniga, hammolda aravaga
	auto Facing = [](ABozorNpc* N)
	{
		const USkeletalMeshComponent* M = N->GetMesh();
		FVector T = M->GetSocketLocation(TEXT("ball_l")) - M->GetSocketLocation(TEXT("foot_l")) + M->GetSocketLocation(TEXT("ball_r")) - M->GetSocketLocation(TEXT("foot_r"));
		T.Z = 0.0;
		return T.GetSafeNormal();
	};
	int32 Walkers = 0, Backward = 0;
	for (TActorIterator<ABozorNpc> It(GetWorld()); It; ++It)
	{
		if (It->bCart)
		{
			UE_LOG(LogBozorNpc, Display, TEXT("[BozorNpc] hammol %s kadr=%d orqaga=%d aylantirish=%d voz_kechish=%d tezlik=%.0f aravaga_qarash=%.2f"), *It->GetActorNameOrLabel(),
				It->CartFrames, It->CartBackFrames, It->CartTurns, It->CartGiveUps, It->GetVelocity().Size2D(), FVector::DotProduct(Facing(*It), It->GetActorForwardVector()));
		}
		else if (It->GetVelocity().Size2D() > 50.f)
		{
			Walkers++;
			Backward += FVector::DotProduct(Facing(*It), It->GetVelocity().GetSafeNormal2D()) < 0.f ? 1 : 0;
		}
	}
	UE_LOG(LogBozorNpc, Display, TEXT("[BozorNpc] yuruvchilar=%d teskari_qaragan=%d"), Walkers, Backward);
	UE_LOG(LogBozorNpc, Display, TEXT("[BozorNpc] %s t=%.0f npc=%d harakatda=%d navbatda=%d rasta_oldida=%d savdo(hozir)=%d savdo(boshlangan)=%d sotib_olgan=%d o'tirgan=%d o'tirishlar=%d fps=%.0f"),
		Tag, GetWorld()->GetTimeSeconds() - StartTime, Chars, Moving, InQueue, AtStall, Trading, TradesStarted, TradesBought, Sitting, SitCount,
		1.0 / FMath::Max(FApp::GetDeltaTime(), 1e-4));
}

void ABozorNpcManager::AutoTest(double Now)
{
	// kamera nuqtalari: kirish, yo'lak o'rtasi, rasta yonida, yuqoridan, sotuvchilar qatori, xaridorlar yaqindan
	static const FVector Locs[] = { FVector(-400, 0, 350), FVector(1200, 0, 170), FVector(800, 100, 160), FVector(-600, 0, 1300), FVector(700, 60, 165), FVector(2000, -120, 160) };
	static const FRotator Rots[] = { FRotator(-12, 0, 0), FRotator(-5, 0, 0), FRotator(-8, 90, 0), FRotator(-38, 0, 0), FRotator(-6, -90, 0), FRotator(-6, -60, 0) };
	// oxirgi kadr: aravali hammol yaqindan (yon tomondan, tepadan)
	const int32 Count = UE_ARRAY_COUNT(Locs) + 1;
	if (Now < NextShot) return;
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC) return;
	// birinchi ishga tushishda shader'lar kompilyatsiya qilinadi, NavMesh quriladi: tayyor bo'lgach 12 s o'tib suratga olinadi
	if (ReadyTime <= 0.0)
	{
		int32 Jobs = 0;
#if WITH_EDITOR
		Jobs = GShaderCompilingManager ? GShaderCompilingManager->GetNumRemainingJobs() : 0;
#endif
		UNavigationSystemV1* Nav = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
		const bool bNavBusy = Nav && Nav->IsNavigationBuildInProgress();
		if (Jobs > 0 || bNavBusy)
		{
			UE_LOG(LogBozorNpc, Display, TEXT("[BozorNpc] kutilmoqda: shader=%d navmesh=%d"), Jobs, bNavBusy ? 1 : 0);
			NextShot = Now + 5.0;
			return;
		}
		ReadyTime = Now;
		NextShot = Now + 12.0;
		LogStats(TEXT("tayyor"));
		return;
	}
	const int32 Round = AutoShot / Count;
	if (Round >= 2)
	{
		LogStats(TEXT("yakun"));
		UKismetSystemLibrary::QuitGame(this, PC, EQuitPreference::Quit, false);
		NextShot = DBL_MAX;
		return;
	}
	const int32 Shot = AutoShot % Count;
	if (Shot == 0) LogStats(Round == 0 ? TEXT("1-tur") : TEXT("2-tur"));
	if (!AutoCamera)
	{
		AutoCamera = GetWorld()->SpawnActor<ACameraActor>(Locs[0], Rots[0]);
		AutoCamera->GetCameraComponent()->SetFieldOfView(70.f);
		AutoCamera->GetCameraComponent()->bConstrainAspectRatio = false;
	}
	FVector CamLoc = Locs[FMath::Min(Shot, Count - 2)];
	FRotator CamRot = Rots[FMath::Min(Shot, Count - 2)];
	if (Shot == Count - 1)
	{
		for (TActorIterator<ABozorNpc> It(GetWorld()); It; ++It)
		{
			if (!It->bCart) continue;
			const FVector P = It->GetActorLocation();
			CamLoc = P - It->GetActorRightVector() * 380.f - It->GetActorForwardVector() * 120.f + FVector(0.f, 0.f, 160.f);
			CamRot = (P - FVector(0.f, 0.f, 40.f) - It->GetActorForwardVector() * 80.f - CamLoc).Rotation();
			break;
		}
	}
	AutoCamera->SetActorLocationAndRotation(CamLoc, CamRot);
	PC->SetViewTarget(AutoCamera);
	FTimerHandle H;
	const FString Name = FString::Printf(TEXT("Bozor_%d_%d"), Round + 1, Shot + 1);
	GetWorldTimerManager().SetTimer(H, [Name]() { FScreenshotRequest::RequestScreenshot(Name, false, false); }, 1.0f, false);
	AutoShot++;
	NextShot = Now + (Shot == Count - 1 ? 15.0 : 2.0);
}
