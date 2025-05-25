#include "Ultrasonic.h"
#include <Arduino.h>

void Ultrasonic::begin()
{
    DDRB |= (1 << TRIG);  // trig out
    DDRB &= ~(1 << ECHO); // echo in
}

uint16_t Ultrasonic::distanceMm()
{
    // pulse 10 us
    PORTB &= ~(1 << TRIG);
    _delay_us(2);
    PORTB |= (1 << TRIG);
    _delay_us(10);
    PORTB &= ~(1 << TRIG);

    uint32_t start = micros();
    while (!(PINB & (1 << ECHO)))
        if (micros() - start > 20000)
            return 0xFFFF; // timeout
    uint32_t echoStart = micros();
    while (PINB & (1 << ECHO))
        if (micros() - echoStart > 30000)
            return 0xFFFF;
    uint32_t echoTime = micros() - echoStart;
    return (uint16_t)(echoTime / 5.8);
}
