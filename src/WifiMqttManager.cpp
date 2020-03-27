#include <Ticker.h>
#include <EEPROM.h>
#include "WifiMqttManager.h"

char requestAnswer[1024]; // answer received from the mqtt server
int waste = 0;            // wasttype
boolean isTrue = 0;       // flag if the selected waste will be picked
Ticker timeoutTicker;     // ticker to indicate communication timeouts over mqtt
Leds led;                 // object to control the ws2812b leds depending on the request results

static int clientIdentifier; // client id to have a unique number to identify the single client at the server

/**
 * setup the connetions (wifi and mqtt) and init the ws2812b leds
 */
void WifiMqttManager::setupEverything() {
    led.init(50); // initialise the led with a fixed bigness
    initVars(); // read values from the eeprom

    initID(); // read/generate a id from the eeprom

    client.setClient(espClient);
    setup_wifi(); // configure the wifi connection
    client.setServer(MQTTSERVER.c_str(), MQTTPORT); // configure the mqtt connection and connect
    client.setCallback(callback); // set the callback method the react to received mqtt messages
}

/**
 * method th keep the wifi connection alive reconnect if dropped
 */
void WifiMqttManager::stayAlive() {
    if (!client.connected()) { // check if the connection dropped
        reconnect(); // reconnect if the connection dropped to keep the client requestable
    }
    client.loop();
}

/**
 * setup the wifi connection to request data from the server
 */
void WifiMqttManager::setup_wifi() {
    delay(10); // short pause to be sure that the esp is ready for the wifi setup

    // print a status message to indicate the init process
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(WIFISSID);

    WiFi.begin(WIFISSID, WIFIPASSWORD); // connect to the wifi network with the set ssid and passwort

    int retries = 0;
    while (WiFi.status() != WL_CONNECTED) { // wait until the connection to up and running
        delay(500); // delay between checks
        Serial.print("."); // print a "." to indicate the connecting process
        retries++;
        if (retries > 32){
            led.setWifiTimeout();
        }
    }
    led.disableWifiTimeout();

    randomSeed(micros());

    // print the most important connection data
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

/**
 * callback function is called ba the manager when receiving data at the request topic
 *
 * @param topic
 * @param payload
 * @param length
 */
void WifiMqttManager::callback(char *topic, byte *payload, unsigned int length) {
    for (unsigned int i = 0; i < length; i++) { // loop through the single digits of the requested data as string
        requestAnswer[i] = (char) payload[i]; // cast the received data and store in an char array
    }
    char delimiter[] = ","; // set the diameter between the single parameters received over mqtt
    // print the received data for debugging purposes
    Serial.print("RequestAnswer: ");
    Serial.println(requestAnswer);

    if (atoi(strtok(requestAnswer, delimiter)), String(clientIdentifier)) { // identifies the received data as an answer to a request done by this client
        waste = atoi(strtok(NULL, delimiter)); // filter the information about the waste type of the received data
        Serial.print("waste: ");
        Serial.println(waste);
        if (waste < 0) { // client not configured in the database
            led.setNotConfigured(); // set the leds to show that this client is not configured jet
        } else if (waste == 0) { // waste will not be picked up
            led.clearLeds(); // clear all leds
        } else { // client already configured
            isTrue = atoi(strtok(NULL, delimiter)); // filter the state of the selected waste type
            Serial.print("y/n: ");
            Serial.println(isTrue);
            led.setNotificationLed(waste, isTrue); // set the leds depending on the waste type and the state flag
        }
        timeoutTicker.detach(); // detach the ticker to indicate a timeout because the requested data has sent back
    }
}

/**
 * function to request data over the mqtt topic from the server/database
 */
void WifiMqttManager::request() {
    // start the timeout ticker to identify a request timeout while requesting data
    timeoutTicker.once(REQUESTTIMEOUT, []() {
        // is the request does not arrive in the REQUESTTIMEOUT print a message and set the leds to indicate
        Serial.println("Failed to request data!!!");
        led.setRequestTimeout();
    });

    // print a message to show the request was sent
    Serial.print("Request: ");
    String temp = String(clientIdentifier);

    char tempc[temp.length()]; // create an array with the parameters to send for a request
    temp.toCharArray(tempc, temp.length() + 1); // convert the String in a char array

    Serial.println(tempc);

    // send the request information to the server
    client.publish("TopicIn", tempc); // clientIdentifier
}

/**
 * function to reconnect the client to the wifi and the mqtt server
 */
void WifiMqttManager::reconnect() {
    // Loop until we're reconnected
    while (!client.connected()) { // while the client is not connected
        Serial.print("Attempting MQTT connection...");
        // Create a random client ID
        String clientId = "ESP8266Client-";
        clientId += String(random(0xffff), HEX);
        // Attempt to connect
        if (client.connect(clientId.c_str())) { // if the connection with the generated client id worked
            Serial.println("connected");
            // Once connected, publish an announcement...
            //client.publish("Topic", "give my data");
            // ... and resubscribe
            client.subscribe("TopicOut");
        } else { // id the connection could not be established
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            led.setWifiTimeout(); // set the leds the indicate a wifi timeout
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }

}

/**
 * initialize the id to find the client specific configs in the database
 */
void WifiMqttManager::initID(){
    EEPROM.begin(4096); // init the eeprom
    EEPROM.get(EA_ID, clientIdentifier);
    if (clientIdentifier < 1000 || clientIdentifier > 9999) { // is the value at the EEPROMIDADDRESS is not between 1000 and 9999
        EEPROM.put(EA_ID, random(1000, 9999)); // write a newly generated id between 1000 and 9999 to the eeprom
        EEPROM.commit(); // write the changes to the eeprom
    }
    Serial.println("ID:" + clientIdentifier);
    EEPROM.end(); // stop the eeprom communication
}

void WifiMqttManager::writeEEPROMString(int startAddress, String in){
    EEPROM.begin(4096); // init the eeprom
    int i = 0;
    in += '\n';
    while(in[i]!= '\n')
    {
        EEPROM.write(i+startAddress, in[i]);
        EEPROM.commit();
        i++;
    }
    EEPROM.write(i+startAddress, '\n');
    EEPROM.commit();
    EEPROM.end(); // stop the eeprom communication
}

String WifiMqttManager::readEEPROMString(int startAddress){
    EEPROM.begin(4096); // init the eeprom
    int i = 0;
    char in;
    String esid;
    in = char(EEPROM.read(i+startAddress));
    while(char(in) != '\n')
    {
        esid += in;
        i++;
        in = char(EEPROM.read(i+startAddress));
    }
    EEPROM.end(); // stop the eeprom communication
    return esid;
}

void WifiMqttManager::writeEEPROMInt(int startAddress, int inint){
    EEPROM.begin(4096); // init the eeprom
    EEPROM.put(startAddress, inint);
    EEPROM.commit(); // write the changes to the eeprom
    EEPROM.end(); // stop the eeprom communication
}

int WifiMqttManager::readEEPROMInt(int startAddress){
    int value = 0;
    EEPROM.begin(4096); // init the eeprom
    EEPROM.get(startAddress, value);
    EEPROM.end(); // stop the eeprom communication
    return value;
}

void WifiMqttManager::initVars(){
    /*
     * char* WIFISSID = "maumauperfekt";
     * char* WIFIPASSWORD = "gutesPasswort";
     * char* MQTTSERVER = "mqtt.heili.eu";
     * int MQTTPORT = 1883;
     * */
    WIFISSID = (readEEPROMString(EA_SSID));
    WIFIPASSWORD = (readEEPROMString(EA_PASSWD));
    MQTTSERVER = (readEEPROMString(EA_MQTTIP));
    MQTTPORT = (readEEPROMInt(EA_MQTTPORT));

//    WIFISSID = "Virusprogrammierer-Gast";
//    WIFIPASSWORD = "1qayxsw2";
//    MQTTSERVER = "mqtt.heili.eu";
//    MQTTPORT = 1883;
}

void WifiMqttManager::storeVars(){
    /*
     * char* WIFISSID = "maumauperfekt";
     * char* WIFIPASSWORD = "gutesPasswort";
     * char* MQTTSERVER = "mqtt.heili.eu";
     * int MQTTPORT = 1883;
     * */
    writeEEPROMString(EA_SSID, WIFISSID);
    writeEEPROMString(EA_PASSWD, WIFIPASSWORD);
    writeEEPROMString(EA_MQTTIP, MQTTSERVER);
    writeEEPROMInt(EA_MQTTPORT, MQTTPORT);
}

String WifiMqttManager::getID(){
    return String(clientIdentifier);
}

String WifiMqttManager::getSSID(){
    return WIFISSID;
}

String WifiMqttManager::getPassword(){
    return WIFIPASSWORD;
}

String WifiMqttManager::getServer(){
    return MQTTSERVER;
}

String WifiMqttManager::getPort(){
    return String(MQTTPORT);
}

void WifiMqttManager::setID(int id){
    clientIdentifier = id;
}

void WifiMqttManager::setSSID(String ssid){
    WIFISSID = ssid;
}

void WifiMqttManager::setPassword(String password){
    WIFIPASSWORD = password;
}

void WifiMqttManager::setServer(String server){
    MQTTSERVER = server;
}

void WifiMqttManager::setPort(int port){
    MQTTPORT = port;
}