#include "Buzzer.h"

volatile uint16_t Buzzer::_remain = 0;

static constexpr struct
{
    uint8_t csBits;
    uint16_t div;
} prescTbl[] = {
    {1, 1},   // CS20  prescaler 1
    {2, 8},   // CS21
    {3, 32},  // CS21|CS20
    {4, 64},  // CS22
    {5, 128}, // CS22|CS20
    {6, 256}, // CS22|CS21
    {7, 1024} // CS22|CS21|CS20
};

uint8_t Buzzer::pickPrescaler(uint16_t freq, uint8_t &ocrOut)
{
    for (auto p : prescTbl)
    {
        uint32_t ocr = (F_CPU / 2UL / p.div) / freq - 1;
        if (ocr <= 255 && ocr >= 1)
        {
            ocrOut = static_cast<uint8_t>(ocr);
            return p.csBits;
        }
    }
    ocrOut = 255;
    return 7; // prescaler 1024
}

void Buzzer::begin()
{
    DDRB |= (1 << PB5); // pin 13 out
    PORTB &= ~(1 << PB5);
}

void Buzzer::tone(uint16_t freq, uint16_t ms)
{
    if (!freq)
    {
        stop();
        return;
    }

    uint8_t ocr, cs;
    cs = pickPrescaler(freq, ocr);
    OCR2A = ocr;

    TCCR2A = (1 << WGM21);
    TCCR2B = cs;
    TIMSK2 = (1 << OCIE2A);

    _remain = ms;
}

void Buzzer::stop()
{
    TCCR2B = 0;
    TIMSK2 = 0;
    PORTB &= ~(1 << PB5);
}

ISR(TIMER2_COMPA_vect)
{
    PORTB ^= (1 << PB5);

    if (Buzzer::_remain)
    {
        static uint16_t msCnt = 0;
        ++msCnt;
        if (msCnt >= Buzzer::_remain)
        {
            TCCR2B = 0;
            TIMSK2 = 0;
            PORTB &= ~(1 << PB5);
            msCnt = 0;
        }
    }
}
