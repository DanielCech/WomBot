/*
 * WomBotLegs.h - Walking gait controller
 *
 * Controls leg servos via MagicBitDriver to produce walking gaits.
 * Default assumes 4 legs, one servo per leg, on MagicBit channels S1-S4.
 *
 * IMPORTANT: Servo channel assignments and neutral angles must be
 * calibrated for the physical robot! Adjust the constants below.
 */

#ifndef WOMBOT_LEGS_H
#define WOMBOT_LEGS_H

#include "MagicBitDriver.h"

class WomBotLegs {
public:
  // Servo channel assignments (MagicBit S1-S4 = PCA9685 channels 0-3)
  static const uint8_t NUM_LEGS = 4;
  static const uint8_t LEG_CHANNELS[NUM_LEGS];

  // Gait parameters (degrees)
  static const float NEUTRAL_ANGLE;    // Center position
  static const float STRIDE_ANGLE;     // How far each leg swings from center
  static const float TURN_ANGLE;       // Angle offset for turning

  WomBotLegs(MagicBitDriver& driver);

  void setNeutral();
  void walkForward(int steps);
  void walkBackward(int steps);
  void turnLeft(int steps);
  void turnRight(int steps);
  void stop();

  // Calibration: set per-leg offsets (added to all angles)
  void setLegOffset(uint8_t leg, float offset);

private:
  MagicBitDriver& _driver;
  float _offsets[NUM_LEGS];
  uint16_t _stepDelayMs;

  void setLeg(uint8_t leg, float degrees);
  void gaitStep(float angles[NUM_LEGS]);
  void executeGait(const float gait[][NUM_LEGS], int numPhases, int steps);
};

#endif
