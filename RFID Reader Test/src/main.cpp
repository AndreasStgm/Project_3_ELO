#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 9
#define SS_PIN 10

MFRC522 mfrc522(SS_PIN, RST_PIN); // Instance of the class

MFRC522::MIFARE_Key key;

byte Steven[4] = {167, 154, 66, 51};

//-----Function Declaration-----
byte printDec(byte *buffer, byte bufferSize);

//----------
void setup()
{
    Serial.begin(9600); // Initialize serial communications with the PC
    SPI.begin();        // Init SPI bus
    mfrc522.PCD_Init(); // Init MFRC522 card
}

void loop()
{
    if (!mfrc522.PICC_IsNewCardPresent()) // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
    {
        return;
    }
    if (!mfrc522.PICC_ReadCardSerial()) // Select one of the cards
    {
        return;
    }
    //mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid)); //dump some details about the card

    // printHex(mfrc522.uid.uidByte, mfrc522.uid.size);
    // Serial.print("\n");
    // printDec(mfrc522.uid.uidByte, mfrc522.uid.size);
    // Serial.print("\n");

    if (printDec(mfrc522.uid.uidByte, mfrc522.uid.size) == *Steven)
    {
        Serial.print("Tis ferm goed steven!\n");
    }

    delay(1000); //change value if you want to read cards faster
}

//-----Functions-----
byte printDec(byte *buffer, byte bufferSize)
{
    for (byte i = 0; i < bufferSize; i++)
    {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], DEC);
    }
    Serial.println();
    return *buffer;
}