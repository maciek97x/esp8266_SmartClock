#pragma once
#include "Arduino.h"

class VFD {
public:
    VFD();
    VFD(uint8_t din, uint8_t clk, uint8_t cs, uint8_t reset);

    void setBrightness(uint8_t brightness);
    void writeStr(uint8_t* str);
    void setCustomChar(uint8_t address, uint8_t* custom_char);
    void setDigits(uint8_t digits);

private:
    uint8_t _din;
    uint8_t _clk;
    uint8_t _cs;
    uint8_t _reset;

    void writeData(uint8_t data);
    void show();
};