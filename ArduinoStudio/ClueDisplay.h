/*
 * ClueDisplay.h - Adafruit CLUE 1.3" TFT display wrapper
 *
 * The CLUE has a 240x240 ST7789 TFT display.
 * This provides simple text-based UI for robot status.
 */

#ifndef CLUE_DISPLAY_H
#define CLUE_DISPLAY_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

class ClueDisplay {
public:
  // CLUE TFT pins
  static const int TFT_CS = 31;
  static const int TFT_DC = 32;
  static const int TFT_RST = 33;
  static const int TFT_BACKLIGHT = 34;

  void begin();
  void clear();
  void showSplash(const char* title);
  void showMode(const char* modeName, bool active);
  void showDistance(float cm);
  void showScan(float leftCm, float frontCm, float rightCm);
  void showText(const String& text);

private:
  Adafruit_ST7789* _tft = nullptr;
};

#endif
