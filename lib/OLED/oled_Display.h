#ifndef oled_Display_H
#define oled_Display_H
#include <Arduino.h>
#include <spi.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

void oledSetup();
void updateDisplay();
void displayCenteredText(const char *text, uint8_t textSize);
void turnOffDisplay();
void turnONDisplay();
#endif