#include <ESP8266WiFi.h>
#include <Ambient.h>
#include <BME280I2C.h>
#include <Wire.h>

#define WIFI_SSID ""
#define WIFI_PASSWORD ""

float samplingPeriod = 6.0;  // second

unsigned int channelId = 0;
const char* writeKey = "";

WiFiClient client;
Ambient ambient;

uint8_t tosr   = 0x1;  // No oversampling
uint8_t hosr   = 0x1;  // No oversampling
uint8_t posr   = 0x0;  // Skipped
uint8_t mode   = 0x3;  // Normal mode
uint8_t st     = 0x2;  // 125 ms
uint8_t filter = 0x4;  // 16
BME280I2C bme(tosr, hosr, posr, mode, st, filter);
bool metric = true;

// FIXME: workaround for initializing. 2 values should be read in setup.
float h_1 = 0;
float h_2 = 0;

void setup() {
  while(!bme.begin()) delay(500);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  ambient.begin(channelId, writeKey, &client);
}

void loop() {
  float temp(NAN), hum(NAN), pres(NAN);
  uint8_t pressureUnit(0);
  bme.read(pres, temp, hum, metric, pressureUnit);

  // FIXME: workaround for initializing. 2 values should be read in setup.
  if (h_2 != 0) {
    float d2_h;
    d2_h = (hum - 2* h_1 + h_2) / (samplingPeriod * samplingPeriod);
    char strd2_h[16];
    dtostrf(d2_h, 5, 3, strd2_h);
    ambient.set(1, strd2_h);
    ambient.send();
  }

  h_2 = h_1;
  h_1 = hum;

  delay((unsigned long)samplingPeriod * 1000);
}
