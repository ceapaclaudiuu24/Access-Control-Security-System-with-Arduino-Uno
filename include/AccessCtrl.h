#pragma once
#include <Arduino.h>
#include <avr/eeprom.h>

#include "Keypad4x4.h"
#include "Ultrasonic.h"
#include "Buzzer.h"
#include "ServoPWM.h"
#include "LcdI2C.h"

class AccessCtrl
{
public:
    void begin();
    void task();

private:
    /* periferice */
    Keypad4x4 kp;
    Ultrasonic us;
    Buzzer bz;
    ServoPWM sv;
    LcdI2C lcd;

    /* parametri */
    static constexpr uint16_t PRES_MM = 500;
    static constexpr uint32_t PRES_TO_MS = 30000UL;
    static constexpr uint32_t AUTO_CLOSE_MS = 5000UL;
    static constexpr uint16_t DIST_INT_MS = 300;
    static constexpr uint8_t SERVO_OPEN = 140;
    static constexpr uint8_t SERVO_CLOSE = 25;
    static constexpr uint8_t CODE_LEN = 4;

    /* EEPROM layout */
    static uint8_t EEMEM eeDoorState;
    static uint8_t EEMEM eeCode[CODE_LEN];

    /* stare curenta */
    bool presence = false;
    bool doorOpen = false;
    uint32_t lastPresMS = 0;
    uint32_t lastDistMS = 0;
    uint32_t noPresenceSince = 0;
    uint8_t presCount = 0;
    String entered;
    String tmpNew;

    enum class Mode : uint8_t
    {
        IDLE,
        AWAIT_CODE,
        DOOR_OPEN,
        CHANGE_NEW,
        CHANGE_CONFIRM
    };
    Mode mode = Mode::IDLE;

    /* intern */
    void beepKey() { bz.tone(500, 100); }
    void beepSuccess()
    {
        bz.tone(2000, 50);
        delay(100);
        bz.tone(2000, 500);
    }
    void beepFailure() { bz.tone(400, 600); }
    void beepCloseDoor() { bz.tone(1000, 200); }

    void openDoor();
    void closeDoor();
    void showIdle();
    void loadCode(char *buf);
    void saveCode(const char *buf);
    bool checkCode(const String &s);
};
