MQTT publisher code
%
Code verified!

interessant en waar ik mijn info van heb https://diyi0t.com/microcontroller-to-raspberry-pi-wifi-mqtt-communication/


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Mqtt testen in cli RPI
----------------------

MQTT cmds:
sudo systemctl status mosquitto
sudo systemctl start mosquitto
sudo systemctl stop mosquitto

open 2 terminals, 1 voor pub ander voor sub zo kunden MQTT eerst testen.

publish: mosquitto_pub -t RFID tag -h localhost -m "Testerino"
subcribe: mosquitto_sub -t RFID tag -u ijmert -P ijmert -h localhost

-t ${topic} -u ${username} -P ${password} -h ${host/ip} -m ${message}

-u en -P zijn in principe niet nodig naargelang hoe ge uw MQTT instelt.
Bij mij is het u esp32 en p esp32 omdat ik die al eerder zo had ingesteld.
Bij ons project was het nu ijmert ijmert als ik me niet vergis.

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
