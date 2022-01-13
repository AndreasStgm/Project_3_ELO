
# Project_3_AI

Hier vinden we de code terug voor de verschillende onderdelen op de PCB: de Arduino NANO 33 BLE sense, en de ESP32. Ook is er wat uitleg om het systeem te configureren.

De Arduino gaat zorgen voor de spraakherkenning en de RFID-tag.
Een Raspberry Pi zorgt voor de gezichtsherkenning.
De ESP32 zorgt voor de communicatie tussen de Arduino en de Raspberry Pi waar de gezichtsherkenning op draait, omdat de NANO geen Wi-Fi capaciteiten heeft.
De communicatie tussen de ESP32 en da NANO verloopt via UART.


## Gebruik Code
Voor beide projecten moet men de PlatformIO IDE plugin in Visual Studio Code gebruiken.
De .ini bestanden moeten normaal niet aangpast worden, de upload poort wordt automatisch correct gekozen.
Indien dit wel nodig zou zijn, dan kan men simpelweg dit toevoegen: ```upload_port = COM?``` in het .ini bestand. Welke COM poort actief is, kan men vinden in de device manager.

### ESP32_MQTT_PUblisher
ESP32 code moet indien nodig de ssid, wifi_password, mqtt_server, mqtt_username, mqtt_password en de clientID worden aangepast.
Als men wilt is het mogelijk om de topics aan te passen, de slot_topic en de FacialRecognition_topic.

Voor de rest zou er niets aan de code moeten aangepast worden.

### Arduino Nano 33 BLE Sense Event CTRL
Hier moet men normaal geen code aanpassen.
Het kan soms zijn dat men met problemen komt bij het uploaden van het programma, men moet dan Arduino proberen een keer in de boatloader te zetten en dan terug in normale modus te gaan.

### Home Assistant OS Raspberry Pi
Als men het systeem wilt laten draaien op een ander netwerk, moet men een paar commando's uitvoeren in de cli van Home Assistant OS:

```nmcli device wifi rescan```
Dit commando gaat een Wi-Fi scan uitvoeren om nabije Wi-Fi netwerken te vinden.

```nmcli device wifi```
Dit commando gaat alle Wi-Fi netwerken die zijn gevonden weergeven.

```nmcli device wifi connect "YOUR_SSID" password "YOUR_WIFI_PASSWORD"```
Met dit commando kan je dan connecteren met het gewenste Wi-Fi netwerk.

```nmcli con show```
Zo kan je bekijken of het is gelukt, normaal moet hier de nieuwe connectie bijstaan.
