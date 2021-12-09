#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "uart_project.h"

#define DEBUG

//----------

// WiFi
// const char *ssid = "ijmertnet";
// const char *wifi_password = "computer";
// // MQTT
// const char *mqtt_server = "192.168.137.134"; // IP van MQTT broker invullen
// const char *validation_topic = "RFIDtag";    // home/topic nog in te vullen
// const char *mqtt_username = "ijmert";        // MQTT username invullen
// const char *mqtt_password = "ijmert";        // MQTT pw invullen
// const char *clientID = "client_home";        // MQTT client ID invullen

//----------

UARTPayload testpayload;

// WiFiClient wifiClient;
// PubSubClient client(mqtt_server, /*LISTENER PORT BROKER INVULLEN INGEVAL ANDERE*/ 1883, wifiClient);

//----------

// void connect_MQTT() // functie voor met de MQTT te verbinden over wifi
// {
//   debugSerial.print("Connecting to ");
//   debugSerial.println(ssid);

//   WiFi.begin(ssid, wifi_password);

//   // wachten op connectie
//   while (WiFi.status() != WL_CONNECTED)
//   {
//     delay(500);
//     debugSerial.print(".");
//   }

//   // Debugging - IP check ESP32
//   debugSerial.println("WiFi connected");
//   debugSerial.print("IP address: ");
//   debugSerial.println(WiFi.localIP());

//   // Connect met MQTT Broker
//   // client.connect checkt als de connectie gelukt is
//   if (client.connect(clientID, mqtt_username, mqtt_password))
//     debugSerial.println("Connected to MQTT Broker!");
//   else
//     debugSerial.println("Connection to MQTT Broker failed...");
// }

//----------

void setup(void)
{
  debugSerial.begin(9600);
  commsSerial.begin(115200);

  // connect_MQTT();
  commsSerial.flush();
}

void loop(void)
{
  if (commsSerial.available() > 0)
  {
    testpayload = commsRead();

    debugSerial.println((String)testpayload.facialName);
    debugSerial.println((String)testpayload.audioName);
    debugSerial.println((String)testpayload.rfidName);
  }
  delay(10);
  //   if (commsSerial.available() > 0)
  //   {
  //     debugSerial.print(commsSerial.available());
  //     UARTPayload receivedPayload = commsRead();
  // #ifdef DEBUG
  //     debugSerial.print("Name: ");
  //     debugSerial.print(receivedPayload.name);
  //     debugSerial.print("\tUID: ");
  //     for (byte i = 0; i < receivedPayload.uidSize; i++)
  //     {
  //       debugSerial.print(receivedPayload.uid[i] < 0x10 ? " 0" : " ");
  //       debugSerial.print(receivedPayload.uid[i], DEC);
  //     }
  //     debugSerial.print("\tRecognized: ");
  //     debugSerial.println(receivedPayload.userIdentified);
  // #endif

  //     String dataToSend = ""; // omzetten naar een string voor de MQTT verzending
  //     dataToSend = dataToSend + receivedPayload.uidSize + " ";
  //     for (uint8_t i = 0; i < receivedPayload.uidSize; i++)
  //       dataToSend = dataToSend + receivedPayload.uid[i] + " ";
  //     dataToSend = dataToSend + receivedPayload.userIdentified + " " + receivedPayload.name;

  //     // data check
  //     debugSerial.print("Data: ");
  //     debugSerial.println(dataToSend);

  //     // PUBLISH naar MQTT Broker (topic = Validation)
  //     if (client.publish(validation_topic, String(dataToSend).c_str()))
  //     {
  //       delay(100);
  //       debugSerial.println("Data sent!");
  //     }
  //     //Als het niet lukt krijgen we volgende melding en probeert hij opnieuw
  //     else
  //     {
  //       debugSerial.println("Data failed to send. Reconnecting to MQTT Broker and trying again");
  //       client.connect(clientID, mqtt_username, mqtt_password);
  //       delay(10); // Zorgt ervoor dat client.publish en client.connect niet botsen
  //       if (client.publish(validation_topic, String(dataToSend).c_str()))
  //       {
  //         delay(100);
  //         debugSerial.println("Data sent after retry!");
  //       }
  //     }
  //     client.disconnect(); // disconnect MQTT broker
  //     delay(10);
  //   }
}