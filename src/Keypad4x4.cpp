#include "Keypad4x4.h"

Keypad4x4::Keypad4x4()
{
    /* Linii: 12(PB4) 8(PB0) 7(PD7) 6(PD6)
       Coloane: 5(PD5) 4(PD4) 3(PD3) 2(PD2) */
    DDRB |= (1 << PB4) | (1 << PB0);  // output
    DDRD |= (1 << PD7) | (1 << PD6);  // output
    PORTB |= (1 << PB4) | (1 << PB0); // HIGH
    PORTD |= (1 << PD7) | (1 << PD6); // HIGH

    DDRD &= ~((1 << PD5) | (1 << PD4) | (1 << PD3) | (1 << PD2)); // input
    PORTD |= (1 << PD5) | (1 << PD4) | (1 << PD3) | (1 << PD2);   // pull-up
}

void Keypad4x4::selectRow(uint8_t r)
{
    if (r == 0)
        PORTB &= ~(1 << PB4);
    else
        PORTB |= (1 << PB4);

    if (r == 1)
        PORTB &= ~(1 << PB0);
    else
        PORTB |= (1 << PB0);

    if (r == 2)
        PORTD &= ~(1 << PD7);
    else
        PORTD |= (1 << PD7);

    if (r == 3)
        PORTD &= ~(1 << PD6);
    else
        PORTD |= (1 << PD6);
}

char Keypad4x4::getKey()
{
    for (uint8_t row = 0; row < 4; ++row)
    {
        selectRow(row);
        _delay_us(3);
        uint8_t cols = ~PIND & ((1 << PD5) | (1 << PD4) | (1 << PD3) | (1 << PD2));
        if (cols)
        {
            _delay_ms(20); // debounce
            cols = ~PIND & ((1 << PD5) | (1 << PD4) | (1 << PD3) | (1 << PD2));
            if (cols)
            {
                if (cols & (1 << PD5))
                    return keymap[row][0];
                if (cols & (1 << PD4))
                    return keymap[row][1];
                if (cols & (1 << PD3))
                    return keymap[row][2];
                if (cols & (1 << PD2))
                    return keymap[row][3];
            }
        }
    }
    return 0;
}
