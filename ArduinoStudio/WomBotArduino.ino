/*
 * WomBot - Walking Robot based on Adafruit CLUE + MagicBit V2.0
 *
 * Hardware:
 *   - Adafruit CLUE (nRF52840) as brain
 *   - Emakefun MagicBit V2.0 (PCA9685) for servo control
 *   - ElecFreaks Sonar:bit for distance measurement
 *   - Servos on legs + head
 *
 * Board: Adafruit CLUE (nRF52840) in Arduino IDE
 * Required libraries: Adafruit_PWMServoDriver, Adafruit_GFX, Adafruit_ST7789
 */

#include <Adafruit_TinyUSB.h>  // Required for nRF52840 Serial
#include <Wire.h>
#include "MagicBitDriver.h"
#include "SonarBit.h"
#include "WomBotLegs.h"
#include "WomBotHead.h"
#include "ClueDisplay.h"

// --- Pin Configuration ---
static const int SONAR_PIN = 2;       // Sonar:bit signal pin (P2 on edge connector)
static const int BUTTON_A_PIN = 5;    // CLUE Button A
static const int BUTTON_B_PIN = 11;   // CLUE Button B

// --- Robot Modes ---
enum RobotMode {
  MODE_IDLE,
  MODE_AUTONOMOUS,
  MODE_DEMO,
  MODE_SENSOR_DISPLAY,
  MODE_COUNT
};

static const char* MODE_NAMES[] = {
  "Idle", "Autonomous", "Demo", "Sensors"
};

// --- Global Objects ---
MagicBitDriver magicbit;
SonarBit sonar(SONAR_PIN);
WomBotLegs legs(magicbit);
WomBotHead head(magicbit);
ClueDisplay display;

RobotMode currentMode = MODE_IDLE;
bool modeActive = false;
unsigned long lastButtonATime = 0;
unsigned long lastButtonBTime = 0;
static const unsigned long DEBOUNCE_MS = 250;

// --- Forward declarations ---
void runAutonomous();
void runDemo();
void runSensorDisplay();

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("WomBot 1.0 starting...");

  pinMode(BUTTON_A_PIN, INPUT_PULLUP);
  pinMode(BUTTON_B_PIN, INPUT_PULLUP);

  Wire.begin();

  if (!magicbit.begin()) {
    Serial.println("ERROR: PCA9685 not found! Check MagicBit connection.");
    while (1) delay(100);
  }
  Serial.println("MagicBit (PCA9685) initialized.");

  display.begin();
  display.showSplash("WomBot 1.0");
  delay(1500);

  legs.setNeutral();
  head.lookCenter();

  display.showMode(MODE_NAMES[currentMode], modeActive);
  Serial.println("Ready!");
}

void loop() {
  // Button A: cycle mode
  if (digitalRead(BUTTON_A_PIN) == LOW && millis() - lastButtonATime > DEBOUNCE_MS) {
    lastButtonATime = millis();
    modeActive = false;
    currentMode = (RobotMode)((currentMode + 1) % MODE_COUNT);
    legs.stop();
    head.lookCenter();
    display.showMode(MODE_NAMES[currentMode], modeActive);
    Serial.print("Mode: ");
    Serial.println(MODE_NAMES[currentMode]);
    delay(200);
  }

  // Button B: start/stop within mode
  if (digitalRead(BUTTON_B_PIN) == LOW && millis() - lastButtonBTime > DEBOUNCE_MS) {
    lastButtonBTime = millis();
    modeActive = !modeActive;
    if (!modeActive) {
      legs.stop();
      head.lookCenter();
    }
    display.showMode(MODE_NAMES[currentMode], modeActive);
    Serial.print(modeActive ? "Started: " : "Stopped: ");
    Serial.println(MODE_NAMES[currentMode]);
    delay(200);
  }

  if (!modeActive) return;

  switch (currentMode) {
    case MODE_AUTONOMOUS:
      runAutonomous();
      break;
    case MODE_DEMO:
      runDemo();
      break;
    case MODE_SENSOR_DISPLAY:
      runSensorDisplay();
      break;
    default:
      break;
  }
}

// --- Autonomous obstacle avoidance ---
void runAutonomous() {
  static const float OBSTACLE_CLOSE = 15.0;   // cm
  static const float OBSTACLE_FAR = 30.0;     // cm

  float dist = sonar.getDistanceCm();
  display.showDistance(dist);

  if (dist > 0 && dist < OBSTACLE_CLOSE) {
    // Too close - stop and scan
    legs.stop();
    delay(200);

    head.lookLeft();
    delay(400);
    float leftDist = sonar.getDistanceCm();

    head.lookRight();
    delay(400);
    float rightDist = sonar.getDistanceCm();

    head.lookCenter();
    delay(200);

    display.showScan(leftDist, dist, rightDist);

    if (leftDist > rightDist && leftDist > OBSTACLE_CLOSE) {
      legs.turnLeft(3);
    } else if (rightDist > OBSTACLE_CLOSE) {
      legs.turnRight(3);
    } else {
      // Both sides blocked - turn around
      legs.turnLeft(6);
    }
  } else if (dist > 0 && dist < OBSTACLE_FAR) {
    // Getting close - slow walk
    legs.walkForward(1);
  } else {
    // Clear path - walk
    legs.walkForward(1);
  }
}

// --- Demo sequence ---
void runDemo() {
  static int step = 0;

  display.showText("Demo step " + String(step));

  switch (step) {
    case 0: legs.setNeutral(); delay(1000); break;
    case 1: head.scan(); delay(500); break;
    case 2: legs.walkForward(4); break;
    case 3: legs.stop(); head.lookLeft(); delay(500); break;
    case 4: legs.turnRight(3); break;
    case 5: legs.walkForward(4); break;
    case 6: legs.stop(); head.lookRight(); delay(500); break;
    case 7: legs.turnLeft(3); break;
    case 8: legs.walkForward(2); break;
    case 9: legs.setNeutral(); head.lookCenter(); delay(1000); break;
  }

  step = (step + 1) % 10;
}

// --- Show sensor readings ---
void runSensorDisplay() {
  float dist = sonar.getDistanceCm();
  display.showDistance(dist);
  delay(200);
}
