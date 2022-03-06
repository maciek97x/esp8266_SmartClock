#include <Arduino.h>
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#define PCF8574_LOW_MEMORY
#include <PCF8574.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <Wire.h>

#include "config.h"
#include "vfd.h"

VFD display;
uint16_t brightness = 0x0F;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

uint8_t joystick_event = 0;
PCF8574 pcf(0x20);

void ICACHE_RAM_ATTR PCF_interrupt()
{
  uint8_t read = 0xFF ^ pcf.read8();
  if (read) {
    joystick_event = read;
  }
}

DHT dht(DHT_PIN, DHT11);

void setup() {
    pinMode(BUILTIN_LED, OUTPUT);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        digitalWrite(BUILTIN_LED, LOW);
        delay(100);
        digitalWrite(BUILTIN_LED, HIGH);
        delay(100);
    }

    timeClient.begin();
    
    pcf.begin();

    pinMode(PCF_IRQ_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(PCF_IRQ_PIN), PCF_interrupt, FALLING);
    
    dht.begin();

    display = VFD(VFD_DIN, VFD_CLK, VFD_CS, VFD_RST);
    display.setBrightness(0x0F);

    Serial.begin(9600);
}

void loop()
{
    Serial.printf("%d %d\n", analogRead(A0), analogRead(A0) - 256*3);
    brightness = (brightness * 15 + max(analogRead(A0) - 256*3, 1)) >> 4;
    display.setBrightness(brightness > 0xFF ? 0xFF : brightness);
    
    timeClient.update();
    Serial.printf("%d:%d:%d\n", timeClient.getHours(), timeClient.getMinutes(), timeClient.getSeconds());
    if ((timeClient.getSeconds() >> 2) & 0x1) {
        display.displayTime(timeClient.getHours(), timeClient.getMinutes(), timeClient.getSeconds());
    } else {
        display.displayDateAndTime(timeClient.getEpochTime());
    }
    delay(100);
}
