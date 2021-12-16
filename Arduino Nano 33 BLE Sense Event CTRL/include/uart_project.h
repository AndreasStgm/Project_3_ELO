#include <Arduino.h>

#define commsSerial Serial2 //the serial used for communicating between the two devices
#define debugSerial Serial  //used for debugging

#define SOT 0xFA //start of transmission
#define STX 0xFB //start of text
#define ETX 0xFC //end of text
#define EOT 0xFD //end of transmission

#define BOOL_TRUE 0xFF
#define BOOL_FALSE 0xFE

struct RFIDPayload
{
    byte uid[10];
    byte uidSize;
    char name[20];
    bool userIdentified;
};

RFIDPayload commsRead();

RFIDPayload formatIntoStruct();

void commsSend(RFIDPayload *sendPayload);
