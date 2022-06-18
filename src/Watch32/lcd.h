#ifndef _LCD_H
#define _LCD_H

#include <SPI.h>

#include "src/Adafruit_GFX_Library/Adafruit_GFX.h"    // Core graphics library
#include "src/Adafruit_ST7735_and_ST7789_Library/Adafruit_ST7735.h" // Hardware-specific library for ST7735

#define RGB565(r, g, b) ((((r>>3)<<11) | ((g>>2)<<5) | (b>>3)))

#define TFT_CS          27 // Hallowing display control pins: chip select
#define TFT_RST         33 // Display reset
#define TFT_DC          15 // Display data/command select
#define TFT_BACKLIGHT   25 // Display backlight pin

#define CANVAS_WIDTH    SCREEN_WIDTH
#define CANVAS_HEIGHT   SCREEN_HEIGHT

// real screen
// Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

SPIClass SPI2(HSPI);
Adafruit_ST7735 tft = Adafruit_ST7735(&SPI2, TFT_CS, TFT_DC, TFT_RST);


// frame buffer in RAM
GFXcanvas16 canvas = GFXcanvas16(CANVAS_WIDTH, CANVAS_HEIGHT);

#define ST77XX_GRAY  0x7BEF

const int brightness_max = 255;
const int brightness_delta = brightness_max / 20;

static bool ledc_setup = false;

void fillScreen(uint16_t color) {
  canvas.fillRect(0, 0, 160, 80, color);
}

void tftFillScreen(uint16_t color) {
  tft.fillRect(0, 0, 160, 80, color);
}


void lcd_set_brightness(uint8_t brightness) {
  if (ledc_setup) {
    Serial.print("lcd_set_brightness: ");
    Serial.println(brightness);
    ledcWrite(0, 255 - brightness);
  }
}

long last_lcd_flush_time = 0;

void sendGRAM(bool force) {
  long m = millis();
  if (force || m - last_lcd_flush_time > 16.667) {
    tft.drawRGBBitmap(0, 0, canvas.getBuffer(), CANVAS_WIDTH, CANVAS_HEIGHT);
    last_lcd_flush_time = millis();
  } else {
    // Serial.printf("sendGRAM() ignored, delta time=%ld\r\n", m - last_lcd_flush_time);
  }
}

void sendGRAM() {
  sendGRAM(false);
}


void lcd_init() {
  tft.initR(INITR_MINI160x80);  // Init ST7735S mini display
  tft.invertDisplay(true);
  canvas.invertDisplay(true);

  tft.setRotation(1);

  canvas.fillScreen(ST77XX_BLUE);
  canvas.setCursor(20, 30);
  canvas.setTextSize(2);
  canvas.setTextColor(ST77XX_WHITE);
  canvas.print("loading...");
  canvas.setTextSize(1);
  sendGRAM();
  // setup pwm channel
  ledcSetup(0, 5000, 8);
  ledcAttachPin(TFT_BACKLIGHT, 0);
  ledc_setup = true;

  // low is backlight on
  lcd_set_brightness(LCD_DEFAULT_BRIGHTNESS);
}

void lcd_sleep_in() {
  tft.sendCommand(ST77XX_SLPIN);
}



#endif // _LCD_H
