#include <Arduino.h>
#include <PubSubClient.h> // connecteer en publish naar MQTT BROKER
#include <WiFi.h>         // Wifi enabler voor ESP32
#include "uart_project.h"

// WiFi
const char *ssid = "LAPTOP-Paco";   // wifi invullen
const char *wifi_password = "Elpolloloco69"; // pw invullen

//~ const char *ssid = "ijmertnet";         // wifi invullen
//~ const char *wifi_password = "computer"; // pw invullen

//const char *ssid = "Orange-6f9d5";         // wifi invullen
//const char *wifi_password = "Qwcf933c"; // pw invullen

// MQTT Ijmerts
// const char *mqtt_server = "192.168.137.134"; // IP van MQTT broker invullen 192.168.0.13
// const char *RFIDtag_topic = "RFIDtag";       // home/topic nog in te vullen
// const char *mqtt_username = "ijmert";        // MQTT username invullen
// const char *mqtt_password = "ijmert";        // MQTT pw invullen
// const char *clientID = "client_home";        // MQTT client ID invullen

// MQTT Mathias
//~ const char *mqtt_server = "192.168.0.13"; // IP van MQTT broker invullen 192.168.0.13
//~ const char *RFIDtag_topic = "RFIDtag";       // home/topic nog in te vullen
//~ const char *mqtt_username = "esp32";        // MQTT username invullen
//~ const char *mqtt_password = "esp32";        // MQTT pw invullen
//~ const char *clientID = "client_home";        // MQTT client ID invullen

const char *mqtt_server = "192.168.137.195"; // IP van MQTT broker invullen 192.168.0.13
const char *RFIDtag_topic = "RFIDtag";       // home/topic nog in te vullen
const char *mqtt_username = "esp32";         // MQTT username invullen
const char *mqtt_password = "esp32";         // MQTT pw invullen
const char *clientID = "client_home";        // MQTT client ID invullen

///////////////////////////////////////////////////////////////////////////////////////////////////////////
const char *VoiceRecognition_topic = "VoiceRecognition"; // mathias sub_test
const char *FacialRecognition_topic = "FacialRecognition";
///////////////////////////////////////////////////////////////////////////////////////////////////////////

char *Topic;
byte *buffer;
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
  Topic = topic;
  Rflag = true;   //will use in main loop
  r_len = length; //will use in main loop
  debugSerial.print("length message received in callback= ");
  debugSerial.println(length);
  for (int i = 0; i < length; i++)
  {
    buffer[i] = payload[i];
    debugSerial.print((char)payload[i]);
  }
  Serial.println();
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
    /////////////////////////////////////////////////////////////////////////////////////////////////
    //boolean succes = client.subscribe(FacialRecognition_topic);    // mathias subtest
    // client.setCallback(callback);
    /////////////////////////////////////////////////////////////////////////////////////////////////
  }
  else
  {
    debugSerial.println("Connection to MQTT Broker failed...");
  }
}

//-----

void setup()
{
  // for (int i = 0; i < 4; i++)
  //   receivedPayload.uid[i] = 69;
  // receivedPayload.uidSize = 4;
  // strncpy(receivedPayload.name, "Gamer", 20);
  // receivedPayload.userIdentified = true;

  debugSerial.begin(9600);
  commsSerial.begin(115200);

  //client.setServer(mqtt_server, 1883); //test 02/12
  client.setCallback(callback);
  succes = client.subscribe(FacialRecognition_topic);
  connect_MQTT();
  // client.subscribe(FacialRecognition_topic,2);
}

void loop()
{

  /////////////////////////////////////////////////////////////////////////////////////////////////
  // client.subscribe(FacialRecognition_topic,2);
  String msg = "test message";
  client.publish(FacialRecognition_topic, String(msg).c_str());

  delay(1000);
  client.loop();

  if (Rflag != 0)
  {
    debugSerial.print("Message arrived in main loop[");
    debugSerial.print(Topic);
    debugSerial.print("] ");
    debugSerial.print("message length = ");
    debugSerial.print(r_len);
    //Serial.print(Payload);
    for (int i = 0; i < r_len; i++)
    {
      debugSerial.print((char)buffer[i]);
    }
    debugSerial.println();
    Rflag = false;
  }
  
  
  
  /////////////////////////////////////////////////////////////////////////////////////////////////

  if (commsSerial.available() > 0)
  {
    RFIDPayload receivedPayload = commsRead();

    String dataToSend = "";
    dataToSend = dataToSend + receivedPayload.uidSize + " ";
    for (uint8_t i = 0; i < receivedPayload.uidSize; i++)
      dataToSend = dataToSend + receivedPayload.uid[i] + " ";
    dataToSend = dataToSend + receivedPayload.userIdentified + " " + receivedPayload.name;

    // data check
    debugSerial.print("Data: ");
    debugSerial.println(dataToSend);

    // PUBLISH naar MQTT Broker (topic = Validation)
    if (client.publish(RFIDtag_topic, String(dataToSend).c_str()))
    {
      delay(100);
      debugSerial.println("Data sent!");
    }
    //Als het niet lukt krijgen we volgende melding en probeert hij opnieuw
    else
    {
      debugSerial.println("Data failed to send. Reconnecting to MQTT Broker and trying again\n");
      client.connect(clientID, mqtt_username, mqtt_password);
      delay(10); // Zorgt ervoor dat client.publish en client.connect niet botsen
      client.publish(RFIDtag_topic, String(dataToSend).c_str());
    }

    client.disconnect(); // disconnect MQTT broker
    delay(10);
  }
}
