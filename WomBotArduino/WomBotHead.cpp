/*
 * WomBotHead.cpp - Head servo controller
 */

#include <Arduino.h>
#include "WomBotHead.h"

const float WomBotHead::CENTER_ANGLE = 90.0f;
const float WomBotHead::LEFT_ANGLE = 140.0f;
const float WomBotHead::RIGHT_ANGLE = 40.0f;
const float WomBotHead::SCAN_STEP = 5.0f;

WomBotHead::WomBotHead(MagicBitDriver& driver)
  : _driver(driver), _currentAngle(CENTER_ANGLE) {}

void WomBotHead::lookCenter() {
  setAngle(CENTER_ANGLE);
}

void WomBotHead::lookLeft() {
  setAngle(LEFT_ANGLE);
}

void WomBotHead::lookRight() {
  setAngle(RIGHT_ANGLE);
}

void WomBotHead::setAngle(float degrees) {
  if (degrees < 0) degrees = 0;
  if (degrees > 180) degrees = 180;
  _currentAngle = degrees;
  _driver.setServo(HEAD_CHANNEL, degrees);
}

void WomBotHead::scan() {
  // Sweep from center to left
  for (float a = _currentAngle; a <= LEFT_ANGLE; a += SCAN_STEP) {
    setAngle(a);
    delay(30);
  }
  // Sweep from left to right
  for (float a = LEFT_ANGLE; a >= RIGHT_ANGLE; a -= SCAN_STEP) {
    setAngle(a);
    delay(30);
  }
  // Return to center
  for (float a = RIGHT_ANGLE; a <= CENTER_ANGLE; a += SCAN_STEP) {
    setAngle(a);
    delay(30);
  }
}
