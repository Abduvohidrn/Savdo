# NPC'larni qo'lda yozilgan ssenariy bilan boshqarish (2026-11-09)

Foydalanuvchi qarori: avtomatik (tasodifiy) xulq juda ko'p xato beradi. Endi **har bir NPC'ning o'z yo'li bor**: qayerga boradi, nima qiladi, qaysi animatsiyani o'ynaydi - hammasi oldindan belgilanadi.

Ishlatilgan skillar (`D:\skill\awesome-gamedev-agent-skills`, `router` bo'yicha): `unreal-cpp-gameplay`, `unreal-behavior-trees`, `game-ai`, `ai-behavior-trees-utility-ai`, `level-design`.

---

## 1. Tadqiqot: O'zbekiston bozorida odamlar qanday harakatlanadi

Eski tadqiqot (yurish tezligi, guruhlar, proksemika, savdo qadamlari) `bozor_npc_xulq_atvori.md` 1-bo'limida. Bu yerda yangi topilmalar va ularning harakatga aylanishi.

### 1.1 Salomlashish va odob
- Salom: *"Assalomu alaykum"* - *"Va alaykum assalom"*. Erkaklar qo'l berib ko'rishadi; tanish bo'lsa quchoqlashadi. Hurmat belgisi - **qo'lni ko'krakka qo'yib, biroz egilish** (manbalarda o'ng yoki chap qo'l deb farqli yoziladi). Ayolga erkak qo'l uzatmaydi - faqat qo'l ko'krakda, bosh egiladi.
- Narsani **o'ng qo'l** bilan beradi va oladi (pul, paket).
- Sotuvchi xaridorga qarindoshdek murojaat qiladi: *"aka", "opa", "akajon", "singlim"*. Kattalarga alohida hurmat.
- Savdodan oldin biroz gaplashish odat: hol-ahvol, yangiliklar. Doimiy xaridor bilan munosabat yaqin, unga yaxshiroq narx beriladi.

### 1.2 Savdolashish
- Savdolashish - majburiy "marosim", lekin **do'stona, o'yin kabi**. Ovoz ko'tarilishi mumkin, bu janjal hisoblanmaydi.
- Sotuvchi birinchi narxni oshirib aytadi; xaridor 20-30% kam taklif qiladi. Birinchi narxga darhol rozi bo'lish odobsizlik deb ham qaraladi.
- Usullar: befarqlik ko'rsatish, molning kamchiligini aytish, aniq summani ko'rsatib uzatish. Kelishmasa - **"ketish marosimi"**: xaridor indamay ketadi, keyin qaytishi mumkin.
- Savdolashayotganda qo'shni sotuvchi o'z molini taklif qilmaydi (odob).
- Sotuvchi **tatib ko'rishga** beradi (meva, quruq meva, qazi). Katta xariddan keyin *"jondan"* deb bir-ikki meva qo'shib beradi, ko'pincha bolalarga va ayollarga.
- Narx yozilmaydi, sotuvchi xaridorning tashqi ko'rinishiga qarab narx aytadi. Tarozi bilan bog'liq gap-so'z ham bo'ladi.
- Xayrlashuv: *"Rahmat! Bozoringizni bersin!"* - *"Yana keling!"*, qo'l ko'krakda.

### 1.3 Molni ko'rsatish va sotuvchining kuni
- Mol chiroyli teriladi: tog'oralarda bir xil mevalar, sabzavot piramida qilib, ko'katga suv sepiladi.
- Bo'sh sotuvchi: peshtaxtaga suyanadi yoki tirsagini qo'yadi, molni teradi/to'g'rilaydi, qo'shnisi bilan gaplashadi, telefonga qaraydi, choy ichadi, o'tiradi.
- Chaqiriq: o'tayotganni chaqiradi (*"Keling, opa, arzon!"*), qo'l siltaydi, molni ko'rsatadi.
- Sotuvchilar yillab bir joyda turadi: qo'shni sotuvchilar bir-biri bilan tez-tez gaplashadi.
- Ertalab va kechqurun savdo faol; kechqurun qolgan mol arzonlashadi.

### 1.4 Xaridorlar va yurish
- Xaridor sekin yuradi (0.7-1.0 m/s), rastalarga qarab o'tadi, 3-4 rastadan birida to'xtaydi. Tajribali xaridor avval aylanib chiqib, keyin sotib oladi.
- Ko'pchilik guruhda yuradi (juft 31%, uchtalik 18%). Guruh yolg'izdan **~37% sekin**. Bekor yurganda yonma-yon, gavjumda **V shaklida** yoki birin-ketin.
- Keksa odamlar sekin, ko'pincha qo'llari orqada yuradi. Ayollar qo'lida xalta/paket, ba'zan bola qo'lidan ushlagan.
- Qo'lda naqd pul (mayda pul), xalta (avoska).
- Choyxona/ovqat qatori: umumiy stol, notanish odamlar yonma-yon o'tiradi.

### 1.5 Hammol (aravakash)
- Arava bilan yuk tashiydi, tez yuradi (1.3-1.5 m/s), odamlarni ogohlantiradi; odamlar unga yo'l beradi.

### Manbalar
- [Chorsu Bazaar - Central Asia Travel](https://www.centralasia-travel.com/en/countries/uzbekistan/places/bazaars/chorsu_bazaar)
- [Chorsu Bazaar - DiscoverStan](https://discoverstan.com/chorsu-bazaar-tashkent/)
- [Things to know before traveling to Uzbekistan - Lonely Planet](https://www.lonelyplanet.com/articles/things-to-know-before-traveling-to-uzbekistan)
- [Customs and etiquette in Uzbekistan - Facts and Details](https://factsanddetails.com/central-asia/Uzbekistan/sub8_3d/entry-4704.html)
- [Uzbek Greetings - OSU](https://u.osu.edu/uzbek/uzbek-greetings/)
- [Базары Узбекистана - OrexCA](https://www.orexca.com/rus/uzbekistan/bazaars.htm)
- [Как торговаться на базарах Ташкента - AVO Bank](https://avobank.uz/ru/blog/post/bazzaars-in-tashkent)
- [Узбекские базары - Синьхуа](http://russian.news.cn/2019-11/15/c_138557710.htm)
- [Are prices higher at bazaars or supermarkets in Uzbekistan - EastFruit](https://east-fruit.com/en/news/are-prices-higher-for-fruits-and-vegetables-at-bazaars-or-supermarkets-in-uzbekistan-the-answer-will-surprise)
- [Haridorga nisbatan zulm va hiyonat - kun.uz](https://kun.uz/news/2017/01/09/haridorga-nisbatan-zulm-va-hienat)
- [Sotuvchining shirin so'zi bo'lmasa - kun.uz](https://kun.uz/news/2018/06/11/sotuvcining-serli-suzi-bulmasa)
- [Women and Bazaars: Gendering Entrepreneurship in Uzbekistan - Springer](https://link.springer.com/chapter/10.1007/978-3-031-55341-7_13)
- [The Walking Behaviour of Pedestrian Social Groups (Moussaid va boshq., 2010)](https://pmc.ncbi.nlm.nih.gov/articles/PMC2850937/)
- [Social groups in pedestrian crowds: review](https://arxiv.org/pdf/2107.13293)

Eslatma: manbalarning ko'pi sayohat qo'llanmalari; ilmiy etnografik ish kam. Qo'lni ko'krakka qo'yishda qaysi qo'l ekani manbalarda farqli.

---

## 2. Harakat -> animatsiya xaritasi

Hozir loyihada (`/Game/Bozor/MetaAnim`, MetaHuman skeletiga retarget qilingan) ~60 klip. Arxivda (`D:\Unreal Projects\BozorUnreal_Arxiv\Content`) ishlatilmagan paketlar: Vendors_And_Customers (Customer 143, Chef 47, Cashier 22, Bartender 21, Server 11), CityAnimations_pack2 (50), DialogAnimations (27), CivilianNPCsAI (47), WW100Idle (108).

| Harakat (bozorda) | Klip | Hozir loyihada? |
|---|---|---|
| Salom, qo'l ko'krakda, egilish | `Cashier_Greeting_Bow` | bor |
| Qo'l berib quchoqlashib ko'rishish (tanishlar) | `Customer_Standing_ShakeHug`, `Customer_Standing_Hug` | arxivda |
| Sotuvchi chaqiradi, qo'l siltaydi | `Salesman_HandGesture_WaveOver`, `Cashier_Greeting_WaveHello`, `Salesman_HandGesture_OpenArms` | bor |
| Molni ko'rsatish | `Cashier_Show_LeftSide/RightSide` | bor |
| Peshtaxtaga tayanish / tirsak qo'yish | `Bartender_..._HandsOnTable_Loop`, `..._ElbowLean_Loop` | bor |
| Tayangan holda gaplashish | `Bartender_..._ElbowLean_TalkGesture01-03` | arxivda |
| Molni terish, to'g'rilash | `Server_PlaceFoodOnTable_L/R`, `Bartender_WipeCounter_Loop`, `Chef_WorkStation_Inspect(_LowHeight)` | qisman |
| Sabzavot tanlab savatga solish (xaridor) | `Collect_in_basket_1..5` (+in/out) | arxivda |
| Molni ko'rib tanlash | `Customer_Standing_BrowsingItems01/02` | bor |
| Savdolashish (oddiy / qizg'in) | `Dialog_Calm_*`, `Dialog_Intense_*`, `Server_TalkGesture_*` | qisman |
| Rozi / rozi emas | `Cashier_Talk_HeadNod_Yes` / `Cashier_Talk_HeadNod_No` | Yes bor, No arxivda |
| "Qimmat ekan" deb ketish | `AS_Disappointed_Anim` | bor |
| O'ylanish (narxni hisoblash) | `AS_Thinking_Anim` | bor |
| Tarozida tortish, paket uzatish | `Cashier_Handout(_Start/Loop/End)` | Handout bor |
| Pul berish | `Cashier_Handout` (xaridor tomoni) | bor |
| Navbatda kutish | `Customer_Standing_WaitingInLine01-03`, `Waiting_1-3` | bor |
| Telefon | `AS_Phone_Loop`, `AS_Texting_Anim` | bor |
| Go'sht chopish (qassob) | `Chef_WorkStation_KnifeChop_Loop01-03` | arxivda |
| Supurish (ertalab) | `Server_Broom_Sweep_Loop` | bor |
| Choyxonada o'tirish, choy | `Customer_Sit_Chair_*`, `..._Drink_Cup_*` | bor |
| Ovqat qatorida ovqatlanish | `Customer_Sit_Chair_Eat_withKnifeFork_*`, `..._NoodleBowl_*` | arxivda |
| Stol atrofida suhbat | `Communication_at_the_table_1-4`, `Talk_by_the_fire_1-3` | arxivda |
| Suhbat (tik turib) | `AS_Talking_*`, `Server_TalkGesture_FriendlyConvo` | bor |
| Kerishish, charchash | `AS_Arm_Stretching_Anim` | bor |

Cheklov: keksalarning **qo'li orqada yurishi** uchun AnimBP'da yuqori tana sloti kerak (ikkilik asset). Hozircha yurishda faqat tezlik va yurish uslubi farq qiladi.

---

## 3. Tizim (skill qoidalari asosida)

- **Qaror va harakat ajratilgan** (`game-ai`): ssenariy *nima qilish*ni aytadi (manzil, harakat), yurishni NavMesh + crowd bajaradi.
- **Ssenariy - Behavior Tree'dagi Sequence** (`ai-behavior-trees-utility-ai`): qadamlar ketma-ket bajariladi, har qadam tugaguncha "Running". `BT_`/`BB_` asset'lar ishlatilmaydi: ular ikkilik fayl, loyiha mantiqi C++ + matnli ma'lumotda (`unreal-behavior-trees`: oddiy mantiq uchun kichik holat mashinasi arzonroq).
- **Har NPC'ning o'z animatsiyalari**: NPC'da `Actions` ro'yxati (kategoriya -> klip). `Do()` avval NPC'ning o'z ro'yxatidan, bo'lmasa menejerdan oladi.
- **Ma'lumot matnda**: `Scripts/npc_ssenariy.json` - 100 NPC'ning har biri uchun persona, tezlik, animatsiyalar va qadamlar. `Scripts/npc_ssenariy.py` uni xaritadagi NPC'larga yozadi.

### Qadam turlari
| Qadam | Ma'nosi |
|---|---|
| `goto` | nuqtaga boradi (navbat nuqtasi, rasta oldi, o'rindiq, darvoza yoki koordinata) |
| `anim` | klip/kategoriyani N soniya o'ynaydi |
| `face` | sotuvchiga, rastaga yoki nuqtaga buriladi |
| `greet` | salomlashadi (sotuvchi ham javob beradi) |
| `deal` | savdo: narx so'rash -> savdolashish -> (tatib ko'rish) -> tortish -> to'lov -> rahmat yoki "qimmat" deb ketish |
| `sit` | o'rindiqqa o'tirib, N soniya choy ichadi |
| `wait` | kutadi |
| `loop` | ssenariy boshiga qaytadi |

---

## 4. Holat (bajarilganlar shu yerga yoziladi)

- [x] Tadqiqot (1-bo'lim)
- [x] Animatsiya xaritasi (2-bo'lim)
- [x] 12 klip arxivdan MetaHuman skeletiga retarget qilindi (`Scripts/npc_anim_retarget.py`): Collect_in_basket_1-5, Cashier_Talk_HeadNod_No, Dialog_Intense_* (4), Bartender WipeCounter_Loop, Customer_Standing_ShakeHug
- [x] C++ (`BozorNpc.h/.cpp`): `FBozorStep` + `EBozorStepOp` (GoTo, Anim, Face, Greet, Stall, Sit, Wait), `ScriptLoop`, `VisitStall`, `SitAt`, `OwnActions` + `PickAction/DoFirst`, savdo tadqiqot bo'yicha (salom -> narx -> ko'rsatish -> savdolashish -> tatib ko'rish -> rozi/rozi emas -> tortish -> to'lov -> ikki tomonlama rahmat)
- [x] 100 NPC ssenariysi: `Scripts/npc_ssenariy.json` (asosiy manba, qo'lda tahrirlanadi). Birinchi variantini `npc_ssenariy_yarat.js` yaratdi, xaritaga `npc_ssenariy.py` yozadi; editorsiz: `npc_ssenariy_qur.py` (commandlet)
- [x] Sahnada kuzatish (180 s, ~33 fps)

### Fayllar
| Fayl | Vazifasi |
|---|---|
| `Scripts/npc_sahna.json` | xaritadan olingan ma'lumot: NPC'lar, 57 navbat, 6 o'rindiq, yurish nuqtalari |
| `Scripts/npc_ssenariy_yarat.js` | persona (tanaga qarab), tezlik, animatsiya to'plami va yo'llarni yaratadi (`node npc_ssenariy_yarat.js`) |
| `Scripts/npc_ssenariy.json` | **har NPC'ning ssenariysi** - shu faylni tahrirlang |
| `Scripts/npc_ssenariy.py` | JSON'ni xaritadagi NPC'larga yozadi (muharrirda) |
| `Scripts/npc_ssenariy_qur.py` | editorsiz: retarget + yozish + xaritani saqlash |

Persona: keksa_ota 7, keksa_momo 8, ona 10, qiz 17, yigit 10, erkak 10, hammol 2, sotuvchi (erkak 19, ayol 9, keksa 8). Sotuvchi, hamroh va suhbatdosh o'z sikllarida qoladi, lekin o'z animatsiyalarini oladi; xaridor (guruh boshlig'i ham), yo'lovchi va hammol ssenariy bo'yicha yuradi.

### Kuzatuv natijasi (180 s, tiqilish = AI yuryapti, tezlik < 10 sm/s, 2.5 s+)
| | Hammol | Hamroh | Xaridor | Yo'lovchi | Jami |
|---|---|---|---|---|---|
| Tasodifiy xulq (boshida) | 12.7 s | 63.0 s | 36.5 s | 39.2 s | 151 s |
| Ssenariy, 1-urinish | 22.6 s | 315 s | 41.2 s | 51.0 s | 430 s |
| Ssenariy, tuzatilgandan keyin | 28.9 s | 2.8 s | 19.8 s | 66.6 s | 118 s |
| + yo'lakdagi arava surildi | 72.1 s | 2.6 s | 7.9 s | 11.1 s | 94 s |
| + Hammol_12 yo'li o'zgardi | 0 | 0 | 15.7 s | 8.6 s | **24 s** (eng uzuni 5.5 s) |

1-urinishdan keyingi tuzatishlar: guruh boshlig'i orqada qolgan hamrohini kutadi (4.5 m+, ko'pi bilan 8 s); hamroh boshliqdan 6 m uzoqlashsa to'g'ri unga yo'l oladi; aylanib o'tib bo'lmasa crowd steering 1.5 s o'chadi; ssenariy xaridori navbatda 2 tadan ko'p odam bo'lsa rastani tashlab ketadi, sabri 10-20 s.

**Xarita o'zgarishi (foydalanuvchi roziligi bilan, 2026-11-09):** janubiy yo'lak o'rtasidagi bezak aravasi (`Bozor_Jihozlar` / `SM_SM_Aravakash_Arava_LOD0_0`, 2-nusxa) yo'lakni to'sib, o'tishni Sotuvchi_12 navbatiga surardi. U sharqiy darvoza tashqarisiga, boshqa aravaning yoniga ko'chirildi: **eski** (2495.6, -1060, 1.27) yaw -90 -> **yangi** (4760, 200, 1) yaw 0. NavMesh qayta qurildi, xarita saqlandi.

Hammol_12 yo'li sharqiy qism ichidan (53-navbat va poldagi mol yonida 57 s tiqilgan) x~3160 dagi keng o'tishga ko'chirildi (`npc_ssenariy_yarat.js`).

Hali tegilmagan: janubiy yo'lakning sharqiy uchida, x~3140, y~-1085 da qovun uyumi (`Bozor_Mahsulotlar` / `SM_SM_Uyum_Qovun`) ham yo'lakni toraytiradi.

Vaqtincha: retarget manbasi bo'lgan `Content/Vendors_And_Customers`, `CityAnimations_pack2`, `DialogAnimations` loyihaga qayta nusxalandi; yangi kliplar ularga bog'liq emas (tekshirildi), editor yopiq paytda o'chirsa bo'ladi (arxivda nusxasi bor).
