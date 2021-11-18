#include <Arduino.h>
#include <Wire.h>
#include <PubSubClient.h> // connecteer en publish naar MQTT BROKER
#include <WiFi.h>         // Wifi enabler voor ESP32
#include "uart_project.h"
//#include "ESP8266WiFi.h"                                      // Wifi enable voor ESP8266

// WiFi
// const char *ssid = "LAPTOP-16QBLINN 5906";   // wifi invullen
// const char *wifi_password = "Elpolloloco69"; // pw invullen

const char *ssid = "ijmertnet";         // wifi invullen
const char *wifi_password = "computer"; // pw invullen

// MQTT
const char *mqtt_server = "192.168.137.134"; // IP van MQTT broker invullen
const char *validation_topic = "RFID tag";   // home/topic nog in te vullen
const char *mqtt_username = "ijmert";        // MQTT username invullen
const char *mqtt_password = "ijmert";        // MQTT pw invullen
const char *clientID = "client_home";        // MQTT client ID invullen

// Start Wifi en MQTT
WiFiClient wifiClient;
RFIDPayload receivedPayload;
PubSubClient client(mqtt_server, /*LISTENER PORT BROKER INVULLEN INGEVAL ANDERE*/ 1883, wifiClient);

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
  for (int i = 0; i < 4; i++)
    receivedPayload.uid[i] = 69;
  receivedPayload.uidSize = 4;
  strncpy(receivedPayload.name, "Gamer", 20);
  receivedPayload.userIdentified = true;

  debugSerial.begin(9600);
  commsSerial.begin(115200);
  connect_MQTT();
}

void loop()
{
  // if (commsSerial.available() > 0)
  // {
  // receivedPayload = commsRead();

  String dataToSend;
  dataToSend = receivedPayload.uidSize + " ";
  for (uint8_t i = 0; i < receivedPayload.uidSize; i++)
    dataToSend = dataToSend + receivedPayload.uid[i] + " ";
  dataToSend = dataToSend + receivedPayload.userIdentified + " " + receivedPayload.name;

  // data check
  // debugSerial.print("Data: ");
  debugSerial.print(dataToSend);

  // PUBLISH naar MQTT Broker (topic = Validation)
  if (client.publish(validation_topic, String(dataToSend).c_str()))
  {
    // delay(100);
    // debugSerial.println("Data sent!");
  }
  //Als het niet lukt krijgen we volgende melding en probeert hij opnieuw
  else
  {
    debugSerial.println("\nData failed to send. Reconnecting to MQTT Broker and trying again\n");
    client.connect(clientID, mqtt_username, mqtt_password);
    delay(10); // Zorgt ervoor dat client.publish en client.connect niet botsen blijkbaar
    client.publish(validation_topic, String(dataToSend).c_str());
  }

  client.disconnect(); // disconnect MQTT broker
  delay(30000);
  // }
}