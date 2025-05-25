#include <Arduino.h>
#include "Keypad4x4.h"

Keypad4x4 keypad;

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  char k = keypad.getKey();
  if (k)
  {
    Serial.print("Tasta: ");
    Serial.println(k);

    while (keypad.getKey())
      ;
    delay(20); // debounce suplimentar
  }
}
