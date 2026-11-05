#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BozorSotuv.generated.h"

// Sotuv jarayoni: o'yinchi sotuvchi bilan matn orqali gaplashadi.
// "Miya" (UBozorSotuvchiMiya) faqat gapni tushunadi va javob matnini beradi; narx, pul va qaytimni
// suhbat (UBozorSuhbat) hisoblaydi va tekshiradi - miya keyin AI (local yoki Claude) bilan almashtiriladi.

USTRUCT(BlueprintType)
struct FBozorMahsulot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString Nomi = TEXT("pomidor");
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString Birlik = TEXT("kilo");
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 Narx = 12000;        // so'ralgan narx, bir birlik uchun
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 EngPastNarx = 10000; // bundan arzon sotmaydi
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 Qadam = 1000;        // savdolashganda bir tushirish
};

UENUM(BlueprintType)
enum class EBozorBosqich : uint8
{
	Boshlanish,
	NarxAytildi,
	PulKutilmoqda,
	Tugadi
};

// Miya so'raydigan harakat - suhbat uni tekshirib bajaradi
UENUM(BlueprintType)
enum class EBozorHarakat : uint8
{
	Yoq,
	Salom,
	NarxAytish,
	Savdolashish,
	Tortish,       // Qiymat - miqdor (birlik)
	PulQabul,      // Qiymat - o'yinchi bergan pul
	Xayr,
	Tushunmadi
};

USTRUCT(BlueprintType)
struct FBozorSuhbatHolati
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly) FBozorMahsulot Mahsulot;
	UPROPERTY(BlueprintReadOnly) EBozorBosqich Bosqich = EBozorBosqich::Boshlanish;
	UPROPERTY(BlueprintReadOnly) int32 JoriyNarx = 0;   // hozir kelishilgan narx (bir birlik)
	UPROPERTY(BlueprintReadOnly) float Miqdor = 0.f;
	UPROPERTY(BlueprintReadOnly) int32 Summa = 0;
	UPROPERTY(BlueprintReadOnly) int32 Savdolashuv = 0;  // necha marta tushirish so'raldi
	UPROPERTY(BlueprintReadOnly) bool bSalomlashdi = false;
};

USTRUCT(BlueprintType)
struct FBozorTushuncha
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly) EBozorHarakat Harakat = EBozorHarakat::Tushunmadi;
	UPROPERTY(BlueprintReadOnly) float Qiymat = 0.f;
	UPROPERTY(BlueprintReadOnly) bool bSalomBor = false;
};

// Suhbat natijasi - miya yozadigan javob turi
UENUM(BlueprintType)
enum class EBozorJavob : uint8
{
	Salom,
	Narx,
	TushirdiMan,
	TushirmaymanEngPast,
	TushirmaymanHozircha,
	MiqdorSora,
	Tortdim,
	PulYetmaydi,
	HamyondaYoq,
	PulOldim,
	Xayr,
	XayrSotibOlmay,
	SalomBering,
	Tushunmadim,
	Tugagan
};

// Gapni tushunadi va javob matnini yozadi. Hozir - tayyor javoblar; keyin AI bilan almashtiriladi.
UCLASS(Abstract, BlueprintType)
class BOZORUNREAL_API UBozorSotuvchiMiya : public UObject
{
	GENERATED_BODY()

public:
	virtual FBozorTushuncha Tushun(const FString& Gap, const FBozorSuhbatHolati& Holat) PURE_VIRTUAL(UBozorSotuvchiMiya::Tushun, return {};);
	virtual FString Yoz(EBozorJavob Javob, const FBozorSuhbatHolati& Holat, int32 Qaytim) PURE_VIRTUAL(UBozorSotuvchiMiya::Yoz, return {};);
};

// Kalit so'zlar bo'yicha tushunadi, javoblarni tayyor variantlardan tasodifiy tanlaydi
UCLASS()
class BOZORUNREAL_API UBozorTayyorJavob : public UBozorSotuvchiMiya
{
	GENERATED_BODY()

public:
	virtual FBozorTushuncha Tushun(const FString& Gap, const FBozorSuhbatHolati& Holat) override;
	virtual FString Yoz(EBozorJavob Javob, const FBozorSuhbatHolati& Holat, int32 Qaytim) override;
};

// Ota-ona bergan xarid ro'yxatidagi bitta qator
USTRUCT(BlueprintType)
struct FBozorRoyxatQator
{
	GENERATED_BODY()

	FBozorRoyxatQator() = default;
	FBozorRoyxatQator(const TCHAR* N, float M, const TCHAR* B) : Nomi(N), Miqdor(M), Birlik(B) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString Nomi;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float Miqdor = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString Birlik = TEXT("dona");
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bOlindi = false;
};

// O'yinchining puli, xarid ro'yxati (cho'ntakdagi qog'oz) va xarid qilganlari
UCLASS(ClassGroup = Bozor, meta = (BlueprintSpawnableComponent))
class BOZORUNREAL_API UBozorHamyon : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bozor")
	int32 Pul = 50000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bozor")
	TArray<FBozorRoyxatQator> Royxat = {
		{ TEXT("Non"), 2.f, TEXT("dona") },
		{ TEXT("Pomidor"), 1.f, TEXT("kg") },
		{ TEXT("Kartoshka"), 2.f, TEXT("kg") },
	};

	// qog'oz pastidagi ona yozuvi
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bozor")
	FString Eslatma = TEXT("Qaytimni sanab ol!\n— Oying");

	UPROPERTY(BlueprintReadOnly, Category = "Bozor")
	TArray<FString> Savat;
};

UCLASS(BlueprintType)
class BOZORUNREAL_API UBozorSuhbat : public UObject
{
	GENERATED_BODY()

public:
	void Boshla(const FBozorMahsulot& Mahsulot, UBozorHamyon* Hamyon, UBozorSotuvchiMiya* Miya);

	// O'yinchi gapi -> sotuvchi javobi
	FString Yoz(const FString& Gap);

	const FBozorSuhbatHolati& GetHolat() const { return Holat; }

private:
	FString Javob(EBozorJavob J, int32 Qaytim = 0);

	UPROPERTY() FBozorSuhbatHolati Holat;
	UPROPERTY() TObjectPtr<UBozorHamyon> Hamyon;
	UPROPERTY() TObjectPtr<UBozorSotuvchiMiya> Miya;
};

namespace BozorSotuv
{
	// 12000 -> "12 000"
	FString Som(int64 N);
	// 1.5 -> "1,5", 2 -> "2"
	FString Miqdor(float M);
}
