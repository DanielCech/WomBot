# WomBot - TODO / Implementation Plan

## Vize projektu

WomBot je Lego robot ovládaný **Adafruit CLUE** (nRF52840) na expanzní desce **Magic:bit V2.0**.
Cíl: interaktivní robot, který chodí, reaguje na prostředí, řeší bludiště a komunikuje
s počítačem přes Bluetooth. Plně využít HW možnosti obou desek.

---

## Hardware Overview

### Adafruit CLUE (mozek)

| Vlastnost | Detail |
|-----------|--------|
| Procesor | Nordic nRF52840 — 64 MHz Cortex M4, 1 MB Flash, 256 KB RAM |
| Displej | 1.3" 240x240 Color IPS TFT (ST7789) |
| Bluetooth | BLE 5.0 (nRF52840) |
| Flash | 2 MB interní (datalogging, obrázky, fonty) |
| Senzory | LSM6DS3TR (akcel/gyro), LIS3MDL (magnetometr), APDS9960 (proximity/light/color/gesture), PDM mikrofon, SHT30 (vlhkost), BMP280 (teplota/tlak/výška) |
| LED | 1x RGB NeoPixel, 2x bílé přední LED |
| Zvuk | Buzzer/speaker |
| Tlačítka | A (pin 5), B (pin 11), Reset |
| Expanze | STEMMA QT / Qwiic (I2C), edge connector (micro:bit kompatibilní) |

### Magic:bit V2.0 (tělo)

| Funkce | Detail | Piny/Kanály |
|--------|--------|-------------|
| 8x servo (S1-S8) | PCA9685 kanály 8-15 | Sx → ch (x+7) |
| 4x DC motor (M1-M4) | PCA9685 kanály 0-7, H-bridge | 2 ch/motor |
| 2x krokový motor | 28BYJ-48 kompatibilní, sdílí ch s motory | Stepper1=ch4-7, Stepper2=ch0-3 |
| 4x RGB LED | NeoPixel WS2812B | Pin P16 |
| Buzzer | Pasivní, jumper propojka | Pin P0 |
| IR přijímač | NEC protokol | Na desce |
| RGB ultrazvuk | Trig+echo single-pin, 6 vestavěných RGB LED | Konektor nahoře |
| I2C | PCA9685 @ 0x40 | P19 (SCL), P20 (SDA) |
| Napájení | 18650 Li-Ion + micro USB nabíjení + ext. 3-5V vstup + 5V výstup | |
| Montáž | Lego-kompatibilní otvory | |

### Externě připojeno

| Zařízení | Připojení |
|----------|-----------|
| Sonar:bit (ElecFreaks) | Pin P2, HC-SR04 single-pin, 4-400 cm |

---

## Aktuální zapojení serv

| Konektor | PCA9685 ch | Funkce | Kód (index/konstanta) |
|----------|------------|--------|----------------------|
| S1 | 8 | Pravá zadní noha | LEG_CHANNELS[3] = 8 |
| S2 | 9 | Hlava | HEAD_CHANNEL = 9 |
| S3 | 10 | Levá zadní noha | LEG_CHANNELS[2] = 10 |
| S4 | 11 | Pravá přední noha | LEG_CHANNELS[1] = 11 |
| S5 | 12 | Levá přední noha | LEG_CHANNELS[0] = 12 |

LEG_CHANNELS index: [0]=FL, [1]=FR, [2]=BL, [3]=BR

---

## Platforma: Arduino IDE (C/C++)

- Adafruit nRF52 Arduino BSP
- PlatformIO: board `adafruit_clue_nrf52840`

### Aktuální knihovny
- `Wire` (I2C)
- `Adafruit_TinyUSB` (nRF52840 Serial)
- `Adafruit_GFX` + `Adafruit_ST7789` (TFT)

### Potřebné knihovny (pro rozšíření)
- `Adafruit_NeoPixel` (RGB LED)
- `Adafruit_LSM6DS` (akcel/gyro)
- `Adafruit_LIS3MDL` (magnetometr)
- `Adafruit_APDS9960` (proximity/gesture/color)
- `Adafruit_BMP280` (tlak/teplota)
- `Adafruit_SHT31` (vlhkost)
- `IRremote` nebo vlastní NEC dekodér (IR)
- `Bluefruit` / `Adafruit_nRF52_BLE` (BLE)

---

## Fáze implementace

### Phase 1: Základní kostra (HOTOVO)

- [x] Arduino projekt (`WomBotArduino.ino` + moduly)
- [x] MagicBitDriver — PCA9685 I2C driver (adresa 0x40, AI bit, 50 Hz)
- [x] SonarBit — ultrazvukový senzor vzdálenosti
- [x] ClueDisplay — TFT displej wrapper
- [x] WomBotLegs — 4 nohy, trot gait, turn
- [x] WomBotHead — servo hlavy, scan
- [x] Mode system (Idle, Autonomous, Demo, Sensors) + tlačítka A/B
- [x] Oprava PCA9685 kanálů: servo S1-S8 = ch 8-15 (ne 0-7!)
- [x] Oprava Auto-Increment bit v MODE1 registru

### Phase 2: Ověření a kalibrace serv

- [ ] Ověřit, že serva reagují po opravě kanálů
- [ ] Kalibrace neutrálních úhlů pro každé servo
- [ ] Kalibrace směru otáčení (některá serva mohou být zrcadlově)
- [ ] Servo test mode — postupné testování jednotlivých kanálů
- [ ] Doladit STRIDE_ANGLE a TURN_ANGLE pro reálnou chůzi

### Phase 3: RGB LED + Buzzer + Emoce

- [ ] **RGBController** — ovládání 4x NeoPixel na P16
  - Stavové barvy: idle (modrá), walking (zelená), obstacle (červená), demo (duhová)
  - Dýchací efekt, blikání, rotace
- [ ] **BuzzerController** — pasivní buzzer na P0
  - Zvuky: startup melodie, obstacle warning, happy beep, sad tone
  - Jednoduchý tone player
- [ ] **Emotions** — kombinace displej + LED + buzzer
  - Obličeje/emotikony na TFT displeji
  - Synchronizace s LED efekty a zvuky
  - Reakce na události: překážka, dotyk (proximity), zvuk (mikrofon)

### Phase 4: Senzory CLUE

- [ ] **AccelGyro** — LSM6DS3TR
  - Detekce náklonu (prevence pádu)
  - Detekce nárazu / zvednutí
  - Kompenzace chůze
- [ ] **Magnetometer** — LIS3MDL
  - Kompas — orientace v prostoru
  - Užitečné pro navigaci v bludišti
- [ ] **ProximityLight** — APDS9960
  - Blízká detekce překážek (doplňuje sonar)
  - Detekce barev podlahy (sledování čáry?)
  - Gesta — interakce s uživatelem
- [ ] **Microphone** — PDM
  - Detekce zvuku / tlesknutí → reakce
  - Jednoduchý level meter na displeji
- [ ] **Environment** — BMP280 + SHT30
  - Teplota, vlhkost, tlak na displeji
  - Datalogging do interní flash

### Phase 5: Bluetooth (BLE)

- [ ] **BLE Remote Control**
  - UART service pro příkazy z počítače/mobilu
  - Ovládání pohybu: forward, backward, left, right, stop
  - Nastavení režimů, kalibrace
- [ ] **BLE Telemetrie**
  - Streamování dat ze senzorů do počítače
  - Vzdálenosti, orientace, teplota
  - Real-time monitoring
- [ ] **PC companion app** (Python/web)
  - BLE připojení přes `bleak` (Python) nebo Web Bluetooth
  - Vizualizace dat, mapování prostředí
  - Možnost posílat složitější příkazy

### Phase 6: Inteligentní chování

- [ ] **Maze Solver**
  - Wall-following algoritmus (pravá/levá ruka)
  - Sonar + head scan pro detekci stěn
  - Kompas pro orientaci
  - Mapování na displeji
- [ ] **Obstacle Course**
  - Inteligentní navigace kolem překážek
  - Kombinace sonar + proximity + akcel pro stabilitu
- [ ] **Follow Mode**
  - Sledování objektu před robotem (sonar)
  - Udržování konstantní vzdálenosti
- [ ] **Dance Mode**
  - Předprogramované taneční sekvence
  - Synchronizace s hudbou / blikáním LED
  - Reakce na zvuk z mikrofonu

### Phase 7: IR ovládání

- [ ] **IRReceiver** — NEC dekodér
  - Emakefun dálkový ovladač: A,B,C,D, UP,DOWN,LEFT,RIGHT, OK, 0-9, +/-
  - Ovládání pohybu přes IR
  - Přepínání režimů
  - Přímé ovládání serv pro kalibraci

### Phase 8: Rozšíření motorů

- [ ] **DC motory** — až 4x (M1-M4)
  - Pohon kol (pokud se přidají k Lego konstrukci)
  - Jiné mechanismy (zvedání, uchopování)
- [ ] **Krokové motory** — 28BYJ-48
  - 2x stepper interface na Magic:bitu (5-pin konektory)
  - Přesné polohování
  - Pozor: sdílí PCA9685 kanály s DC motory (nelze kombinovat)

---

## Nápady na interakce

- **Pozdrav**: Když se přiblíží člověk (proximity), zamává hlavou, zabliká LED, zahraje melodii
- **Strach**: Při blízké překážce couvne, zčervená LED, žalostný tón
- **Zvědavost**: Otáčí hlavou za zvukem (mikrofon), zeleně bliká
- **Taneček**: Na zvuk hudby se začne pohybovat v rytmu
- **Průzkumník**: Systematicky mapuje místnost, zobrazuje mapu na displeji
- **Hra**: Bludiště z Lega — najde cestu ven
- **Počasí**: Zobrazuje teplotu/vlhkost s emocemi (sluníčko/mráček na TFT)
- **Dálkové ovládání**: IR ovladač nebo BLE z telefonu/PC

---

## Klíčové technické poznámky

### PCA9685 kanály (OVĚŘENO)

```
Kanály  0-7:  DC motory (M1-M4, H-bridge, 2 kanály/motor)
Kanály 8-15:  Serva (S1=ch8, S2=ch9, ..., S8=ch15)
Formule:      Sx → PCA9685 kanál (x + 7)
```

Zdroj: oficiální TypeScript kód `emakefun/pxt-magicbit`: `setPwm(index + 7, 0, value)`

**Pozor**: Python knihovna `magicbit/Magicbit.py` má chybu — chybí offset +7!

### I2C

- PCA9685 @ 0x40 přes edge connector (P19=SCL, P20=SDA)
- Arduino `Wire.begin()` automaticky použije správné piny
- CLUE onboard senzory sdílí stejný I2C bus (ale na jiných adresách)

### Napájení serv

- Serva potřebují dostatek proudu — 18650 baterie nebo ext. 5V napájení
- Při slabé baterii serva nemusí reagovat i když komunikace funguje

---

## Sonar:bit Connection

| Wire | Connection |
|------|-----------|
| G | GND |
| V | 3.3V |
| S | P2 (pin 2) |
