/**
 * wifi and mqtt manager header file
 * created by Emil Meindl, Lukas Heiligenbrunner
 * in 2019
 * last edited 2020
 */

#pragma once

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include "Leds.h"

#define EA_ID 0 // define the address in the eeprom to store the id
#define EA_SSID 100
#define EA_PASSWD 140
#define EA_MQTTIP 180
#define EA_MQTTPORT 10

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

    void writeEEPROMString(int startAddress, String inchar);

    String readEEPROMString(int startAddress);

    void writeEEPROMInt(int startAddress, int inint);

    int readEEPROMInt(int startAddress);

    void initVars();

    void storeVars();

    String getID();

    String getSSID();

    String getPassword();

    String getServer();

    String getPort();

    void setID(int id);

    void setSSID(String ssid);

    void setPassword(String password);

    void setServer(String server);

    void setPort(int port);

private:
    WiFiClient espClient; // wifi manager
    PubSubClient client;  // mqtt manager

    //char* WIFISSID = "maumauperfekt";
    //char* WIFIPASSWORD = "gutesPasswort";
    //char* MQTTSERVER = "192.168.65.15";
    //char* MQTTSERVER = "mqtt.heili.eu";
    //int MQTTPORT = 1883;
    String WIFISSID;
    String WIFIPASSWORD;
    String MQTTSERVER;
    int MQTTPORT;
};
