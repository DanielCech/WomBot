/*
 * MagicBitDriver.h - PCA9685 servo/motor driver for MagicBit V2.0
 *
 * The MagicBit V2.0 expansion board uses a PCA9685 PWM controller
 * at I2C address 0x40 to drive 8 servo outputs (S1-S8) and 4 DC motors.
 * Servo channels map to PCA9685 channels 0-7 (S1=ch0 ... S8=ch7).
 */

#ifndef MAGICBIT_DRIVER_H
#define MAGICBIT_DRIVER_H

#include <Wire.h>

class MagicBitDriver {
public:
  static const uint8_t PCA9685_ADDR = 0x40;
  static const uint8_t NUM_SERVO_CHANNELS = 8;

  // Servo pulse range in microseconds
  static const uint16_t SERVO_MIN_US = 600;
  static const uint16_t SERVO_MAX_US = 2400;
  static const uint16_t PWM_FREQ = 50;

  bool begin();
  void setServo(uint8_t channel, float degrees);
  void setServoPulse(uint8_t channel, uint16_t pulseUs);
  void setAllServosOff();

private:
  void setPWMFreq(uint16_t freq);
  void setPWM(uint8_t channel, uint16_t on, uint16_t off);
  void writeReg(uint8_t reg, uint8_t val);
  uint8_t readReg(uint8_t reg);
};

#endif
