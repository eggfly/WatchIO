#ifndef _CONFIG_H_
#define _CONFIG_H_

#define SCREEN_WIDTH  160
#define SCREEN_HEIGHT  80

// #define APP_DEBUG

#define BUTTON_HOME   0

// #define LIU_YU_XUAN_ENABLED   1

#ifdef WATCHIO_OLD_VERSION 
  #define SWITCH_UP     9
#else
  #define SWITCH_UP     5
#endif

#define SWITCH_DOWN  12
#define SWITCH_PUSH  13

#define VBAT_SENSOR  32


#define LCD_MAX_BRIGHTNESS      255
#define LCD_MIN_BRIGHTNESS      0

#define LCD_DEFAULT_BRIGHTNESS  25

#define LONG_PRESS_TIME   1000

#endif // _CONFIG_H_
