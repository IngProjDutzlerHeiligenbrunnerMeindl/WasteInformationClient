#pragma once

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include "Leds.h"

#define EEPROMIDADDRESS 0 // define the address in the eeprom to store the id

// define the connection settings
#define WIFISSID "maumauperfekt"
#define WIFIPASSWORD "gutesPasswort"
//#define MQTTSERVER "192.168.65.15"

//#define WIFISSID "Virusprogrammierer-Gast"
//#define WIFIPASSWORD "1qayxsw2"
#define MQTTSERVER "mqtt.heili.eu"
#define MQTTPORT 1883

#define REQUESTTIMEOUT 4 // define a request timeout

/**
 * class to mange the connection to the wifi and mqtt server
 */
class WifiMqttManager {
public:
    void reconnect();

    void setup_wifi();

    static void callback(char *topic, byte *payload, unsigned int length);

    void setupEverything();

    void stayAlive();

    void request();

    void initID();

private:
    WiFiClient espClient; // wifi manager
    PubSubClient client;  // mqtt manager

};
