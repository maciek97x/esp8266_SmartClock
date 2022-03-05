#include "vfd_custom_font.h"

uint8_t custom_font_small[] = {
    // small 0
    0b0000000,
    0b1111100,
    0b1000100,
    0b1111100,
    0b0000000,
    // small 1
    0b0000000,
    0b0000000,
    0b0000000,
    0b1111100,
    0b0000000,
    // small 2
    0b0000000,
    0b1110100,
    0b1010100,
    0b1011100,
    0b0000000,
    // small 3
    0b0000000,
    0b1010100,
    0b1010100,
    0b1111100,
    0b0000000,
    // small 4
    0b0000000,
    0b0011100,
    0b0010000,
    0b1111100,
    0b0000000,
    // small 5
    0b0000000,
    0b1011100,
    0b1010100,
    0b1110100,
    0b0000000,
    // small 6
    0b0000000,
    0b1111100,
    0b1010100,
    0b1110100,
    0b0000000,
    // small 7
    0b0000000,
    0b0000100,
    0b0000100,
    0b1111100,
    0b0000000,
    // small 8
    0b0000000,
    0b1111100,
    0b1010100,
    0b1111100,
    0b0000000,
    // small 9
    0b0000000,
    0b1011100,
    0b1010100,
    0b1111100,
    0b0000000,
    // small colon
    0b0000000,
    0b0000000,
    0b0101000,
    0b0000000,
    0b0000000,
};
uint8_t custom_font_big[] = {
    // big 0
    0b1111111,
    0b1000001,
    0b1000001,
    0b1000001,
    0b1111111,
    // big 1
    0b0000000,
    0b0000000,
    0b0000000,
    0b0000000,
    0b1111111,
    // big 2
    0b1111001,
    0b1001001,
    0b1001001,
    0b1001001,
    0b1001111,
    // big 3
    0b1001001,
    0b1001001,
    0b1001001,
    0b1001001,
    0b1111111,
    // big 4
    0b0001111,
    0b0001000,
    0b0001000,
    0b0001000,
    0b1111111,
    // big 5
    0b1001111,
    0b1001001,
    0b1001001,
    0b1001001,
    0b1111001,
    // big 6
    0b1111111,
    0b1001001,
    0b1001001,
    0b1001001,
    0b1111001,
    // big 7
    0b0000001,
    0b0000001,
    0b0000001,
    0b0000001,
    0b1111111,
    // big 8
    0b1111111,
    0b1001001,
    0b1001001,
    0b1001001,
    0b1111111,
    // big 9
    0b1001111,
    0b1001001,
    0b1001001,
    0b1001001,
    0b1111111,
    // big colon
    0b0000000,
    0b0000000,
    0b0100010,
    0b0000000,
    0b0000000,
};

uint8_t* CUSTOM_FONT::getChar(uint8_t c, uint8_t font_type) {
    uint8_t* font;
    switch (font_type) {
    case FONT_TYPE::SMALL:
        font = custom_font_small;
        break;
    case FONT_TYPE::BIG:
        font = custom_font_big;
        break;
    }
    if ('0' <= c && c <= '9') {
        return &font[DIGIT(c - '0')];
    }
    if (':' == c) {
        return &font[COLON];
    }
}

uint8_t* CUSTOM_FONT::getDigit(uint8_t d, uint8_t font_type) {
    return getChar(d + '0', font_type);
}