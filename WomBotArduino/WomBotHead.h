/*
 * WomBotHead.h - Head servo controller
 *
 * Controls a single servo that pans the head left/right.
 * Used for directional scanning with the Sonar:bit sensor.
 */

#ifndef WOMBOT_HEAD_H
#define WOMBOT_HEAD_H

#include "MagicBitDriver.h"

class WomBotHead {
public:
  static const uint8_t HEAD_CHANNEL = 9;  // MagicBit S2 = PCA9685 channel 9 (hlava)

  static const float CENTER_ANGLE;
  static const float LEFT_ANGLE;
  static const float RIGHT_ANGLE;
  static const float SCAN_STEP;

  WomBotHead(MagicBitDriver& driver);

  void lookCenter();
  void lookLeft();
  void lookRight();
  void setAngle(float degrees);

  // Sweep head from left to right and back
  void scan();

private:
  MagicBitDriver& _driver;
  float _currentAngle;
};

#endif
