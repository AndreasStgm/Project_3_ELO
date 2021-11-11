#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <RF24.h>

#define RFID_RST 9
#define RFID_SS 10

#define ESP_SS 6

struct RFIDPayload
{
    byte uid[10];
    byte uidSize;
    char name[20];
    bool userIdentified;
} userPayload;

MFRC522 RfidReader(RFID_SS, RFID_RST); // Instance of the class
RF24 Transceiver(7, 8);                // using pin 7 for the CE pin, and pin 8 for the CSN pin

//-----RFID Users-----
byte Steven[4] = {167, 154, 66, 51};
byte Andreas[4] = {163, 245, 191, 50};

//-----nRF24 variables-----
uint64_t address = 0x696969696969;

//-----Function Declaration-----

void printDec(byte *buffer, byte bufferSize);
void UserCorrect(byte *userUid, byte userUidSize, String userName);
void UserIncorrect(byte *userUid, byte userUidSize);
void SendRF(RFIDPayload message);

//----------
void setup()
{
    Serial.begin(9600);    // Initialize serial communications with the PC
    SPI.begin();           // Init SPI bus
    RfidReader.PCD_Init(); // Init MFRC522 card

    if (!Transceiver.begin()) //Init transceiver on the SPI bus
    {
        Serial.println("Radio hardware not responding.");
        while (1) //When the radio is not responding, hold in an infinite loop
        {
        }
    }
    // Set the PA Level low to try preventing power supply related problems
    // because these examples are likely run with nodes in close proximity to
    // each other.
    Transceiver.setPALevel(RF24_PA_LOW);             //RF24_PA_MAX is default.
    Transceiver.setPayloadSize(sizeof(userPayload)); //cutting down on transmission time by only sending needed data
    Transceiver.openWritingPipe(address);
    Transceiver.stopListening(); //makes it so that this transceiver is configured as transmitter
}

void loop()
{
    if (!RfidReader.PICC_IsNewCardPresent()) // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
        return;
    if (!RfidReader.PICC_ReadCardSerial()) // Select one of the cards
        return;

    if (*RfidReader.uid.uidByte == *Steven)
    {
        printDec(RfidReader.uid.uidByte, RfidReader.uid.size);
        UserCorrect(RfidReader.uid.uidByte, RfidReader.uid.size, "Steven");
        SendRF(userPayload);
    }
    else if (*RfidReader.uid.uidByte == *Andreas)
    {
        printDec(RfidReader.uid.uidByte, RfidReader.uid.size);
        UserCorrect(RfidReader.uid.uidByte, RfidReader.uid.size, "Andreas");
        SendRF(userPayload);
    }
    else
    {
        printDec(RfidReader.uid.uidByte, RfidReader.uid.size);
        UserIncorrect(RfidReader.uid.uidByte, RfidReader.uid.size);
        SendRF(userPayload);
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

    Serial.print(userName);
    Serial.println(" identified.");
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

    Serial.println("Unknown UID");
}
void SendRF(RFIDPayload message)
{
    unsigned long start_timer = micros(); //starting the timer

    bool report = Transceiver.write(&message, sizeof(struct RFIDPayload)); //transmit & save the report
    unsigned long end_timer = micros();                                    //ending the timer
    if (report)                                                            //if the report == true => delivery was successful
    {
        Serial.println("Transmission successful");
        Serial.print("Time to transmit: ");
        Serial.print(end_timer - start_timer); //result of the timer
        Serial.println("us");
    }
    else
        Serial.println("Transmission failed or timed out");
}