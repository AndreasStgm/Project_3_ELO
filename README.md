
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
Indien dit wel nodig zou zijn, dan kan men simpelweg dit toevoegen:
```upload_port = COM?```
Als men in device manager, makkelijk toegankelijk via CTRL+X, kan men de correcte poort terugvinden van de device.

### ESP32_MQTT_PUblisher
In de publisher moet men enkel het gedeelte

### Arduino Nano 33 BLE Sense Event CTRL
