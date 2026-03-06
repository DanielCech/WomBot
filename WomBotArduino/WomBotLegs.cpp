/*
 * WomBotLegs.cpp - Walking gait controller
 *
 * Uses a simple alternating gait pattern:
 *   Phase 1: Front-Left + Back-Right swing forward, others swing back
 *   Phase 2: Front-Right + Back-Left swing forward, others swing back
 *
 * This creates a diagonal walking pattern (trot gait).
 *
 * Servo channels (ADJUST THESE for your robot):
 *   S1 (ch 0) = Front Left
 *   S2 (ch 1) = Front Right
 *   S3 (ch 2) = Back Left
 *   S4 (ch 3) = Back Right
 */

#include <Arduino.h>
#include "WomBotLegs.h"

const uint8_t WomBotLegs::LEG_CHANNELS[NUM_LEGS] = {0, 1, 2, 3};

const float WomBotLegs::NEUTRAL_ANGLE = 90.0f;
const float WomBotLegs::STRIDE_ANGLE = 25.0f;
const float WomBotLegs::TURN_ANGLE = 30.0f;

WomBotLegs::WomBotLegs(MagicBitDriver& driver)
  : _driver(driver), _stepDelayMs(200) {
  for (int i = 0; i < NUM_LEGS; i++) _offsets[i] = 0;
}

void WomBotLegs::setLeg(uint8_t leg, float degrees) {
  if (leg >= NUM_LEGS) return;
  _driver.setServo(LEG_CHANNELS[leg], degrees + _offsets[leg]);
}

void WomBotLegs::setLegOffset(uint8_t leg, float offset) {
  if (leg < NUM_LEGS) _offsets[leg] = offset;
}

void WomBotLegs::setNeutral() {
  for (int i = 0; i < NUM_LEGS; i++) {
    setLeg(i, NEUTRAL_ANGLE);
  }
  delay(_stepDelayMs);
}

void WomBotLegs::stop() {
  setNeutral();
}

void WomBotLegs::gaitStep(float angles[NUM_LEGS]) {
  for (int i = 0; i < NUM_LEGS; i++) {
    setLeg(i, angles[i]);
  }
  delay(_stepDelayMs);
}

void WomBotLegs::executeGait(const float gait[][NUM_LEGS], int numPhases, int steps) {
  for (int s = 0; s < steps; s++) {
    for (int p = 0; p < numPhases; p++) {
      float angles[NUM_LEGS];
      for (int i = 0; i < NUM_LEGS; i++) angles[i] = gait[p][i];
      gaitStep(angles);
    }
  }
}

void WomBotLegs::walkForward(int steps) {
  // Trot gait: diagonal pairs move together
  //                     FL                   FR                   BL                   BR
  const float gait[4][NUM_LEGS] = {
    {NEUTRAL_ANGLE + STRIDE_ANGLE, NEUTRAL_ANGLE,                NEUTRAL_ANGLE,                NEUTRAL_ANGLE + STRIDE_ANGLE},   // FL+BR forward
    {NEUTRAL_ANGLE + STRIDE_ANGLE, NEUTRAL_ANGLE,                NEUTRAL_ANGLE,                NEUTRAL_ANGLE + STRIDE_ANGLE},   // Hold
    {NEUTRAL_ANGLE,                NEUTRAL_ANGLE + STRIDE_ANGLE, NEUTRAL_ANGLE + STRIDE_ANGLE, NEUTRAL_ANGLE},                  // FR+BL forward
    {NEUTRAL_ANGLE,                NEUTRAL_ANGLE + STRIDE_ANGLE, NEUTRAL_ANGLE + STRIDE_ANGLE, NEUTRAL_ANGLE},                  // Hold
  };
  executeGait(gait, 4, steps);
}

void WomBotLegs::walkBackward(int steps) {
  const float gait[4][NUM_LEGS] = {
    {NEUTRAL_ANGLE - STRIDE_ANGLE, NEUTRAL_ANGLE,                NEUTRAL_ANGLE,                NEUTRAL_ANGLE - STRIDE_ANGLE},
    {NEUTRAL_ANGLE - STRIDE_ANGLE, NEUTRAL_ANGLE,                NEUTRAL_ANGLE,                NEUTRAL_ANGLE - STRIDE_ANGLE},
    {NEUTRAL_ANGLE,                NEUTRAL_ANGLE - STRIDE_ANGLE, NEUTRAL_ANGLE - STRIDE_ANGLE, NEUTRAL_ANGLE},
    {NEUTRAL_ANGLE,                NEUTRAL_ANGLE - STRIDE_ANGLE, NEUTRAL_ANGLE - STRIDE_ANGLE, NEUTRAL_ANGLE},
  };
  executeGait(gait, 4, steps);
}

void WomBotLegs::turnLeft(int steps) {
  // All left legs forward, all right legs back
  const float gait[2][NUM_LEGS] = {
    {NEUTRAL_ANGLE - TURN_ANGLE, NEUTRAL_ANGLE + TURN_ANGLE, NEUTRAL_ANGLE - TURN_ANGLE, NEUTRAL_ANGLE + TURN_ANGLE},
    {NEUTRAL_ANGLE + TURN_ANGLE, NEUTRAL_ANGLE - TURN_ANGLE, NEUTRAL_ANGLE + TURN_ANGLE, NEUTRAL_ANGLE - TURN_ANGLE},
  };
  executeGait(gait, 2, steps);
}

void WomBotLegs::turnRight(int steps) {
  const float gait[2][NUM_LEGS] = {
    {NEUTRAL_ANGLE + TURN_ANGLE, NEUTRAL_ANGLE - TURN_ANGLE, NEUTRAL_ANGLE + TURN_ANGLE, NEUTRAL_ANGLE - TURN_ANGLE},
    {NEUTRAL_ANGLE - TURN_ANGLE, NEUTRAL_ANGLE + TURN_ANGLE, NEUTRAL_ANGLE - TURN_ANGLE, NEUTRAL_ANGLE + TURN_ANGLE},
  };
  executeGait(gait, 2, steps);
}
