#pragma once
#include "Arduino.h"

#define DIGIT(digit) 5*(digit)
#define COLON 5*(10)

namespace CUSTOM_FONT {
    enum FONT_TYPE{
        SMALL = 0,
        BIG   = 1
    };

    uint8_t* getChar(uint8_t c, uint8_t font_type);
    uint8_t* getDigit(uint8_t d, uint8_t font_type);
}