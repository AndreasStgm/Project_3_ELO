#include <Arduino.h>
#include "uart_project.h"

//#define DEBUG

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
    strncpy(formattedData.facialName, "", 20);
    strncpy(formattedData.audioName, "", 20);
    strncpy(formattedData.rfidName, "", 20);

    uint8_t readDataLocation = 0;

    if (readData[readDataLocation] == STX) //facial recognition name
    {
        for (uint8_t i = 0; i < sizeof(readData); i++)
        {
            readDataLocation++;
            if (readData[i + 1] == ETX)
                break;
            else
                formattedData.facialName[i] = readData[i + 1];
        }
    }
#ifdef DEBUG
    debugSerial.println(readDataLocation);
#endif

    readDataLocation++;
    if (readData[readDataLocation] == STX) //audio recognition name
    {
        for (uint8_t i = 0; i < sizeof(readData); i++)
        {
            readDataLocation++;
            if (readData[readDataLocation] == ETX)
                break;
            else
                formattedData.audioName[i] = readData[readDataLocation];
        }
    }
#ifdef DEBUG
    debugSerial.println(readDataLocation);
#endif

    readDataLocation++;
    if (readData[readDataLocation] == STX) //rfid recognition name
    {
        for (uint8_t i = 0; i < sizeof(readData); i++)
        {
            readDataLocation++;
            if (readData[readDataLocation] == ETX)
                break;
            else
                formattedData.rfidName[i] = readData[readDataLocation];
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
            if (sendPayload->facialName[i] != '\0')
                commsSerial.write(sendPayload->facialName[i]);
            else
                break;
        }
        commsSerial.write(ETX);

        commsSerial.write(STX); // audio recognition name
        for (uint8_t i = 0; i < sizeof(sendPayload->audioName); i++)
        {
            if (sendPayload->audioName[i] != '\0')
                commsSerial.write(sendPayload->audioName[i]);
            else
                break;
        }
        commsSerial.write(ETX);

        commsSerial.write(STX); // rfid tag name
        for (uint8_t i = 0; i < sizeof(sendPayload->rfidName); i++)
        {
            if (sendPayload->rfidName[i] != '\0')
                commsSerial.write(sendPayload->rfidName[i]);
            else
                break;
        }
        commsSerial.write(ETX);

        commsSerial.write(EOT); // end of transmission
    }
}