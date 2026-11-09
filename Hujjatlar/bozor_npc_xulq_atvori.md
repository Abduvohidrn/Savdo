# Bozorda odamlar qanday harakatlanadi va savdo qanday bo'ladi (2026-10-19)

NPC'larni tuzatish uchun asos. Avval tadqiqot natijalari, keyin ulardan chiqqan NPC qoidalari, oxirida hozirgi sahnadagi xatolar.

---

## 1. Tadqiqot natijalari

### 1.1 Yurish tezligi
| Kim | Tezlik | Manba |
|---|---|---|
| Ishga shoshilayotgan yo'lovchi | 1.4–1.5 m/s | Tokio va Malayziya kuzatuvlari |
| Savdo qiluvchi (mall) | ayol 0.81, erkak 0.93 m/s | Kuala-Lumpur savdo markazlari |
| Guruh a'zosi | yolg'iz yuruvchidan **37% sekinroq** | Milan, Vittorio Emanuele galereyasi |
| Do'kon yonidan o'tayotgan, qarab ketayotganlar | oqim ichida sekinlashadi | metro do'koni tadqiqoti (arXiv) |

**Xulosa:** bozorda odamlar sekin yuradi, taxminan **0.7–1.1 m/s**. Faqat hammol (aravakash) va bozorni kesib o'tayotganlar 1.3–1.5 m/s tezlikda yuradi.

### 1.2 Guruhlar
- Savdo ko'chasida (Crociani, 2013): **16% yolg'iz, 31% juft, 18% uchtalik, 36% to'rt va undan ko'p**.
- Dam olish va savdo joylarida guruhda yuruvchilar 55–70% ni tashkil qiladi. Dam olish kunlari 70% gacha.
- Kichik guruhlar har doim ko'proq uchraydi.
- **Xulosa:** bozordagi odamlarning yarmidan ko'pi guruhda yuradi (er-xotin, ona-bola, oila, do'stlar). Hozir hamma NPC yolg'iz yuradi, bu eng katta "sun'iylik" belgisi.

### 1.3 Shaxsiy masofa (proksemika, Hall)
- Yaqin masofa < 0.46 m (faqat oila va guruh ichida)
- Shaxsiy masofa 0.46–1.2 m
- Ijtimoiy masofa 1.2–3.7 m (sotuvchi bilan gaplashish masofasi)
- Odamlar tor yo'lakda va gavjum joyda kamroq masofaga ko'nikadi.
- **Xulosa:** begonalar orasida kamida ~0.5 m, guruh ichida 0.3–0.5 m, navbatda 0.6–0.8 m masofa bo'lishi kerak. Sotuvchi bilan xaridor orasida esa peshtaxta turadi, ya'ni ~0.8–1.2 m.

### 1.4 O'zbek bozorida savdo jarayoni (Chorsu, Alay va boshqalar)
Manbalar savdolashish odat ekanini bir ovozdan aytadi: so'ralgan narxni savdolashmasdan to'lash kamdan-kam uchraydi. Sotuvchilar tatib ko'rishni taklif qiladi. Tajribali xaridorlar avval hamma qatorlarni aylanib chiqadi, keyin sotib oladi. Savdo bilan birga gap-so'z ham bo'ladi: salomlashish, yangiliklar.

**Bitta xariddagi qadamlar:**
1. **Ko'z tashlash:** xaridor yo'lak bo'ylab sekin yuradi va rastalarga qarab o'tadi. Ko'pincha to'xtamaydi.
2. **Chaqiriq:** sotuvchi yaqindan o'tayotganni chaqiradi: *"Keling, arzoni bor!"*, qo'l siltaydi.
3. **To'xtash va ko'rish:** xaridor rasta oldiga keladi, molni ushlab ko'radi, tanlaydi.
4. **Narx so'rash:** *"Necha pul?" / "Qanchadan?"*
5. **Savdolashish:** *"Juda qimmat ekan!" → "Tushirib bering" → "Narxi shu" → "Yana qo'shing"*. Tomonlar bir necha marta gap almashadi.
6. **Tatib ko'rish:** sotuvchi *"Yeb ko'ring"* deb meva yoki quruq mevadan uzatadi.
7. **Tortish:** *"Tortib bering"*. Sotuvchi tarozida tortadi va paketga soladi.
8. **To'lov:** naqd pul (mayda pul kerak bo'ladi) yoki telefon orqali.
9. **Xayrlashuv:** *"Rahmat! Bozoringizni bersin!" – "Yana keling!"*
10. Xaridor qo'lida paket bilan keyingi rastaga o'tadi.

**Bozordagi boshqa rollar:**
- **Hammol (aravakash):** arava bilan yuk tashiydi va tez yuradi. Odamlar unga yo'l beradi. Aravani bozor ma'muriyatidan ijaraga oladi.
- **Aylanib sotuvchi:** choy, suv yoki non olib, qatorlar orasida aylanib yuradi.
- **Bo'sh turgan sotuvchi:** stulda o'tiradi, telefoniga qaraydi, qo'shnisi bilan gaplashadi, molini to'g'rilaydi, ko'katga suv sepadi.

### 1.5 O'yinlardagi tajriba (NPC animatsiyasi)
- Har bir NPC turida kamida **3–5 xil idle**, yaxshisi 5–8 xil bo'lishi kerak. NPC ekranda vaqtining ko'p qismini idle holatida o'tkazadi.
- Har NPC'ga **tasodifiy faza** beriladi, aks holda qadamlar va harakatlar sinxron bo'lib qoladi.
- Fon NPC'si uchun minimal to'plam: idle, yurish, gaplashish, atrofga qarash, narsa ko'tarib yurish, yaqindagi voqeaga reaksiya.
- Uzoqdagi NPC'larga soddalashtirilgan animatsiya va kamroq yangilanish (LOD) beriladi.

---

## 2. NPC uchun qoidalar (tuzatishda shunga amal qilamiz)

### Rollar va ulushlar (taxminan 90–100 kishi)
| Rol | Soni | Xulq |
|---|---|---|
| Sotuvchi | har rastada 1 ta; ~20% rastada 2 ta (er-xotin yoki ota-o'g'il) | peshtaxta **ortida**, yo'lakka qarab turadi |
| Xaridor (yolg'iz) | ~25% | rastalarni aylanadi va xarid qiladi |
| Xaridor guruhi (2–4) | ~45% | birga yuradi va birga to'xtaydi; savdolashadigani bitta, qolganlari kutib turadi |
| Yo'lovchi | ~15% | bozorni kesib o'tadi, tezroq yuradi |
| Hammol (aravakash) | 2–3 ta | tez yuradi, odamlar unga yo'l beradi |
| Gaplashib turganlar | 3–4 ta guruh | yo'lak chetida, o'tishga xalaqit bermasdan turadi |

### Tezliklar
- Xaridor: 0.7–1.0 m/s. Guruh eng sekin a'zosiga moslashadi.
- Yo'lovchi: 1.1–1.4 m/s. Hammol: 1.3–1.5 m/s.
- **Yurish animatsiyasining tezligi agent tezligiga mos bo'lishi shart**, aks holda oyoq sirpanadi. Animatsiya klipining asl tezligini o'lchab, Animator tezligini `agent.velocity / klip_tezligi` qilib beramiz.

### Xaridor sikli
1. Yo'lak bo'ylab sekin yuradi va rastalarga boshini burib qaraydi. 3–4 ta rastadan 1 tasida to'xtaydi.
2. To'xtaganda peshtaxtadan 0.5–0.7 m narida, **yuzi rastaga qaragan** holda turadi.
3. Mol ko'radi: 4–10 s.
4. Sotuvchi bo'sh bo'lsa savdolashadi: navbatma-navbat gapiradi, jami 15–40 s. Band bo'lsa yonida 0.7 m masofada kutadi.
5. Ba'zan tatib ko'radi va tortdiradi: sotuvchi tarozi tomonga buriladi.
6. Pul beradi, xayrlashadi va keyingi rastaga o'tadi. Taxminan 60% xarid qiladi, 40% "qimmat ekan" deb ketadi.
7. Xariddan keyin qo'lida paket bilan yuradi (keyingi bosqichda).

### Sotuvchi sikli
- **Bo'sh paytida** quyidagilardan birini tasodifiy qiladi: tik turadi, peshtaxtaga suyanadi, stulda o'tiradi, telefon ko'radi, molni to'g'rilaydi, qo'shnisi bilan gaplashadi.
- **Kimdir 3 m ichidan o'tsa** 30% ehtimol bilan chaqiradi: qo'l siltaydi, molni ko'rsatadi.
- **Xaridor kelsa** unga qaraydi va savdolashadi: gapiradi, bosh irg'aydi, mol ko'rsatadi, tortadi, uzatadi.
- Joyidan **hech qachon** peshtaxta oldiga chiqmaydi.

### Joylashuv qoidalari
- Sotuvchi faqat peshtaxta ortidagi "sotuvchi zonasida" turadi. Joy rasta modelining haqiqiy o'lchamidan (bounds) hisoblanadi, taxminan emas.
- Xaridor turadigan joy peshtaxta oldi chizig'idan 0.5–0.7 m oldinda bo'ladi va yerdagi qoplarga to'g'ri kelmasligi kerak.
- Qoplar, yashiklar va stullar NavMesh'da to'siq bo'ladi, odamlar ular ustidan yurmaydi.

### Ko'rinish
- Maneken bir xil rangda va yalang'och ko'rinmasligi kerak. Tanaga kiyim rangi, qo'l va boshga teri rangi beriladi.
- Bo'y: erkaklar 1.68–1.85 m, ayollar 1.55–1.70 m. Bolalar uchun alohida model kerak (keyinroq).
- Yaqindagi NPC'lar bir xil animatsiyani bir vaqtda o'ynamasligi kerak.

---

## 3. Hozirgi sahnadagi xatolar (kadrlar va kod tahlili)

| # | Xato | Sababi | Tuzatish |
|---|---|---|---|
| 1 | Oyoq sirpanadi, yurish juda tez yoki sekin | Agent tezligi 0.95–1.35, animatsiya esa doim bir xil tezlikda | Klip tezligini o'lchab, Animator tezligini agent tezligiga bog'lash; xaridor tezligini 0.7–1.0 ga tushirish |
| 2 | Odamlar bir-biriga urilib o'tadi | Agent radiusi 0.28 va `LowQuality` avoidance; sotuvchi va suhbatdoshlarda agent yo'q | Radius 0.35, `HighQuality` avoidance; turgan NPC'larga `NavMeshObstacle` |
| 3 | Sotuvchi ba'zan rasta oldida yoki ichida turadi | Joy qo'lda yozilgan raqamlardan (`frontX ± 1.3`) hisoblangan, rasta modeli va sotuvchi buyumlarini hisobga olmaydi | Sotuvchi joyini sahnadagi haqiqiy peshtaxta bounds'idan va bo'sh joy tekshiruvidan topish |
| 4 | Sotuvchi juda ko'p (33 ta), harakatlari bir xil | Har 4 m modulga bitta sotuvchi; idle 6 ta, chaqiriq 30% | Rastalarni haqiqiy peshtaxtalar bo'yicha sanash; bo'sh paytdagi harakatlarni ko'paytirish (suyanish, o'tirish, telefon, molni to'g'rilash) |
| 5 | Xaridor qoplar ustida yoki rastaga yon turadi | Joy yerdagi qoplarga to'g'ri keladi; burilish `updateRotation` bilan ziddiyatga kiradi | Qoplarni hisobga olib joy tanlash; to'xtaganda agentni o'chirib, aniq burilish |
| 6 | Xaridor juda qisqa yoki juda uzoq turadi | Ko'rish 3–6 s, gaplashish 5–10 s, savdo bosqichlari yo'q | Bosqichli savdo sikli (1.4 va 2-bo'limdagi vaqtlar bilan) |
| 7 | Hamma yolg'iz yuradi | Guruh tizimi yo'q | Juft, uchtalik va oila guruhlari |
| 8 | Manekenlar yalang'och va teri rangida ko'rinadi | Palitra 2-materialga teri rangini beryapti; Manny material slotlari qaysi qismga to'g'ri kelishi hali tekshirilmagan | Har bir material slotini tekshirib, tanaga kiyim rangi, qo'l-boshga teri rangi berish |
| 9 | Animatsiyalar sinxron | Hamma bir vaqtda boshlaydi | Tasodifiy faza (`Play(state, 0, Random.value)`) |

---

## 4. Kod tahlili va tuzatishlar (2026-10-21)

| Kim | Xato | Tuzatish |
|---|---|---|
| Xaridor, hammol | Paket va arava sahnaga ulanmagan edi (prefab yo'li `Prefabs/` deb yozilgan, fayllar `Prefabs/07_Jihoz/` ichida) | Builder prefabni `Prefabs/` ichidan nomi bo'yicha qidiradi |
| Hamroh | Yurish sikli har 0.25 s da boshidan boshlanardi (`CrossFade` o'sha holatga ham uni qayta boshlaydi) | `Play` allaqachon o'ynayotgan holatni qayta boshlamaydi |
| Hamroh | Guruh qator bo'lib yurardi: steering manzilga 1.2 m qolganda sekinlashadi, hamroh esa boshliq tezligida edi | Tezlik = boshliq tezligi + orqada qolgan masofa; hamrohda `arriveDistance = 0` |
| Hamroh | Navbat nuqtalarida turib qolardi (boshliq yonida 0.65 m, navbat ham shu chiziqda 0.7 m) | Boshliq orqasida, navbat nuqtalaridan 0.6 m uzoq joyda kutadi (`BozorQueue.NearAnyPoint`) |
| Hamroh | Rastada bitta harakatni 20–60 s qilardi | Har 4–9 s da yangi `F_Idle` |
| Hamma | Steering har yoqilganda throttle 0 dan boshlanardi (to'xtab-yurish) | Agentning hozirgi tezligidan davom etadi |
| Hamma | Oxirgi 0.5 m da burilish yo'q edi (steering `updateRotation` ni qaytarmasdi) | Boshqaruv agentga qaytganda burilish ham qaytadi |
| Hamma | Loop bo'lmagan klip tugagach oxirgi kadrda qotib qolardi (salom, bosh irg'ash, supurish oxiri) | Klip tugashidan 0.25 s oldin tik turishga (o'tirgan bo'lsa - o'tirish sikliga) qaytadi |
| Sotuvchi | Chaqiriq bir martadan keyin to'xtardi: agenti yo'qligi uchun `callCooldown` kamaymasdi | Hisoblagich `Update` boshida |
| Sotuvchi | Xaridor salomlashtirgach, sotuvchi eski taymeri tugaguncha qotib turardi | `Greet()`: salomdan keyin yangi harakat boshlaydi |
| Sotuvchi | Tinglayotganda "yo'q" deb bosh chayqardi (`HeadNod_No`), supurishning oxirgi 1 s i (`Broom_Sweep_End`) o'ynardi | `HeadNod_Yes`, `Broom_Sweep_Loop` |
| Sotuvchi | Ta'zim 1.2 s da kesilardi | `EndDeal(busy)` |
| Sotuvchi | Ikkinchi sotuvchi hech qachon paydo bo'lmasdi (shart > 1.1 m, nomzodlar orasidagi eng katta masofa 1 m), paydo bo'lganda ham joyga bog'lanmasdi | Chegara 0.9 m; ikkinchi sotuvchi o'ziga yaqin joydagi xaridorlar bilan savdo qiladi |
| Xaridor | Xarid ulushi 48% edi (80% × 60%) | 80% × 75% = ~60% |
| Yo'lovchi | O'rindiqqa yetmasa ham sakrab o'tirardi; o'tirish/turishda 0.33 m sakrash; o'tirish 2 kadrlik qotgan poza edi | Yetib kelganini tekshiradi; tana klip davomida suriladi; `Customer_Sit_Chair_Idle`, choy ichish (`Drink_Cup`) |
| Yo'lovchi | Optimallashtiruvchi o'tirgan odamni darvozaga ko'chirardi | O'tirganlar ko'chirilmaydi |
| Hammol | Arava world AABB bo'yicha burilardi (~50% ko'ndalang); radius 0.55 NavMesh'dan (0.3) katta | O'lcham hammolning lokal o'qlarida; radius 0.3; arava steering'da to'siq |
| Suhbatdosh | Ikkalasi bir vaqtda gapirardi | Bitta gapiradi, qolganlari tinglaydi, gap almashadi |
| Uzoqdagilar | LOD2'da steering va avoidance o'chiq edi, odamlar bir-biridan o'tib ketardi | LOD2: steering har 6 kadrda, oddiy avoidance |

Tekshiruv: `BozorKit/NPC/Harakatni yozib olish` (`BozorNpcCapture`) oxirida `[BozorNpc] Tekshiruv` qatorini yozadi.

## 5. Har bir NPC'ni parallel kuzatish (2026-10-21)

`BozorKit/NPC/Har bir NPC'ni kuzatish (10 s)` (`BozorNpcKuzatuv`) Play'da 100 ta NPC'ning hammasini bir vaqtda 10 s kuzatadi. Har kadrda joyi, tezligi, agenti, animatsiyasi va eng yaqin odami yoziladi, xatolar avtomatik aniqlanadi. Har bir NPC'ga ergashadigan kamera soniyasiga bitta kadr oladi. Hammollar qo'shimcha ravishda tepadan soniyasiga 4 marta suratga olinadi (`NNN_tKK.png`, tomlar kesiladi). Natijalar `Logs/NpcKuzatuv/` papkasida: `xulosa.txt`, `npc.csv`, `kadr/`. Editorda ishga tushirilsa, Unity oynasi fonda bo'lsa ham o'yin to'xtamaydi ("Run In Background" vaqtincha yoqiladi).

Batch rejimda ishga tushirish: `Unity.exe -batchmode -projectPath ... -executeMethod BozorNpcKuzatuv.Run -kuzatishBoshi 20 -kuzatishSekund 10`

Birinchi kuzatuvda 102 tadan 20 ta NPC'da xato topildi: ba'zilari 10 s tiqilib qolgan, biri itarib ketilgan, biri yopiq kioskda turgan, ikkitasi qop ichida turgan. Oxirgi kuzatuvlarda (20–30, 50–60, 80–90 s) har oynada 100 tadan 5–10 ta NPC'da faqat qisqa holatlar qoldi:
- 1 s gacha yonlama yurish;
- aravaning 0.7–2.6 s tegib o'tishi;
- qo'shni ikki odam bir xil klipni 0.1 s ichida boshlab qolgan tasodifiy holatlar.

| Kim | Xato (kuzatuvda) | Sababi | Tuzatish |
|---|---|---|---|
| Xaridor | Savdolashayotgan xaridorni o'tib ketayotgan yo'lovchi 2 m surib ketdi | `StopMoving`da `ResetPath()` `isStopped`ni qaytarib o'chirardi. Turgan agent esa boshqalardan qochib, ular bilan birga siljirdi | Avval `ResetPath`, keyin `isStopped`. Turgan odamning ustuvorligi 0 |
| Yo'lovchi | Ochiq maydondagi sotuvchiga 10 s tiralib turdi | Sotuvchi bilan uning moli orasi NavMesh'da ochiq edi, to'siq carving qilmasdi | `PointStall` zonasi (`ModifierBox`), turgan odam to'sig'i carving qiladi |
| Hamma | Tik turgan odamlar orasidan o'tolmay, oldinga-orqaga yurdi yoki tiqilib qoldi | Yo'l izlash agentlarni bilmaydi: yo'l tik turgan odam ichidan chizilardi | Uzoq turganlar (navbat, rasta oldi, hamroh, o'rindiq, pauza) to'siqqa aylanadi: agent o'chadi, carving `NavMeshObstacle` yoqiladi (`Freeze`/`Unfreeze`) |
| Hamroh | Orqasi bilan yurdi (112°) | Steering tanani ko'zlangan yo'nalishga burardi, avoidance esa uni boshqa tomonga surardi | Tana haqiqiy (silliqlangan) siljish tomoniga buriladi. Tiqilganda yoki 70° dan ko'p og'ganda boshqaruv 2 s agentga beriladi |
| Sotuvchi | Yopiq kiosk ichida ko'rinmay turardi, xaridorlar u yerga navbatga turardi | `PointStall` joyi qo'lda yozilgan, kiosk tekshirilmagan | Nomida "Yopiq" bo'lgan buyum ichiga sotuvchi qo'yilmaydi |
| Sotuvchi | Qop yoki yashik ichida turardi | Bo'sh joy topilmasa, taxminiy nuqta tekshirilmasdan olinardi | Kengroq qidiruv, topilmasa past buyumlardan eng uzoq nuqta |
| Hamma | Oyoq sirpandi (0.3–0.9 m/s) | Erkaklar klipi ~1.4 m/s, ijro 0.6x dan sekinlashmasdi. Steering esa 0.25x gacha sekinlashardi | Animatsiya haqiqiy siljishdan hisoblanadi. `MinWalkSpeed`/`MaxWalkSpeed` (klip 0.5x..1.3x), steering undan sekinlashmaydi |
| Hamroh | Ayol hamroh erkak boshliqqa yetolmay 3 m orqada qoldi | Ayol klipi 1.1 m/s dan tez ko'rsatolmaydi | Guruh eng sekin a'zosiga moslashadi |
| Xaridor | Ikki rasta joyi orasidagi navbat nuqtasida oldinga-orqaga yurdi | Navbat nuqtasi qo'shni joydan 0.7 m da edi | Navbat nuqtasi boshqa joydan kamida 1 m uzoqda |
| Hammol | Aravasi qop, rasta va odamlarga kirdi, hammol aravani ushlamasdi | Arava hammolga qattiq bog'langan edi. Model 2.0 x 1.06 m. Steering faqat konus bo'yicha tekshirardi | Arava 1.5 m gacha kichraytirildi, qo'llar IK bilan dastani ushlaydi (pastdagi qatorga qarang) |
| Hammol | Aravani g'ildirakli uchidan ushlab yurardi; burilishda arava hammol atrofida og'irligi yo'qdek aylanardi (foydalanuvchi skrinshoti) | Model pivoti g'ildirakli uchida, kod esa pivot tomonini hammolga qo'yardi. Arava hammolga qattiq bog'langan, o'qi `Slerp` bilan burilardi | Dasta hammolga, g'ildiraklar oldinga qaraydi. NavMesh agenti endi aravaning g'ildirak o'qi: u yo'l bo'ylab oldinda yuradi, hammol esa dastadan ushlab tirkama kabi orqasidan ergashadi. G'ildirak yonga sirpanmaydi: burilishda arava o'qi atrofida buriladi va hammol tashqariga suriladi. Keskin burilishda arava sekinlashadi, lekin yurish klipi ko'rsata oladigan tezlikdan sekin emas. Dasta qo'l balandligiga ko'tarilib, arava g'ildiraklari ustida egiladi (tirgovichi yerdan uziladi). Hammolning steering'i passiv: yo'nalishni arava beradi |
| Hammol | Manzil orqada qolsa 3–5 s orqasi bilan yurib, aravani tortardi. Aravani aylantira olmay 10 s qotib turdi | G'ildirak o'qi (agent) orqaga yurardi. Aylantirish yo'lida rasta bo'lsa, NavMesh hammolni qaytarib turardi | Manzil orqada bo'lsa, agent kutadi va hammol aravani g'ildiraklari atrofida 35°/s tezlikda aylantiradi. 1.5 s siljimasa teskari tomonga aylantiradi, u ham bo'lmasa boshqa manzil tanlaydi. Arava baribir orqaga yursa, hammol o'girilib uni orqasidan tortadi. Yangi manzil iloji boricha oldinda tanlanadi |
| Suhbatdosh | Tinglovchilar bir xil klipni bir kadrda boshlab, bir xil qimirlardi | Navbatli gapda hamma bir vaqtda `C_Listen` qilardi | Tinglovchilar 0–0.8 s kechikib boshlaydi. Loop bo'lmagan klip ham biroz tasodifiy joydan boshlanadi |
| Hamroh | Kutish joyiga yetolmay, 8 s `Move` holatida teskari qarab qotdi; to'xtashi bilan joyi o'zgarib, tebrandi | Yetib bo'lmaydigan joy. Kutish joyi har tikda qayta hisoblanar, unga hamrohning o'z teshigi ta'sir qilardi | Kutish joyi boshliq to'xtaganda bir marta tanlanadi. 1.5 s yaqinlashmasa, turgan joyida kutadi |
| Hamma | `WalkTo` tiqilganda 90 s kutardi; yo'lovchi yonidagi nuqtani manzil qilib tanlardi | | 3 s yaqinlashmasa voz kechadi; manzil kamida 3 m narida |
| Hamroh | Yo'nalishi mayda chayqalardi, steering soniyasiga 1–2 marta yoqilib-o'chardi | Har 0.1 s dagi `SetDestination`dan keyin yo'l hisoblanayotgan kadrda steering o'chib, burilishni agent olardi | Yo'l qayta hisoblanayotganda steering eski yo'l bilan davom etadi |

Qolgan, tuzatilmagan holatlar:
- Hammol aravasi gavjum joyda ba'zan odamga yoki qopga 0.5–2 s tegadi. Aravani itarish animatsiyasi yo'q, faqat qo'llar IK bilan qo'yilgan, oyoqlar oddiy yurish klipida.
- Aravaning geometriyasi (g'ildirak o'qi, dasta) `SM_Aravakash_Arava` modelidan Blender'da o'lchangan va `BozorNpc.AttachCart`ga ulush (%) sifatida yozilgan. Model o'zgarsa, bu qiymatlar ham yangilanishi kerak.
- Hamroh boshlig'i yonidan joy tanlab aylanayotganda 1 s ga yonlama yuradi.
- Yo'lovchi kameradan uzoqda va ko'rinmay qolsa, optimallashtiruvchi uni darvozaga ko'chiradi. Kuzatuv buni "sakradi" deb yozadi, lekin bu ataylab qilingan.

## 6. Unreal sahnasi (2026-10-22)

Avto-sinov (`Bozor_Oyin` + `-BozorAutoTest`) 55 ta NPC'dan **birortasi ham yurmayotganini** ko'rsatdi: harakatda=0, navbatda=0, savdo=0.

| Xato | Sababi | Tuzatish |
|---|---|---|
| Hamma NPC joyida qotib turibdi | `Bozor.umap` bo'sh NavMesh bilan saqlangan. `RuntimeGeneration=Dynamic` rejimida o'yin uni o'zi qayta qurmaydi | NavMesh qurilib, xarita bilan saqlandi. `ABozorSavdo::EnsureNavMesh` NavMesh bo'sh bo'lsa, o'yin boshida uni quradi. `build_level.py` NPC'lardan keyin ham NavMesh quradi |
| Arava ustida turgan odam | NavMesh buyumlar ustida ham bor, odam o'sha joyda paydo bo'lgan | `SnapToGround`: oyog'i 30 sm dan baland bo'lsa, pastdagi yo'lakka tushiriladi |
| Civilian yo'lovchilari faqat nuqtadan nuqtaga yuradi, rastaga qaramaydi | Paketda xarid qilish yo'q | Guruh boshliqlari va yolg'izlarning ~30% i xaridor: 3–4 rastadan birida to'xtaydi, 55% holatda savdolashadi (60% sotib oladi), qolganida mol ko'radi. Hamrohlar yonida kutadi (`C_Wait`) |
| Hamma telefon, SMS yoki sigaret bilan yuradi | Paket har 0.2 s qur'a tashlaydi | Xaridorda o'chirilgan, yo'lovchi va hamrohda 1–2,5 daqiqada bir marta |
| Yurish juda tez | Civilian 135/160 sm/s, navbat xaridori 200 sm/s | Xaridor 80–100, yo'lovchi 115–135, navbat xaridori 85–110 sm/s. Hamroh boshliq tezligiga moslashadi |
| Sotuvchi faqat idle o'ynaydi | `V_Call`, `V_Busy`, `V_Lean` ishlatilmagan edi | 3 m ichidan o'tayotganni 30% ehtimol bilan chaqiradi va unga qaraydi. ~20 s da bir marta molni ko'radi, cho'ziladi yoki peshtaxtaga suyanadi. Telefon va supurgi kliplari chiqarib tashlangan, chunki qo'lda buyum yo'q |
| Xaridor doim o'sha rastaga qaytadi | Paket menejeri eng yaqin navbatni tanlaydi | `AssignQueues`: 3–25 m ichidagi 6 ta eng yaqin bo'sh navbatdan tasodifiysi tanlanadi, oxirgi navbat takrorlanmaydi |
| Navbat oldida savdo kam boshlanadi | `StartOnly` rejimida yolg'iz xaridor navbat oxirida, peshtaxtadan ~2 m narida turadi, paket uni oldinga siljitmaydi | Oldinda hech kim bo'lmasa, `ABozorSavdo` uni AI orqali peshtaxta oldiga olib boradi. `NewEnumerator0` rejimi sinab ko'rildi va yaroqsiz chiqdi: oldindan joylashtirilgan xaridorlar ikki navbatga yozilib, qotib qoldi. Xaritada `StartOnly` qoldirildi |

Avto-sinov natijasi (74 s): oldin harakatda=0, savdo=0. Hozir harakatda ~30, navbatda 15 xaridor, 21 ta savdo (10 tasi navbatda, 11 tasi navbatsiz), 15 ta xarid. Band navbatlarning 8/10 tasida xaridor peshtaxta oldida turibdi.

## 7. Unreal: NPC xulqi Unity bilan bir xil (2026-10-22)

Paketlarning AI'si (Civilian, NPC Queue, Natural Movement, 100 Idle) va ularni bog'lovchi `ABozorSavdo` o'rniga endi `BozorNpc.cs` ning C++ ko'chirmasi ishlaydi: `Source/BozorUnreal/BozorNpc.h/.cpp`.
- `ABozorNpc` (Character, DetourCrowd AI) – 6 ta rol, Unity korutinlari C++20 korutinlari sifatida, mantiq qatorma-qator bir xil: `VendorLoop`, `CustomerLoop` + `DealWith`, `FollowerLoop` + `WaitSpot`, `WalkerLoop` + `SitDown`, `TalkerLoop`, `WalkTo` (3 s tiqilsa voz kechadi).
- `ABozorNpcManager` – Unity `BozorNpcManager` + `BozorQueue` (navbat tanlash, mashhurlik, sabr) + `BozorCrowdAI` (o'rindiqlar). Harakatlar, navbat nuqtalari, o'rindiqlar `scene.json` dan.
- Animatsiya: `ABP_Manny` (yurish/tik turish) + `DefaultSlot` montajida Unity kategoriyalari (V_Idle, C_Talk, ...), tasodifiy faza bilan.
- Xaritaga joylash: `Scripts/rebuild_npcs.py` (faqat NPC'larni almashtiradi) yoki `build_level.py` (`bozor_npc.py` orqali).

Avto-sinov (74 s): 100 NPC, 39 ta savdo, 15 ta xarid, 27 xaridor navbatda, o'rindiqqa 16 marta borildi, 80–93 fps.

Hammol aravasi (2026-10-23) ham Unity'dagidek: kapsula (AI agenti) - aravaning g'ildirak o'qi, hammol tanasi (mesh) dastadan ushlab tirkama kabi orqasidan ergashadi (`UpdateCart` - Unity `LateUpdate`). Manzil orqada bo'lsa agent to'xtaydi (`PauseMove`) va arava g'ildiraklari atrofida 35°/s aylantiriladi, 1.5 s siljimasa teskari tomonga, u ham bo'lmasa manzildan voz kechadi; keskin burilishda sekinlashadi. Yo'lning keyingi burilish nuqtasi yo'l nuqtalaridan olinadi (crowd rejimida PathFollowing faqat oxirgi nuqtani beradi). Qo'llar dastani two-bone IK bilan ushlaydi: `UBozorNpcMesh::FinalizeBoneTransform` (Unity `OnAnimatorIK`). Avto-sinov: ikkala hammol aravaga qarab yuradi (0.94–0.99), 78 s da 4 marta arava burildi, orqaga yurish 6280 kadrdan 15 tasi.

Unity'dan farqi (hali ko'chirilmagan): boshqalar aravani emas, faqat g'ildirak o'qini (agentni) aylanib o'tadi; uzoq turganlar carving to'siqqa aylanmaydi (DetourCrowd ularni aylanib o'tadi); sotuvchi ovozlari (`BozorSotuvchiOvozi`) yo'q; yurish klipi tezligi agentga moslanmaydi (`ABP_Manny` blendspace).

## 8. Unreal: har bir NPC'ni kuzatish (2026-11-09)

Simulate, 180 s, ~35 fps, 100 NPC (36 sotuvchi, 28 xaridor, 15 hamroh, 10 yo'lovchi, 9 suhbatdosh, 2 hammol). Har 0.25 s da joy, tezlik, burilish, oyoq-yer, AI holati yozildi; keyin 10 s har kadrda oyoq sirpanishi o'lchandi. Skriptlar sessiya scratchpad'ida edi (`npc_rec.py`, `npc_tahlil.py`, `npc_oyoq.py`).

Muhim: muharrir fonda bo'lsa Simulate 3 fps ga tushadi (`bThrottleCPUWhenNotForeground`) - kuzatishdan oldin o'chirish kerak, aks holda natija soxta.

**Xatosiz:** odamlar bir-biriga kirmaydi (<50 sm yo'q), sakrash yo'q, yiqilish yo'q, oyoq yerda (+3..6 sm), sotuvchi va suhbatdoshlar joyidan siljimaydi, montaj o'ynab turib yurish yo'q, teskari/yonlama yurish deyarli yo'q.

| # | Kim | Xato (kuzatuvda) | Sababi (kod) |
|---|---|---|---|
| 1 | Yo'lovchi, hammol, xaridor | Tik turgan odamga (navbat, rasta oldi, guruh) tiralib, MOVING holatida tezlik 0 bilan 3-10 s turadi. Yolovchi_06 10 s, Yolovchi_01 9 s, Hammol_11 3 marta (13 s) | Yo'l tik turganlarni bilmaydi (Unity'dagi carving to'siq ko'chirilmagan). `WalkTo` 3 s dan keyin voz kechadi, lekin yangi manzil o'sha tomonda bo'lsa yana tiraladi |
| 2 | Hamroh | Boshliq ketib qoladi, hamroh MOVING/tezlik 0 da 9 s qotadi: Guruh03_Hamroh1 boshliqdan 8.4 m uzoqlashdi | `FollowerLoop`da boshliq yurayotganda tiqilish tekshiruvi yo'q (`StuckUntil` faqat boshliq turganda) |
| 3 | Hamroh | Boshlig'iga tiralib (67 sm - kapsulalar tegib turibdi), joyida aylanadi: 2 s ichida 160° burilish (Guruh04, 08, 09) | Kutish joyi boshliqning narigi tomonida, yo'l boshliq tanasi orqali; "yetib bo'lmadi" sharti har 50 sm slot o'zgarishida qayta boshlanadi |
| 4 | Yo'lovchi | Gavjum joyda gavda u yoq-bu yoqqa titraydi: Yolovchi_02 10 s da 50 marta yo'nalish almashtirdi, tor joyda 360° aylandi | `bOrientRotationToMovement` + `RotationRate` 360°/s crowd avoidance tezligining har kadrdagi tebranishiga ergashadi |
| 5 | Yo'lovchi | Manzilga yetib, darhol 180° burilib orqaga qaytadi (Yolovchi_01) | `NextWalkTarget` faqat 3 m dan uzoq nuqta tanlaydi, yo'nalishga qaramaydi (faqat hammolda) |
| 6 | Hamma yuruvchilar | Oyoq sirpanishi: yerga tekkan oyoq 25-38 sm/s siljiydi (tana 80-110 sm/s) - taxminiy o'lchov, ko'z bilan tasdiqlash kerak | Yurish klipi tezligi agent tezligiga moslanmagan (7-bo'limda ham qayd etilgan) |
| 7 | Hamroh | Turganda har 4 s da boshliq va rasta orasida burilib turadi (10-17 marta/daq) | `F_Idle` har 4-9 s, `TurnTowards` 60% rasta / 40% boshliq - mayda, lekin ko'zga tashlanadi |

Tiqilish joylari: (1750-2550, 950-1100) yo'lagi - Guruh00/07/09 va Xaridor_12 shu yerda turadi; (3800, 400) - Guruh04; (1300-1900, -1050) - Guruh08 + suhbatdosh; (4200, -250) navbati.

Boshqa: `BP_City_Bus` har kadr "Divide by zero" ogohlantirishi beradi (NPC emas).

### 8.1 Tuzatishlar (1-3 xatolar, 2026-11-09)

`BozorNpc.cpp/.h`. Har bosqichdan keyin xuddi shu 180 s kuzatuv qaytarildi; natijalar tasodif tufayli yugurishdan yugurishga farq qiladi.

| Xato | Tuzatish |
|---|---|
| 1 | `WalkTo`: 0.6 s tezlik yo'q bo'lsa oldidagi odamni topadi (`BlockerAhead`, `PathDir`) va uning yonidan aylanib o'tadi (`DetourPoint`, 95 yoki 72 sm, ko'pi bilan 2 marta; aylanish vaqti ham 3 s tiqilish hisobiga kiradi). Manzilga 1.2 m qolganda oldida begona tursa - yetib keldi. Voz kechilgan yo'nalish 10 s tanlanmaydi (`AvoidDir`, `NextWalkTarget`) |
| 2 | Hamroh: boshliq yo'lga chiqishi bilan (AI holati Moving, hali sekin bo'lsa ham) yuradi; boshliq yo'lida tursa chetga chiqadi, aks holda orqasiga o'tadi. Tiralsa aylanib o'tadi yoki boshliqning o'ziga yo'l so'raydi. Yo'l faqat manzil 30 sm surilganda qayta so'raladi (avval har 0.1 s, `game-ai` skill qoidasi) |
| 3 | Hamroh kutish joyi boshliq to'xtaganda bir marta tanlanadi (navbatsiz ham), hozirgi joyi yaroqli bo'lsa qimirlamaydi, aks holda eng yaqin nomzod. Boshliqqa 1 s tegib tursa to'xtaydi. To'xtatish tezlikka emas, AI holatiga qarab (tiralgan hamroh tezligi 0 bo'lib, `StopMoving` chaqirilmay qolardi). Begona odam unga tiralib o'tolmasa, 70 sm chetga chiqadi |
| 4 | Hech kimga tiralmay qotish (NavMesh chetida). Sabab sinovda aniqlandi: polda turgan mol yoki rasta burchagi yonida agent NavMesh chetiga radiusidan yaqin kelsa, DetourCrowd uni teskari tomonga tortib, joyida aylantiradi (tezlik 0-5 sm/s, 11-28 s). Crowd steering o'chirilganda o'sha nuqtadan darhol 100 sm/s da yurib ketdi; to'qnashuv (Pawn kanali) va NavMesh to'g'ri (agent qadam balandligi 12 sm, mol NavMesh'da teshik). Tuzatish: 0.6 s qotib, oldida odam bo'lmasa `SetCrowdSteering(false)` 1.5 s (`UCrowdFollowingComponent::SuspendCrowdSteering`), keyin qayta yoqiladi; `WalkTo` tugaganda ham qayta yoqiladi. Hamrohda ham shunday |

| Kuzatuv (180 s) | Hammol | Hamroh | Xaridor | Yo'lovchi | Jami | Eng uzun |
|---|---|---|---|---|---|---|
| Oldin | 12.7 s | 63.0 s | 36.5 s | 39.2 s | 151 s | 10.2 s |
| 1-3 dan keyin | 5.5 s | 0 | 8.4 s | 17.4 s | 31 s | 5.5 s |
| 4 dan keyin | 0 | 5.1 s | 0 | 8.9 s | 14 s | 5.1 s |

Hamroh boshlig'idan eng ko'p 8.4 m -> 3.9 m uzoqlashadi. Odam ichiga kirish va sakrash yo'q.

Kuzatuv tuzoqlari: muharrir oynasi yig'ilgan (minimize) bo'lsa ham Simulate 3 fps bo'ladi. NPC'ni qo'lda `MoveTo` bilan sinashda yuruvchi rolni olma - uning korutini o'z `MoveTo`si bilan urishadi; sotuvchi (korutini yurmaydi) toza sinov beradi. Faqat funksiya ichi o'zgarsa Live Coding (`LiveCoding.Compile`) yetadi, header o'zgarsa editor yopilib `Build.bat`.

---

## Manbalar
- Yurish tezligi: [NILIM 2016 (Tokio, Shinbashi)](https://www.nilim.go.jp/english/annual/annual2016/pdf_file/82.pdf), [NILIM 2017](https://www.nilim.go.jp/english/annual/annual2017/pdf_file/88.pdf), [UNITEN – savdo markazlari](https://irepository.uniten.edu.my/handle/123456789/24875), [IJIE – temir yo'l terminali](https://publisher.uthm.edu.my/ojs/index.php/ijie/article/view/5425), [arXiv 2310.04818](https://www.arxiv.org/abs/2310.04818)
- Guruhlar: [Social groups in pedestrian crowds – review (arXiv 2107.13293)](https://arxiv.org/pdf/2107.13293), [Walking behaviour of pedestrian social groups (arXiv 1003.3894)](https://arxiv.org/pdf/1003.3894)
- Proksemika: [Social Force in Pedestrian Crowd (arXiv 2109.12597)](https://arxiv.org/pdf/2109.12597), [Crowd egress with stressors (arXiv 2206.01393)](https://arxiv.org/pdf/2206.01393)
- O'zbek bozori: [OSU – Uzbek Bazaars](https://u.osu.edu/uzbek/bazaars/), [Orexca – Bazaars of Uzbekistan](https://www.orexca.com/uzbekistan/bazaars.htm), [Turkestan Travel – Chorsu](https://www.turkestantravel.com/en/sights/chorsu-bazaar-in-tashkent/), [Tripadvisor – Tashkent markets](https://www.tripadvisor.co.nz/Attractions-g293968-Activities-c26-t142-Tashkent_Tashkent_Province.html), [Eurasia.travel – Chorsu](https://eurasia.travel/ru/uzbekistan/tashkent/chorsu-bazaar/), [Eurasia.travel – Andijon bozorlari](https://eurasia.travel/uzbekistan/andijan/bazaars), [Kun.uz – bozordagi aravakashlar](https://kun.uz/ru/news/2022/03/12/antimonopolisty-reshili-uregulirovat-deyatelnost-vozchikov-s-telejkami-na-tashkenskix-rynkax)
- O'yin NPC'lari: [MoCap Online – NPC animation guide](https://mocaponline.com/blogs/mocap-news/npc-animation-behavior-guide), [Crowd simulation animation guide](https://mocaponline.com/blogs/mocap-news/crowd-simulation-animation-guide), [UWO – ambient NPC behaviour](https://www.csd.uwo.ca/~ebuitron/downloads/ProjectReport.pdf)
