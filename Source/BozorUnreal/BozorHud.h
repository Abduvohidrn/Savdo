#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BozorHud.generated.h"

class USoundBase;
class UBozorHamyon;

// Xarid ro'yxati qog'ozi. Intro tugagach bola uni cho'ntagidan chiqaradi: qog'oz ekran pastidan o'ng tomonga ko'tariladi
// (ro'yxat, pul, onaning eslatmasi). Birinchi marta - E/Enter yoki yura boshlasa yopiladi; keyin Tab bosib turilganda ochiq.
// Qog'oz yopilgach - boshqaruv maslahati (pastda, o'yinchi yurib ko'rgach yo'qoladi) va bozor darvozasi belgisi (yetib borguncha).
// Ma'lumot o'yinchi hamyonida (UBozorHamyon::Royxat, Pul). Hozircha Canvas bilan chiziladi
UCLASS()
class BOZORUNREAL_API ABozorHud : public AHUD
{
	GENERATED_BODY()

public:
	ABozorHud();

	virtual void Tick(float DeltaSeconds) override;
	virtual void DrawHUD() override;

	void Qogoz(bool bOch);

private:
	UBozorHamyon* Hamyon() const;
	void QogozChiz(float Ochiqlik);
	void MaslahatChiz();
	void BelgiChiz();

	UPROPERTY() TObjectPtr<USoundBase> Shitir;

	bool bFaol = false;       // intro tugadi, HUD ishlaydi
	bool bOchiq = false;
	bool bBirinchi = false;   // introdan keyingi birinchi ochilish
	bool bTest = false;
	bool bTestSurat = false;
	float Ochiqlik = 0.f;     // 0 - cho'ntakda, 1 - ko'z oldida
	double OchilganVaqt = 0.0;

	// boshqaruv maslahati va bozor belgisi (faqat intro o'ynalgan bo'lsa)
	bool bIntroBor = false;
	FVector BozorJoyi = FVector::ZeroVector;
	double MaslahatBosh = -1.0;   // ko'rsatila boshlagan vaqt
	FVector MaslahatJoy = FVector::ZeroVector;   // o'sha paytda o'yinchi qayerda edi
	bool bMaslahatTugadi = false;
	float MaslahatAlfa = 0.f;
	bool bBelgi = false;
	float BelgiAlfa = 0.f;
	bool bMaslahatSurat = false;
};
