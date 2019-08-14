#ifndef _BMP280_H_
#define _BMP280_H_

#include "src/Adafruit_BMP280_Library/Adafruit_BMP280.h"

Adafruit_BMP280 bmp; // I2C
bool bmp280_initialized = false;
long bmp280_timer = 0;

float bmp280_temperature = 0;
float bmp280_pressure = 0;
float bmp280_altitude = 0;

// 初始化
void bmp280_init(void) {
  if (!bmp.begin(BMP280_ADDRESS_ALT)) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    bmp280_initialized = false;
  } else {
    bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                    Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                    Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                    Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                    Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
    bmp280_initialized = true;
  }
}


void read_bmp280() {
  if (bmp280_initialized && millis() - bmp280_timer > 500) {
    bmp280_temperature = bmp.readTemperature();
    bmp280_pressure = bmp.readPressure();
    bmp280_altitude = bmp.readAltitude(1013.25);
    bmp280_timer = millis();
  }
}

#endif // _BMP280_H_
