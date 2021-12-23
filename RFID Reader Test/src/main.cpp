#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include "uart_project.h"

#define RFID_RST 2
#define RFID_SS 10

#define DEBUG // Enable to have debug info

UARTPayload userPayload;

MFRC522 RfidReader(RFID_SS, RFID_RST); // Instance of the class

// //-----RFID Users-----
byte Steven[4] = {167, 154, 66, 51};
byte Andreas[4] = {163, 245, 191, 50};

// //-----Function Declaration-----

void printDec(byte *buffer, byte bufferSize);
void UserCorrect(String userName);
void UserIncorrect();

//----------
void setup()
{
#ifdef DEBUG
    debugSerial.begin(9600); // Initialize serial communications with the PC
#endif
    commsSerial.begin(115200); // Init serial comm
    SPI.begin();               // Init SPI for card reader
    RfidReader.PCD_Init();     // Init MFRC522 card
}

void loop()
{
    if (!RfidReader.PICC_IsNewCardPresent()) // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
        return;
    if (!RfidReader.PICC_ReadCardSerial()) // Select one of the cards
        return;

    if (*RfidReader.uid.uidByte == *Steven) // Check voor steven tag
    {
#ifdef DEBUG
        printDec(RfidReader.uid.uidByte, RfidReader.uid.size);
#endif
        UserCorrect("Steven");
        commsSend(&userPayload);
    }
    else if (*RfidReader.uid.uidByte == *Andreas) // Check voor andreas kaart
    {
#ifdef DEBUG
        printDec(RfidReader.uid.uidByte, RfidReader.uid.size);
#endif
        UserCorrect("Andreas");
        commsSend(&userPayload);
    }
    else
    {
#ifdef DEBUG
        printDec(RfidReader.uid.uidByte, RfidReader.uid.size);
#endif
        UserIncorrect();
        commsSend(&userPayload);
    }

    delay(1000); //change value if you want to read cards faster
}

//-----Functions-----
void printDec(byte *buffer, byte bufferSize)
{
    for (byte i = 0; i < bufferSize; i++)
    {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], DEC);
    }
    Serial.println();
}
void UserCorrect(String userName)
{
    strncpy(userPayload.rfidName, userName.c_str(), 20);
#ifdef DEBUG
    Serial.print(userName);
    Serial.println(" identified.");
#endif
}
void UserIncorrect()
{
    strncpy(userPayload.rfidName, "Unidentified", 20);
#ifdef DEBUG
    Serial.println("Unknown UID");
#endif
}