#include <Arduino.h>
#include <nrf52840.h>
#include <nrf52840_peripherals.h>
#include <uart_project.h>
#include <SpeechRec.h>
#include <Scheduler.h>
#include <SPI.h>
#include <MFRC522.h>

#define RFID_RST 9
#define RFID_SS 10

//-----Function Declaration-----
void UserCorrect(String userName);
void UserIncorrect();
void RFID_Read();
String RX_Handler();
SchedulerTask voice_recognition();
String stemherkenning();
void setupReaderAndComms();

//-----Variable Declaration-----
String naam_received = "";
bool stem_herkent = false;
bool tag_herkent = false;
String rfid_naam = "";
String stem_naam = "";
const char *names[3] = {"Steven", "Andreas", "noname"}; //all users are stored in this array. Last value is dummy value that returns when std::find() does not find occurence of string.

//-----Instance Declaration-----
MFRC522 RfidReader(RFID_SS, RFID_RST); // Instance of the class
UARTPayload recognitionPayload;

// //-----RFID Users-----
byte Steven[4] = {167, 154, 66, 51};
byte Andreas[4] = {163, 245, 191, 50};

void setup()
{
    setupReaderAndComms();

    pinMode(LED_RED, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_BLUE, OUTPUT);
}

void loop()
{
    naam_received = RX_Handler();
    // naam_received = "Steven"; //dit is gewoon om te simuleren, gebruik lijn hierboven voor echt programma (kweet nie of die functie werkt)
    if (naam_received == "unknown")
    {
        Scheduler.startLoop(RFID_Read);
        strncpy(recognitionPayload.rfidName, "kaka", 20);
        while (recognitionPayload.rfidName == "kaka")
        {
        }
        debugSerial.println(recognitionPayload.rfidName);
        String speech_Name = stemherkenning();
        String RFID_Name = (String)recognitionPayload.rfidName;
        if (speech_Name != "unknown" && speech_Name == RFID_Name)
        {
            // send open
            commsSerial.write(SOT);
            commsSerial.write(STX);
            commsSerial.print(1);
            commsSerial.write(ETX);
            commsSerial.write(EOT);
        }
        else
        {
            // send close
            commsSerial.write(SOT);
            commsSerial.write(STX);
            commsSerial.print(0);
            commsSerial.write(ETX);
            commsSerial.write(EOT);
        }
    }
    else if (*std::find(std::begin(names), std::end(names), naam_received) != "noname")
    {
        String speech_Name = stemherkenning();
        if (speech_Name != "unknown" && speech_Name == naam_received)
        {
            // send open
            commsSerial.write(SOT);
            commsSerial.write(STX);
            commsSerial.print(1);
            commsSerial.write(ETX);
            commsSerial.write(EOT);
        }
        else
        {
            String RFID_Name = "kaka";
            // String RFID_Name = RFID_Read();
            if (RFID_Name != "unknown" && RFID_Name == naam_received)
            {
                // send open
                commsSerial.write(SOT);
                commsSerial.write(STX);
                commsSerial.print(1);
                commsSerial.write(ETX);
                commsSerial.write(EOT);
            }
            else
            {
                // send close
                commsSerial.write(SOT);
                commsSerial.write(STX);
                commsSerial.print(0);
                commsSerial.write(ETX);
                commsSerial.write(EOT);
            }
        }
    }
    // String writenames = SOT + STX + stem_naam + ETX + STX + rfid_naam + ETX + EOT;
    // Serial1.print(writenames);
    delay(1000);
}

String RX_Handler()
{
    if (commsSerial.available() > 0)
    {
        recognitionPayload = commsRead();
    }
    return (String)recognitionPayload.facialName;
}

void UserCorrect(String userName)
{
    strncpy(recognitionPayload.rfidName, userName.c_str(), 20);
#ifdef DEBUG
    Serial.print(userName);
    Serial.println(" identified.");
#endif
}

void UserIncorrect()
{
    strncpy(recognitionPayload.rfidName, "unknown", 20);
#ifdef DEBUG
    Serial.println("Unknown UID");
#endif
}

void RFID_Read()
{
    //Get RFID tag and wait maybe? return "unkown" if none was read.

    if (!RfidReader.PICC_IsNewCardPresent()) // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
        return;
    if (!RfidReader.PICC_ReadCardSerial()) // Select one of the cards
        return;

    if (*RfidReader.uid.uidByte == *Steven) // Check voor steven tag
    {
        UserCorrect("Steven");
        // commsSend(&recognitionPayload);
    }
    else if (*RfidReader.uid.uidByte == *Andreas) // Check voor andreas kaart
    {
        UserCorrect("Andreas");
        // commsSend(&recognitionPayload);
    }
    else
    {
        UserIncorrect();
        // commsSend(&recognitionPayload);
    }

    delay(1000); //change value if you want to read cards faster
}

String stemherkenning()
{
    digitalWrite(LED_BLUE, LOW);
    int i = 0;
    while (!stem_herkent || i < 17)
    {
        bool m = microphone_inference_record();
        if (!m)
        {
            ei_printf("ERR: Failed to record audio...\n");
            return;
        }
        signal_t signal;
        signal.total_length = EI_CLASSIFIER_SLICE_SIZE;
        signal.get_data = &microphone_audio_signal_get_data;
        ei_impulse_result_t result = {0};
        EI_IMPULSE_ERROR r = run_classifier_continuous(&signal, &result, debug_nn);
        if (r != EI_IMPULSE_OK)
        {
            ei_printf("ERR: Failed to run classifier (%d)\n", r);
            return;
        }
        if (++print_results >= (EI_CLASSIFIER_SLICES_PER_MODEL_WINDOW))
        {
            for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++)
            {
                if (result.classification[ix].value > 0.7)
                {
                    if (ix == 0)
                        return "Andreas";
                    else if (ix == 2)
                        return "Steven";
                    stem_herkent = true;
                }
            }
#if EI_CLASSIFIER_HAS_ANOMALY == 1
            ei_printf("    anomaly score: %.3f\n", result.anomaly);
#endif
            print_results = 0;
        }
        if (i == 16)
        {
            for (int k = 0; i < 5; i++)
            {
                digitalWrite(LED_RED, LOW);
                delay(500);
                digitalWrite(LED_RED, HIGH);
                delay(500);
            }
            return "unknown";
        }
        else
            i++;
    }
    digitalWrite(LED_BLUE, HIGH);
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
