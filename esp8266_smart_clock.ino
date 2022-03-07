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

enum MENU : int8_t {
    CLOCK = 0,
    CLOCK_WITH_DATE,
    SENSOR_READS,
    SENSOR_READS_PLOT_T,
    SENSOR_READS_PLOT_H,
    WEATHER_TODAY,
    WEATHER_WEEK,
    MAX_MENU_IDX
};

int8_t menu_transitions[MAX_MENU_IDX][6] = {
    // set, middle, right, left, down, up
    { -1, CLOCK_WITH_DATE, -1, -1, -1, -1 },
    { -1, -1, -1, -1, SENSOR_READS, WEATHER_WEEK },
    { -1, -1, SENSOR_READS_PLOT_H, SENSOR_READS_PLOT_T, WEATHER_TODAY, CLOCK_WITH_DATE },
    { -1, -1, SENSOR_READS, SENSOR_READS_PLOT_H, WEATHER_TODAY, CLOCK_WITH_DATE },
    { -1, -1, SENSOR_READS_PLOT_T, SENSOR_READS, WEATHER_TODAY, CLOCK_WITH_DATE },
    { -1, -1, -1, -1, WEATHER_WEEK , SENSOR_READS},
    { -1, -1, -1, -1, CLOCK_WITH_DATE, WEATHER_TODAY },
};

uint8_t current_menu = CLOCK;
uint32_t menu_return_interval = 10 * 1000; // 10 s

void clock_view();
void clock_with_date_view();
void sensor_reads_view();
void weather_today_view();
void weather_week_view();

VFD display;
uint32_t display_refresh_interval = 100; // 100 ms
uint16_t brightness = 0x0F;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

enum JOYSTICK_EVENT{
    RESET   = 0x01,
    SET     = 0x02,
    MIDDLE  = 0x04,
    RIGHT   = 0x08,
    LEFT    = 0x10,
    DOWN    = 0x20,
    UP      = 0x40
};

uint8_t joystick_event = 0;
PCF8574 pcf(0x20);

void ICACHE_RAM_ATTR PCF_interrupt()
{
    uint8_t read = 0xFF ^ pcf.read8();
    if (read) {
        joystick_event = read;
    }
}

uint8_t temperature;
uint8_t humidity;
constexpr uint8_t sensor_history_size = 40;
uint8_t temperature_history[sensor_history_size];
uint8_t humidity_history[sensor_history_size];
uint8_t history_len = 0;
uint32_t sensor_read_interval = 1 * 1000; // 10 s
uint32_t history_write_interval = 72 * 60 * 1000; // 72 min

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

    //Serial.begin(9600);
}

uint32_t display_previous_milis = 0;
uint32_t menu_previous_milis = 0;
uint32_t sensor_read_previous_milis = 0;
uint32_t history_write_previous_milis = 0;
uint32_t current_milis = 0;

void loop()
{
    current_milis = millis();

    if (joystick_event) {
        int8_t transition = -1;

        switch (joystick_event) {
        case RESET:
            break;
        case SET:
            transition = 0;
            break;
        case MIDDLE:
            transition = 1;
            break;
        case RIGHT:
            transition = 2;
            break;
        case LEFT:
            transition = 3;
            break;
        case DOWN:
            transition = 4;
            break;
        case UP:
            transition = 5;
            break;
        }
        if (transition >= 0) {
            if (menu_transitions[current_menu][transition] >= 0) {
                current_menu = menu_transitions[current_menu][transition];
            }
        }
        joystick_event = 0;
        menu_previous_milis = current_milis;
    } else if (current_milis - menu_previous_milis >= menu_return_interval) {
        current_menu = CLOCK;
    }

    if (current_milis - display_previous_milis >= display_refresh_interval) {
        brightness = (brightness * 15 + max(analogRead(A0) - 256*3, 1)) >> 4;
        display.setBrightness(brightness > 0xFF ? 0xFF : brightness);

        switch (current_menu) {
        case CLOCK:
            clock_view();
            break;
        case CLOCK_WITH_DATE:
            clock_with_date_view();
            break;
        case SENSOR_READS:
            sensor_reads_view();
            break;
        case SENSOR_READS_PLOT_T:
            sensor_reads_plot_view('T');
            break;
        case SENSOR_READS_PLOT_H:
            sensor_reads_plot_view('H');
            break;
        case WEATHER_TODAY:
            weather_today_view();
            break;
        case WEATHER_WEEK:
            weather_week_view();
            break;
        }
        
        timeClient.update();

        display_previous_milis = current_milis;
    }

    if (current_milis - sensor_read_previous_milis >= sensor_read_interval) {
        uint8_t read = (uint8_t)dht.readTemperature();
        uint8_t retries = 16;

        while (read == 0xFF && retries >= 0) {
            delayMicroseconds(10);
            read = (uint8_t)dht.readTemperature();
            --retries;
        }
        temperature = read;

        read = (uint8_t)dht.readHumidity();
        retries = 16;

        while (read == 0xFF && retries >= 0) {
            delayMicroseconds(10);
            read = (uint8_t)dht.readHumidity();
            --retries;
        }

        humidity = read;
        
        sensor_read_previous_milis = current_milis;
    }

    if (current_milis - history_write_previous_milis >= history_write_interval) {
        for (uint8_t i = 0; i < sensor_history_size - 1; ++i) {
            temperature_history[i] = temperature_history[i + 1];
            humidity_history[i] = humidity_history[i + 1];
        }
        temperature_history[sensor_history_size - 1] = temperature;
        humidity_history[sensor_history_size - 1] = humidity;

        if (history_len < sensor_history_size) {
            ++history_len;
        }

        history_write_previous_milis = current_milis;
    }
}

void clock_view() {
    display.displayTime(timeClient.getHours(), timeClient.getMinutes(), timeClient.getSeconds());
}

void clock_with_date_view() {
    display.displayDateAndTime(timeClient.getEpochTime());
}

void sensor_reads_view() {
    char print_str[9];
    display.loadCustomDigits(CUSTOM_FONT::FONT_TYPE::SMALL);
    sprintf(print_str, "T%c%c H%c%c ", (temperature / 10) % 10, temperature % 10, (humidity / 10) % 10, humidity % 10);
    display.writeStr(print_str);
}

void sensor_reads_plot_view(char c) {
    switch (c){
    case 'T':
        display.plot(history_len, temperature_history, 'T');
        break;
    case 'H':
        display.plot(history_len, temperature_history, 'H');
        break;
    }
}
void weather_today_view() {
    display.writeStr("W TODAY ");
}

void weather_week_view() {
    display.writeStr("W WEEK  ");
}
