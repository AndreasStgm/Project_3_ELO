#include <Arduino.h>
#include <nrf52840.h>
#include <nrf52840_peripherals.h>
#include <uart_project.h>
#include <Scheduler.h>
#include <SPI.h>
#include <speech_rec.h>
#include <MFRC522.h>

#define RFID_RST 9
#define RFID_SS 10
#define EI_CLASSIFIER_SLICES_PER_MODEL_WINDOW 3
#define led_wit 3
#define led_blauw 4
#define led_rood 5
#define led_geel 6

//-----Function Declaration-----
String RFID_Read();
String RX_Handler();
void setupReaderAndComms();
String RFID_Read_loop();
void send_over_serial(char i);
//-----Variable Declaration-----
bool tag_herkent = false;
String rfid_naam = "";
String stem_naam = "";
const char *names[3] = {"Steven", "Andreas", "noname"}; //all users are stored in this array. Last value is dummy value that returns when std::find() does not find occurence of string.


//-----Instance Declaration-----
MFRC522 RfidReader(RFID_SS, RFID_RST); // Instance of the class
// UARTPayload recognitionPayload;

// //-----RFID Users-----
byte Steven[4] = {0xB9, 0x34, 0xEF, 0xA3};
byte Andreas[4] = {0x9C, 0x6E, 0xF3, 0x8C};

//setups all serial comms and leds
void setup()
{
    setupReaderAndComms();
    setupSpeech();
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_BLUE, OUTPUT);
    pinMode(led_wit, OUTPUT);
    pinMode(led_geel, OUTPUT);
    pinMode(led_blauw, OUTPUT);
    pinMode(led_rood, OUTPUT);
    digitalWrite(LED_RED, HIGH);
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_BLUE, HIGH);
}


//Main program loop
//Checks uart connection from ESP continuously (every time loop starts). This ESP will send Facial Recognition data whenever a face is detected on facial recognition module.
//We have 3 possibilities:
//1) Received name over UART is "unknown", then we check speech recognition and RFID. If these are equal: send 1 back to ESP, the ESP will then send 'open' on the 'slot' topic, which will open the lock
//2) Received name is in names array variable. Next, speech recognition turns on. If the speech recognition name is equal to name_received, we open the lock (^^^^^^)
//3) Received name is in names array variable. Next, speech recognition turns on. If the speech recognition name is not equal to name_received, we try RFID and if RFID name is equal to 
//   name_received, we open the lock. If it is not equal, we close the lock.
void loop()
{
    String name_received = RX_Handler();
    debugSerial.println("naam_received: ");
    debugSerial.println(name_received);

    if (name_received == "unknown")
    {
        String speech_Name = stemherkenning();
        debugSerial.print("Speech Name: ");
        debugSerial.println(speech_Name);
        String RFID_Name = RFID_Read_loop();
        debugSerial.println(RFID_Name);
        if (speech_Name != "unknown" && speech_Name == RFID_Name)
        {
            debugSerial.println("open unknown");
            send_over_serial(1); //sends 1 to ESP, which makes ESP send 'open' on mqtt to open lock.
        }
        else
        {
            debugSerial.println("close unknown");
            send_over_serial(0); //sends 0 to ESP, which makes ESP send 'close' on mqtt to close lock.
        }
    }
    else if (std::find(std::begin(names), std::end(names), name_received) != std::end(names))
    {
        String speech_Name = stemherkenning();
        debugSerial.println(speech_Name);
        if (speech_Name != "unknown" && speech_Name == name_received)
        {
            debugSerial.println("open SPEECH and FACIAL");
            send_over_serial(1); //sends 1 to ESP, which makes ESP send 'open' on mqtt to open lock.
        }
        else
        {
            String RFID_Name = RFID_Read_loop();
            debugSerial.println(RFID_Name);
            if (RFID_Name != "unknown" && RFID_Name == name_received)
            {
                debugSerial.println("open RFID and FACIAL");
                send_over_serial(1); //sends 1 to ESP, which makes ESP send 'open' on mqtt to open lock.
            }
            else
            {
                debugSerial.println("close RFID and FACIAL");
                send_over_serial(0); //sends 0 to ESP, which makes ESP send 'close' on mqtt to close lock.
            }
        }
    }
    delay(500);
}

//This function sends an integer over serial.
void send_over_serial(char i)
{
    commsSerial.write(SOT);
    commsSerial.write(STX);
    commsSerial.print(i);
    commsSerial.write(ETX);
    commsSerial.write(EOT);
}

//This function reads facialName over serial from ESP and returns it. 
String RX_Handler()
{
    if (commsSerial.available() > 0)
    {
        return (String)commsRead().facialName;
    }
    else
    {
        return "None";
    }
}

//This function reads from RFID every .25 seconds. returns if a rfid name is detected.
String RFID_Read_loop()
{
    digitalWrite(led_geel, 1);
    for (int i = 0; i < 20; i++)
    {
        String name = RFID_Read();
        if (name != "unknown")
        {
            digitalWrite(led_geel, 0);
            return name;
        }
    }
    digitalWrite(led_geel, 0);
    return "unknown";

}

void setupReaderAndComms()
{
#ifdef DEBUG
    debugSerial.begin(9600); // Initialize serial communications with the PC
#endif
    commsSerial.begin(115200); // Init serial comm
    SPI.begin();               // Init SPI for card reader
    RfidReader.PCD_Init();     // Init MFRC522 card
}

//returns name of rfid tag if rfid tag is detected.
String RFID_Read()
{
    delay(250);
    if (!RfidReader.PICC_IsNewCardPresent()) // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
        return "unknown";
    if (!RfidReader.PICC_ReadCardSerial()) // Select one of the cards
        return "unknown";

    if (*RfidReader.uid.uidByte == *Steven) // Check for steven tag
    {
        return "Steven";
    }
    else if (*RfidReader.uid.uidByte == *Andreas) // Check for andreas kaart
    {
        return "Andreas";
    }
    else
    {
        return "unknown";
    }
}
