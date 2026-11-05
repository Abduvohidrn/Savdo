#include "BozorHud.h"

#include "BozorAvtobus.h"
#include "BozorOyinchi.h"
#include "CanvasItem.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "Engine/Font.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"
#include "InputCoreTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/CommandLine.h"
#include "Misc/Paths.h"
#include "Sound/SoundBase.h"
#include "UnrealClient.h"
#include "UObject/ConstructorHelpers.h"

namespace
{
	const FLinearColor Qogozrang(0.96f, 0.93f, 0.83f, 1.f);
	const FLinearColor Siyoh(0.07f, 0.12f, 0.38f, 1.f);
	const FLinearColor Chiziq(0.45f, 0.62f, 0.85f, 0.55f);
	const FLinearColor Hoshiya(0.85f, 0.25f, 0.25f, 0.6f);

	// 50 000 -> "1 × 50 000"; 37 000 -> "1 × 20 000, 1 × 10 000, 1 × 5 000, 1 × 2 000"
	FString Kupyuralar(int32 Pul)
	{
		static const int32 Turlari[] = { 200000, 100000, 50000, 20000, 10000, 5000, 2000, 1000 };
		TArray<FString> Q;
		for (int32 K : Turlari)
		{
			const int32 N = Pul / K;
			if (N > 0) Q.Add(FString::Printf(TEXT("%d × %s"), N, *BozorSotuv::Som(K)));
			Pul -= N * K;
		}
		return FString::Join(Q, TEXT(", "));
	}
}

ABozorHud::ABozorHud()
{
	PrimaryActorTick.bCanEverTick = true;
	static ConstructorHelpers::FObjectFinder<USoundBase> S(TEXT("/Game/Bozor/Audio/SFX/sfx_pul_01.sfx_pul_01"));
	if (S.Succeeded()) Shitir = S.Object;
}

UBozorHamyon* ABozorHud::Hamyon() const
{
	const ABozorOyinchi* O = PlayerOwner ? Cast<ABozorOyinchi>(PlayerOwner->GetPawn()) : nullptr;
	return O ? O->Hamyon.Get() : nullptr;
}

void ABozorHud::Qogoz(bool bOch)
{
	if (bOchiq == bOch) return;
	bOchiq = bOch;
	if (bOch) OchilganVaqt = GetWorld()->GetTimeSeconds();
	else bBirinchi = false;
	if (Shitir) UGameplayStatics::PlaySound2D(this, Shitir, bOch ? 0.8f : 0.5f, bOch ? 1.f : 1.15f);
}

void ABozorHud::Tick(float Dt)
{
	Super::Tick(Dt);
	if (!bFaol)
	{
		// intro tugaguncha qog'oz yo'q; intro bo'lgan bo'lsa - darhol cho'ntakdan chiqariladi
		const ABozorAvtobusIntro* Intro = nullptr;
		for (TActorIterator<ABozorAvtobusIntro> It(GetWorld()); It; ++It) Intro = *It;
		if (Intro && Intro->BoshqaruvKutilmoqda()) return;
		bFaol = true;
		bTest = FParse::Param(FCommandLine::Get(), TEXT("AvtobusTest"));
		if (Intro && Intro->Oynaldi())
		{
			Qogoz(true);
			bBirinchi = true;
			bIntroBor = true;
			BozorJoyi = Intro->QarashNuqtasi;
		}
		return;
	}

	const double Hozir = GetWorld()->GetTimeSeconds();
	APlayerController* P = PlayerOwner.Get();
	const APawn* Pw = P ? P->GetPawn() : nullptr;
	if (P)
	{
		// Tab bosib turilganda ochiq, qo'yib yuborilsa yopiladi. Introdan keyingi birinchi ochilishda Tab bosilsa - shu rejimga o'tadi
		if (P->IsInputKeyDown(EKeys::Tab))
		{
			bBirinchi = false;
			Qogoz(true);
		}
		else if (bOchiq && !bBirinchi) Qogoz(false);
		else if (bOchiq && bBirinchi && Hozir - OchilganVaqt > 1.5)
		{
			const bool bYurdi = Pw && Pw->GetVelocity().Size2D() > 30.f;
			// sinovda qog'oz o'zi yopiladi - maslahat ham suratga tushsin
			if (bYurdi || P->WasInputKeyJustPressed(EKeys::E) || P->WasInputKeyJustPressed(EKeys::Enter) || (bTest && Hozir - OchilganVaqt > 2.5))
				Qogoz(false);
		}
	}
	Ochiqlik = FMath::FInterpConstantTo(Ochiqlik, bOchiq ? 1.f : 0.f, Dt, 2.8f);

	// birinchi qog'oz yopilgach: maslahat va belgi. Maslahat o'yinchi ~4 m yurib, 6 s o'tgach (yoki 15 s dan keyin) yo'qoladi;
	// belgi darvozaga 6 m qolganda
	if (bIntroBor && Pw)
	{
		if (MaslahatBosh < 0.0 && !bOchiq && !bBirinchi)
		{
			MaslahatBosh = Hozir;
			MaslahatJoy = Pw->GetActorLocation();
			bBelgi = true;
		}
		if (MaslahatBosh >= 0.0 && !bMaslahatTugadi)
		{
			const double O = Hozir - MaslahatBosh;
			if ((O > 6.0 && FVector::Dist2D(Pw->GetActorLocation(), MaslahatJoy) > 400.f) || O > 15.0) bMaslahatTugadi = true;
		}
		if (bBelgi && FVector::Dist2D(Pw->GetActorLocation(), BozorJoyi) < 600.f) bBelgi = false;
	}
	MaslahatAlfa = FMath::FInterpConstantTo(MaslahatAlfa, MaslahatBosh >= 0.0 && !bMaslahatTugadi ? 1.f : 0.f, Dt, 2.f);
	BelgiAlfa = FMath::FInterpConstantTo(BelgiAlfa, bBelgi && !bOchiq ? 1.f : 0.f, Dt, 2.f);

	if (bTest && !bTestSurat && Ochiqlik >= 1.f)
	{
		bTestSurat = true;
		FScreenshotRequest::RequestScreenshot(FPaths::ProjectSavedDir() / TEXT("Screenshots/AvtobusTest/4c_royxat.png"), false, false);
	}
	if (bTest && !bMaslahatSurat && MaslahatAlfa >= 1.f)
	{
		bMaslahatSurat = true;
		FScreenshotRequest::RequestScreenshot(FPaths::ProjectSavedDir() / TEXT("Screenshots/AvtobusTest/4d_maslahat.png"), false, false);
	}
}

void ABozorHud::DrawHUD()
{
	Super::DrawHUD();
	if (!bFaol || !Canvas) return;
	if (BelgiAlfa > 0.f) BelgiChiz();
	if (MaslahatAlfa > 0.f) MaslahatChiz();
	if (Ochiqlik > 0.f) QogozChiz(Ochiqlik);

	// yopiq paytda burchakda eslatma (maslahat ko'rinib turganda - unda Tab bor)
	if (Ochiqlik < 1.f && MaslahatAlfa < 1.f)
	{
		const float S = Canvas->ClipY / 1080.f;
		FCanvasTextItem T(FVector2D(40.f * S, Canvas->ClipY - 70.f * S), FText::FromString(TEXT("Tab (bosib turing) — ro'yxat")), GEngine->GetMediumFont(),
			FLinearColor(1.f, 1.f, 1.f, 0.75f * (1.f - Ochiqlik) * (1.f - MaslahatAlfa)));
		T.Scale = FVector2D(1.3f * S);
		T.EnableShadow(FLinearColor(0.f, 0.f, 0.f, 0.6f * (1.f - Ochiqlik) * (1.f - MaslahatAlfa)));
		Canvas->DrawItem(T);
	}
}

void ABozorHud::QogozChiz(float A)
{
	const UBozorHamyon* H = Hamyon();
	if (!H) return;
	UFont* Shrift = GEngine->GetLargeFont();
	const float S = Canvas->ClipY / 1080.f;
	const float Yuz = 1.f - FMath::Pow(1.f - A, 3.f);   // ease-out: cho'ntakdan tez chiqib, sekin to'xtaydi

	const float Qator = 66.f * S;
	const float W = 580.f * S;
	const float Bosh = 130.f * S;   // sarlavha joyi
	const int32 N = H->Royxat.Num();
	const float Hgt = Bosh + Qator * FMath::Max(N, 1) + 245.f * S;
	const float X = Canvas->ClipX - W - 90.f * S;
	const float Y = FMath::Lerp(Canvas->ClipY + 30.f * S, (Canvas->ClipY - Hgt) * 0.5f, Yuz);

	auto Tort = [this](float X0, float Y0, float W0, float H0, const FLinearColor& C)
	{
		FCanvasTileItem T(FVector2D(X0, Y0), FVector2D(W0, H0), C);
		T.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(T);
	};
	auto Chiz = [this](float X0, float Y0, float X1, float Y1, const FLinearColor& C, float Q)
	{
		FCanvasLineItem L(FVector2D(X0, Y0), FVector2D(X1, Y1));
		L.SetColor(C);
		L.LineThickness = Q;
		Canvas->DrawItem(L);
	};
	auto Yoz = [this, Shrift](const FString& Matn, float X0, float Y0, float Kattalik, const FLinearColor& C)
	{
		FCanvasTextItem T(FVector2D(X0, Y0), FText::FromString(Matn), Shrift, C);
		T.Scale = FVector2D(Kattalik);
		Canvas->DrawItem(T);
	};

	// soya va qog'oz
	Tort(X + 10.f * S, Y + 12.f * S, W, Hgt, FLinearColor(0.f, 0.f, 0.f, 0.35f));
	Tort(X, Y, W, Hgt, Qogozrang);
	// daftar chiziqlari va chap hoshiya
	for (float Ly = Y + Bosh; Ly < Y + Hgt - 20.f * S; Ly += Qator)
		Chiz(X + 12.f * S, Ly, X + W - 12.f * S, Ly, Chiziq, 1.5f * S);
	Chiz(X + 62.f * S, Y, X + 62.f * S, Y + Hgt, Hoshiya, 2.f * S);

	const float Kt = 2.1f * S;   // matn kattaligi (GetLargeFont kichik - 10-13 yoshli bola uzoqdan o'qisin)
	Yoz(TEXT("Xarid ro'yxati"), X + 80.f * S, Y + 38.f * S, 2.6f * S, Siyoh);

	for (int32 I = 0; I < N; ++I)
	{
		const FBozorRoyxatQator& R = H->Royxat[I];
		const float Ly = Y + Bosh + Qator * I;          // qator ostidagi chiziq ustidan yoziladi
		const float Ty = Ly + Qator * 0.5f - 22.f * S;
		// katakcha
		const float Kx = X + 20.f * S, Ky = Ty + 8.f * S, Kk = 28.f * S;
		Chiz(Kx, Ky, Kx + Kk, Ky, Siyoh, 2.f * S);
		Chiz(Kx + Kk, Ky, Kx + Kk, Ky + Kk, Siyoh, 2.f * S);
		Chiz(Kx + Kk, Ky + Kk, Kx, Ky + Kk, Siyoh, 2.f * S);
		Chiz(Kx, Ky + Kk, Kx, Ky, Siyoh, 2.f * S);
		const FString Matn = FString::Printf(TEXT("%s — %s %s"), *R.Nomi, *BozorSotuv::Miqdor(R.Miqdor), *R.Birlik);
		Yoz(Matn, X + 80.f * S, Ty, Kt, R.bOlindi ? Siyoh * FLinearColor(1.f, 1.f, 1.f, 0.5f) : Siyoh);
		if (R.bOlindi)
		{
			// belgi va ustidan chizish
			Chiz(Kx + 4.f * S, Ky + Kk * 0.5f, Kx + Kk * 0.4f, Ky + Kk - 3.f * S, FLinearColor(0.1f, 0.5f, 0.15f), 3.f * S);
			Chiz(Kx + Kk * 0.4f, Ky + Kk - 3.f * S, Kx + Kk + 6.f * S, Ky - 6.f * S, FLinearColor(0.1f, 0.5f, 0.15f), 3.f * S);
			float Tw = 0.f, Th = 0.f;
			Canvas->TextSize(Shrift, Matn, Tw, Th, Kt, Kt);
			Chiz(X + 78.f * S, Ty + Th * 0.55f, X + 82.f * S + Tw, Ty + Th * 0.5f, Siyoh, 2.f * S);
		}
	}

	// pul va eslatma
	const float Py = Y + Bosh + Qator * FMath::Max(N, 1) + 20.f * S;
	Yoz(FString::Printf(TEXT("Pul: %s so'm"), *BozorSotuv::Som(H->Pul)), X + 80.f * S, Py, Kt, Siyoh);
	Yoz(FString::Printf(TEXT("(%s)"), *Kupyuralar(H->Pul)), X + 80.f * S, Py + 52.f * S, 1.5f * S, Siyoh * FLinearColor(1.f, 1.f, 1.f, 0.75f));
	Yoz(H->Eslatma, X + 80.f * S, Py + 110.f * S, 1.8f * S, FLinearColor(0.55f, 0.1f, 0.1f));

	// birinchi ochilishda: qanday yopilishi va keyin qanday ochilishi (Tab bilan ochilganda yozuv kerak emas)
	if (!bBirinchi) return;
	FCanvasTextItem T(FVector2D(X, Y + Hgt + 18.f * S),
		FText::FromString(TEXT("Yura boshlang — keyin ro'yxatni Tab'ni bosib turib ko'rasiz")),
		GEngine->GetMediumFont(), FLinearColor(1.f, 1.f, 1.f, 0.85f * Yuz));
	T.Scale = FVector2D(1.6f * S);
	T.EnableShadow(FLinearColor(0.f, 0.f, 0.f, 0.7f * Yuz));
	Canvas->DrawItem(T);
}

// Pastda, o'rtada: tugmalar va nima qilishi. Tugma - oq qopqoq ichida qora harf
void ABozorHud::MaslahatChiz()
{
	const float S = Canvas->ClipY / 1080.f;
	const float A = MaslahatAlfa;
	UFont* Shrift = GEngine->GetLargeFont();
	const float Kt = 1.7f * S;
	struct FBand { TArray<FString> Tugmalar; FString Nima; };
	const FBand Bandlar[] = {
		{ { TEXT("W"), TEXT("A"), TEXT("S"), TEXT("D") }, TEXT("yurish") },
		{ { TEXT("Sichqoncha") }, TEXT("atrofga qarash") },
		{ { TEXT("Tab") }, TEXT("ro'yxat (bosib turing)") },
	};
	const float Pad = 14.f * S, Oraliq = 8.f * S, BandOraliq = 46.f * S, QopH = 46.f * S;

	auto Olcham = [this, Shrift, Kt](const FString& T) { float W = 0.f, H = 0.f; Canvas->TextSize(Shrift, T, W, H, Kt, Kt); return FVector2D(W, H); };
	// umumiy kenglik
	float Jami = 0.f;
	for (const FBand& B : Bandlar)
	{
		for (const FString& T : B.Tugmalar) Jami += FMath::Max(QopH, Olcham(T).X + 2.f * Pad) + Oraliq;
		Jami += Olcham(B.Nima).X + BandOraliq;
	}
	Jami -= BandOraliq;

	const float PanelH = QopH + 2.f * 18.f * S + 44.f * S;
	const float X0 = (Canvas->ClipX - Jami) * 0.5f;
	const float Y0 = Canvas->ClipY - PanelH - 60.f * S + (1.f - A) * 30.f * S;   // pastdan sal ko'tarilib chiqadi

	auto Tort = [this](float X, float Y, float W, float H, const FLinearColor& C)
	{
		FCanvasTileItem T(FVector2D(X, Y), FVector2D(W, H), C);
		T.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(T);
	};
	auto Yoz = [this, Shrift](const FString& M, float X, float Y, float K, const FLinearColor& C, bool bSoya)
	{
		FCanvasTextItem T(FVector2D(X, Y), FText::FromString(M), Shrift, C);
		T.Scale = FVector2D(K);
		if (bSoya) T.EnableShadow(FLinearColor(0.f, 0.f, 0.f, 0.7f * C.A));
		Canvas->DrawItem(T);
	};

	Tort(X0 - 30.f * S, Y0, Jami + 60.f * S, PanelH, FLinearColor(0.f, 0.f, 0.f, 0.5f * A));
	Yoz(TEXT("Bozor darvozasi tomon yuring"), X0, Y0 + 12.f * S, 1.45f * S, FLinearColor(1.f, 0.85f, 0.3f, A), true);

	float X = X0;
	const float Yq = Y0 + 18.f * S + 44.f * S;
	for (const FBand& B : Bandlar)
	{
		for (const FString& T : B.Tugmalar)
		{
			const FVector2D O = Olcham(T);
			const float W = FMath::Max(QopH, O.X + 2.f * Pad);
			Tort(X, Yq + 3.f * S, W, QopH, FLinearColor(0.f, 0.f, 0.f, 0.6f * A));          // qopqoq soyasi
			Tort(X, Yq, W, QopH, FLinearColor(0.95f, 0.95f, 0.93f, 0.95f * A));
			Yoz(T, X + (W - O.X) * 0.5f, Yq + (QopH - O.Y) * 0.5f, Kt, FLinearColor(0.08f, 0.08f, 0.1f, A), false);
			X += W + Oraliq;
		}
		const FVector2D O = Olcham(B.Nima);
		Yoz(B.Nima, X + 4.f * S, Yq + (QopH - O.Y) * 0.5f, Kt, FLinearColor(1.f, 1.f, 1.f, A), true);
		X += O.X + BandOraliq;
	}
}

// Bozor darvozasi belgisi: ekranda bo'lsa - darvoza ustida, orqada yoki chetda bo'lsa - ekran chetida, o'sha tomonda.
// Sariq romb va "Bozor · 35 m"
void ABozorHud::BelgiChiz()
{
	APlayerController* P = PlayerOwner.Get();
	if (!P || !P->PlayerCameraManager) return;
	const float S = Canvas->ClipY / 1080.f;
	const float A = BelgiAlfa;
	const FVector K = P->PlayerCameraManager->GetCameraLocation();
	const FVector L = P->PlayerCameraManager->GetCameraRotation().UnrotateVector(BozorJoyi - K);   // X oldinga, Y o'ngga, Z yuqoriga

	const FVector2D Markaz(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);
	const float Chet = 70.f * S;
	FVector2D E;
	bool bEkranda = false;
	if (L.X > 1.f)
	{
		const FVector Pr = Project(BozorJoyi, false);   // Canvas koordinatasida
		E = FVector2D(Pr.X, Pr.Y);
		bEkranda = E.X > Chet && E.X < Canvas->ClipX - Chet && E.Y > Chet && E.Y < Canvas->ClipY - Chet;
	}
	if (!bEkranda)
	{
		// orqada yoki chetda: kamera tekisligidagi yo'nalish bo'yicha ekran chetiga
		FVector2D D(L.Y, -L.Z * 0.3f);
		if (D.IsNearlyZero()) D = FVector2D(1.f, 0.f);
		D.Normalize();
		const float Kx = (Canvas->ClipX * 0.5f - Chet) / FMath::Max(FMath::Abs(D.X), 0.001f);
		const float Ky = (Canvas->ClipY * 0.5f - Chet) / FMath::Max(FMath::Abs(D.Y), 0.001f);
		E = Markaz + D * FMath::Min(Kx, Ky);
	}

	// romb (45° burilgan kvadrat)
	const float R = 18.f * S;
	FCanvasTileItem Romb(E - FVector2D(R, R), FVector2D(2.f * R, 2.f * R), FLinearColor(1.f, 0.8f, 0.15f, 0.95f * A));
	Romb.BlendMode = SE_BLEND_Translucent;
	Romb.Rotation = FRotator(0.f, 45.f, 0.f);
	Romb.PivotPoint = FVector2D(0.5f, 0.5f);
	Canvas->DrawItem(Romb);
	FCanvasTileItem Ichi(E - FVector2D(R * 0.45f, R * 0.45f), FVector2D(R * 0.9f, R * 0.9f), FLinearColor(0.1f, 0.2f, 0.5f, 0.95f * A));
	Ichi.BlendMode = SE_BLEND_Translucent;
	Ichi.Rotation = FRotator(0.f, 45.f, 0.f);
	Ichi.PivotPoint = FVector2D(0.5f, 0.5f);
	Canvas->DrawItem(Ichi);

	const APawn* Pw = P->GetPawn();
	const int32 Metr = Pw ? FMath::RoundToInt(FVector::Dist2D(Pw->GetActorLocation(), BozorJoyi) / 100.f) : 0;
	FCanvasTextItem T(E + FVector2D(0.f, R + 22.f * S), FText::FromString(FString::Printf(TEXT("Bozor · %d m"), Metr)), GEngine->GetLargeFont(),
		FLinearColor(1.f, 1.f, 1.f, A));
	T.Scale = FVector2D(1.4f * S);
	T.bCentreX = true;
	T.EnableShadow(FLinearColor(0.f, 0.f, 0.f, 0.8f * A));
	Canvas->DrawItem(T);
}
