
#include "config.h"
#include "res.h"
#include "imu.h"
#include "bmp280.h"
#include "power.h"
#include "battery.h"
#include "lcd.h"
#include "rtc.h"
#include "math.h"
#include "3d.h"
#include "maze.h"
#include "flappy_bird.h"
#include "warning.h"

#include <math.h>
#include <string.h>

// static const char *TAG = "app";

#define calibration_x 0
#define calibration_y 0

#define sensitivity_x 1.9
#define sensitivity_y 1.2

#define level_calibration_y 0

const uint16_t COLORS_LIGHT[10] = {
  0xFA55, 0x0C3E, 0xC01E, 0xF255, 0xF820,
  0xF321, 0xFFA0, 0x17A0, 0x04BF, 0xC01F
};

const uint32_t COLORS_DARK[10] = {
  0x40E6, 0x0128, 0x3809, 0x38C5, 0x4001,
  0x40E0, 0x4A20, 0x0220, 0x0108, 0x3008
};

// End of constructor list

int stateA = 0;

//int BUTTON_HOME = 34;
RTC_DATA_ATTR int bootCount = 0;

void print_wakeup_reason() {
  esp_sleep_wakeup_cause_t wakeup_reason;
  wakeup_reason = esp_sleep_get_wakeup_cause();
  switch (wakeup_reason) {
    case 1  : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case 2  : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case 3  : Serial.println("Wakeup caused by timer"); break;
    case 4  : Serial.println("Wakeup caused by touchpad"); break;
    case 5  : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.println("Wakeup was not caused by deep sleep"); break;
  }
}

long last_rtc_update_time = 0;

bool modify_time_mode = false;
int modify_time_digit = 0;

void show_time() {
  if (millis() - last_rtc_update_time > 1000) {
    last_rtc_update_time = millis();
    now = rtc.now();
  }
  char buf[100];
  strncpy(buf, "YYYY.MM.DD hh:mm:ss\0", 100);
  now.format(buf);
  Serial.println(buf);
  canvas.setCursor(28, 70);
  if (modify_time_mode) {
    canvas.setTextColor(ST77XX_RED);
  } else {
    canvas.setTextColor(ST77XX_WHITE);
  }
  canvas.print(buf);
  if (modify_time_mode) {
    canvas.setCursor(2, 70);
    canvas.setTextColor(ST77XX_CYAN);
    canvas.print("MOD:");
    int start = 39;
    int y = 62;
    switch (modify_time_digit) {
      case 0:
        canvas.setCursor(start + 18 * 5, y);
        break;
      case 1:
        canvas.setCursor(start + 18 * 4, y);
        break;
      case 2:
        canvas.setCursor(start + 18 * 3, y);
        break;
      case 3:
        canvas.setCursor(start + 18 * 2, y);
        break;
      case 4:
        canvas.setCursor(start + 18, y);
        break;
      case 5:
        canvas.setCursor(start - 8, y);
        break;
    }
    canvas.print("__");
  }
}

void setup() {
  Wire.begin();
  Serial.begin(115200);
  imu_init();
  imu_init_interrupt();
  // delay(1000);
  int16_t accX = 0, accY = 0, accZ = 0;
  read_imu(&accX, &accY, &accZ);
  // abs(accX) <= 1500 && abs(accY) <= 1500 && abs(accZ) > 2300
  if (true) {
    Serial.println("yes, continue boot");
  } else {
    // 不是正确姿态
    Serial.println("no");
    deep_sleep_with_imu_interrupt();
  }

  init_power();
  init_rtc();
  lcd_init();

  pinMode(BUTTON_HOME,  INPUT | PULLUP);
  pinMode(SWITCH_PUSH,  INPUT | PULLUP);
  pinMode(SWITCH_UP,    INPUT | PULLUP);
  pinMode(SWITCH_DOWN,  INPUT | PULLUP);

  ++bootCount;
  Serial.println("Boot number: " + String(bootCount));

  //Print the wakeup reason for ESP32
  print_wakeup_reason();

  attachInterrupt(digitalPinToInterrupt(BUTTON_HOME), home_isr, CHANGE);
  attachInterrupt(digitalPinToInterrupt(SWITCH_UP), sw_up_isr, FALLING);
  attachInterrupt(digitalPinToInterrupt(SWITCH_DOWN), sw_down_isr, FALLING);
  attachInterrupt(digitalPinToInterrupt(SWITCH_PUSH), sw_push_isr, CHANGE);

  bmp280_init();

#ifdef LIU_YU_XUAN_ENABLED
  draw_liuyuxuan();
  delay(5000);
#endif
}

long loopTime, startTime, endTime, fps;

#define PAGE_CLOCK              0
#define PAGE_TIMER              1
#define PAGE_KEYBOARD           2
#define PAGE_ELECTRONIC_LEVEL   3
#define PAGE_3D                 4
#define PAGE_MAZE               5
#define PAGE_FLAPPY_BIRD        6


#define PAGE_COUNT              7

int current_page = PAGE_CLOCK;

int cursorX, cursorY;
int clicked_cursor_x = -1, clicked_cursor_y = -1;

void draw_cursor() {
  int times = 1;
  int average_accX = 0, average_accY = 0;
  int16_t accX = 0, accY = 0, accZ = 0;
  for (int i = 0; i < times; i++) {
    read_imu(&accX, &accY, &accZ);
    average_accX += accX;
    average_accY += accY;
  }
  average_accX /= times;
  average_accY /= times;
  cursorX = 80 + calibration_x - sensitivity_x * average_accY / 50;
  cursorY = 40 + calibration_y - sensitivity_y * average_accX / 50;
  if (cursorX < 0) {
    cursorX = 0;
  } else if (cursorX > 159) {
    cursorX = 159;
  }
  if (cursorY < 0) {
    cursorY = 0;
  } else if (cursorY > 79) {
    cursorY = 79;
  }
  // draw cursor
  canvas.fillCircle(cursorX, cursorY, 2, ST77XX_RED);
}


void draw_level() {
  int times = 1;
  int average_accX = 0, average_accY = 0;
  int16_t accX = 0, accY = 0, accZ = 0;
  for (int i = 0; i < times; i++) {
    read_imu(&accX, &accY, &accZ);
    average_accX += accX;
    average_accY += accY;
  }
  average_accX /= times;
  average_accY /= times;
  cursorX = 80 + calibration_x - sensitivity_x * average_accY / 50;
  cursorY = 40 + calibration_y + level_calibration_y - sensitivity_y * average_accX / 50;
  if (cursorX < 0) {
    cursorX = 0;
  } else if (cursorX > 159) {
    cursorX = 159;
  }
  if (cursorY < 0) {
    cursorY = 0;
  } else if (cursorY > 79) {
    cursorY = 79;
  }

  // draw cursor
  float r_percent = 0.9;
  float r = min(SCREEN_HEIGHT, SCREEN_WIDTH) / 2 * r_percent;
  int dx = cursorX - SCREEN_WIDTH / 2;
  int dy = cursorY - SCREEN_HEIGHT / 2;
  if (dx * dx + dy * dy > r * r) {
    float scale = r / sqrt(dx * dx + dy * dy);
    cursorX = SCREEN_WIDTH / 2 + dx * scale;
    cursorY = SCREEN_HEIGHT / 2 + dy * scale;
  }
  canvas.drawCircle(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, r, ST77XX_RED);
  canvas.fillCircle(cursorX, cursorY, 4, ST77XX_GREEN);
}

void draw_bmp280() {
  read_bmp280();

  canvas.setCursor(0, 2);
  canvas.setTextColor(ST77XX_ORANGE);

  canvas.print(F(""));
  canvas.print(bmp280_temperature, 0);
  // canvas.drawCircle(13, 2, 2,ST77XX_WHITE);
  canvas.drawPixel(11, 0, ST77XX_ORANGE);
  canvas.print("C ");

  canvas.print(F(""));
  canvas.print(bmp280_altitude); /* Adjusted to local forecast! */
  canvas.print("m ");

  canvas.print(F(""));
  canvas.print(bmp280_pressure / 1000, 2);
  canvas.print("kPa ");
}

void draw_menu() {
  canvas.drawRect(0, 0, 33, 11, ST77XX_WHITE);

  canvas.setCursor(2, 2);
  canvas.setTextColor(ST77XX_ORANGE);
  canvas.print("Timer");

  canvas.drawRect(35, 0, 35, 11, ST77XX_WHITE);
  canvas.setCursor(38, 2);
  canvas.setTextColor(ST77XX_ORANGE);
  canvas.print("Clock");

  canvas.drawRect(36 + 35, 0, 50, 11, ST77XX_WHITE);
  canvas.setCursor(38 + 35, 2);
  canvas.setTextColor(ST77XX_ORANGE);
  canvas.print("Keyboard");
}

void page_1_2() {
  show_time();
  loopTime = millis();
  int count = 0;
  if (current_page == PAGE_CLOCK) {
    count = now.hour() * 100 + now.minute();
  } else {
    // 4 digit 100ms counter
    count = (loopTime / 100) % 10000;
  }
  startTime = loopTime;

  int x_start = 6;
  int x_delta = 7;
  int r = 2;
  for (int n = 0; n < 10; n++) {
    // canvas.fillCircle(x_start + x_delta * n, 4, r, COLORS_LIGHT[n]);
  }

  int y2 = 11;
  for (int n = 0; n < 10; n++) {
    // canvas.fillCircle(x_start + x_delta * n, y2, r, COLORS_DARK[n]);
  }

  int y_start = 19;
  for (int pos = 0; pos < 4; pos++) {
    uint8_t curr_digit = 0;
    if (pos == 0) {
      curr_digit = count / 1000;
    } else if (pos == 1) {
      curr_digit = count / 100 % 10;
    } else if (pos == 2) {
      curr_digit = count / 10 % 10;
    } else if (pos == 3) {
      curr_digit = count % 10;
    }
    for (int row = 0; row < 7; row++) {
      for (int col = 0; col < 5; col++) {
        uint32_t color = DIGITS[curr_digit][row][col] ?
                         COLORS_LIGHT[curr_digit] : COLORS_DARK[curr_digit];
        canvas.fillCircle(x_start + col * 7, y_start + row * 7, r, color);
      }
    }
    x_start += 39;
  }
}

char input_text[128] = {};

int keyboard_start_x = 3;
int keyboard_start_y = 33;
int keyboard_offset_x = 5;

int _1_center_x = keyboard_start_x + 2;
int _1_center_y = keyboard_start_y + 3;

double keyboard_x_space = 12.0;
double keyboard_y_space = 12.0;

char keyboard[4][14] = {
  {'`', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', ' '},
  {'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', '\\'},
  {'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'', ' ', ' '},
  {'Z', 'X', 'C', 'V', 'B', 'N', 'M', ',', '.', '/', ' ', ' ', ' '}
};

void handle_keyboard_click(int x, int y) {
  // calculate y first so that offset is right
  double y_delta_count = (y - _1_center_y) / keyboard_y_space;
  double x_delta_count = (x - _1_center_x - y_delta_count * keyboard_offset_x /* x has some offset */) / keyboard_x_space;
  int xx = int(x_delta_count + 0.5);
  int yy = int(y_delta_count + 0.5);
  char clicked_char = keyboard[yy][xx];
  Serial.printf("debug: y=%lf, x=%lf, char: %c\r\n", y_delta_count, x_delta_count, clicked_char);
  int len = strlen(input_text);
  input_text[len] = clicked_char;
  input_text[len + 1] = '\0';
}

void handle_other_area_click(int x, int y) {
  if (x >= 111 && x <= 124 && y >= 16 && y <= 27) {
    Serial.println("cl clicked");
    input_text[0] = '\0';
  }
  if (x >= 126 && x <= 157 && y >= 16 && y <= 27) {
    Serial.println("<- clicked");
    int len = strlen(input_text);
    if (len > 0) {
      input_text[len - 1] = '\0';
    }
  }
}

void page_keyboard() {
  int input_start_x = 0;
  canvas.drawRect(input_start_x, 15, 110, 13, ST77XX_WHITE);
  canvas.setTextColor(ST77XX_WHITE);
  // print cl button
  canvas.drawRect(input_start_x + 111, 15, 14, 13, ST77XX_WHITE);
  canvas.setCursor(input_start_x + 111 + 2, 15 + 2);
  canvas.print("cl");
  // print backspace button
  canvas.drawRect(input_start_x + 112 + 14, 15, 32, 13, ST77XX_WHITE);
  canvas.setCursor(input_start_x + 112 + 14 + 2, 15 + 2);
  canvas.print("<-");
  // print input text
  canvas.setCursor(input_start_x + 3, 18);
  canvas.print(input_text);

  canvas.setCursor(keyboard_start_x, keyboard_start_y);
  canvas.print("` 1 2 3 4 5 6 7 8 9 0 - =");
  canvas.setCursor(keyboard_start_x + keyboard_offset_x, keyboard_start_y + 12);
  canvas.print("Q W E R T Y U I O P [ ] \\");
  canvas.setCursor(keyboard_start_x + keyboard_offset_x * 2, keyboard_start_y + 24);
  canvas.print("A S D F G H J K L ; '    ");
  canvas.setCursor(keyboard_start_x + keyboard_offset_x * 3, keyboard_start_y + 36);
  canvas.print("Z X C V B N M , . /      ");
  canvas.setTextSize(1);
  if (clicked_cursor_x >= 0 && clicked_cursor_y >= 0) {
    if (clicked_cursor_y > 33) {
      handle_keyboard_click(clicked_cursor_x, clicked_cursor_y);
    } else {
      handle_other_area_click(clicked_cursor_x, clicked_cursor_y);
    }
    clicked_cursor_x = clicked_cursor_y = -1;
  }
}

void page_electronic_level() {
  draw_level();
}

#define GAME_STATE_INIT     0
#define GAME_STATE_PLAYING  1
int game_state = GAME_STATE_INIT;

#define STICK_STILL 0
#define STICK_LEFT  1
#define STICK_RIGHT 2

int game_stick_state = STICK_STILL;

#define STICK_LENGTH_SHORT   25
#define STICK_LENGTH_NORMAL  40
#define STICK_LENGTH_LONG    80

#define STICK_START_Y  72
#define STICK_HEIGHT    3
int stick_length = STICK_LENGTH_NORMAL;

#define BALL_R 2

double stick_pos = 80.0;
double ball_x = 80.0;
double ball_y = STICK_START_Y - BALL_R * 2;
double ball_speed_x = 2.0;
double ball_speed_y = -2.0;

void page_game_old() {
  if (game_state == GAME_STATE_INIT) {
    canvas.setTextSize(2);
    canvas.setTextColor(ST77XX_RED);
    canvas.setCursor(14, 18);
    canvas.print("DxBall Game");
    canvas.setCursor(18, 50);
    canvas.setTextSize(1);
    canvas.setTextColor(ST77XX_WHITE);
    canvas.print("press button to start");
    int arrow_center_x = 77;
    canvas.drawFastVLine(arrow_center_x, 65, 5, ST77XX_WHITE);
    canvas.fillTriangle(arrow_center_x - 3, 70,
                        arrow_center_x + 3, 70,
                        arrow_center_x, 75, ST77XX_WHITE);
  } else if (game_state == GAME_STATE_PLAYING) {
    // update stick position
    if (game_stick_state == STICK_LEFT) {
      stick_pos -= 4;
    } else if (game_stick_state == STICK_RIGHT) {
      stick_pos += 4;
    }
    ball_x += ball_speed_x;
    ball_y += ball_speed_y;
    if (ball_x <= BALL_R || ball_x >= SCREEN_WIDTH - BALL_R) {
      ball_speed_x = -ball_speed_x;
      Serial.printf("case 1: %lf\r\n", ball_speed_x);
    } else if (ball_y <= BALL_R || ball_y >= STICK_START_Y - BALL_R) {
      ball_speed_y = -ball_speed_y;
      Serial.printf("case 2: %lf\r\n", ball_speed_y);
    }
    // update ball position
    canvas.fillRect(stick_pos - stick_length / 2 , STICK_START_Y,
                    stick_length, STICK_HEIGHT, ST77XX_RED);
    canvas.fillCircle(ball_x, ball_y, BALL_R, ST77XX_GREEN);
  }
  if (digitalRead(BUTTON_HOME) == 0) {
    // Serial.println("button 1 is down");
    game_button1_pressed();
  } else if (digitalRead(SWITCH_UP) == 0) {
    // Serial.println("button 2 is down");
    game_button2_pressed();
  } else {
    game_no_button_pressed();
  }
}

void game_button1_pressed() {
  if (game_state == GAME_STATE_PLAYING) {
    game_stick_state = STICK_LEFT;
  }
}

void game_button2_pressed() {
  if (game_state == GAME_STATE_INIT) {
    game_state = GAME_STATE_PLAYING;
  } else if (game_state == GAME_STATE_PLAYING) {
    game_stick_state = STICK_RIGHT;
  }
}

void game_no_button_pressed() {
  if (game_state == GAME_STATE_PLAYING) {
    game_stick_state = STICK_STILL;
  }
}

boolean _3d_inited = false;

uint8_t lcd_brightness = LCD_DEFAULT_BRIGHTNESS;
boolean lcd_brightness_changed = false;

void check_lcd_brightness_change() {
  if (lcd_brightness_changed) {
    lcd_set_brightness(lcd_brightness);
    lcd_brightness_changed = false;
  }
}


bool sw_down = false;
bool sw_up = false;
bool sw_home_down = false;
bool sw_home_up = false;

unsigned long last_isr_time;

void loop() {
  if (sw_home_down || sw_home_up) {
    if (sw_home_up) {
      delay(100);
      deep_sleep_with_imu_interrupt();
    }
  }
  if (is_sw_push_long_press_reached()) {
    deep_sleep_with_imu_interrupt();
  }
  if (imu_interrupted) {
    imu_interrupted = false;
    int16_t accX = 0, accY = 0, accZ = 0;
    read_imu(&accX, &accY, &accZ);
    Serial.printf("imu_interrupted! acc=%ld,%ld,%ld\r\n", accX, accY, accZ);
  }
  if (current_page != PAGE_3D) {
    fillScreen(ST77XX_BLACK);
  }
  check_update_battery();
  // check_lcd_brightness_change();
  if (current_page == PAGE_CLOCK || current_page == PAGE_TIMER) {
    draw_bmp280();
    page_1_2();
    // draw_cursor();
    draw_battery_percent();
    if (current_page == PAGE_CLOCK) {
      if (modify_time_mode) {
        delay(500);
      } else {
        delay(1000);
      }
    } else {
      delay(100);
    }
    if (modify_time_mode) {
      if (sw_down) {
        sw_down = false;
        modify_time_digit++;
        if (modify_time_digit > 5) {
          modify_time_digit = 0;
          modify_time_mode = false;
        }
      }
      if (sw_up) {
        sw_up = false;
        now = rtc.now();
        Serial.printf("modify_time_digit is %d\r\n", modify_time_digit);
        switch (modify_time_digit) {
          case 0:
            {
              uint8_t second =  now.second();
              if (second >= 59) {
                second = 0;
              } else {
                second++;
              }
              Serial.printf("setsecond=%d\r\n", second);
              now.setsecond(second);
            }
            break;
          case 1:
            {
              uint8_t minute =  now.minute();
              if (minute >= 59) {
                minute = 0;
              } else {
                minute++;
              }
              Serial.printf("setminute=%d\r\n", minute);
              now.setminute(minute);
            }
            break;
          case 2:
            {
              uint8_t hour =  now.hour();
              if (hour >= 23) {
                hour = 0;
              } else {
                hour++;
              }
              Serial.printf("sethour=%d\r\n", hour);
              now.sethour(hour);
            }
            break;
          case 3:
            {
              uint8_t day =  now.day();
              if (day >= 31) {
                day = 1;
              } else {
                day++;
              }
              Serial.printf("setday=%d\r\n", day);
              now.setday(day);
            }
            break;
          case 4:
            {
              uint8_t month =  now.month();
              if (month >= 12) {
                month = 1;
              } else {
                month++;
              }
              Serial.printf("setmonth=%d\r\n", month);
              now.setmonth(month);
            }
            break;
          case 5:
            {
              uint16_t year =  now.year();
              if (year >= 2030) {
                year = 2018;
              } else {
                year++;
              }
              Serial.printf("setyear=%d\r\n", year);
              now.setyear(year);
            }
            break;
        }
        rtc.adjust(now);
        Serial.printf("rtc.adjust()\r\n");

      }
    } else if (sw_down) {
      while (digitalRead(SWITCH_DOWN) == 0);
      sw_down = false;
      if (millis() - last_isr_time > 1000) {
        Serial.println("switch down released, modify time mode!");
        modify_time_mode = true;
      }
    }
  } else if (current_page == PAGE_KEYBOARD) {
    draw_bmp280();
    page_keyboard();
    draw_cursor();
    draw_battery_percent();
    // send frame then delay
    sendGRAM();
    delay(25); // fps wrong fix
  } else if (current_page == PAGE_ELECTRONIC_LEVEL) {
    page_electronic_level();
    draw_battery_percent();
    // send frame then delay
    sendGRAM();
    delay(25); // fps wrong fix
  } else if (current_page == PAGE_3D) {
    if (!_3d_inited) {
      init_3d();
      _3d_inited = true;
    }
    page_3d();
    delay(20);
  } else if (current_page == PAGE_MAZE) {
    page_maze();
  } else if (current_page == PAGE_FLAPPY_BIRD) {
    page_flappy_bird();
    // code goes here indicates that flappy-bird has exited.
  }

  loopTime = millis();
  endTime = loopTime;
  unsigned long delta = endTime - startTime;
  fps = 1000 / delta;
  // Serial.printf("fill+draw+send GRAM cost: %ldms, calc fps:%ld, real fps:%ld\r\n", delta, fps, fps > 60 ? 60 : fps);
  check_battery_warning_and_escape();
}


#define ISR_DITHERING_TIME_MS   300
#define ISR_SHORT_DITHERING_TIME_MS   50

// 中断函数
void home_isr() {
  if (millis() - last_isr_time < ISR_SHORT_DITHERING_TIME_MS) {
    return;
  }
  last_isr_time = millis();
  if (digitalRead(BUTTON_HOME)) {
    sw_home_up = true;
  } else {
    sw_home_down = true;
  }
}

void navigateToNextPage() {
  current_page++;
  if (current_page > PAGE_COUNT - 1) {
    current_page = 0;
  }
}

long last_sw_push_down_time = 0;

bool is_sw_push_long_press_reached() {
  bool reached = last_sw_push_down_time != 0 && (millis() - last_sw_push_down_time > LONG_PRESS_TIME);
  if (reached) {
    Serial.println("sw_push_long_press_reached!");
  }
  return reached;
}

// 中断函数
void sw_push_isr() {
  feed_battery_warning();
  if (digitalRead(SWITCH_PUSH) == LOW) {
    // key down
    if (millis() - last_isr_time < ISR_DITHERING_TIME_MS) {
      return;
    }
    last_sw_push_down_time = millis();
  } else {
    // key up
    last_sw_push_down_time = 0;
    navigateToNextPage();
  }
  last_isr_time = millis();
}

// 中断函数
void sw_up_isr() {
  if (millis() - last_isr_time < ISR_DITHERING_TIME_MS) {
    return;
  }
  feed_battery_warning();
  last_isr_time = millis();
  if (current_page == PAGE_CLOCK) {
    // 调节时间
    sw_up = true;
  }
}

// 中断函数
void sw_down_isr() {
  if (millis() - last_isr_time < ISR_DITHERING_TIME_MS) {
    return;
  }
  feed_battery_warning();
  last_isr_time = millis();
  if (current_page == PAGE_CLOCK) {
    // 进入调节时间模式
    sw_down = true;
  }
}

void old_button_isr() {
  if (millis() - last_isr_time < ISR_DITHERING_TIME_MS) {
    return;
  }
  feed_battery_warning();
  last_isr_time = millis();
  if (current_page == PAGE_KEYBOARD) {
    // Serial.printf("cursorX=%d, cursorY=%d\r\n", cursorX, cursorY);
    clicked_cursor_x = cursorX;
    clicked_cursor_y = cursorY;
  } else {
    lcd_brightness++;
    if (lcd_brightness > LCD_MAX_BRIGHTNESS) {
      lcd_brightness = LCD_MIN_BRIGHTNESS;
    }
    Serial.printf("lcd_set_brightness=%d\r\n", lcd_brightness);
    lcd_brightness_changed = true;
  }
}
