
#include <Arduino.h>

#include <WiFi.h>
#include <WiFiMulti.h>

#include "lcd.h"

#include <HTTPClient.h>

#define USE_SERIAL Serial

WiFiMulti wifiMulti;

#define FPS  10

#define video_width 160
#define video_height 80

#define frame_length (video_width * video_height * 2)
#define buffer_length (5000)

uint8_t buff[buffer_length] = { 0 };
uint8_t * frame_buffer;
int frame_pos = 0;

void setup() {
  frame_buffer = (uint8_t *)malloc(frame_length);
  USE_SERIAL.begin(115200);
  lcd_init();
  USE_SERIAL.printf("fb: %p\r\n", frame_buffer);
  USE_SERIAL.println();
  for (uint8_t t = 3; t > 0; t--) {
    USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
    USE_SERIAL.flush();
    delay(1000);
  }
  wifiMulti.addAP("MIWIFI8", "12345678");
}

void check_and_delay_with_fps(long uptime, int frame_count) {
  long time_diff = frame_count * 1000 / FPS - uptime;
  if (time_diff >= 0) {
    Serial.printf("time_diff=%ld\r\n", time_diff);
    delay(time_diff);
  }
}

void loop() {
  // wait for WiFi connection
  if ((wifiMulti.run() == WL_CONNECTED)) {
    HTTPClient http;
    USE_SERIAL.print("[HTTP] begin...\n");
    // configure server and url
    http.begin("http://192.168.31.233:8000/watchio_videos/tingwode_160x80_10fps.bin");
    // http.begin("http://mirrors.aliyun.com/ubuntu-releases/disco/ubuntu-19.04-desktop-amd64.iso");
    //http.begin("192.168.1.12", 80, "/test.html");
    USE_SERIAL.print("[HTTP] GET...\n");
    // start connection and send HTTP header
    int httpCode = http.GET();
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);
      // file found at server
      if (httpCode == HTTP_CODE_OK) {
        // get lenght of document (is -1 when Server sends no Content-Length header)
        int len = http.getSize();
        // create buffer for read
        // get tcp stream
        WiFiClient * stream = http.getStreamPtr();
        // read all data from server
        long total_bytes = 0;
        long start_time = millis();
        long render_start_time = 0;
        int frame_count = 0;
        while (http.connected() && (len > 0 || len == -1)) {
          // get available data size
          size_t size = stream->available();
          if (size) {
            // read up byte
            int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
            total_bytes += c;
            int new_data_length1 = min(c, (frame_length - frame_pos));
            memcpy(frame_buffer + frame_pos, buff, new_data_length1);
            frame_pos += c;
            bool frame_ready = frame_pos >= frame_length;
            if (frame_ready) {
              if (render_start_time <= 0) {
                render_start_time = millis();
              }
              canvas.drawRGBBitmap((CANVAS_WIDTH - video_width) / 2, 0, (uint16_t*)frame_buffer, video_width, video_height);
              sendGRAM();
              frame_count++;
              check_and_delay_with_fps(millis() - render_start_time, frame_count);
            }
            frame_pos %= frame_length;
            if (c > new_data_length1) {
              memcpy(frame_buffer, buff + new_data_length1, frame_pos);
            }
            // write it to Serial
            USE_SERIAL.printf("read %d bytes, total %ld bytes, %d kB/s\r\n", c, total_bytes, total_bytes / (millis() - start_time));
            if (len > 0) {
              len -= c;
            }
          }
          delay(1);
        }

        USE_SERIAL.printf("video real play time costs: %d ms\r\n", millis() - render_start_time);
        USE_SERIAL.print("[HTTP] connection closed or file end.\r\n");
        // reset position
        frame_pos = 0;
      }
    } else {
      USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  }

  delay(10000);
}
