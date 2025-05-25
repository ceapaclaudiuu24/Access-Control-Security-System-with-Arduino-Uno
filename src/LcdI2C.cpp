#include "LcdI2C.h"

void LcdI2C::begin()
{
    lcd.init();
    lcd.backlight();
}

void LcdI2C::print(uint8_t c, uint8_t r, const char *t)
{
    lcd.setCursor(c, r);
    lcd.print(t);
}
