#include <Arduino.h>
//#include <C:/Users/Suicidal_Kitten/.platformio/packages/framework-arduinoespressif32/libraries/Wire/src/Wire.h>
#include <Wire.h>

struct RFIDPayload
{
  byte uid[10];
  byte uidSize;
  char name[20];
  bool userIdentified;
} userPayload;

void receiveCallback(int amount)
{
  while (1 < Wire.available())
  {
    char c = Wire.read();
    Serial.print(c);
  }
  int x = Wire.read();
  Serial.print(x);
}

void setup(void)
{
  Serial.begin(9600);
  Wire.setPins(21, 22);
  Wire.begin(21, 22);
  Wire.setClock(400000);
  Wire.onReceive(receiveCallback);
}

void loop(void)
{
  delay(100);
}