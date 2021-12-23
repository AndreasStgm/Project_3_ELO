#include <Arduino.h>

#define commsSerial Serial1 //the serial used for communicating between the two devices
#define debugSerial Serial  //used for debugging

#define SOT 0xFF //start of transmission
#define EOT 0xFE //end of transmission
#define STX 0xFD //start of text
#define ETX 0xFC //end of text

struct UARTPayload
{
    char facialName[20];
    char audioName[20];
    char rfidName[20];
};

UARTPayload commsRead();

UARTPayload formatIntoStruct();

void commsSend(UARTPayload *sendPayload);
