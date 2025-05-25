#pragma once
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

class Buzzer
{
public:
    void begin();
    void tone(uint16_t freq, uint16_t ms);
    void stop();

    static volatile uint16_t _remain;

private:
    static uint8_t pickPrescaler(uint16_t freq, uint8_t &ocrOut);
};
