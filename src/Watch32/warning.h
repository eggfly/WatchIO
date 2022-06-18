#ifndef _WARNING_H
#define _WARNING_H

#include "lcd.h"

#define INACTIVE_TIMEOUT_SECONDS (60)

long last_button_active_time = -1;


boolean is_usb_plugged_in;

void draw_warning() {
  canvas.setTextSize(1);
  canvas.setCursor(1, 1);
  canvas.print("This watch has been inactive for 1 minute without any USB power.");

  canvas.setTextSize(2);
  canvas.setCursor(15, 28);
  canvas.print("SHUTDOWN TO");
  canvas.setCursor(15, 49);
  canvas.print("SAVE POWER!");

  canvas.setTextSize(1);
  canvas.setCursor(35, 70);
  canvas.print("Or press any button.");
}

void deep_sleep_with_imu_interrupt() {
  int count = 20;
  float delta = (LCD_DEFAULT_BRIGHTNESS - LCD_MIN_BRIGHTNESS) / float(count);
  for (int i = 1; i <= count; i++) {
    delay(300 / count);
    lcd_set_brightness(LCD_DEFAULT_BRIGHTNESS - delta * i);
  }

  tftFillScreen(ST77XX_BLACK);
  // Set max brigtness to show white lines more clearly!
  lcd_set_brightness(LCD_DEFAULT_BRIGHTNESS + 10);
  for (int i = 0; i < SCREEN_HEIGHT / 2; i++) {
    tft.drawFastHLine(0, i - 1, SCREEN_WIDTH, ST77XX_BLACK);
    tft.drawFastHLine(0, i, SCREEN_WIDTH, ST77XX_WHITE);
    tft.drawFastHLine(0, SCREEN_HEIGHT - i + 1, SCREEN_WIDTH, ST77XX_BLACK);
    tft.drawFastHLine(0, SCREEN_HEIGHT - i, SCREEN_WIDTH, ST77XX_WHITE);
    delay(200 / SCREEN_HEIGHT);
  }
  tftFillScreen(ST77XX_BLACK);
  for (int i = 0; i < SCREEN_WIDTH / 2; i++) {
    tft.drawFastHLine(0, SCREEN_HEIGHT / 2, SCREEN_WIDTH, ST77XX_BLACK);
    tft.drawFastHLine(i, SCREEN_HEIGHT / 2, SCREEN_WIDTH - i * 2, ST77XX_WHITE);
    delay(200 / SCREEN_WIDTH);
  }

  while (digitalRead(SWITCH_PUSH) == LOW) {}
  // esp_sleep_enable_ext0_wakeup(GPIO_NUM_4, 0); //1 = High, 0 = Low
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_13, 0); //1 = High, 0 = Low
  // esp_sleep_enable_ext0_wakeup(GPIO_NUM_39, 0); //1 = High, 0 = Low
  // esp_sleep_enable_ext1_wakeup(1 << 0, ESP_EXT1_WAKEUP_ANY_LOW);

  lcd_sleep_in();
  Serial.println("Going to sleep now");
  esp_deep_sleep_start();
  Serial.println("This will never be printed");
}

void check_battery_warning_and_escape() {
  if (last_button_active_time < 0) {
    last_button_active_time = millis();
  }
  if (millis() - last_button_active_time > INACTIVE_TIMEOUT_SECONDS * 1000) {
    deep_sleep_with_imu_interrupt();
  }
}

void feed_battery_warning() {
  last_button_active_time = millis();
}

#endif // _WARNING_H
