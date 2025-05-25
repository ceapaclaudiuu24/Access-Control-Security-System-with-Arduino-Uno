#pragma once
#include <LiquidCrystal_I2C.h>

class LcdI2C
{
public:
    void begin();
    void clear() { lcd.clear(); }
    void print(uint8_t col, uint8_t row, const char *txt);

private:
    LiquidCrystal_I2C lcd{0x27, 16, 2};
};
