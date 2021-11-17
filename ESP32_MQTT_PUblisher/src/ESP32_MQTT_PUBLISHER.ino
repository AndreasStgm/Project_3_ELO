#include <Wire.h>
#include "PubSubClient.h"                                       // connecteer en publish naar MQTT BROKER
#include "WiFi.h"                                               // Wifi enabler voor ESP32
//#include "ESP8266WiFi.h"                                      // Wifi enable voor ESP8266

// WiFi
const char* ssid = "LAPTOP-16QBLINN 5906";                                          // wifi invullen
const char* wifi_password = "Elpolloloco69";                                 // pw invullen

// MQTT
const char* mqtt_server = "192.168.137.204";                                   // IP van MQTT broker invullen
const char* validation_topic = "home/validation";    // home/topic nog in te vullen
const char* mqtt_username = "esp32";                            // MQTT username invullen
const char* mqtt_password = "esp32";                            // MQTT pw invullen
const char* clientID = "client_home";                     // MQTT client ID invullen

// Start Wifi en MQTT
WiFiClient wifiClient;
PubSubClient client(mqtt_server, /*LISTENER PORT BROKER INVULLEN INGEVAL ANDERE*/1883, wifiClient); 


// functie voor met de MQTT te verbinden over wifi
void connect_MQTT()
  {
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, wifi_password);

  // wachten op connectie
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Debugging - IP check ESP32
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Connect met MQTT Broker
  // client.connect checkt als de connectie gelukt is 
  if (client.connect(clientID, mqtt_username, mqtt_password)) {
    Serial.println("Connected to MQTT Broker!");
  }
  else {
    Serial.println("Connection to MQTT Broker failed...");
  }
}


void setup() {
  Serial.begin(9600); 
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off". 
  // Changes the output state according to the message
  if (String(topic) == "esp32/whattodo") {
    Serial.print("Changing output to ");
    if(messageTemp == "on"){
      Serial.println("on");
      digitalWrite(ledPin, HIGH); // vervangen door code voor de card en voice aan te sturen?
    }
    else if(messageTemp == "off"){
      Serial.println("off");
      digitalWrite(ledPin, LOW); // vervangen door code voor de card en voice aan te sturen?
    }
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("esp32")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("home/whattodo");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  callback();
  
  connect_MQTT();
  Serial.setTimeout(2000);
  
 // string voor MQTT + inc data
  String dat = "I dont know why you say goodbye I say hello!";                                    // inkomende uitgelezen struct van Arduino BLE
  
  // data check
  Serial.print("Data: ");
  Serial.print(dat);

  // PUBLISH naar MQTT Broker (topic = Validation)
  if (client.publish(validation_topic, String(dat).c_str())) {
    Serial.println("Data sent!");
  }
  //Als het niet lukt krijgen we volgende melding en probeert hij opnieuw
  else {
    Serial.println("Data failed to send. Reconnecting to MQTT Broker and trying again");
    client.connect(clientID, mqtt_username, mqtt_password);
    delay(10); // Zorgt ervoor dat client.publish en client.connect niet botsen blijkbaar
    client.publish(validation_topic, String(dat).c_str());
  }

  
  client.disconnect();                                // disconnect MQTT broker
  delay(1000*60);                                     // reprint elke min
}
