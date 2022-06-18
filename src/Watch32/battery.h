#ifndef _BATTERY_H
#define _BATTERY_H

#include "lcd.h"

static const float levels[] = {4.13, 4.06, 3.98, 3.92, 3.87, 3.82, 3.79, 3.77, 3.74, 3.68, 3.45, 3.00};

unsigned long vbat_last_update;
double vbat;

float getBatteryLevel(float voltage) {
  float level = 1;
  if (voltage >= levels[0]) {
    level = 1;
  } else if (voltage >= levels[1]) {
    level = 0.9;
    level += 0.1 * (voltage - levels[1]) / (levels[0] - levels[1]);
  } else if (voltage >= levels[2]) {
    level = 0.8;
    level += 0.1 * (voltage - levels[2]) / (levels[1] - levels[2]);
  } else if (voltage >= levels[3]) {
    level = 0.7;
    level += 0.1 * (voltage - levels[3]) / (levels[2] - levels[3]);
  } else if (voltage >= levels[4]) {
    level = 0.6;
    level += 0.1 * (voltage - levels[4]) / (levels[3] - levels[4]);
  } else if (voltage >= levels[5]) {
    level = 0.5;
    level += 0.1 * (voltage - levels[5]) / (levels[4] - levels[5]);
  } else if (voltage >= levels[6]) {
    level = 0.4;
    level += 0.1 * (voltage - levels[6]) / (levels[5] - levels[6]);
  } else if (voltage >= levels[7]) {
    level = 0.3;
    level += 0.1 * (voltage - levels[7]) / (levels[6] - levels[7]);
  } else if (voltage >= levels[8]) {
    level = 0.2;
    level += 0.1 * (voltage - levels[8]) / (levels[7] - levels[8]);
  } else if (voltage >= levels[9]) {
    level = 0.1;
    level += 0.1 * (voltage - levels[9]) / (levels[8] - levels[9]);
  } else if (voltage >= levels[10]) {
    level = 0.05;
    level += 0.05 * (voltage - levels[10]) / (levels[9] - levels[10]);
  } else if (voltage >= levels[11]) {
    level = 0.00;
    level += 0.05 * (voltage - levels[11]) / (levels[10] - levels[11]);
  } else {
    level = 0.00;
  }
  return level;
}

void draw_battery_percent() {
  float battery_percent = getBatteryLevel(vbat);
  canvas.setTextColor(0xAA00FF00);
  canvas.setCursor(122, 2);
  canvas.print(battery_percent * 100, 1);
  canvas.print("%");
}

void check_update_battery() {
  unsigned long now = millis();
  if (now - vbat_last_update > 400) {
    vbat_last_update = now;
    int sensorValue = analogRead(VBAT_SENSOR);
    // vbat = sensorValue / 4096.0 * 3.3 * 4.0 / 3.0 * 1.113;
    vbat = sensorValue / 4096.0 * 3.3 * 2.0 / 1.0 * 1.113;
  }
}


#endif // _BATTERY_H
