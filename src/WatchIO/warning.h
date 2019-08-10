#ifndef _WARNING_H
#define _WARNING_H

#include "lcd.h"

#define INACTIVE_TIMEOUT_SECONDS (10 * 60)

long last_button_active_time = -1;

long last_axp_update_time = 0;

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

void check_battery_warning_and_escape() {
  // TODO
}

void feed_battery_warning() {
  last_button_active_time = millis();
}

#endif // _WARNING_H
