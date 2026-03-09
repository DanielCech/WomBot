/*
 * SonarBit.cpp - ElecFreaks Sonar:bit driver
 *
 * Single-pin HC-SR04 protocol: the same pin is used for trigger (output)
 * and echo (input). Send a 10us pulse, then measure the return pulse width.
 */

#include "SonarBit.h"

SonarBit::SonarBit(uint8_t pin) : _pin(pin) {}

float SonarBit::getDistanceCm() {
  // Send trigger pulse
  pinMode(_pin, OUTPUT);
  digitalWrite(_pin, LOW);
  delayMicroseconds(2);
  digitalWrite(_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(_pin, LOW);

  // Switch to input and measure echo
  pinMode(_pin, INPUT);
  unsigned long duration = pulseIn(_pin, HIGH, TIMEOUT_US);

  if (duration == 0) return 0;  // No echo (out of range)

  float distance = duration / 58.0f;

  if (distance < 4.0f || distance > 400.0f) return 0;

  return distance;
}
