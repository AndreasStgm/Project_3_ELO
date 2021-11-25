#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include "uart_project.h"

#define RFID_RST 9
#define RFID_SS 10

#define DEBUG // Enable to have debug info

RFIDPayload userPayload;

MFRC522 RfidReader(RFID_SS, RFID_RST); // Instance of the class

// //-----RFID Users-----
byte Steven[4] = {167, 154, 66, 51};
byte Andreas[4] = {163, 245, 191, 50};

// //-----Function Declaration-----

void printDec(byte *buffer, byte bufferSize);
void UserCorrect(byte *userUid, byte userUidSize, String userName);
void UserIncorrect(byte *userUid, byte userUidSize);

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

    if (*RfidReader.uid.uidByte == *Steven)
    {
#ifdef DEBUG
        printDec(RfidReader.uid.uidByte, RfidReader.uid.size);
#endif
        UserCorrect(RfidReader.uid.uidByte, RfidReader.uid.size, "Steven");
        commsSend(&userPayload);
    }
    else if (*RfidReader.uid.uidByte == *Andreas)
    {
#ifdef DEBUG
        printDec(RfidReader.uid.uidByte, RfidReader.uid.size);
#endif
        UserCorrect(RfidReader.uid.uidByte, RfidReader.uid.size, "Andreas");
        commsSend(&userPayload);
    }
    else
    {
#ifdef DEBUG
        printDec(RfidReader.uid.uidByte, RfidReader.uid.size);
#endif
        UserIncorrect(RfidReader.uid.uidByte, RfidReader.uid.size);
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
void UserCorrect(byte *userUid, byte userUidSize, String userName)
{
    for (byte i = 0; i < userUidSize; i++)
    {
        userPayload.uid[i] = userUid[i];
    }
    userPayload.uidSize = userUidSize;
    strncpy(userPayload.name, userName.c_str(), 20);
    userPayload.userIdentified = true;
#ifdef DEBUG
    Serial.print(userName);
    Serial.println(" identified.");
#endif
}
void UserIncorrect(byte *userUid, byte userUidSize)
{
    for (byte i = 0; i < userUidSize; i++)
    {
        userPayload.uid[i] = userUid[i];
    }
    userPayload.uidSize = userUidSize;
    strncpy(userPayload.name, "Unidentified", 20);
    userPayload.userIdentified = false;
#ifdef DEBUG
    Serial.println("Unknown UID");
#endif
}