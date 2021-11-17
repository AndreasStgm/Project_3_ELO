#include <Arduino.h>
#include "uart_project.h"

#define DEBUG

RFIDPayload receivedPayload;

void setup(void)
{
  debugSerial.begin(9600);
  commsSerial.begin(115200);
}

void loop(void)
{
  if (commsSerial.available() > 0)
  {
    receivedPayload = commsRead();
#ifdef DEBUG
    debugSerial.print("Name: ");
    debugSerial.print(receivedPayload.name);
    debugSerial.print("\tUID: ");
    for (byte i = 0; i < receivedPayload.uidSize; i++)
    {
      debugSerial.print(receivedPayload.uid[i] < 0x10 ? " 0" : " ");
      debugSerial.print(receivedPayload.uid[i], DEC);
    }
    debugSerial.print("\tRecognized: ");
    debugSerial.println(receivedPayload.userIdentified);
#endif
  }
  delay(10);

  // if (commsSerial.available() > 1)
  // {
  //   debugSerial.print(commsSerial.read());
  //   debugSerial.print(" ");
  // }
  // delay(10);
}