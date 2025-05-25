#pragma once
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

class Keypad4x4
{
public:
    Keypad4x4();
    char getKey();

private:
    inline static const char keymap[4][4] = {
        {'1', '2', '3', 'A'},
        {'4', '5', '6', 'B'},
        {'7', '8', '9', 'C'},
        {'*', '0', '#', 'D'}};
    void selectRow(uint8_t r);
};