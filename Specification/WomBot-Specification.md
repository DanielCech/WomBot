# WomBot - Technical Specification

## 1. Project Overview

WomBot is a Lego-compatible walking robot controlled by an **Adafruit CLUE** (nRF52840) board mounted on an **Emakefun Magic:bit V2.0** expansion board. The firmware is written in Arduino C++.

The goal is an interactive robot that walks, reacts to its environment, solves mazes, and communicates with a PC via Bluetooth. The design aims to fully exploit the hardware capabilities of both boards.

### Repository Structure

```
WomBot/
├── ArduinoStudio/          Arduino C++ source code (main project)
│   ├── WomBotArduino.ino   Main sketch — setup, loop, mode system
│   ├── MagicBitDriver.h/cpp PCA9685 I2C servo/motor driver
│   ├── WomBotLegs.h/cpp    Leg servo gait controller
│   ├── WomBotHead.h/cpp    Head servo controller
│   ├── SonarBit.h/cpp      Ultrasonic distance sensor driver
│   └── ClueDisplay.h/cpp   TFT display wrapper
├── Python/                 Legacy CircuitPython/MicroPython code
├── Resources/              Third-party libraries (magicbit, sonarbit)
├── Specification/          This document
├── TODO.md                 Project roadmap
└── README.md
```

---

## 2. Hardware

### 2.1 Adafruit CLUE (Brain)

| Feature | Detail |
|---------|--------|
| Processor | Nordic nRF52840 — 64 MHz Cortex M4, 1 MB Flash, 256 KB RAM |
| Display | 1.3" 240x240 Color IPS TFT (ST7789 via SPI1) |
| Bluetooth | BLE 5.0 |
| Internal flash | 2 MB (datalogging, images, fonts) |
| Buttons | A (pin 5), B (pin 11), Reset |
| NeoPixel | 1x RGB indicator LED |
| White LEDs | 2x front illumination |
| Buzzer | On-board speaker for tones |
| STEMMA QT | I2C expansion connector |
| Edge connector | micro:bit compatible (same form factor and pinout) |

**On-board sensors:**

| Sensor | Chip | Function |
|--------|------|----------|
| Accelerometer + Gyroscope | LSM6DS3TR-C (v2) / LSM6DS33 (v1) | 6-DoF motion |
| Magnetometer | LIS3MDL | 3-axis compass |
| Proximity / Light / Color / Gesture | APDS9960 | Multi-function optical |
| Microphone | PDM | Sound level / detection |
| Humidity | SHT30 | Relative humidity |
| Barometric pressure + Temperature | BMP280 | Altitude estimation |

### 2.2 Magic:bit V2.0 (Body)

Expansion board with a **PCA9685** 16-channel PWM controller at I2C address **0x40**.

**Board connectors and features:**

| Feature | Detail | Pin / Channel |
|---------|--------|---------------|
| 8x servo outputs (S1-S8) | PCA9685 channels 8-15 | Sx = ch (x + 7) |
| 4x DC motor outputs (M1-M4) | PCA9685 channels 0-7 (H-bridge, 2 ch per motor) | See section 3.2 |
| 2x stepper motor connectors | Shares channels with DC motors | Stepper1 = ch 4-7, Stepper2 = ch 0-3 |
| 4x RGB LED (NeoPixel WS2812B) | On-board | Pin P16, indices 0-3 |
| Passive buzzer | Connected via jumper | Pin P0 |
| IR receiver | NEC protocol | On-board |
| RGB ultrasonic connector | Trig+echo single-pin, 6 built-in RGB LEDs (indices 4-9) | Top of board |
| I2C connector | PH2.0-4Pin | P19 (SCL), P20 (SDA) |
| 8-pin IO port | General GPIO breakout | Bottom left |
| Power | 18650 Li-Ion + micro USB charging + 3-5V ext. input + 5V ext. output | Self-locking switch |
| Mounting | Lego-compatible holes | |

### 2.3 External Peripherals

| Device | Model | Connection |
|--------|-------|------------|
| Ultrasonic sensor | ElecFreaks Sonar:bit | RGB ultrasonic connector (top of board), 3-wire |
| Stepper motor (planned) | 28BYJ-48 | 5-pin connector on Magic:bit |

### 2.4 Sonar:bit Wiring

The Sonar:bit connects to the **RGB ultrasonic connector** at the top of the Magic:bit board. This 4-pin connector has pins: `5V, GND, RGB, P2`.

| Sonar:bit wire | Magic:bit pin | Note |
|----------------|---------------|------|
| S (Signal) | P2 | Trigger + echo on single pin |
| V (VCC) | 5V | Sensor power |
| G (GND) | GND | Ground |
| — | RGB | Leave unconnected (used only by RGB ultrasonic module with built-in LEDs) |

The RGB pin on the connector is part of a NeoPixel chain (P16, indices 4-9) intended for the emakefun RGB ultrasonic module which has 6 built-in RGB LEDs. The standard ElecFreaks Sonar:bit does not use this pin.

In firmware: `static const int SONAR_PIN = 2;` (pin P2 on edge connector).

---

## 3. PCA9685 — PWM Controller

### 3.1 Communication

- **I2C address**: 0x40
- **I2C pins** (edge connector): P19 = SCL (nRF52840 P0.25), P20 = SDA (nRF52840 P0.24)
- Arduino `Wire.begin()` uses these pins by default on CLUE
- On-board CLUE sensors share the same I2C bus (different addresses)

### 3.2 Channel Allocation

The PCA9685 has 16 PWM channels (0-15). On Magic:bit V2.0 they are physically routed as:

```
Channels  0-7:   DC motors / stepper motors
Channels  8-15:  Servo outputs S1-S8
```

**Servo mapping** (verified from official emakefun/pxt-magicbit TypeScript source):

| Connector | PCA9685 Channel | Formula |
|-----------|----------------|---------|
| S1 | 8 | 1 + 7 |
| S2 | 9 | 2 + 7 |
| S3 | 10 | 3 + 7 |
| S4 | 11 | 4 + 7 |
| S5 | 12 | 5 + 7 |
| S6 | 13 | 6 + 7 |
| S7 | 14 | 7 + 7 |
| S8 | 15 | 8 + 7 |

**DC motor mapping** (each motor uses 2 channels for H-bridge direction control):

| Motor | Forward ch (pp) | Reverse ch (pn) |
|-------|-----------------|-----------------|
| M1 | 4 | 5 |
| M2 | 6 | 7 |
| M3 | 0 | 1 |
| M4 | 2 | 3 |

**Stepper motor mapping** (shares channels with DC motors — cannot use simultaneously):

| Stepper | Channels | Overlaps with |
|---------|----------|---------------|
| Stepper Moto 1 | 4, 5, 6, 7 | M1 + M2 |
| Stepper Moto 2 | 0, 1, 2, 3 | M3 + M4 |

Compatible stepper: **28BYJ-48** (5-wire, 5V)

### 3.3 Initialization Sequence

```cpp
// 1. Reset
writeReg(MODE1, 0x00);
writeReg(MODE2, 0x04);          // OUTDRV = totem pole

// 2. Enable Auto-Increment (AI = bit 5 = 0x20)
//    CRITICAL: required for multi-byte I2C writes in setPWM()
writeReg(MODE1, 0x20);
delay(5);

// 3. Wake up (clear SLEEP bit)
mode1 = readReg(MODE1);
mode1 &= ~0x10;
writeReg(MODE1, mode1);
delay(5);

// 4. Set PWM frequency to 50 Hz (servo standard)
setPWMFreq(50);

// 5. Turn off all channels
setAllServosOff();
```

### 3.4 Servo PWM Calculation

```
Frequency:  50 Hz → period = 20,000 us → 4096 ticks
Pulse:      600 us (0°) to 2,400 us (180°)

pulse_us = 600 + (degrees / 180.0) * 1800
ticks    = pulse_us * 4096 / 20000
```

### 3.5 PCA9685 Registers (per channel N)

| Register | Address | Description |
|----------|---------|-------------|
| LED_ON_L | 0x06 + 4*N | PWM on time, low byte |
| LED_ON_H | 0x07 + 4*N | PWM on time, high byte |
| LED_OFF_L | 0x08 + 4*N | PWM off time, low byte |
| LED_OFF_H | 0x09 + 4*N | PWM off time, high byte |

---

## 4. Current Servo Wiring

| Magic:bit | PCA9685 ch | Robot part | Code constant |
|-----------|------------|------------|---------------|
| S1 | 8 | Right rear leg (pravá zadní) | `LEG_CHANNELS[3]` |
| S2 | 9 | Head (hlava) | `HEAD_CHANNEL` |
| S3 | 10 | Left rear leg (levá zadní) | `LEG_CHANNELS[2]` |
| S4 | 11 | Right front leg (pravá přední) | `LEG_CHANNELS[1]` |
| S5 | 12 | Left front leg (levá přední) | `LEG_CHANNELS[0]` |

**Leg array indexing**: `[0]=FL, [1]=FR, [2]=BL, [3]=BR`

**Servo orientation**: Left-side servos (FL, BL) are physically mirrored. The code compensates by inverting their angle: `angle = 180 - angle` when `LEG_REVERSED[leg]` is true.

---

## 5. Software Architecture

### 5.1 Module Overview

| Module | File | Responsibility |
|--------|------|----------------|
| **Main** | `WomBotArduino.ino` | Setup, main loop, mode system, button handling |
| **MagicBitDriver** | `MagicBitDriver.h/cpp` | Low-level PCA9685 I2C driver |
| **WomBotLegs** | `WomBotLegs.h/cpp` | 4-leg gait controller |
| **WomBotHead** | `WomBotHead.h/cpp` | Head servo pan/scan |
| **SonarBit** | `SonarBit.h/cpp` | HC-SR04 ultrasonic distance sensor |
| **ClueDisplay** | `ClueDisplay.h/cpp` | ST7789 TFT display (240x240) |

### 5.2 MagicBitDriver

Low-level PCA9685 driver. Communicates over I2C via the Arduino `Wire` library.

**Public interface:**

```cpp
class MagicBitDriver {
  static const uint8_t PCA9685_ADDR = 0x40;
  static const uint8_t NUM_SERVO_CHANNELS = 16;
  static const uint16_t SERVO_MIN_US = 600;
  static const uint16_t SERVO_MAX_US = 2400;
  static const uint16_t PWM_FREQ = 50;

  bool begin();                                    // Init PCA9685, returns false if not found
  void setServo(uint8_t channel, float degrees);   // 0-180 degrees
  void setServoPulse(uint8_t channel, uint16_t pulseUs);
  void setAllServosOff();
};
```

### 5.3 WomBotLegs

Controls 4 leg servos. Supports mirrored servo orientation and per-leg calibration offsets.

**Constants:**

| Constant | Value | Description |
|----------|-------|-------------|
| NEUTRAL_ANGLE | 90.0 | Center position (degrees) |
| STRIDE_ANGLE | 25.0 | Forward/backward swing from center |
| TURN_ANGLE | 30.0 | Turning swing amplitude |
| _stepDelayMs | 200 | Delay between gait phases (ms) |

**Servo reversal**: Left-side servos (indices 0, 2) are reversed. In `setLeg()`:
```cpp
float angle = degrees + _offsets[leg];
if (LEG_REVERSED[leg]) angle = 180.0f - angle;
_driver.setServo(LEG_CHANNELS[leg], angle);
```

**Implemented gaits:**

| Method | Pattern | Phases |
|--------|---------|--------|
| `walkForward(steps)` | Trot gait — diagonal pairs (FL+BR, FR+BL) | 4 |
| `walkBackward(steps)` | Reverse trot | 4 |
| `turnLeft(steps)` | Left legs back, right legs forward, alternating | 2 |
| `turnRight(steps)` | Right legs back, left legs forward, alternating | 2 |
| `setNeutral()` | All legs to NEUTRAL_ANGLE | 1 |

### 5.4 WomBotHead

Single servo for head panning. Used for directional scanning with the Sonar:bit.

| Constant | Value | Description |
|----------|-------|-------------|
| HEAD_CHANNEL | 9 | PCA9685 channel (S2) |
| CENTER_ANGLE | 90.0 | Straight ahead |
| LEFT_ANGLE | 140.0 | Maximum left (+50) |
| RIGHT_ANGLE | 40.0 | Maximum right (-50) |
| SCAN_STEP | 5.0 | Sweep increment |

**`scan()`** sweeps center → left → right → center in 5-degree steps with 30 ms delay per step.

### 5.5 SonarBit

ElecFreaks Sonar:bit driver. Single-pin HC-SR04 variant — the same pin switches between output (trigger) and input (echo).

| Parameter | Value |
|-----------|-------|
| Signal pin | P2 (pin 2) |
| Trigger pulse | 10 us HIGH |
| Timeout | 25,000 us (~430 cm) |
| Valid range | 4-400 cm |
| Calculation | `distance_cm = pulse_us / 58.0` |
| Out-of-range return | 0.0 |

### 5.6 ClueDisplay

Wrapper for the CLUE's 240x240 ST7789 TFT display using hardware SPI1.

| Pin | Function | CLUE pin |
|-----|----------|----------|
| TFT_CS | Chip select | 31 |
| TFT_DC | Data/command | 32 |
| TFT_RST | Reset | 33 |
| TFT_BACKLIGHT | Backlight control | 34 |

**Display methods:**

| Method | Description |
|--------|-------------|
| `showSplash(title)` | Startup screen with title and board info |
| `showMode(name, active)` | Current mode name + RUNNING/STOPPED status |
| `showDistance(cm)` | Distance readout with color coding (red < 15 cm, yellow < 30 cm, green) |
| `showScan(left, front, right)` | Three-way scan results with turn recommendation |
| `showText(text)` | Generic text overlay in the bottom area |

### 5.7 Mode System

The main loop implements a state machine with 4 modes, controlled by two buttons:

| Button | Action |
|--------|--------|
| A (pin 5) | Cycle to next mode, stop current activity |
| B (pin 11) | Toggle start/stop within current mode |

Debounce: 250 ms.

| Mode | Name | Behavior |
|------|------|----------|
| MODE_IDLE | Idle | No activity, waits for input |
| MODE_AUTONOMOUS | Autonomous | Obstacle avoidance: walk forward, scan when obstacle < 15 cm, turn to clearest direction |
| MODE_DEMO | Demo | 10-step predefined sequence: neutral → scan → walk → turn → walk → ... |
| MODE_SENSOR_DISPLAY | Sensors | Show sonar distance on display, updated every 200 ms |

**Autonomous algorithm:**
1. Read sonar distance
2. If < 15 cm: stop, scan left/right with head, turn towards clearer side
3. If both sides blocked: turn left (180-degree turn, 6 steps)
4. If 15-30 cm: walk forward slowly (1 step)
5. If > 30 cm: walk forward (1 step)

---

## 6. Pin Assignment Summary

### 6.1 CLUE Pins Used

| Pin | nRF52840 | Function |
|-----|----------|----------|
| P2 | P0.04 | Sonar:bit signal |
| P5 | P0.30 | Button A |
| P11 | P0.31 | Button B |
| P19 | P0.25 | I2C SCL (to PCA9685) |
| P20 | P0.24 | I2C SDA (to PCA9685) |
| 31 | — | TFT CS |
| 32 | — | TFT DC |
| 33 | — | TFT RST |
| 34 | — | TFT Backlight |
| SPI1 | — | TFT data (hardware SPI) |

### 6.2 Magic:bit Pins Used

| Pin | Function |
|-----|----------|
| P0 | Buzzer (passive, via jumper) |
| P16 | 4x NeoPixel RGB LEDs |
| S1-S5 | Servo outputs (see section 4) |
| I2C (edge connector) | PCA9685 communication |

### 6.3 Available for Expansion

| Resource | Available |
|----------|-----------|
| Servo connectors | S6, S7, S8 (3 free) |
| DC motor outputs | M1-M4 (4 available, PCA9685 ch 0-7) |
| Stepper connectors | Stepper Moto 1, Stepper Moto 2 (share ch with motors) |
| Edge connector GPIO | P0, P1, P8, P12, P13, P14, P15 |
| STEMMA QT | I2C expansion port (unused) |
| BLE | Not yet implemented |
| On-board sensors | Accel/Gyro, Magnetometer, Proximity, Microphone, Humidity, Pressure — all unused |
| IR receiver | On Magic:bit, not yet implemented |
| NeoPixel LEDs | 4x on P16, not yet implemented |
| Buzzer | On P0, not yet implemented |

---

## 7. Build Configuration

| Setting | Value |
|---------|-------|
| Board | Adafruit CLUE (nRF52840) |
| Arduino BSP | Adafruit nRF52 |
| PlatformIO board | `adafruit_clue_nrf52840` |
| Serial | Via `Adafruit_TinyUSB`, 115200 baud |

### Required Arduino Libraries

| Library | Purpose |
|---------|---------|
| `Wire` | I2C communication |
| `Adafruit_TinyUSB` | USB Serial for nRF52840 |
| `Adafruit_GFX` | Graphics primitives |
| `Adafruit_ST7789` | TFT display driver |
| `SPI` | Hardware SPI for display |

### Additional Libraries (for future features)

| Library | Purpose |
|---------|---------|
| `Adafruit_NeoPixel` | RGB LED control (P16) |
| `Adafruit_LSM6DS` | Accelerometer / Gyroscope |
| `Adafruit_LIS3MDL` | Magnetometer (compass) |
| `Adafruit_APDS9960` | Proximity / Light / Color / Gesture |
| `Adafruit_BMP280` | Barometric pressure / Temperature |
| `Adafruit_SHT31` | Humidity |
| `IRremote` | IR receiver decoding |
| `Bluefruit` / `Adafruit_nRF52_BLE` | Bluetooth LE |

---

## 8. Known Issues and Notes

### 8.1 PCA9685 Channel Offset

The Python library `magicbit/Magicbit.py` (from emakefun) has a bug: the `servo(index, degree)` function passes the servo index directly as the PCA9685 channel, missing the +7 offset. The official TypeScript library (`emakefun/pxt-magicbit`) correctly uses `setPwm(index + 7, 0, value)`.

### 8.2 PCA9685 Auto-Increment Bit

The MODE1 register must have the AI bit (bit 5 = 0x20) set for multi-byte I2C writes to work. Without it, the `setPWM()` function writes all 4 data bytes to the same register instead of advancing through ON_L/ON_H/OFF_L/OFF_H. The original Python code does not need this because it writes each register in a separate I2C transaction.

### 8.3 I2C Bus Sharing

The CLUE's on-board sensors and the Magic:bit's PCA9685 share the same I2C bus (Wire, pins P19/P20). This works because they use different addresses. If more I2C devices are added (via STEMMA QT), address conflicts must be avoided.

### 8.4 CircuitPython I2C Pin Error

The legacy `code.py` (CircuitPython for CLUE) uses `busio.I2C(board.P0, board.P1)` which are the large edge connector pads, **not** the I2C pins. The correct pins are `board.P19` / `board.P20` (or `board.SCL` / `board.SDA`).

### 8.5 Servo Power

Servos draw significant current. Under weak battery conditions, I2C communication may succeed (PCA9685 responds) but servos may not move due to insufficient current. Use a fully charged 18650 or external 5V supply.

### 8.6 micro:bit vs CLUE Compatibility

The edge connector is mechanically and electrically compatible. I2C pins (P19/P20) are identical. The Magic:bit V2 works with the CLUE without hardware modifications. Key differences:

| | micro:bit V2 | Adafruit CLUE |
|--|-------------|---------------|
| CPU | nRF52833, 64 MHz | nRF52840, 64 MHz |
| RAM | 128 KB | 256 KB |
| Flash | 512 KB | 1 MB |
| Display | 5x5 LED matrix | 1.3" 240x240 TFT |
| I2C bus | Separate internal/external (V2) | Single shared bus |

---

## 9. References

- [emakefun/pxt-magicbit](https://github.com/emakefun/pxt-magicbit) — Official MakeCode extension (TypeScript, servo channel mapping source)
- [emakefun/python-lib](https://github.com/emakefun/python-lib) — Python library (has servo mapping bug)
- [Adafruit CLUE — Learn Guide](https://learn.adafruit.com/adafruit-clue)
- [Adafruit CLUE — Pinouts](https://learn.adafruit.com/adafruit-clue/pinouts)
- [PCA9685 Datasheet](https://www.nxp.com/docs/en/data-sheet/PCA9685.pdf)
- [Adafruit PWM Servo Driver Library](https://github.com/adafruit/Adafruit-PWM-Servo-Driver-Library)
