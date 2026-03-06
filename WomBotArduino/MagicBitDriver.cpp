/*
 * MagicBitDriver.cpp - PCA9685 driver for MagicBit V2.0
 *
 * Direct I2C communication with PCA9685, matching the protocol
 * from the original MagicBit Python library (Magicbit.py).
 */

#include <Arduino.h>
#include "MagicBitDriver.h"
#include <math.h>

// PCA9685 registers
#define PCA9685_MODE1     0x00
#define PCA9685_MODE2     0x01
#define PCA9685_PRESCALE  0xFE
#define PCA9685_LED0_ON_L 0x06

bool MagicBitDriver::begin() {
  // Reset PCA9685
  writeReg(PCA9685_MODE1, 0x00);
  writeReg(PCA9685_MODE2, 0x04);

  // Set auto-increment
  writeReg(PCA9685_MODE1, 0x01);
  delay(5);

  uint8_t mode1 = readReg(PCA9685_MODE1);
  if (mode1 == 0xFF) return false;  // No device found

  mode1 &= ~0x10;  // Wake up (clear sleep bit)
  writeReg(PCA9685_MODE1, mode1);
  delay(5);

  setPWMFreq(PWM_FREQ);
  setAllServosOff();

  return true;
}

void MagicBitDriver::setServo(uint8_t channel, float degrees) {
  if (channel >= NUM_SERVO_CHANNELS) return;
  if (degrees < 0) degrees = 0;
  if (degrees > 180) degrees = 180;

  // Map degrees to pulse width in microseconds
  uint16_t pulseUs = SERVO_MIN_US + (uint16_t)((degrees / 180.0f) * (SERVO_MAX_US - SERVO_MIN_US));
  setServoPulse(channel, pulseUs);
}

void MagicBitDriver::setServoPulse(uint8_t channel, uint16_t pulseUs) {
  if (channel >= NUM_SERVO_CHANNELS) return;

  // At 50Hz, one period = 20000us, 12-bit resolution = 4096 ticks
  // ticks = pulseUs * 4096 / 20000
  uint16_t ticks = (uint16_t)((uint32_t)pulseUs * 4096 / 20000);
  setPWM(channel, 0, ticks);
}

void MagicBitDriver::setAllServosOff() {
  for (uint8_t i = 0; i < 16; i++) {
    setPWM(i, 0, 0);
  }
}

void MagicBitDriver::setPWMFreq(uint16_t freq) {
  float prescaleval = 25000000.0f;
  prescaleval /= 4096.0f;
  prescaleval /= (float)freq;
  prescaleval -= 1.0f;
  uint8_t prescale = (uint8_t)floor(prescaleval + 0.5f);

  uint8_t oldmode = readReg(PCA9685_MODE1);
  uint8_t newmode = (oldmode & 0x7F) | 0x10;  // Sleep
  writeReg(PCA9685_MODE1, newmode);
  writeReg(PCA9685_PRESCALE, prescale);
  writeReg(PCA9685_MODE1, oldmode);
  delay(5);
  writeReg(PCA9685_MODE1, oldmode | 0x80);  // Restart
}

void MagicBitDriver::setPWM(uint8_t channel, uint16_t on, uint16_t off) {
  uint8_t reg = PCA9685_LED0_ON_L + 4 * channel;
  Wire.beginTransmission(PCA9685_ADDR);
  Wire.write(reg);
  Wire.write(on & 0xFF);
  Wire.write(on >> 8);
  Wire.write(off & 0xFF);
  Wire.write(off >> 8);
  Wire.endTransmission();
}

void MagicBitDriver::writeReg(uint8_t reg, uint8_t val) {
  Wire.beginTransmission(PCA9685_ADDR);
  Wire.write(reg);
  Wire.write(val);
  Wire.endTransmission();
}

uint8_t MagicBitDriver::readReg(uint8_t reg) {
  Wire.beginTransmission(PCA9685_ADDR);
  Wire.write(reg);
  Wire.endTransmission();
  Wire.requestFrom((uint8_t)PCA9685_ADDR, (uint8_t)1);
  if (Wire.available()) return Wire.read();
  return 0xFF;
}
