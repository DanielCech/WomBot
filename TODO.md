# WomBot - TODO / Implementation Plan

## Project Overview

WomBot is a small walking robot (wombat-inspired) controlled by **Adafruit CLUE** (nRF52840)
mounted on a **MagicBit V2.0** expansion board (PCA9685-based servo/motor driver).

### Hardware Components
- **Brain**: Adafruit CLUE (nRF52840, micro:bit edge connector compatible)
- **Expansion board**: Emakefun MagicBit V2.0 (PCA9685 @ I2C address 0x40, 8 servo ports S1-S8)
- **Servos**: One per leg + one for head movement
- **Distance sensor**: ElecFreaks Sonar:bit (3-wire, HC-SR04 protocol, single signal pin, 3-5V)
- **Onboard sensors**: Accelerometer, gyroscope, proximity, temperature, humidity, barometric pressure

### Key Technical Details
- MagicBit connects via edge connector; PCA9685 communicates over I2C (SCL=P19, SDA=P20)
- PCA9685 frequency for servos: 50 Hz
- Servo pulse: 600-2400us maps to 0-180 degrees
- Sonar:bit protocol: 10us trigger pulse on signal pin, then pulseIn to read echo (distance = pulse_us / 58 cm)
- Adafruit CLUE I2C also available via STEMMA QT connector

### Platform Decision: **Arduino IDE (C/C++)**
- Adafruit provides full Arduino BSP for CLUE (nRF52840)
- Excellent library support: `Adafruit_PWMServoDriver` for PCA9685, `Adafruit_CLUE` for onboard sensors
- Better performance for real-time servo control vs. CircuitPython
- PlatformIO also supported (board: `adafruit_clue_nrf52840`)

---

## Phase 1: Project Setup

- [ ] Create Arduino project structure (`WomBot.ino` + headers)
- [ ] Configure for Adafruit CLUE board (nRF52840)
- [ ] Add required library dependencies:
  - `Adafruit_PWMServoDriver` (PCA9685 control)
  - `Adafruit_GFX` + `Adafruit_ST7789` (TFT display)
  - `Adafruit_Sensor` (unified sensor framework)
  - `Adafruit_LSM6DS` (accelerometer/gyro)
  - `Adafruit_LIS3MDL` (magnetometer)
  - `Adafruit_APDS9960` (proximity/gesture/color)
  - `Adafruit_BMP280` (pressure/temperature)
  - `Adafruit_SHT31` (humidity)
  - `Wire` (I2C)
  - `Adafruit_TinyUSB` (required for nRF52840 Serial)

## Phase 2: Hardware Abstraction Layer

- [ ] **MagicBitDriver** - PCA9685 servo control class
  - Initialize I2C communication with PCA9685 (address 0x40)
  - Set PWM frequency to 50 Hz
  - `setServo(channel, degrees)` - map 0-180 degrees to PWM pulse width
  - `setAllServosOff()` - safety stop

- [ ] **SonarBit** - Ultrasonic distance sensor class
  - Configure signal pin (digital I/O)
  - `getDistanceCm()` - trigger pulse + measure echo, return distance in cm
  - Timeout handling (max ~400cm / 25ms)

- [ ] **ClueDisplay** - TFT display wrapper
  - Show robot status, sensor readings, current mode
  - Simple text-based UI

## Phase 3: Robot Kinematics

- [ ] **WomBotLegs** - Walking gait controller
  - Define servo-to-leg mapping (which PCA9685 channel = which leg)
  - Define neutral/center positions for each servo
  - Implement basic gaits:
    - `standNeutral()` - all legs at rest position
    - `walkForward()` - coordinated leg movement pattern
    - `walkBackward()`
    - `turnLeft()` / `turnRight()`
    - `stop()` - return to neutral

- [ ] **WomBotHead** - Head servo controller
  - Define head servo channel
  - `lookCenter()`, `lookLeft()`, `lookRight()`
  - `scan()` - sweep head to find obstacles

## Phase 4: Autonomous Behavior

- [ ] **ObstacleAvoidance** - Basic autonomous navigation
  - Use Sonar:bit for forward distance measurement
  - Head scanning for left/right obstacle detection
  - Decision logic: if obstacle < threshold, stop, scan, turn to clearest direction
  - Walk forward when path is clear

- [ ] **SensorIntegration** - Use CLUE onboard sensors
  - Accelerometer for tilt detection (fall prevention)
  - Proximity sensor for close-range detection
  - Button A/B for mode switching

## Phase 5: User Interface

- [ ] **Mode System**
  - Button A: cycle through modes (Manual, Autonomous, Demo, Sensor Display)
  - Button B: action within mode (start/stop, trigger action)
  - Display current mode and status on TFT

- [ ] **Demo Mode**
  - Pre-programmed sequence: walk, scan, react to obstacles
  - Show distance readings on display

## Phase 6: Polish & Extras

- [ ] Servo calibration utility (adjust center positions per servo)
- [ ] BLE remote control (optional, CLUE has BLE built-in)
- [ ] Sound effects via buzzer (if connected to MagicBit P0)
- [ ] NeoPixel status indicators (MagicBit has 4 RGB LEDs on P16)

---

## Servo Channel Assignment (to be confirmed with physical robot)

| Channel | Function        | Neutral Angle |
|---------|----------------|---------------|
| S1 (0)  | Front Left Leg  | 90           |
| S2 (1)  | Front Right Leg | 90           |
| S3 (2)  | Back Left Leg   | 90           |
| S4 (3)  | Back Right Leg  | 90           |
| S5 (4)  | Head            | 90           |

> Note: Channel assignments and neutral angles need to be calibrated
> with the physical robot. The mapping above is a starting point.

## Sonar:bit Connection

| Wire    | Connection      |
|---------|----------------|
| G       | GND            |
| V       | 3.3V           |
| S       | P1 or P2 (configurable) |
