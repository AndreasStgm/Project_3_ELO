#include <Arduino.h>
#include <nrf52840.h>
#include <nrf52840_peripherals.h>
#include <uart_project.h>
#include <Scheduler.h>
#include <SPI.h>
#include <Names_inferencing.h>
#include <PDM.h>
#include <MFRC522.h>

#define RFID_RST 2
#define RFID_SS 10
#define EI_CLASSIFIER_SLICES_PER_MODEL_WINDOW 3
//-----Function Declaration-----
void RFID_Read();
String RX_Handler();
SchedulerTask voice_recognition();
String stemherkenning();
void setupReaderAndComms();
void ei_printf(const char *format, ...);
static void pdm_data_ready_inference_callback(void);
static bool microphone_inference_start(uint32_t n_samples);
static bool microphone_inference_record(void);
static int microphone_audio_signal_get_data(size_t offset, size_t length, float *out_ptr);
static void microphone_inference_end(void);
void setupSpeech();
void RFID_Read_loop();
//-----Variable Declaration-----
String naam_received = "";
bool stem_herkent = false;
bool tag_herkent = false;
String rfid_naam = "";
String stem_naam = "";
const char *names[3] = {"Steven", "Andreas", "noname"}; //all users are stored in this array. Last value is dummy value that returns when std::find() does not find occurence of string.
typedef struct
{
    signed short *buffers[2];
    unsigned char buf_select;
    unsigned char buf_ready;
    unsigned int buf_count;
    unsigned int n_samples;
} inference_t;

static inference_t inference;
static bool record_ready = false;
static signed short *sampleBuffer;
static bool debug_nn = false; // Set this to true to see e.g. features generated from the raw signal
static int print_results = -(EI_CLASSIFIER_SLICES_PER_MODEL_WINDOW);

//-----Instance Declaration-----
MFRC522 RfidReader(RFID_SS, RFID_RST); // Instance of the class
UARTPayload recognitionPayload;

// //-----RFID Users-----
byte Steven[4] = {0xB9, 0x34, 0xEF, 0xA3};
byte Andreas[4] = {163, 245, 191, 50};

void setup()
{
    setupReaderAndComms();
    setupSpeech();
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_BLUE, OUTPUT);
    digitalWrite(LED_RED, HIGH);
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_BLUE, HIGH);
    // Scheduler.startLoop(RFID_Read);
}

void loop()
{
    naam_received = RX_Handler();
    debugSerial.println("naam_received: ");
    debugSerial.println(naam_received);

    // naam_received = "Steven"; //dit is gewoon om te simuleren, gebruik lijn hierboven voor echt programma (kweet nie of die functie werkt)
    if (naam_received == "unknown")
    {
        // debugSerial.println("RFID name:");
        // debugSerial.println(recognitionPayload.rfidName);
        // commsSerial.write(SOT);
        // commsSerial.write(STX);
        // commsSerial.print(0);
        // commsSerial.write(ETX);
        // commsSerial.write(EOT);
        // debugSerial.println(recognitionPayload.rfidName);
        String speech_Name = stemherkenning();
        debugSerial.println("speech shit");
        debugSerial.println(speech_Name);
        // String speech_Name = "aids";
        strncpy(recognitionPayload.rfidName, "", 20);
        digitalWrite(LED_GREEN, LOW);
        RFID_Read_loop();
        digitalWrite(LED_GREEN, HIGH);
        String RFID_Name = (String)recognitionPayload.rfidName;
        debugSerial.println(RFID_Name);
        if (speech_Name != "unknown" && speech_Name == RFID_Name)
        {
            // send open
            debugSerial.println("open unknown");
            commsSerial.write(SOT);
            commsSerial.write(STX);
            commsSerial.print(1);
            commsSerial.write(ETX);
            commsSerial.write(EOT);
        }
        else
        {
            // send close
            debugSerial.println("close unknown");
            commsSerial.write(SOT);
            commsSerial.write(STX);
            commsSerial.print(0);
            commsSerial.write(ETX);
            commsSerial.write(EOT);
        }
    }
    else if (std::find(std::begin(names), std::end(names), naam_received) != std::end(names))
    {
        String speech_Name = stemherkenning();
        debugSerial.println(speech_Name);
        if (speech_Name != "unknown" && speech_Name == naam_received)
        {
            // send open
            debugSerial.println("open SPEECH and FACIAL");
            commsSerial.write(SOT);
            commsSerial.write(STX);
            commsSerial.print(1);
            commsSerial.write(ETX);
            commsSerial.write(EOT);
        }
        else
        {
            strncpy(recognitionPayload.rfidName, "", 20);
            digitalWrite(LED_GREEN, LOW);
            RFID_Read_loop();
            digitalWrite(LED_GREEN, HIGH);
            String RFID_Name = (String)recognitionPayload.rfidName;
            debugSerial.println(RFID_Name);
            if (RFID_Name != "unknown" && RFID_Name == naam_received)
            {
                // send open
                debugSerial.println("open RFID and FACIAL");
                commsSerial.write(SOT);
                commsSerial.write(STX);
                commsSerial.print(1);
                commsSerial.write(ETX);
                commsSerial.write(EOT);
            }
            else
            {
                // send close
                debugSerial.println("close RFID and FACIAL");
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
        return (String)recognitionPayload.facialName;
    }
    else
    {
        return "kaka";
    }
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

void RFID_Read_loop()
{
    for (int i = 0; i < 20; i++)
    {
        RFID_Read();
    }
}

void RFID_Read()
{
    delay(250);
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
    // delay(250); //change value if you want to read cards faster
}

String stemherkenning()
{
    stem_herkent = false;
    digitalWrite(LED_BLUE, LOW);
    int i = 0;
    while (!stem_herkent || i < 100)
    {
        microphone_inference_record();
        signal_t signal;
        signal.total_length = EI_CLASSIFIER_SLICE_SIZE;
        signal.get_data = &microphone_audio_signal_get_data;
        ei_impulse_result_t result = {0};
        EI_IMPULSE_ERROR r = run_classifier_continuous(&signal, &result, debug_nn);
        if (r != EI_IMPULSE_OK)
        {
            ei_printf("ERR: Failed to run classifier (%d)\n", r);
            break;
        }

        if (++print_results >= (EI_CLASSIFIER_SLICES_PER_MODEL_WINDOW))
        {

            for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++)
            {
                /*ei_printf("    %s: %.5f\n", result.classification[ix].label,
                          result.classification[ix].value);*/
                if (result.classification[ix].value > 0.5)
                {
                    if (ix == 0)
                    {
                        stem_herkent = true;
                        digitalWrite(LED_BLUE, HIGH);
                        return "Andreas";
                    }
                    else if ((ix == 2))
                    {
                        stem_herkent = true;
                        digitalWrite(LED_BLUE, HIGH);
                        return "Steven";
                    }
                    else
                        i++;
                }
                else if (i == 99)
                {
                    digitalWrite(LED_BLUE, HIGH);
                    for (int i = 0; i < 4; i++)
                    {
                        digitalWrite(LED_RED, LOW);
                        delay(500);
                        digitalWrite(LED_RED, HIGH);
                        delay(500);
                    }
                    return "kaka";
                }
                else
                {
                    i++;
                }
            }
            print_results = 0;
        }
    }
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

void ei_printf(const char *format, ...)
{
    static char print_buf[1024] = {0};

    va_list args;
    va_start(args, format);
    int r = vsnprintf(print_buf, sizeof(print_buf), format, args);
    va_end(args);

    if (r > 0)
    {
        Serial.write(print_buf);
    }
}
static void pdm_data_ready_inference_callback(void)
{
    int bytesAvailable = PDM.available();

    // read into the sample buffer
    int bytesRead = PDM.read((char *)&sampleBuffer[0], bytesAvailable);

    if (record_ready == true)
    {
        for (int i = 0; i < bytesRead >> 1; i++)
        {
            inference.buffers[inference.buf_select][inference.buf_count++] = sampleBuffer[i];

            if (inference.buf_count >= inference.n_samples)
            {
                inference.buf_select ^= 1;
                inference.buf_count = 0;
                inference.buf_ready = 1;
            }
        }
    }
}
static bool microphone_inference_start(uint32_t n_samples)
{
    inference.buffers[0] = (signed short *)malloc(n_samples * sizeof(signed short));

    if (inference.buffers[0] == NULL)
    {
        return false;
    }

    inference.buffers[1] = (signed short *)malloc(n_samples * sizeof(signed short));

    if (inference.buffers[1] == NULL)
    {
        free(inference.buffers[0]);
        return false;
    }

    sampleBuffer = (signed short *)malloc((n_samples >> 1) * sizeof(signed short));

    if (sampleBuffer == NULL)
    {
        free(inference.buffers[0]);
        free(inference.buffers[1]);
        return false;
    }

    inference.buf_select = 0;
    inference.buf_count = 0;
    inference.n_samples = n_samples;
    inference.buf_ready = 0;

    // configure the data receive callback
    PDM.onReceive(&pdm_data_ready_inference_callback);

    PDM.setBufferSize((n_samples >> 1) * sizeof(int16_t));

    // initialize PDM with:
    // - one channel (mono mode)
    // - a 16 kHz sample rate
    if (!PDM.begin(1, EI_CLASSIFIER_FREQUENCY))
    {
        ei_printf("Failed to start PDM!");
    }

    // set the gain, defaults to 20
    PDM.setGain(127);

    record_ready = true;

    return true;
}
static bool microphone_inference_record(void)
{
    bool ret = true;
    /*if (inference.buf_ready == 1)
    {
        ei_printf(
            "Error sample buffer overrun. Decrease the number of slices per model window "
            "(EI_CLASSIFIER_SLICES_PER_MODEL_WINDOW)\n");
        ret = false;
    }*/

    while (inference.buf_ready == 0)
    {
        delay(1);
    }

    inference.buf_ready = 0;

    return ret;
}
static int microphone_audio_signal_get_data(size_t offset, size_t length, float *out_ptr)
{
    numpy::int16_to_float(&inference.buffers[inference.buf_select ^ 1][offset], out_ptr, length);

    return 0;
}
static void microphone_inference_end(void)
{
    PDM.end();
    free(inference.buffers[0]);
    free(inference.buffers[1]);
    free(sampleBuffer);
}
void setupSpeech()
{
    run_classifier_init();
    if (microphone_inference_start(EI_CLASSIFIER_SLICE_SIZE) == false)
    {
        ei_printf("ERR: Failed to setup audio sampling\r\n");
        return;
    }
}

#if !defined(EI_CLASSIFIER_SENSOR) || EI_CLASSIFIER_SENSOR != EI_CLASSIFIER_SENSOR_MICROPHONE
#error "Invalid model for current sensor."
#endif