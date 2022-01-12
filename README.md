
# Project_3_AI

Hier vinden we de gebruikte onderdelen code terug voor het deel op de PCB. 

We hebben hier twee onderdelen in: het deel op de Arduino NANO 33 BLE Sense, en het deel op de ESP32.

De Arduino gaat zorgen voor de spraakherkenning en de RFID-tag.
De ESP32 zorgt voor de communicatie tussen de Arduino en de Raspberry Pi.

De communicatie tussen deze twee onderdelen verloopt via UART.

==================================================

## Gebruik Code
Voor beide projecten moet men de PlatformIO IDE plugin in Visual Studio Code gebruiken.
De .ini bestanden moeten normaal niet aangpast worden, de upload poort wordt automatisch correct gekozen.
Indien dit wel nodig zou zijn, dan kan men simpelweg dit toevoegen: ```upload_port = COM?``` Als men in device manager, makkelijk toegankelijk via CTRL+X, kan men de correcte poort terugvinden van de device.

### ESP32_MQTT_PUblisher
ESP32 code moet indien nodig de ssid, wifi_password, mqtt_server, mqtt_username, mqtt_password en de clientID.
Als men wilt is het mogelijk om de topics aan te passen, de slot_topic en de FacialRecognition_topic.

Voor de rest zou er niets aan de code moeten aangepast worden.

### Arduino Nano 33 BLE Sense Event CTRL
Hier moet men normaal geen code aanpassen.
Het kan soms zijn dat men met problemen komt bij het uploaden van het programma, men moet dan Arduino proberen een keer in de boatloader te zetten en dan terug in normale modus te gaan.
