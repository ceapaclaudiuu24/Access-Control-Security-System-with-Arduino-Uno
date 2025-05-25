#pragma once
#include <avr/io.h>
#include <stdint.h>

class ServoPWM
{
public:
    void begin(); // initializeaza Timer1
    void setAngle(uint8_t deg);

private:
    static constexpr uint16_t kMin = 2000; // 1 ms prescaler 8 (16 MHz)
    static constexpr uint16_t kMax = 4000; // 2 ms
};
