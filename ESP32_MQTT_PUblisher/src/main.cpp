#include <Arduino.h>
#include <PubSubClient.h> // connect and publish to MQTT BROKER
#include <WiFi.h>         // Wifi enabler for ESP32
#include "uart_project.h"

byte receivedData[5] = {0, 0, 0, 0, 0};
String sendData = "";

const char *ssid = "ijmertnet";         // wifi to fill in
const char *wifi_password = "computer"; // password to fill in

// MQTT user data
const char *mqtt_server = "192.168.137.134"; // IP of MQTT broker, our case: 192.168.0.13
const char *slot_topic = "slot";             // MQTT topic over which we will publish
const char *mqtt_username = "ijmert";        // MQTT username to fill in
const char *mqtt_password = "ijmert";        // MQTT password to fill in
const char *clientID = "client_home";        // MQTT client ID to fill in

const char *FacialRecognition_topic = "FacialRecognition";

// Start Wifi and MQTT
WiFiClient wifiClient;
PubSubClient client(mqtt_server, /*LISTENER PORT BROKER INVULLEN INGEVAL ANDERE*/ 1883, wifiClient);

//---------------functions---------------

void callback(char *topic, byte *payload, unsigned int length)
{
  UARTPayload receivedPayload;
  strncpy(receivedPayload.facialName, "", 20);
  strncpy(receivedPayload.audioName, "", 20);
  strncpy(receivedPayload.rfidName, "", 20);

  debugSerial.print("length message received in callback= ");
  debugSerial.println(length);
  for (int i = 0; i < length; i++)
  {
    receivedPayload.facialName[i] = payload[i];
    debugSerial.print(receivedPayload.facialName[i]);
  }
  commsSend(&receivedPayload);
}

// function used for connection the esp32 to the wifi and MQTT broker
void connect_MQTT()
{
  debugSerial.print("Connecting to ");
  debugSerial.println(ssid);

  WiFi.begin(ssid, wifi_password);

  // waiting for the connection
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    debugSerial.print(".");
  }

  // Debugging - IP check ESP32
  debugSerial.println("WiFi connected");
  debugSerial.print("IP address: ");
  debugSerial.println(WiFi.localIP());

  // Connects to  MQTT Broker
  // client.connect checks if the connection is successful
  if (client.connect(clientID, mqtt_username, mqtt_password))
  {
    debugSerial.println("Connected to MQTT Broker!");
  }
  else
  {
    debugSerial.println("Connection to MQTT Broker failed...");
  }
}

//---------------standard arduino framework stuff---------------

void setup()
{
  debugSerial.begin(9600);
  commsSerial.begin(115200);

  client.setCallback(callback);
  connect_MQTT();
}

void loop()
{
  client.connect(clientID, mqtt_username, mqtt_password);
  boolean success = client.subscribe(FacialRecognition_topic);
  debugSerial.printf("Subscribe success: %d (true/false)\n", success);

  client.loop();

  if (commsSerial.available() > 0)
  {
    while (commsSerial.available() > 0)
    {
      if (commsSerial.read() == SOT)
      {
        commsSerial.readBytesUntil(EOT, receivedData, sizeof(receivedData));
      }
    }
    for (uint8_t i = 0; i < sizeof(receivedData); i++)
    {
      debugSerial.println(receivedData[i]);
    }

    if (receivedData[0] == STX && receivedData[2] == ETX && receivedData[1] == 49)
    {
      sendData = "open";
      debugSerial.println("Received: open");
    }
    else
    {
      sendData = "close";
      debugSerial.println("Received: closed");
    }
    receivedData[0] = 0;
    receivedData[1] = 0;
    receivedData[2] = 0;
    receivedData[3] = 0;
    receivedData[4] = 0;

    if (client.publish(slot_topic, String(sendData).c_str()))
    {
      delay(100);
      debugSerial.println("Slot data sent!");
    }
    else
    {
      debugSerial.println("Slot data failed to send. Reconnecting to MQTT Broker and trying again\n");
      client.connect(clientID, mqtt_username, mqtt_password);
      delay(10); // Is nedded so that client.connect and client.publish don't collide
      client.publish(slot_topic, String(sendData).c_str());
    }

    client.disconnect(); // disconnect MQTT broker
    delay(10);
  }
}