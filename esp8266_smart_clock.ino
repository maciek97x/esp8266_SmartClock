#include "Arduino.h"
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#include "config.h"
#include "vfd.h"

VFD display;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

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

    display = VFD(VFD_DIN, VFD_CLK, VFD_CS, VFD_RST);
    display.setBrightness(0xFF);

    Serial.begin(9600);
}

void loop()
{
    timeClient.update();
    Serial.printf("%d:%d:%d\n", timeClient.getHours(), timeClient.getMinutes(), timeClient.getSeconds());
    if (0) {//(timeClient.getSeconds() >> 2) & 0x1) {
        display.displayTime(timeClient.getHours(), timeClient.getMinutes(), timeClient.getSeconds());
    } else {
        Serial.println(display.displayDateAndTime(timeClient.getEpochTime()));
    }
    delay(100);
}
