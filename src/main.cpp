#include <Arduino.h>
#include "Keypad4x4.h"
#include "Ultrasonic.h"
#include "Buzzer.h"

Keypad4x4 keypad;
Ultrasonic us; // HC-SR04: trig = PB2 (pin 10), echo = PB3 (pin 11)
Buzzer bz;     // buzzer pe pin 13 (PB5) – toggling în Timer2 ISR

// parametri configurabili
static constexpr uint16_t PRESENCE_MAX_MM = 500;      // < 50 cm activeaza sistemul
static constexpr uint32_t PRESENCE_TIMEOUT = 15000UL; // 15 s fara mişcare - idle
static constexpr uint16_t DIST_CHECK_MS = 300;        // cat de des citim senzorul
static constexpr char CORRECT_CODE[] = "1234";        // PIN

// state
bool presence = false;
uint32_t lastPresenceMS = 0;
uint32_t lastDistCheck = 0;
String entered;

void setup()
{
  Serial.begin(9600);
  us.begin();
  bz.begin();

  Serial.println(F("Asteptare detectie prezenta (< 50 cm)..."));
}

void beepKey() { bz.tone(500, 100); }
void beepSuccess()
{
  bz.tone(2000, 50);
  delay(100);
  bz.tone(2000, 500);
}
void beepFailure() { bz.tone(400, 600); }
void beepCloseDoor() { bz.tone(1000, 200); }

void loop()
{
  uint32_t now = millis();

  if (now - lastDistCheck >= DIST_CHECK_MS)
  {
    lastDistCheck = now;
    uint16_t dmm = us.distanceMm(); // 0xFFFF = timeout
    if (dmm != 0xFFFF && dmm <= PRESENCE_MAX_MM)
    {
      presence = true;
      lastPresenceMS = now;
      static bool firstMsg = true;
      if (firstMsg)
      {
        Serial.println(F("Persoana detectata! Introduceti codul (apasa D pentru deschidere):"));
        firstMsg = false;
        bz.tone(1000, 200);
      }
    }
    else if (presence && (now - lastPresenceMS > PRESENCE_TIMEOUT))
    {
      presence = false;
      entered = "";
      Serial.println(F("Zona libera. Standby..."));
      Serial.println(F("Asteptare detectie prezenta (<50 cm)..."));
    }
  }

  if (!presence)
    return;

  char k = keypad.getKey();
  if (!k)
    return;

  Serial.print(F("Tasta: "));
  Serial.println(k);

  while (keypad.getKey())
    ;
  delay(15); // debounce suplimentar

  if (k >= '0' && k <= '9')
  {
    if (entered.length() < 4)
    {
      beepKey();
      entered += k;
      Serial.print(F("Cod partial: "));
      Serial.println(entered);
    }
  }
  else if (k == 'D')
  {
    if (entered == CORRECT_CODE)
    {
      beepSuccess();
      Serial.println(F("Acces PERMIS! Usa deschisa."));
    }
    else
    {
      beepFailure();
      Serial.println(F("Cod gresit. Acces REFUZAT."));
    }
    entered = "";
  }
  else if (k == 'C')
  {
    beepCloseDoor();
    Serial.println(F("Usa inchisa."));
    entered = "";
  }
  else
  {
    beepKey();
    Serial.println(F("Tasta ignorata."));
  }
}
