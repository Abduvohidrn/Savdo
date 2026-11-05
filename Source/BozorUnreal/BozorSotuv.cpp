#include "BozorSotuv.h"

namespace
{
	// kichik harf, tutuq belgilarining hammasi ', tinish belgilari bo'shliq
	FString Normal(const FString& In)
	{
		FString S = In.ToLower();
		for (TCHAR& C : S)
		{
			if (C == 0x2BB || C == 0x2BC || C == 0x2018 || C == 0x2019 || C == TEXT('`') || C == 0xB4) C = TEXT('\'');
			else if (!FChar::IsAlnum(C) && C != TEXT('\'') && C != TEXT('.') && C != TEXT(',')) C = TEXT(' ');
		}
		return S;
	}

	bool Bor(const FString& S, std::initializer_list<const TCHAR*> Sozlar)
	{
		for (const TCHAR* W : Sozlar)
			if (S.Contains(W)) return true;
		return false;
	}

	float SozSon(const FString& W)
	{
		static const TMap<FString, float> Sonlar = {
			{TEXT("bir"), 1}, {TEXT("ikki"), 2}, {TEXT("uch"), 3}, {TEXT("to'rt"), 4}, {TEXT("tort"), 4},
			{TEXT("besh"), 5}, {TEXT("olti"), 6}, {TEXT("yetti"), 7}, {TEXT("sakkiz"), 8},
			{TEXT("to'qqiz"), 9}, {TEXT("toqqiz"), 9}, {TEXT("o'n"), 10}, {TEXT("on"), 10}};
		const float* V = Sonlar.Find(W);
		return V ? *V : -1.f;
	}

	bool Raqam(const FString& W) { return W.Len() > 0 && W[0] >= TEXT('0') && W[0] <= TEXT('9'); }

	// "12 000", "12.000", "12 ming" -> 12000; "1,5" -> 1.5; "bir yarim" -> 1.5
	struct FSon { float Qiymat; int32 Joy; };
	TArray<FSon> Sonlar(const TArray<FString>& T)
	{
		TArray<FSon> Out;
		for (int32 i = 0; i < T.Num(); ++i)
		{
			float V = -1.f;
			const int32 Boshi = i;
			if (Raqam(T[i]))
			{
				FString W = T[i];
				const int32 Nuqta = FMath::Max(W.Find(TEXT("."), ESearchCase::CaseSensitive, ESearchDir::FromEnd), W.Find(TEXT(","), ESearchCase::CaseSensitive, ESearchDir::FromEnd));
				if (Nuqta != INDEX_NONE && W.Len() - Nuqta - 1 == 3) W = W.Replace(TEXT("."), TEXT("")).Replace(TEXT(","), TEXT(""));
				V = FCString::Atof(*W.Replace(TEXT(","), TEXT(".")));
				while (i + 1 < T.Num() && T[i + 1].Len() == 3 && T[i + 1].IsNumeric()) V = V * 1000.f + FCString::Atof(*T[++i]);
			}
			else if (T[i] == TEXT("yarim")) V = 0.5f;
			else V = SozSon(T[i]);
			if (V < 0.f) continue;
			if (i + 1 < T.Num() && T[i + 1] == TEXT("yarim")) { V += 0.5f; ++i; }
			if (i + 1 < T.Num() && T[i + 1] == TEXT("ming")) { V *= 1000.f; ++i; }
			Out.Add({V, Boshi});
		}
		return Out;
	}

	const FString& Tanla(const TArray<FString>& V) { return V[FMath::RandRange(0, V.Num() - 1)]; }
}

FString BozorSotuv::Som(int64 N)
{
	FString S = FString::Printf(TEXT("%lld"), FMath::Abs(N));
	for (int32 i = S.Len() - 3; i > 0; i -= 3) S.InsertAt(i, TEXT(' '));
	return N < 0 ? TEXT("-") + S : S;
}

FString BozorSotuv::Miqdor(float M)
{
	FString S = FString::SanitizeFloat(FMath::RoundToFloat(M * 10.f) / 10.f);
	S.RemoveFromEnd(TEXT(".0"));
	return S.Replace(TEXT("."), TEXT(","));
}

// ================= tayyor javoblar

FBozorTushuncha UBozorTayyorJavob::Tushun(const FString& Gap, const FBozorSuhbatHolati& Holat)
{
	FBozorTushuncha T;
	const FString S = TEXT(" ") + Normal(Gap) + TEXT(" ");
	TArray<FString> Soz;
	S.ParseIntoArrayWS(Soz);
	const TArray<FSon> Son = Sonlar(Soz);

	T.bSalomBor = Bor(S, {TEXT("salom"), TEXT("assalom")});

	// miqdor: "kilo" so'zi va kichik son, pul: katta son
	float Kilo = -1.f, Pul = -1.f;
	const bool bKilo = Bor(S, {TEXT("kilo"), TEXT(" kg"), TEXT("dona"), TEXT("bog'")});
	for (const FSon& N : Son)
	{
		if (N.Qiymat >= 500.f) Pul = FMath::Max(Pul, N.Qiymat);
		else if (bKilo && Kilo < 0.f) Kilo = N.Qiymat;
	}
	if (bKilo && Kilo < 0.f && Bor(S, {TEXT(" yarim")})) Kilo = 0.5f;

	const bool bSavdo = Bor(S, {TEXT("arzon"), TEXT("tushir"), TEXT("kamaytir"), TEXT("qimmat"), TEXT("chegirma"), TEXT("kamroq"), TEXT("kelish")});
	const bool bNarx = Bor(S, {TEXT("necha"), TEXT("qancha"), TEXT("narx")});
	const bool bRad = Bor(S, {TEXT("kerak emas"), TEXT("olmayman"), TEXT("kerakmas")});
	const bool bXayr = Bor(S, {TEXT("rahmat"), TEXT("xayr"), TEXT("sog' bo'ling")});
	const bool bRozi = Bor(S, {TEXT("olaman"), TEXT("mayli"), TEXT("xo'p"), TEXT(" xop "), TEXT("bering"), TEXT("tortib"), TEXT("torting"), TEXT("olay"), TEXT("kerak")});

	if (Holat.Bosqich == EBozorBosqich::PulKutilmoqda && Pul > 0.f) { T.Harakat = EBozorHarakat::PulQabul; T.Qiymat = Pul; }
	else if (Kilo > 0.f) { T.Harakat = EBozorHarakat::Tortish; T.Qiymat = Kilo; }
	else if (bSavdo) T.Harakat = EBozorHarakat::Savdolashish;
	else if (bNarx) T.Harakat = EBozorHarakat::NarxAytish;
	else if (bRad || (bXayr && Holat.Bosqich != EBozorBosqich::PulKutilmoqda)) T.Harakat = EBozorHarakat::Xayr;
	else if (bRozi) T.Harakat = Holat.JoriyNarx > 0 ? EBozorHarakat::Tortish : EBozorHarakat::NarxAytish;
	else if (T.bSalomBor) T.Harakat = EBozorHarakat::Salom;
	return T;
}

FString UBozorTayyorJavob::Yoz(EBozorJavob Javob, const FBozorSuhbatHolati& H, int32 Qaytim)
{
	static const TMap<EBozorJavob, TArray<FString>> Matn = {
		{EBozorJavob::Salom, {TEXT("Va alaykum assalom, bolam! Keling, nima kerak?"), TEXT("Vaalaykum assalom! Keling, {mahsulot}larim yangi uzilgan.")}},
		{EBozorJavob::Narx, {TEXT("{mahsulot}ning {birlik}si {narx} so'm."), TEXT("{narx} so'mdan, bolam. Hammasi yangi.")}},
		{EBozorJavob::TushirdiMan, {TEXT("Mayli, siz uchun {narx} so'm qilaman."), TEXT("Xo'p, {narx} so'mdan bera qolay.")}},
		{EBozorJavob::TushirmaymanEngPast, {TEXT("Yo'q bolam, {narx} so'mdan arzon bo'lmaydi, o'zimga ham shunga tushgan."), TEXT("Bundan arzoni yo'q, rostini aytyapman. {narx} so'm.")}},
		{EBozorJavob::TushirmaymanHozircha, {TEXT("Narxi shu, bolam, juda yaxshi mol."), TEXT("{narx} so'm ham arzon-ku!")}},
		{EBozorJavob::MiqdorSora, {TEXT("Necha {birlik} tortay?"), TEXT("Qancha olasiz, bolam? Necha {birlik}?")}},
		{EBozorJavob::Tortdim, {TEXT("Mana, {miqdor} {birlik} {mahsulot}. Jami {summa} so'm bo'ldi."), TEXT("Tortdim: {miqdor} {birlik}. {summa} so'm bo'ladi, bolam.")}},
		{EBozorJavob::PulYetmaydi, {TEXT("Bu yetmaydi, bolam. {summa} so'm kerak, yana {son} so'm bering.")}},
		{EBozorJavob::HamyondaYoq, {TEXT("Hamyoningizda buncha pul yo'q-ku, bolam? Bor pulingizni sanab ko'ring.")}},
		{EBozorJavob::PulOldim, {TEXT("Rahmat! Mana qaytimingiz: {son} so'm. Osh bo'lsin!"), TEXT("Oldim. Qaytimi {son} so'm, sanab oling. Yana keling!")}},
		{EBozorJavob::Xayr, {TEXT("Yana keling, bolam!"), TEXT("Osh bo'lsin, yana keling!")}},
		{EBozorJavob::XayrSotibOlmay, {TEXT("Mayli, boshqa safar keling!"), TEXT("Xo'p, kerak bo'lsa qaytib keling.")}},
		{EBozorJavob::SalomBering, {TEXT("Avval salom bersangiz yaxshi bo'lardi, bolam."), TEXT("Salom-alik qilmaysizmi, bolam?")}},
		{EBozorJavob::Tushunmadim, {TEXT("Tushunmadim, bolam. {mahsulot} olasizmi?"), TEXT("Nima dedingiz? Narxini so'rang yoki necha {birlik} kerakligini ayting.")}},
		{EBozorJavob::Tugagan, {TEXT("Savdo bo'ldi, bolam. Yana kerak bo'lsa keling!")}},
	};

	FString Q = Javob == EBozorJavob::PulOldim && Qaytim == 0 ? FString(TEXT("Rahmat, bolam, to'ppa-to'g'ri! Osh bo'lsin!")) : Tanla(Matn[Javob]);
	FStringFormatNamedArguments A;
	A.Add(TEXT("mahsulot"), H.Mahsulot.Nomi);
	A.Add(TEXT("birlik"), H.Mahsulot.Birlik);
	A.Add(TEXT("narx"), BozorSotuv::Som(H.JoriyNarx > 0 ? H.JoriyNarx : H.Mahsulot.Narx));
	A.Add(TEXT("miqdor"), BozorSotuv::Miqdor(H.Miqdor));
	A.Add(TEXT("summa"), BozorSotuv::Som(H.Summa));
	A.Add(TEXT("son"), BozorSotuv::Som(Qaytim));
	Q = FString::Format(*Q, A);
	Q[0] = FChar::ToUpper(Q[0]);
	return Q;
}

// ================= suhbat: hisob-kitobni shu yerda kod qiladi

void UBozorSuhbat::Boshla(const FBozorMahsulot& Mahsulot, UBozorHamyon* InHamyon, UBozorSotuvchiMiya* InMiya)
{
	Holat = FBozorSuhbatHolati();
	Holat.Mahsulot = Mahsulot;
	Hamyon = InHamyon;
	Miya = InMiya;
}

FString UBozorSuhbat::Javob(EBozorJavob J, int32 Son)
{
	return Miya ? Miya->Yoz(J, Holat, Son) : FString();
}

FString UBozorSuhbat::Yoz(const FString& Gap)
{
	if (!Miya || !Hamyon) return FString();
	const FBozorTushuncha T = Miya->Tushun(Gap, Holat);
	if (Holat.Bosqich == EBozorBosqich::Tugadi)
		return Javob(T.Harakat == EBozorHarakat::Xayr ? EBozorJavob::Xayr : EBozorJavob::Tugagan);
	FString Oldi;
	if (!Holat.bSalomlashdi)
	{
		Holat.bSalomlashdi = true;
		// odob: salomsiz gap boshlansa, sotuvchi eslatadi
		Oldi = T.bSalomBor ? (T.Harakat == EBozorHarakat::Salom ? FString() : Javob(EBozorJavob::Salom)) : Javob(EBozorJavob::SalomBering);
	}
	if (Holat.JoriyNarx == 0 && T.Harakat != EBozorHarakat::Salom && T.Harakat != EBozorHarakat::Tushunmadi) Holat.JoriyNarx = Holat.Mahsulot.Narx;

	FString Asosiy;
	switch (T.Harakat)
	{
	case EBozorHarakat::Salom:
		Asosiy = Javob(EBozorJavob::Salom);
		break;

	case EBozorHarakat::NarxAytish:
		if (Holat.Bosqich == EBozorBosqich::Boshlanish) Holat.Bosqich = EBozorBosqich::NarxAytildi;
		Asosiy = Javob(EBozorJavob::Narx);
		break;

	case EBozorHarakat::Savdolashish:
	{
		++Holat.Savdolashuv;
		const FBozorMahsulot& M = Holat.Mahsulot;
		if (Holat.JoriyNarx <= M.EngPastNarx)
		{
			Holat.JoriyNarx = M.EngPastNarx;
			Asosiy = Javob(EBozorJavob::TushirmaymanEngPast);
		}
		else if (Holat.Savdolashuv == 1 && FMath::FRand() < 0.35f)
		{
			Asosiy = Javob(EBozorJavob::TushirmaymanHozircha);
		}
		else
		{
			Holat.JoriyNarx = FMath::Max(M.EngPastNarx, Holat.JoriyNarx - M.Qadam);
			Asosiy = Javob(EBozorJavob::TushirdiMan);
		}
		if (Holat.Bosqich == EBozorBosqich::Boshlanish) Holat.Bosqich = EBozorBosqich::NarxAytildi;
		// tortilgandan keyin tushirsa ham summa yangi narxdan
		if (Holat.Miqdor > 0.f) Holat.Summa = FMath::RoundToInt(Holat.JoriyNarx * Holat.Miqdor / 100.f) * 100;
		break;
	}

	case EBozorHarakat::Tortish:
		if (T.Qiymat <= 0.f)
		{
			if (Holat.Bosqich == EBozorBosqich::Boshlanish) Holat.Bosqich = EBozorBosqich::NarxAytildi;
			Asosiy = Javob(EBozorJavob::MiqdorSora);
			break;
		}
		Holat.Miqdor = FMath::Clamp(T.Qiymat, 0.5f, 20.f);
		Holat.Summa = FMath::RoundToInt(Holat.JoriyNarx * Holat.Miqdor / 100.f) * 100;
		Holat.Bosqich = EBozorBosqich::PulKutilmoqda;
		Asosiy = Javob(EBozorJavob::Tortdim);
		break;

	case EBozorHarakat::PulQabul:
	{
		const int32 Berildi = FMath::RoundToInt(T.Qiymat);
		if (Berildi > Hamyon->Pul)
		{
			Asosiy = Javob(EBozorJavob::HamyondaYoq);
		}
		else if (Berildi < Holat.Summa)
		{
			Asosiy = Javob(EBozorJavob::PulYetmaydi, Holat.Summa - Berildi);
		}
		else
		{
			const int32 Qaytim = Berildi - Holat.Summa;
			Hamyon->Pul -= Holat.Summa;
			Hamyon->Savat.Add(FString::Printf(TEXT("%s %s %s"), *BozorSotuv::Miqdor(Holat.Miqdor), *Holat.Mahsulot.Birlik, *Holat.Mahsulot.Nomi));
			Holat.Bosqich = EBozorBosqich::Tugadi;
			Asosiy = Javob(EBozorJavob::PulOldim, Qaytim);
		}
		break;
	}

	case EBozorHarakat::Xayr:
		Asosiy = Javob(Holat.Bosqich == EBozorBosqich::Tugadi ? EBozorJavob::Xayr : EBozorJavob::XayrSotibOlmay);
		Holat.Bosqich = EBozorBosqich::Tugadi;
		break;

	default:
		Asosiy = Javob(EBozorJavob::Tushunmadim);
		break;
	}

	return Oldi.IsEmpty() ? Asosiy : Oldi + TEXT(" ") + Asosiy;
}
