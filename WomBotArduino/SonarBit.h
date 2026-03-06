/*
 * SonarBit.h - ElecFreaks Sonar:bit ultrasonic distance sensor
 *
 * 3-wire sensor using HC-SR04 protocol on a single signal pin.
 * Protocol: send 10us trigger pulse, then measure echo pulse width.
 * Distance = pulse_us / 58 (cm)
 * Range: 4-400 cm
 */

#ifndef SONARBIT_H
#define SONARBIT_H

#include <Arduino.h>

class SonarBit {
public:
  SonarBit(uint8_t pin);

  // Returns distance in centimeters (0 if no echo / out of range)
  float getDistanceCm();

private:
  uint8_t _pin;
  static const unsigned long TIMEOUT_US = 25000;  // ~430cm max
};

#endif
