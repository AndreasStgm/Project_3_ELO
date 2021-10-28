#include "PubSubClient.h"                                       // connecteer en publish naar MQTT BROKER
#include "WiFi.h"                                               // Wifi enabler voor ESP32
//#include "ESP8266WiFi.h"                                      // Wifi enable voor ESP8266

// WiFi
const char* ssid = "LAPTOP-16QBLINN 5906";                                          // wifi invullen
const char* wifi_password = "Elpolloloco69";                                 // pw invullen

// MQTT
const char* mqtt_server = "192.168.137.204";                                   // IP van MQTT broker invullen
const char* validation_topic = "home/livingroom/validation";    // home/topic nog in te vullen
const char* mqtt_username = "esp32";                            // MQTT username invullen
const char* mqtt_password = "esp32";                            // MQTT pw invullen
const char* clientID = "client_livingroom";                     // MQTT client ID invullen

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

void loop() {
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
