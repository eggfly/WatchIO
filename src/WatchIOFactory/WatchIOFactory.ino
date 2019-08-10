
#include "src/Adafruit_GFX_Library/Adafruit_GFX.h"    // Core graphics library
#include "src/Adafruit_ST7735_and_ST7789_Library/Adafruit_ST7735.h" // Hardware-specific library for ST7735
#include <SPI.h>
#include <Wire.h>

#include "src/RTCLib_by_NeiroN/RTClib.h"
#include <Adafruit_BMP280.h>

#define TFT_CS          14 // Hallowing display control pins: chip select
#define TFT_RST         33 // Display reset
#define TFT_DC          27 // Display data/command select
#define TFT_BACKLIGHT   15 // Display backlight pin

#define SWITCH_UP        9
#define SWITCH_DOWN     12
#define SWITCH_PUSH     13
#define BTN              0

int sensorPin = 32;    // select the input pin for the potentiometer

// For 1.44" and 1.8" TFT with ST7735 (including HalloWing) use:
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

Adafruit_BMP280 bmp; // I2C
PCF8563 rtc;

static boolean sw_up_active = false;
static boolean sw_down_active = false;
static boolean sw_push_active = false;
static boolean btn_active = false;

static const float vbat_levels[] = {4.13, 4.06, 3.98, 3.92, 3.87, 3.82, 3.79, 3.77, 3.74, 3.68, 3.45, 3.00};

const int brightness_max = 255;
const int brightness_delta = brightness_max / 10;

static int brightness = brightness_max - brightness_delta;

float p = 3.1415926;


float getBatteryLevel(float voltage) {
  float level = 1;
  if (voltage >= vbat_levels[0]) {
    level = 1;
  } else if (voltage >= vbat_levels[1]) {
    level = 0.9;
    level += 0.1 * (voltage - vbat_levels[1]) / (vbat_levels[0] - vbat_levels[1]);
  } else if (voltage >= vbat_levels[2]) {
    level = 0.8;
    level += 0.1 * (voltage - vbat_levels[2]) / (vbat_levels[1] - vbat_levels[2]);
  } else if (voltage >= vbat_levels[3]) {
    level = 0.7;
    level += 0.1 * (voltage - vbat_levels[3]) / (vbat_levels[2] - vbat_levels[3]);
  } else if (voltage >= vbat_levels[4]) {
    level = 0.6;
    level += 0.1 * (voltage - vbat_levels[4]) / (vbat_levels[3] - vbat_levels[4]);
  } else if (voltage >= vbat_levels[5]) {
    level = 0.5;
    level += 0.1 * (voltage - vbat_levels[5]) / (vbat_levels[4] - vbat_levels[5]);
  } else if (voltage >= vbat_levels[6]) {
    level = 0.4;
    level += 0.1 * (voltage - vbat_levels[6]) / (vbat_levels[5] - vbat_levels[6]);
  } else if (voltage >= vbat_levels[7]) {
    level = 0.3;
    level += 0.1 * (voltage - vbat_levels[7]) / (vbat_levels[6] - vbat_levels[7]);
  } else if (voltage >= vbat_levels[8]) {
    level = 0.2;
    level += 0.1 * (voltage - vbat_levels[8]) / (vbat_levels[7] - vbat_levels[8]);
  } else if (voltage >= vbat_levels[9]) {
    level = 0.1;
    level += 0.1 * (voltage - vbat_levels[9]) / (vbat_levels[8] - vbat_levels[9]);
  } else if (voltage >= vbat_levels[10]) {
    level = 0.05;
    level += 0.05 * (voltage - vbat_levels[10]) / (vbat_levels[9] - vbat_levels[10]);
  } else if (voltage >= vbat_levels[11]) {
    level = 0.00;
    level += 0.05 * (voltage - vbat_levels[11]) / (vbat_levels[10] - vbat_levels[11]);
  } else {
    level = 0.00;
  }
  return level;
}

void sw_up() {
  sw_up_active = true;
}

void sw_down() {
  sw_down_active = true;
}

void sw_push() {
  sw_push_active = true;
}

void btn() {
  btn_active = true;
}

bool bmp280_initialized = false;

bool is_rtc_running() {
  DateTime now = rtc.now();
  uint16_t year = now.year();
  return year > 2018 && year < 2070;
}

void checkBatteryLevelOrDeepSleep() {
  double startVoltage = readBatteryVoltage();
  float batteryLevel = getBatteryLevel(startVoltage);
  bool startBatteryIsLow = false;
  if (batteryLevel <= 0.05) {
    startBatteryIsLow = true;
    Serial.println(F("battery is very low!"));
    Serial.print("battery level: ");
    Serial.println(batteryLevel * 100, 1);
  }
  if (startBatteryIsLow) {
    tft.fillScreen(ST77XX_RED);
    tft.setCursor(0, 0);
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(2);
    tft.print("Low Battery!");
    delay(500);
    tft.sendCommand(ST77XX_SLPIN);
    Serial.println("sleep tft..");
    delay(500);
    Serial.println("now go to sleep..");
    esp_deep_sleep_start();
    Serial.println("this will be never printed.");
  }
}

void setup(void) {
  Serial.begin(115200);
  Serial.println(F("Hello! WatchIO"));

  // use this initializer, using a 0.96" 180x60 TFT:
  tft.initR(INITR_MINI160x80);  // Init ST7735S mini display

  // hack is a need
  tft.invertDisplay(true);

  // init color before backlight
  tft.fillScreen(ST77XX_BLUE);

  // setup pwm channel
  ledcSetup(0, 5000, 8);
  ledcAttachPin(TFT_BACKLIGHT, 0);
  // low is backlight on
  ledcWrite(0, brightness);

  checkBatteryLevelOrDeepSleep();

  pinMode(SWITCH_UP, INPUT);
  pinMode(SWITCH_DOWN, INPUT);
  pinMode(SWITCH_PUSH, INPUT);
  pinMode(BTN, INPUT);
  attachInterrupt(SWITCH_UP, sw_up, FALLING);
  attachInterrupt(SWITCH_DOWN, sw_down, FALLING);
  attachInterrupt(SWITCH_PUSH, sw_push, FALLING);
  attachInterrupt(BTN, btn, FALLING);

  Wire.begin();

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

  rtc.begin();
  if (!is_rtc_running()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(__DATE__, __TIME__));
  }

  Serial.println(F("Initialized"));


  uint16_t time = millis();
  // tft.fillScreen(ST77XX_WHITE);
  time = millis() - time;

  Serial.println(time, DEC);

  // rotation will affect on rowstart and colstart
  tft.setRotation(1);

  tft.fillScreen(ST77XX_WHITE);
  delay(100);
  tft.fillScreen(ST77XX_BLACK);
  delay(100);
  tft.fillScreen(ST77XX_RED);
  delay(100);
  tft.fillScreen(ST77XX_GREEN);
  delay(100);
  tft.fillScreen(ST77XX_BLUE);
  delay(100);
  tft.fillScreen(ST77XX_CYAN);
  delay(100);
  tft.fillScreen(ST77XX_MAGENTA);
  delay(100);
  tft.fillScreen(ST77XX_YELLOW);
  delay(100);
  tft.fillScreen(ST77XX_ORANGE);
  delay(100);

  tft.fillScreen(ST77XX_BLACK);
  // large block of text
  testdrawtext("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Curabitur adipiscing ante sed nibh tincidunt feugiat. Maecenas enim massa, fringilla sed malesuada et, malesuada sit amet turpis. Sed porttitor neque ut ante pretium vitae malesuada nunc bibendum. Nullam aliquet ultrices massa eu hendrerit. Ut sed nisi lorem. In vestibulum purus a tortor imperdiet posuere. ", ST77XX_WHITE);
  delay(1000);

  // tft print function!
  tftPrintTest();
  delay(200);

  // a single pixel
  tft.drawPixel(tft.width() / 2, tft.height() / 2, ST77XX_GREEN);
  delay(200);

  // line draw test
  testlines(ST77XX_YELLOW);
  delay(200);

  // optimized lines
  testfastlines(ST77XX_RED, ST77XX_BLUE);
  delay(200);

  testdrawrects(ST77XX_GREEN);
  delay(200);

  testfillrects(ST77XX_YELLOW, ST77XX_MAGENTA);
  delay(200);

  tft.fillScreen(ST77XX_BLACK);
  testfillcircles(10, ST77XX_BLUE);
  testdrawcircles(10, ST77XX_WHITE);
  delay(200);

  testroundrects();
  delay(200);

  testtriangles();
  delay(200);

  Serial.println("done");
}

void i2c_scan() {
  byte error, address;
  int nDevices;

  Serial.println("Scanning...");

  nDevices = 0;
  for (address = 1; address < 127; address++ ) {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      tft.print("Found addr=0x");
      if (address < 16) {
        Serial.print("0");
        tft.println("0");
      }
      Serial.print(address, HEX);
      tft.println(address, HEX);
      Serial.println("  !");
      nDevices++;
    } else if (error == 4) {
      Serial.print("Unknown error at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);
    }
  }
  if (nDevices == 0) {
    Serial.println("No I2C devices found\n");
  }
  else {
    Serial.println("done\n");
  }
}

void print_bmp280() {
  if (bmp280_initialized) {
    float temperature = bmp.readTemperature();
    tft.print(F("Temperature = "));
    tft.print(temperature);
    tft.println(" *C");

    float pressure = bmp.readPressure();
    tft.print(F("Pressure = "));
    tft.print(pressure);
    tft.println(" Pa");

    float altitude = bmp.readAltitude(1013.25);
    tft.print(F("Approx altitude = "));
    tft.print(altitude); /* Adjusted to local forecast! */
    tft.println(" m");
  }
}

void print_time() {
  DateTime now = rtc.now();
  char buf[100];
  strncpy(buf, "DD.MM.YYYY  hh:mm:ss\0", 100);
  now.format(buf);
  Serial.println(buf);
  tft.println(buf);
}

double readBatteryVoltage() {
  int sensorValue = analogRead(sensorPin);
  double voltage = sensorValue / 4096.0 * 3.3 * 4.0 / 3.0 * 1.113;
  return voltage;
}

void loop() {
  Serial.println("loop");

  checkBatteryLevelOrDeepSleep();
  tft.fillScreen(ST77XX_YELLOW);

  tft.setCursor(0, 0);
  tft.setTextColor(ST77XX_RED);
  tft.setTextWrap(true);

  print_time();
  double voltage = readBatteryVoltage();
  tft.print("vbat=");
  tft.print(voltage);
  tft.print("V");
  tft.print(", ");
  float battery_percent = getBatteryLevel(voltage);
  tft.print(battery_percent * 100, 1);
  tft.print("%");
  tft.println();

  print_bmp280();

  if (btn_active) {
    btn_active = false;
    esp_sleep_enable_ext0_wakeup((gpio_num_t)BTN, 0); //1 = High, 0 = Low
    tft.println("btn clicked, sleep now!");
    delay(5000);
    tft.sendCommand(ST77XX_SLPIN);
    Serial.println("sleep tft..");
    delay(5000);
    Serial.println("now go to sleep..");
    esp_deep_sleep_start();
    Serial.println("this will never be printed.");
    // ESP.restart();
  } else if (sw_up_active) {
    sw_up_active = false;
    brightness -= brightness_delta;
    if (brightness <= 0) {
      brightness = 0;
    }
    ledcWrite(0, brightness);
    tft.println("increase brightness");
    tft.print(" brightness=");
    tft.println(brightness);
  } else if (sw_down_active) {
    sw_down_active = false;
    brightness += brightness_delta;
    if (brightness >= brightness_max) {
      brightness = brightness_max;
    }
    ledcWrite(0, brightness);
    tft.println("decrease brightness");
    tft.print(" brightness=");
    tft.println(brightness);
  } else if (sw_push_active) {
    sw_push_active = false;
    tft.println("sw push, i2c_scan");
    i2c_scan();
  }
  delay(900);
}

void testlines(uint16_t color) {
  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x = 0; x < tft.width(); x += 6) {
    tft.drawLine(0, 0, x, tft.height() - 1, color);
    delay(0);
  }
  for (int16_t y = 0; y < tft.height(); y += 6) {
    tft.drawLine(0, 0, tft.width() - 1, y, color);
    delay(0);
  }

  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x = 0; x < tft.width(); x += 6) {
    tft.drawLine(tft.width() - 1, 0, x, tft.height() - 1, color);
    delay(0);
  }
  for (int16_t y = 0; y < tft.height(); y += 6) {
    tft.drawLine(tft.width() - 1, 0, 0, y, color);
    delay(0);
  }

  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x = 0; x < tft.width(); x += 6) {
    tft.drawLine(0, tft.height() - 1, x, 0, color);
    delay(0);
  }
  for (int16_t y = 0; y < tft.height(); y += 6) {
    tft.drawLine(0, tft.height() - 1, tft.width() - 1, y, color);
    delay(0);
  }

  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x = 0; x < tft.width(); x += 6) {
    tft.drawLine(tft.width() - 1, tft.height() - 1, x, 0, color);
    delay(0);
  }
  for (int16_t y = 0; y < tft.height(); y += 6) {
    tft.drawLine(tft.width() - 1, tft.height() - 1, 0, y, color);
    delay(0);
  }
}

void testdrawtext(char *text, uint16_t color) {
  tft.setCursor(0, 0);
  tft.setTextColor(color);
  tft.setTextWrap(true);
  tft.print(text);
}

void testfastlines(uint16_t color1, uint16_t color2) {
  tft.fillScreen(ST77XX_BLACK);
  for (int16_t y = 0; y < tft.height(); y += 5) {
    tft.drawFastHLine(0, y, tft.width(), color1);
  }
  for (int16_t x = 0; x < tft.width(); x += 5) {
    tft.drawFastVLine(x, 0, tft.height(), color2);
  }
}

void testdrawrects(uint16_t color) {
  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x = 0; x < tft.width(); x += 6) {
    tft.drawRect(tft.width() / 2 - x / 2, tft.height() / 2 - x / 2 , x, x, color);
  }
}

void testfillrects(uint16_t color1, uint16_t color2) {
  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x = tft.width() - 1; x > 6; x -= 6) {
    tft.fillRect(tft.width() / 2 - x / 2, tft.height() / 2 - x / 2 , x, x, color1);
    tft.drawRect(tft.width() / 2 - x / 2, tft.height() / 2 - x / 2 , x, x, color2);
  }
}

void testfillcircles(uint8_t radius, uint16_t color) {
  for (int16_t x = radius; x < tft.width(); x += radius * 2) {
    for (int16_t y = radius; y < tft.height(); y += radius * 2) {
      tft.fillCircle(x, y, radius, color);
    }
  }
}

void testdrawcircles(uint8_t radius, uint16_t color) {
  for (int16_t x = 0; x < tft.width() + radius; x += radius * 2) {
    for (int16_t y = 0; y < tft.height() + radius; y += radius * 2) {
      tft.drawCircle(x, y, radius, color);
    }
  }
}

void testtriangles() {
  tft.fillScreen(ST77XX_BLACK);
  int color = 0xF800;
  int t;
  int w = tft.width() / 2;
  int x = tft.height() - 1;
  int y = 0;
  int z = tft.width();
  for (t = 0 ; t <= 15; t++) {
    tft.drawTriangle(w, y, y, x, z, x, color);
    x -= 4;
    y += 4;
    z -= 4;
    color += 100;
  }
}

void testroundrects() {
  tft.fillScreen(ST77XX_BLACK);
  int color = 100;
  int i;
  int t;
  for (t = 0 ; t <= 4; t += 1) {
    int x = 0;
    int y = 0;
    int w = tft.width() - 2;
    int h = tft.height() - 2;
    for (i = 0 ; i <= 16; i += 1) {
      tft.drawRoundRect(x, y, w, h, 5, color);
      x += 2;
      y += 3;
      w -= 4;
      h -= 6;
      color += 1100;
    }
    color += 100;
  }
}

void tftPrintTest() {
  tft.setTextWrap(false);
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0, 5);
  tft.setTextColor(ST77XX_RED);
  tft.setTextSize(1);
  tft.println("1 Hello World!");
  tft.setTextColor(ST77XX_YELLOW);
  tft.setTextSize(2);
  tft.println("2 Hello World!");
  tft.setTextColor(ST77XX_GREEN);
  tft.setTextSize(3);
  tft.println("3 Hello World!");
  tft.setTextColor(ST77XX_BLUE);
  tft.setTextSize(4);
  tft.print(1234.567);
  delay(1000);
  tft.setCursor(0, 0);
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(0);
  tft.println("Hello World!");
  tft.setTextSize(1);
  tft.setTextColor(ST77XX_GREEN);
  tft.print(p, 6);
  tft.println(" Want pi?");
  tft.println(" ");
  tft.print(8675309, HEX); // print 8,675,309 out in HEX!
  tft.println(" Print HEX!");
  tft.println(" ");
  tft.setTextColor(ST77XX_WHITE);
  tft.println("Sketch has been");
  tft.println("running for: ");
  tft.setTextColor(ST77XX_MAGENTA);
  tft.print(millis() / 1000);
  tft.setTextColor(ST77XX_WHITE);
  tft.print(" seconds.");
}
