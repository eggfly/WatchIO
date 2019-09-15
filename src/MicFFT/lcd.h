#ifndef _LCD_H
#define _LCD_H

#include <SPI.h>

#include "src/Adafruit_GFX_Library/Adafruit_GFX.h"    // Core graphics library
#include "src/Adafruit_ST7735_and_ST7789_Library/Adafruit_ST7735.h" // Hardware-specific library for ST7735

#define RGB565(r, g, b) ((((r>>3)<<11) | ((g>>2)<<5) | (b>>3)))

#define TFT_CS          5 // Hallowing display control pins: chip select
#define TFT_RST         33 // Display reset
#define TFT_DC          27 // Display data/command select
#define TFT_BACKLIGHT   15 // Display backlight pin

Adafruit_ST7735 canvas = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

#define ST77XX_GRAY  0x7BEF

const int brightness_max = 255;
const int brightness_delta = brightness_max / 20;

static int backlight = 246;

static bool ledc_setup = false;
void lcd_init() {
  canvas.initR(INITR_MINI160x80);  // Init ST7735S mini display
  canvas.invertDisplay(true);

  canvas.setRotation(1);

  canvas.fillScreen(ST77XX_BLUE);
  canvas.setCursor(20, 30);
  canvas.setTextSize(2);
  canvas.setTextColor(ST77XX_WHITE);
  canvas.print("loading...");
  canvas.setTextSize(1);
  // setup pwm channel
  ledcSetup(0, 5000, 8);
  ledcAttachPin(TFT_BACKLIGHT, 0);
  ledc_setup = true;

  // low is backlight on
  ledcWrite(0, backlight);
}

void lcd_sleep_in() {
  canvas.sendCommand(ST77XX_SLPIN);
}

void lcd_set_brightness(uint8_t brightness) {
  if (ledc_setup) {
    Serial.print("lcd_set_brightness: ");
    Serial.println(brightness);

    ledcWrite(0, 255 - brightness);
  }
}

void sendGRAM() {
}

#endif // _LCD_H
