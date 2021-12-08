#include <Arduino.h>
#include "uart_project.h"

#define DEBUG

byte readData[68]; // maximaal aantal bytes dat het bericht kan bevatten

UARTPayload commsRead()
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

UARTPayload formatIntoStruct()
{
#ifdef DEBUG
    for (uint8_t i = 0; i < sizeof(readData); i++)
    {
        debugSerial.print(readData[i]);
        debugSerial.print(" ");
    }
    debugSerial.println();
#endif

    UARTPayload formattedData;
    uint8_t readDataLocation = 0;

    if (readData[readDataLocation] == STX) //facial recognition name
    {
        readDataLocation++;
        for (uint8_t i = 0; i < sizeof(readData); i++)
        {
            if (readData[i] == ETX)
            {
                readDataLocation++;
                break;
            }
            else
            {
                readDataLocation++;
                formattedData.facialName[i] = readData[i + 1];
            }
        }
    }

    return formattedData;
}

void commsSend(UARTPayload *sendPayload)
{
    if (commsSerial.availableForWrite() > 0)
    {
        commsSerial.write(SOT); //start of transmission

        commsSerial.write(STX); // facial recognition name
        for (uint8_t i = 0; i < sizeof(sendPayload->facialName); i++)
        {
            if (sendPayload->facialName[i] != NULL)
                commsSerial.write(sendPayload->facialName[i]);
            else
                break;
        }
        commsSerial.write(ETX);

        commsSerial.write(STX); // audio recognition name
        for (uint8_t i = 0; i < sizeof(sendPayload->audioName); i++)
        {
            if (sendPayload->audioName[i] != NULL)
                commsSerial.write(sendPayload->audioName[i]);
            else
                break;
        }
        commsSerial.write(ETX);

        commsSerial.write(STX); // rfid tag name
        for (uint8_t i = 0; i < sizeof(sendPayload->rfidName); i++)
        {
            if (sendPayload->rfidName[i] != NULL)
                commsSerial.write(sendPayload->rfidName[i]);
            else
                break;
        }
        commsSerial.write(ETX);

        commsSerial.write(EOT); // end of transmission
    }
}