#include <Arduino.h>
#include <nrf52840.h>
#include <nrf52840_peripherals.h>
#include <uart_project.h>
#include <SpeechRec.h>
#include <Scheduler.h>
#include <SPI.h>

//-----Function Declaration-----
String RFID_Read();
String RX_Handler();
SchedulerTask voice_recognition();
String stemherkenning();
void setupComm();

//-----Variable Declaration-----
String naam_received = "";
bool stem_herkent = false;
bool tag_herkent = false;
String rfid_naam = "";
String stem_naam = "";

void setup()
{
    setupComm();
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_BLUE, OUTPUT);
}

void loop()
{
    naam_received = RX_Handler();
    if (naam_received == "Steven")
    {
        if (stemherkenning() == "Steven")
        {
            //publish steven naar mqtt
        }
        else if (stemherkenning() != "Steven")
        {
            rfid_naam = RFID_Read();
        }
    }
    else if (naam_received == "Andreas")
    {
        if (stemherkenning() == "Andreas")
        {
            //publish steven naar mqtt
        }
        else if (stemherkenning() != "Andreas")
        {
            rfid_naam = RFID_Read();
        }
    }
    else
    {
        stem_naam = stemherkenning();
        rfid_naam = RFID_Read();
    }
    String writenames = SOT + STX + stem_naam + ETX + STX + rfid_naam + ETX + EOT;
    Serial1.print(writenames);
}

String RX_Handler()
{
    if (Serial1.available())
    {
        if (Serial1.readString() == "STX")
            return (Serial1.readStringUntil(ETX));
    }
}

String RFID_Read()
{
    String naam;
    return naam;
}

String stemherkenning()
{
    String naam;

    return naam;
}

void setupComm()
{
    Serial1.begin(115200); // Init serial comm
    SPI.begin();           // Init SPI for card reader
}
