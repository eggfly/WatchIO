#ifndef _POWER_H
#define _POWER_H

#include "soc/rtc.h"
#include <Wire.h>
#include "config.h"
#include "lcd.h"

// #define LOW_CPU_FREQ

void init_power() {
#ifdef LOW_CPU_FREQ
  rtc_cpu_freq_config_t conf = {
  };
  uint32_t source_freq_mhz = rtc_clk_xtal_freq_get();
  conf.source = RTC_CPU_FREQ_SRC_XTAL;
  conf.source_freq_mhz = source_freq_mhz;
  conf.div = source_freq_mhz / 10;
  conf.freq_mhz = 10;
  Serial.print("div=");
  Serial.println(conf.div);
  rtc_clk_cpu_freq_set_config(&conf);
#else
  // rtc_clk_cpu_freq_set(RTC_CPU_FREQ_80M);
#endif
  Wire.begin();
}

#endif // _POWER_H
