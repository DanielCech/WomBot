/*
 * ClueDisplay.cpp - CLUE TFT display (ST7789, 240x240)
 *
 * Pin assignments are specific to the Adafruit CLUE board.
 * Uses hardware SPI for fast display updates.
 */

#include "ClueDisplay.h"

void ClueDisplay::begin() {
  pinMode(TFT_BACKLIGHT, OUTPUT);
  digitalWrite(TFT_BACKLIGHT, HIGH);

  _tft = new Adafruit_ST7789(&SPI1, TFT_CS, TFT_DC, TFT_RST);
  _tft->init(240, 240);
  _tft->setRotation(1);
  clear();
}

void ClueDisplay::clear() {
  if (!_tft) return;
  _tft->fillScreen(ST77XX_BLACK);
  _tft->setCursor(0, 0);
}

void ClueDisplay::showSplash(const char* title) {
  if (!_tft) return;
  clear();
  _tft->setTextSize(3);
  _tft->setTextColor(ST77XX_CYAN);
  _tft->setCursor(20, 80);
  _tft->println(title);

  _tft->setTextSize(2);
  _tft->setTextColor(ST77XX_WHITE);
  _tft->setCursor(20, 130);
  _tft->println("Adafruit CLUE");
  _tft->setCursor(20, 155);
  _tft->println("+ MagicBit V2");
}

void ClueDisplay::showMode(const char* modeName, bool active) {
  if (!_tft) return;
  clear();

  _tft->setTextSize(2);
  _tft->setTextColor(ST77XX_YELLOW);
  _tft->setCursor(10, 10);
  _tft->println("WomBot 1.0");

  _tft->setTextSize(3);
  _tft->setTextColor(ST77XX_CYAN);
  _tft->setCursor(10, 60);
  _tft->println(modeName);

  _tft->setTextSize(2);
  _tft->setCursor(10, 110);
  if (active) {
    _tft->setTextColor(ST77XX_GREEN);
    _tft->println("RUNNING");
  } else {
    _tft->setTextColor(ST77XX_RED);
    _tft->println("STOPPED");
  }

  _tft->setTextSize(1);
  _tft->setTextColor(0x7BEF);  // gray
  _tft->setCursor(10, 200);
  _tft->println("A=Mode  B=Start/Stop");
}

void ClueDisplay::showDistance(float cm) {
  if (!_tft) return;

  // Only update the distance area, not full screen
  _tft->fillRect(10, 150, 220, 40, ST77XX_BLACK);
  _tft->setTextSize(2);
  _tft->setCursor(10, 155);

  if (cm > 0) {
    _tft->setTextColor(cm < 15 ? ST77XX_RED : (cm < 30 ? ST77XX_YELLOW : ST77XX_GREEN));
    _tft->print("Dist: ");
    _tft->print(cm, 1);
    _tft->println(" cm");
  } else {
    _tft->setTextColor(0x7BEF);
    _tft->println("Dist: ---");
  }
}

void ClueDisplay::showScan(float leftCm, float frontCm, float rightCm) {
  if (!_tft) return;
  clear();

  _tft->setTextSize(2);
  _tft->setTextColor(ST77XX_YELLOW);
  _tft->setCursor(10, 10);
  _tft->println("Scanning...");

  _tft->setTextSize(2);
  _tft->setTextColor(ST77XX_WHITE);

  _tft->setCursor(10, 60);
  _tft->print("Left:  ");
  _tft->print(leftCm, 1);
  _tft->println(" cm");

  _tft->setCursor(10, 90);
  _tft->print("Front: ");
  _tft->print(frontCm, 1);
  _tft->println(" cm");

  _tft->setCursor(10, 120);
  _tft->print("Right: ");
  _tft->print(rightCm, 1);
  _tft->println(" cm");

  // Visual indicator
  _tft->setTextSize(2);
  _tft->setCursor(10, 170);
  if (leftCm > rightCm) {
    _tft->setTextColor(ST77XX_GREEN);
    _tft->println("<< Turn LEFT");
  } else {
    _tft->setTextColor(ST77XX_GREEN);
    _tft->println("Turn RIGHT >>");
  }
}

void ClueDisplay::showText(const String& text) {
  if (!_tft) return;
  _tft->fillRect(10, 150, 220, 40, ST77XX_BLACK);
  _tft->setTextSize(2);
  _tft->setTextColor(ST77XX_WHITE);
  _tft->setCursor(10, 155);
  _tft->println(text);
}
