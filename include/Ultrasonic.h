#pragma once
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

class Ultrasonic
{
public:
    void begin();
    uint16_t distanceMm();

private:
    static constexpr uint8_t TRIG = PB2; // pin 10
    static constexpr uint8_t ECHO = PB3; // pin 11
};
