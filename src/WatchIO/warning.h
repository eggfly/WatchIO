#ifndef _WARNING_H
#define _WARNING_H

#include "lcd.h"

#define INACTIVE_TIMEOUT_SECONDS (30)

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
  // esp_sleep_enable_ext0_wakeup(GPIO_NUM_4, 0); //1 = High, 0 = Low
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_0, 0); //1 = High, 0 = Low
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
