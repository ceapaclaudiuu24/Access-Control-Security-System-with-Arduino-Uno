# Access-Control-Door-System

A compact Arduino-based access-control system that combines a 4×4 keypad, ultrasonic presence sensor, servo-driven latch, buzzer and I²C LCD.  
It lets a visitor unlock the door by entering the correct 4-digit code, shows clear prompts on the display, and stores all critical data (door state and code) in the MCU’s EEPROM so it survives power loss.

---

## Features

* **Presence detection** – HC-SR04 ultrasonic module wakes the interface only when someone is within 50 cm.  
* **Keypad authentication** – 4-digit PIN (default `1234`) with visual and acoustic feedback.  
* **Persistent settings** – last door state and current PIN are kept in EEPROM.  
* **Servo lock** – standard hobby servo toggles between *closed* (25 °) and *open* (140 °).  
* **Automatic time-outs** – if no one is detected for 30 s the system returns to idle; after 5 s more it auto-locks.  
* **On-device PIN change** – press **A** while the door is open to enter a new code and confirm it.  
* **Audible cues** – single tone for keypad presses, success/failure jingles, continuous beep while the door is unlocked.

---

## Hardware

| Module | MCU Pin(s) | Notes |
|--------|------------|-------|
| Keypad 4×4 | Rows: PB4, PB0, PD7, PD6<br>Cols: PD5, PD4, PD3, PD2 | Active-low scanning |
| Ultrasonic HC-SR04 | Trig → PB2 (D10) <br>Echo → PB3 (D11) | Direct pulse-timing, no interrupts |
| Servo | OC1A PB1 (D9) | 50 Hz PWM via Timer 1 |
| Buzzer | PB5 (D13) | Square wave via Timer 2 |
| LCD 16×2 I²C | A4 (SDA), A5 (SCL) | Uses `LiquidCrystal_I2C` |
| Optional serial debug | UART 0 @ 9600 Bd | `Serial.begin(9600)` in `begin()` |

Built and tested on Arduino Uno (ATmega328P @ 16 MHz).
