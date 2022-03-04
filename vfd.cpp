#include "vfd.h"

VFD::VFD() {
    
}

VFD::VFD(uint8_t din, uint8_t clk, uint8_t cs, uint8_t reset) {
    _din = din;
    _clk = clk;
    _cs = cs;
    _reset = reset;

    pinMode(clk, OUTPUT);
    pinMode(din, OUTPUT);
    pinMode(cs, OUTPUT);
    pinMode(reset, OUTPUT);

    digitalWrite(reset, LOW);
    delayMicroseconds(5);
    digitalWrite(reset, HIGH);

    setDigits(0x07); // 8 digits
    setBrightness(0x0F);
}

void VFD::setBrightness(uint8_t brightness) {
    digitalWrite(_cs, LOW);
    writeData(0xe4);
    delayMicroseconds(5);
    writeData(brightness);
    digitalWrite(_cs, HIGH);
    delayMicroseconds(5);
}

void VFD::writeStr(uint8_t* str) {
    digitalWrite(_cs, LOW);
    writeData(0x20);
    for (uint8_t i = 0; i < 8; ++i) {
        writeData(str[i]);
    }
    digitalWrite(_cs, HIGH);
    show();
}

void VFD::setCustomChar(uint8_t address, uint8_t* custom_char) {
    digitalWrite(_cs, LOW);
    writeData(0x40 | (address & 0x07));
    delayMicroseconds(5);
    for (uint8_t i = 0; i < 5; ++i ) {
        writeData(custom_char[i] & 0x7F);
        delayMicroseconds(5);
    }
    digitalWrite(_cs, HIGH);
    delayMicroseconds(5);
}

void VFD::setDigits(uint8_t digits) {
    digitalWrite(_cs, LOW);
    writeData(0xe0);
    delayMicroseconds(5);
    writeData(digits);
    digitalWrite(_cs, HIGH);
    delayMicroseconds(5);
}

void VFD::writeData(uint8_t data) {
    uint8_t i;
    for (i = 0; i < 8; i++)
    {
        digitalWrite(_clk, LOW);
        digitalWrite(_din, (data & 0x01) ? HIGH : LOW);
        data >>= 1;
        digitalWrite(_clk, HIGH);
    }
}

void VFD::show(void)
{
    digitalWrite(_cs, LOW);
    writeData(0xe8);
    digitalWrite(_cs, HIGH);
}