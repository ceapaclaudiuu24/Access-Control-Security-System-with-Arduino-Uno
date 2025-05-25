#include "ServoPWM.h"

void ServoPWM::begin()
{
    DDRB |= (1 << PB1);                                 // pin 9 out
    TCCR1A = (1 << COM1A1);                             // non-inverting PWM
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11); // mode 14, prescaler 8
    TCCR1A |= (1 << WGM11);
    ICR1 = 40000; // TOP - 20 ms period
    OCR1A = kMin; // default (0)
}

void ServoPWM::setAngle(uint8_t deg)
{
    if (deg > 180)
        deg = 180;
    uint16_t pwm = kMin + ((uint32_t)(kMax - kMin) * deg) / 180;
    OCR1A = pwm;
}
