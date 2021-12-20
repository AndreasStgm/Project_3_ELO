#include <Arduino.h>
#include <PubSubClient.h> // connecteer en publish naar MQTT BROKER
#include <WiFi.h>         // Wifi enabler voor ESP32
#include "uart_project.h"

byte receivedData[5] = {0, 0, 0, 0, 0};
String sendData = "";

// WiFi
// const char *ssid = "LAPTOP-Paco";            // wifi invullen
// const char *wifi_password = "Elpolloloco69"; // pw invullen

const char *ssid = "ijmertnet";         // wifi invullen
const char *wifi_password = "computer"; // pw invullen

//const char *ssid = "Orange-6f9d5";         // wifi invullen
//const char *wifi_password = "Qwcf933c"; // pw invullen

// MQTT Ijmerts
const char *mqtt_server = "192.168.137.134"; // IP van MQTT broker invullen 192.168.0.13
const char *slot_topic = "slot";             // home/topic nog in te vullen
const char *mqtt_username = "ijmert";        // MQTT username invullen
const char *mqtt_password = "ijmert";        // MQTT pw invullen
const char *clientID = "client_home";        // MQTT client ID invullen

// MQTT Mathias
//~ const char *mqtt_server = "192.168.0.13"; // IP van MQTT broker invullen 192.168.0.13
//~ const char *RFIDtag_topic = "RFIDtag";       // home/topic nog in te vullen
//~ const char *mqtt_username = "esp32";        // MQTT username invullen
//~ const char *mqtt_password = "esp32";        // MQTT pw invullen
//~ const char *clientID = "client_home";        // MQTT client ID invullen

// const char *mqtt_server = "192.168.137.195"; // IP van MQTT broker invullen 192.168.0.13
// const char *RFIDtag_topic = "RFIDtag";       // home/topic nog in te vullen
// const char *mqtt_username = "esp32";         // MQTT username invullen
// const char *mqtt_password = "esp32";         // MQTT pw invullen
// const char *clientID = "client_home";        // MQTT client ID invullen

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// const char *VoiceRecognition_topic = "VoiceRecognition"; // mathias sub_test
const char *FacialRecognition_topic = "FacialRecognition";
///////////////////////////////////////////////////////////////////////////////////////////////////////////

char *Topic;
// byte buffer[15];
boolean Rflag = false;
int r_len;
boolean succes;

// Start Wifi en MQTT
WiFiClient wifiClient;
// RFIDPayload receivedPayload;
PubSubClient client(mqtt_server, /*LISTENER PORT BROKER INVULLEN INGEVAL ANDERE*/ 1883, wifiClient);

/////////////////////////////////////////////////////////////////////////////////////////////////
void callback(char *topic, byte *payload, unsigned int length)
{
  //Payload=[];
  UARTPayload receivedPayload;
  strncpy(receivedPayload.facialName, "", 20);
  strncpy(receivedPayload.audioName, "", 20);
  strncpy(receivedPayload.rfidName, "", 20);

  Topic = topic;
  Rflag = true;   //will use in main loop
  r_len = length; //will use in main loop
  debugSerial.print("length message received in callback= ");
  debugSerial.println(length);
  for (int i = 0; i < length; i++)
  {
    receivedPayload.facialName[i] = payload[i];
    debugSerial.print(receivedPayload.facialName[i]);
  }
  commsSend(&receivedPayload);
}
/////////////////////////////////////////////////////////////////////////////////////////////////

// functie voor met de MQTT te verbinden over wifi
void connect_MQTT()
{
  debugSerial.print("Connecting to ");
  debugSerial.println(ssid);

  WiFi.begin(ssid, wifi_password);

  // wachten op connectie
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    debugSerial.print(".");
  }

  // Debugging - IP check ESP32
  debugSerial.println("WiFi connected");
  debugSerial.print("IP address: ");
  debugSerial.println(WiFi.localIP());

  // Connect met MQTT Broker
  // client.connect checkt als de connectie gelukt is
  if (client.connect(clientID, mqtt_username, mqtt_password))
  {
    debugSerial.println("Connected to MQTT Broker!");
  }
  else
  {
    debugSerial.println("Connection to MQTT Broker failed...");
  }
}

//-----

void setup()
{
  debugSerial.begin(9600);
  commsSerial.begin(115200);

  //client.setServer(mqtt_server, 1883); //test 02/12
  client.setCallback(callback);
  connect_MQTT();
  // client.subscribe(FacialRecognition_topic,2);
}

void loop()
{
  client.connect(clientID, mqtt_username, mqtt_password);
  succes = client.subscribe(FacialRecognition_topic);
  /////////////////////////////////////////////////////////////////////////////////////////////////
  // client.subscribe(FacialRecognition_topic,2);
  // String msg = "test message";
  // client.publish(FacialRecognition_topic, String(msg).c_str());

  // delay(1000);

  client.loop();

  // if (Rflag)
  // {
  //   debugSerial.print("Message arrived in main loop[");
  //   debugSerial.print(Topic);
  //   debugSerial.print("] ");
  //   debugSerial.print("message length = ");
  //   debugSerial.print(r_len);
  //   //Serial.print(Payload);
  //   for (int i = 0; i < r_len; i++)
  //   {
  //     debugSerial.print((char)buffer[i]);
  //   }
  //   debugSerial.println();
  //   Rflag = false;
  // }

  /////////////////////////////////////////////////////////////////////////////////////////////////

  if (commsSerial.available() > 0)
  {
    while (commsSerial.available() > 0)
    {
      if (commsSerial.read() == SOT)
      {
        commsSerial.readBytesUntil(EOT, receivedData, sizeof(receivedData));
      }
    }

    if (receivedData[0] == STX && receivedData[2] == ETX && receivedData[1] == 1)
    {
      sendData = "open";
    }
    else
    {
      sendData = "close";
    }
    receivedData[0] = 0;
    receivedData[1] = 0;
    receivedData[2] = 0;
    receivedData[3] = 0;
    receivedData[4] = 0;
    // UARTPayload receivedPayload = commsRead();

    // String sendDataAudio = "";
    // String sendDataRfid = "";
    // sendDataAudio = sendDataAudio + receivedPayload.audioName;
    // sendDataRfid = sendDataRfid + receivedPayload.rfidName;

    // data check
    // debugSerial.printf("Data: audio_recogn: %s, rfid_tag: %s\n", sendDataAudio, sendDataRfid);

    // PUBLISH naar MQTT Broker (topic = Validation)
    if (client.publish(slot_topic, String(sendData).c_str()))
    {
      delay(100);
      debugSerial.println("RFID data sent!");
    }
    else
    {
      debugSerial.println("RFID data failed to send. Reconnecting to MQTT Broker and trying again\n");
      client.connect(clientID, mqtt_username, mqtt_password);
      delay(10); // Zorgt ervoor dat client.publish en client.connect niet botsen
      client.publish(slot_topic, String(sendData).c_str());
    }
    // if (client.publish(VoiceRecognition_topic, String(sendDataAudio).c_str()))
    // {
    //   delay(100);
    //   debugSerial.println("Audio data sent!");
    // }
    // else
    // {
    //   debugSerial.println("Audio data failed to send. Reconnecting to MQTT Broker and trying again\n");
    //   client.connect(clientID, mqtt_username, mqtt_password);
    //   delay(10); // Zorgt ervoor dat client.publish en client.connect niet botsen
    //   client.publish(VoiceRecognition_topic, String(sendDataAudio).c_str());
    // }
    //Als het niet lukt krijgen we volgende melding en probeert hij opnieuw

    client.disconnect(); // disconnect MQTT broker
    delay(10);
  }
}
