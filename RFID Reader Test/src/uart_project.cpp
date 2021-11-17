#include <Arduino.h>
#include "uart_project.h"

//#define DEBUG

byte readData[36];

RFIDPayload commsRead()
{
    while (commsSerial.available())
    {
        if (commsSerial.read() == SOT)
        {
            commsSerial.readBytesUntil(EOT, readData, sizeof(readData));
        }
    }
    return formatIntoStruct();
}

RFIDPayload formatIntoStruct()
{
#ifdef DEBUG
    for (uint8_t i = 0; i < sizeof(readData); i++)
    {
        debugSerial.print(readData[i]);
        debugSerial.print(" ");
    }
    debugSerial.println();
#endif

    RFIDPayload formattedData;

    formattedData.uidSize = readData[0]; //uidSize

    for (uint8_t i = 0; i < formattedData.uidSize; i++) //uid
        formattedData.uid[i] = readData[i + 1];

    if (readData[formattedData.uidSize + 1] == BOOL_TRUE) //userIdentified
        formattedData.userIdentified = true;
    else
        formattedData.userIdentified = false;

    if (readData[formattedData.uidSize + 2] == STX) //name
    {
        for (uint8_t i = 0; i < sizeof(readData); i++)
        {
            if (readData[i + formattedData.uidSize + 3] == ETX)
                break;
            else
                formattedData.name[i] = readData[i + formattedData.uidSize + 3];
        }
    }

    return formattedData;
}

void commsSend(RFIDPayload *sendPayload)
{
    if (commsSerial.availableForWrite() > 0)
    {
        commsSerial.write(SOT); //start of transmission

        commsSerial.write(sendPayload->uidSize); //uidSize

        for (uint8_t i = 0; i < sendPayload->uidSize; i++) //uid
            commsSerial.write(sendPayload->uid[i]);

        if (sendPayload->userIdentified == true) //userIdentified
            commsSerial.write(BOOL_TRUE);
        else
            commsSerial.write(BOOL_FALSE);

        commsSerial.write(STX); //name
        for (uint8_t i = 0; i < sizeof(sendPayload->name); i++)
        {
            if (sendPayload->name != NULL)
                commsSerial.write(sendPayload->name[i]);
            else
                break;
        }
        commsSerial.write(ETX);

        commsSerial.write(EOT);
    }
}