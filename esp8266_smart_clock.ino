#include "Arduino.h"

#include "vfd.h"

uint8_t din   = 12; // DA
uint8_t clk   = 13; // CK
uint8_t cs    = 0; // CS
uint8_t reset = 2; // RS

uint8_t mychars[5*8] = {
    0b1010101,
    0b0101010,
    0b1010101,
    0b0101010,
    0b1010101,
    
    0b1010101,
    0b0101010,
    0b1010101,
    0b0101010,
    0b1010101,
    
    0b1010101,
    0b0101010,
    0b1010101,
    0b0101010,
    0b1010101,
    
    0b1010101,
    0b0101010,
    0b1010101,
    0b0101010,
    0b1010101,
    
    0b1010101,
    0b0101010,
    0b1010101,
    0b0101010,
    0b1010101,
    
    0b1010101,
    0b0101010,
    0b1010101,
    0b0101010,
    0b1010101,
    
    0b1010101,
    0b0101010,
    0b1010101,
    0b0101010,
    0b1010101,
    
    0b1010101,
    0b0101010,
    0b1010101,
    0b0101010,
    0b1010101
    };

VFD display;

void setup() {
    display = VFD(din, clk, cs, reset);

    display.setBrightness(0xFF);

    for (uint8_t i = 0; i < 8; ++i) {
        display.setCustomChar(i, &mychars[5*i]);
    }
    Serial.begin(9600);
}



int16_t pe = 1;
int16_t dpe = 1;

void loop()
{
    uint8_t print_str[8] = {0, 1, 2, 3, 4, 5, 6, 7};
    for (uint8_t i = 0; i < 5*8; ++i) {
        mychars[i] <<= 1;
        if ((rand() %  128) < pe) {
        mychars[i] |= 1;
        }
    }
    for (uint8_t i = 0; i < 8; ++i) {
        display.setCustomChar(i, &mychars[5*i]);
    }

    if (pe <= 1) {
        dpe = 1;
    }
    if (pe >= 128) {
        dpe = -1;
    }
    pe += dpe;
    
    display.writeStr(print_str);
    delay(100);
}
