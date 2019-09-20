/**
   StreamHTTPClient.ino

    Created on: 24.05.2015
    Benchmark!
*/

#include <Arduino.h>

#include <WiFi.h>
#include <WiFiMulti.h>

#include <HTTPClient.h>

#define USE_SERIAL Serial

WiFiMulti wifiMulti;

void setup() {

  USE_SERIAL.begin(115200);

  USE_SERIAL.println();
  USE_SERIAL.println();
  USE_SERIAL.println();

  for (uint8_t t = 4; t > 0; t--) {
    USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
    USE_SERIAL.flush();
    delay(1000);
  }

  wifiMulti.addAP("MIWIFI8", "12345678");

}

uint8_t buff[4096] = { 0 };

void loop() {
  // wait for WiFi connection
  if ((wifiMulti.run() == WL_CONNECTED)) {

    HTTPClient http;

    USE_SERIAL.print("[HTTP] begin...\n");

    // configure server and url
    http.begin("http://mirrors.aliyun.com/ubuntu-releases/disco/ubuntu-19.04-desktop-amd64.iso");
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
        while (http.connected() && (len > 0 || len == -1)) {
          // get available data size
          size_t size = stream->available();
          if (size) {
            // read up byte
            int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
            total_bytes += c;
            // write it to Serial
            USE_SERIAL.printf("read %d bytes, total read %ld bytes, speed: %d kB/s\r\n", c, total_bytes, total_bytes / (millis() - start_time));
            if (len > 0) {
              len -= c;
            }
          }
          delay(1);
        }

        USE_SERIAL.println();
        USE_SERIAL.print("[HTTP] connection closed or file end.\n");

      }
    } else {
      USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  }

  delay(10000);
}
