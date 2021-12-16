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
const char *names[3] = {"Steven", "Andreas", "noname"}; //all users are stored in this array. Last value is dummy value that returns when std::find() does not find occurence of string.

void setup()
{
    setupComm();
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_BLUE, OUTPUT);
}


void loop()
{
    // naam_received = RX_Handler();
    naam_received = "Steven"; //dit is gewoon om te simuleren, gebruik lijn hierboven voor echt programma (kweet nie of die functie werkt) 
    if (naam_received == "unknown")
    {
        String speech_Name = stemherkenning();
        String RFID_Name = RFID_Read();
        if (speech_Name != "unknown" && speech_Name == RFID_Name)
        {
            // send open
            Serial.print("open");
        }
        else
        {
            // send close
            Serial.print("close");
        }
    }
    else if (*std::find(std::begin(names), std::end(names), naam_received) != "noname")
    {
        String speech_Name = stemherkenning();
        if (speech_Name != "unknown" && speech_Name == naam_received)
        {
            // send open
            Serial.print("open");
        }
        else
        {
            String RFID_Name = RFID_Read();
            if (RFID_Name != "unknown" && RFID_Name == naam_received)
            {
                // send open
                Serial.print("open");
            }
            else
            {
                // send close
                Serial.print("close");         
            }
        }
    }
    // String writenames = SOT + STX + stem_naam + ETX + STX + rfid_naam + ETX + EOT;
    // Serial1.print(writenames);
    delay(1000);
}

String RX_Handler()
{
    if (Serial1.available())
    {
        if (Serial1.readString() == "STX")
            return (Serial1.readStringUntil(ETX));
    }
    return "Andreas";
}

String RFID_Read()
{
    //Get RFID tag and wait maybe? return "unkown" if none was read.

    String naam = "unknown";
    return naam;
}

String stemherkenning()
{
    //get a name (wait a certain amount of time for a name to be read.)
    //return name if name is read, return something ("unknown?") if not.

    String naam = "Andreas";
    return naam;
}

void setupComm()
{
    Serial.begin(115200); // Init serial comm

    SPI.begin();           // Init SPI for card reader
}


