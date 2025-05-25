#include "AccessCtrl.h"

uint8_t EEMEM AccessCtrl::eeDoorState = 0;
uint8_t EEMEM AccessCtrl::eeCode[AccessCtrl::CODE_LEN];

void AccessCtrl::begin()
{
    kp = Keypad4x4();
    us.begin();
    bz.begin();
    sv.begin();
    lcd.begin();

    doorOpen = eeprom_read_byte(&eeDoorState);
    sv.setAngle(doorOpen ? SERVO_OPEN : SERVO_CLOSE);

    char cod[CODE_LEN];
    loadCode(cod);

    Serial.begin(9600);
    bz.tone(1000, 50);
    showIdle();
}

void AccessCtrl::task()
{
    uint32_t now = millis();

    if (now - lastDistMS >= DIST_INT_MS)
    {
        lastDistMS = now;
        uint16_t d = us.distanceMm();
        if (d != 0xFFFF && d <= PRES_MM)
        {
            if (!presence)
            {
                presence = true;
                lastPresMS = now;
                noPresenceSince = 0;

                if (doorOpen)
                {
                    lcd.clear();
                    lcd.print(0, 0, "Door is OPEN");
                    lcd.print(0, 1, "A=chg,B/C=lock");
                    Serial.println(F("Persoana detectata - usa deja deschisa."));
                    bz.tone(800, 150);
                    mode = Mode::DOOR_OPEN;
                }
                else
                {
                    lcd.clear();
                    lcd.print(0, 0, "Insert code:");
                    lcd.print(0, 1, "____");
                    Serial.println(F("Persoana detectata! Introdu codul."));
                    bz.tone(1000, 200);
                    mode = Mode::AWAIT_CODE;
                }
            }
        }
        else if (presence && (now - lastPresMS > PRES_TO_MS))
        {
            presence = false;
            entered = "";
            tmpNew = "";
            noPresenceSince = now;
            mode = Mode::IDLE;
            showIdle();
        }

        if (!presence && doorOpen && noPresenceSince &&
            (now - noPresenceSince >= AUTO_CLOSE_MS))
        {
            Serial.println(F("Fara prezenta > 30 s → usa se inchide automat."));
            closeDoor();
            noPresenceSince = 0;
        }
    }

    char k = kp.getKey();
    if (!k)
        return;

    while (kp.getKey())
        ;
    delay(15);

    if (k >= '0' && k <= '9')
        beepKey();
    else if (k == 'D' || k == 'C' || k == 'A' || k == 'B')
        beepKey();

    switch (mode)
    {
    case Mode::IDLE:
        break;

    case Mode::AWAIT_CODE:
        if (k >= '0' && k <= '9')
        {
            if (entered.length() < CODE_LEN)
            {
                entered += k;
                lcd.print(entered.length() - 1, 1, "*");
            }
        }
        else if (k == 'D')
        {
            if (checkCode(entered))
            {
                openDoor();
            }
            else
            {
                beepFailure();
                lcd.clear();
                lcd.print(0, 0, "Wrong code");
                delay(1000);
                lcd.clear();
                lcd.print(0, 0, "Insert code:");
                lcd.print(0, 1, "____");
            }
            entered = "";
        }
        else if (k == 'C')
        {
            closeDoor();
            entered = "";
        }
        break;

    case Mode::DOOR_OPEN:
        if (k == 'C')
        {
            closeDoor();
        }
        else if (k == 'A')
        {
            tmpNew = "";
            lcd.clear();
            lcd.print(0, 0, "New code:");
            lcd.print(0, 1, "____");
            mode = Mode::CHANGE_NEW;
        }
        break;

    case Mode::CHANGE_NEW:
        if (k >= '0' && k <= '9')
        {
            if (tmpNew.length() < CODE_LEN)
            {
                tmpNew += k;
                lcd.print(tmpNew.length() - 1, 1, "*");
            }
        }
        else if (k == 'B')
        {
            if (tmpNew.length() == CODE_LEN)
            {
                lcd.clear();
                lcd.print(0, 0, "Confirm code:");
                lcd.print(0, 1, "____");
                entered = "";
                mode = Mode::CHANGE_CONFIRM;
            }
        }
        break;

    case Mode::CHANGE_CONFIRM:
        if (k >= '0' && k <= '9')
        {
            if (entered.length() < CODE_LEN)
            {
                entered += k;
                lcd.print(entered.length() - 1, 1, "*");
            }
        }
        else if (k == 'B')
        {
            if (entered == tmpNew && entered.length() == CODE_LEN)
            {
                saveCode(entered.c_str());
                beepSuccess();
                lcd.clear();
                lcd.print(0, 0, "Code changed!");
            }
            else
            {
                beepFailure();
                lcd.clear();
                lcd.print(0, 0, "Mismatch!");
            }
            delay(1200);
            entered = "";
            tmpNew = "";
            lcd.clear();
            lcd.print(0, 0, "Door is OPEN");
            mode = Mode::DOOR_OPEN;
        }
        break;
    }
}

void AccessCtrl::openDoor()
{
    sv.setAngle(SERVO_OPEN);
    doorOpen = true;
    eeprom_write_byte(&eeDoorState, 1);
    beepSuccess();
    lcd.clear();
    lcd.print(0, 0, "Door OPEN");
    lcd.print(0, 1, "A=chg,B/C=lock");
    mode = Mode::DOOR_OPEN;
}

void AccessCtrl::closeDoor()
{
    sv.setAngle(SERVO_CLOSE);
    doorOpen = false;
    eeprom_write_byte(&eeDoorState, 0);
    beepCloseDoor();

    if (presence)
    {
        lcd.clear();
        lcd.print(0, 0, "Door LOCKED");
        delay(1000);
        lcd.clear();
        lcd.print(0, 0, "Insert code:");
        lcd.print(0, 1, "____");
        Serial.println(F("Usa inchisa. Se asteapta un nou cod."));
        mode = Mode::AWAIT_CODE;
    }
    else
    {
        showIdle();
        mode = Mode::IDLE;
    }
}

void AccessCtrl::showIdle()
{
    lcd.clear();
    lcd.print(0, 0, doorOpen ? "Door OPEN" : "Door LOCKED");
    Serial.println(F("Asteptare detectie persoana..."));
}

void AccessCtrl::loadCode(char *buf)
{
    for (uint8_t i = 0; i < CODE_LEN; i++)
    {
        buf[i] = eeprom_read_byte(&eeCode[i]);
        if (buf[i] < '0' || buf[i] > '9')
        {
            strcpy(buf, "1234");
            saveCode(buf);
            break;
        }
    }
}

void AccessCtrl::saveCode(const char *buf)
{
    for (uint8_t i = 0; i < CODE_LEN; i++)
        eeprom_update_byte(&eeCode[i], buf[i]);
}

bool AccessCtrl::checkCode(const String &s)
{
    char buf[CODE_LEN + 1]{};
    loadCode(buf);
    return s == String(buf).substring(0, CODE_LEN);
}
