Main code V1

In de code staat er wat uitleg met comments.
Een gedeelte gaan we zelf nog moeten ingeven zodra de broker in orde is.



MATHIAS COPY & PASTA PARADISE
------------------------------

///////////////////////////////////////////////////////////////////////////////////////////////////////////
const char *validationVOICE_topic = "VOICE tag";  // mathias sub_test
const char *faceid_topic = "faceid tag"; 
///////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////
client.subscribe(faceid_topic); //mathias sub test
///////////////////////////////////////////////////////////////////////////////////////////////////////////
  
// PUBLISH naar MQTT Broker (topic = Validation)
if (client.publish(validationRFID_topic, String(dataToSend).c_str()))
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
  client.publish(validationRFID_topic, String(dataToSend).c_str());
}
  
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// PUBLISH naar MQTT Broker (topic = Validation)
if (client.publish(validationVOICE_topic, String(dataToSend).c_str()))        // mathias pub_test
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
  client.publish(validationVOICE_topic, String(dataToSend).c_str());
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
