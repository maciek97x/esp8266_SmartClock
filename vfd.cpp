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

void VFD::writeStr(const char* str) {
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

void VFD::displayTime(uint8_t hours, uint8_t minutes, uint8_t seconds) {
    uint8_t print_str[] = {0, 1, 6, 2, 3, 6, 4, 5};

    setCustomChar(0, CUSTOM_FONT::getDigit((hours / 10) % 10, CUSTOM_FONT::FONT_TYPE::BIG));
    setCustomChar(1, CUSTOM_FONT::getDigit(hours % 10, CUSTOM_FONT::FONT_TYPE::BIG));
    setCustomChar(2, CUSTOM_FONT::getDigit((minutes / 10) % 10, CUSTOM_FONT::FONT_TYPE::BIG));
    setCustomChar(3, CUSTOM_FONT::getDigit(minutes % 10, CUSTOM_FONT::FONT_TYPE::BIG));
    setCustomChar(4, CUSTOM_FONT::getDigit((seconds / 10) % 10, CUSTOM_FONT::FONT_TYPE::BIG));
    setCustomChar(5, CUSTOM_FONT::getDigit(seconds % 10, CUSTOM_FONT::FONT_TYPE::BIG));
    setCustomChar(6, CUSTOM_FONT::getChar(':', CUSTOM_FONT::FONT_TYPE::BIG));

    writeStr(print_str);
}

void VFD::displayDateAndTime(time_t epoch_time) {
    uint8_t i;
    uint8_t current_char[5];
    struct tm *datetime;
    datetime = localtime (&epoch_time);

    uint8_t hours = datetime->tm_hour;
    uint8_t minutes = datetime->tm_min;
    uint8_t seconds = datetime->tm_sec;
    uint8_t weekday = (datetime->tm_wday + 6) % 7;
    uint8_t day = datetime->tm_mday;
    uint8_t month = datetime->tm_mon + 1;
    uint16_t year = datetime->tm_year + 1900;

    uint8_t print_str[] = {0, 1, 2, 3, 4, 5, 6, 7};

    memcpy(current_char, CUSTOM_FONT::getDigit((hours / 10) % 10, CUSTOM_FONT::FONT_TYPE::SMALL), sizeof(uint8_t)*5);
    if (weekday < 5) {
        current_char[weekday] |= 0x1;
    }
    setCustomChar(0, current_char);
    
    memcpy(current_char, CUSTOM_FONT::getDigit(hours % 10, CUSTOM_FONT::FONT_TYPE::SMALL), sizeof(uint8_t)*5);
    if (weekday >= 5) {
        current_char[weekday - 5] |= 0x1;
    }
    setCustomChar(1, current_char);

    setCustomChar(2, CUSTOM_FONT::getChar(':', CUSTOM_FONT::FONT_TYPE::SMALL));
    setCustomChar(3, CUSTOM_FONT::getDigit((minutes / 10) % 10, CUSTOM_FONT::FONT_TYPE::SMALL));

    memcpy(current_char, CUSTOM_FONT::getDigit(minutes % 10, CUSTOM_FONT::FONT_TYPE::SMALL), sizeof(uint8_t)*5);
    for (i = 0; i < 5; ++i) {
        if ((day >> i) & 0x1) {
            current_char[4 - i] |= 0x1;
        }
    }
    setCustomChar(4, current_char);

    memcpy(current_char, CUSTOM_FONT::getChar(':', CUSTOM_FONT::FONT_TYPE::SMALL), sizeof(uint8_t)*5);
    for (i = 0; i < 5; ++i) {
        if ((month >> i) & 0x1) {
            current_char[4 - i] |= 0x1;
        }
    }
    setCustomChar(5, current_char);

    memcpy(current_char, CUSTOM_FONT::getDigit((seconds / 10) % 10, CUSTOM_FONT::FONT_TYPE::SMALL), sizeof(uint8_t)*5);
    for (i = 0; i < 5; ++i) {
        if (((year / 100) >> i) & 0x1) {
            current_char[4 - i] |= 0x1;
        }
    }
    setCustomChar(6, current_char);

    memcpy(current_char, CUSTOM_FONT::getDigit(seconds % 10, CUSTOM_FONT::FONT_TYPE::SMALL), sizeof(uint8_t)*5);
    for (i = 0; i < 5; ++i) {
        if (((year % 100) >> i) & 0x1) {
            current_char[4 - i] |= 0x1;
        }
    }
    setCustomChar(7, current_char);

    writeStr(print_str);
}

void VFD::loadCustomDigits(CUSTOM_FONT::FONT_TYPE font_type) {
    for (uint8_t i = 0; i < 10; ++i) {
        setCustomChar(i, CUSTOM_FONT::getDigit(i, font_type));
    }
}

void VFD::plot(uint8_t data_len, uint8_t *data, char label) {
    uint8_t print_str[] = {0, 1, 2, 3, 4, 5, 6, 7};
    
    uint8_t plot[40];

    uint8_t min_value = 0xFF;
    uint8_t max_value = 0x0;
    
    for (uint8_t i = 0; i < 40; ++i) {
        plot[i] = 0;
    }

    for (uint8_t i = 0; i < data_len; ++i) {
        max_value = max(max_value, data[i]);
        min_value = min(min_value, data[i]);
    }

    for (uint8_t i = 0; i < data_len; ++i) {
        uint8_t value = 3;
        if (max_value != min_value) {
            value = 6 * (data[i] - min_value) / (max_value - min_value);
        }
        plot[40 - data_len + i] |= 0x1 << (6 - value);
    }
    if (label) {
        plot[0] &= 0b11110000;
        plot[1] &= 0b11110000;
        plot[2] &= 0b11110000;
        plot[3] &= 0b11110000;
    }
    switch (label) {
    case 'T':
        plot[0] |= 0b001;
        plot[1] |= 0b111;
        plot[2] |= 0b001;
        break;
    case 'H':
        plot[0] |= 0b111;
        plot[1] |= 0b010;
        plot[2] |= 0b111;
        break;
    }

    for (uint8_t i = 0; i < 8; ++i) {
        setCustomChar(i, &plot[5*i]);
    }
    
    writeStr(print_str);
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