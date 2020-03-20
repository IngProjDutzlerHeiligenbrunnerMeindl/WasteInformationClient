#include <Ticker.h>
#include <EEPROM.h>
#include "WifiMqttManager.h"

#define REQUESTDELAY 5 // delay between requests

WifiMqttManager manager; // generating a new manager for the mqtt connections
Ticker requestTicker;    // generating a software timer to request data periodically
boolean configMode = true;

// setup() only execute once for initialization
void setup() {
    Serial.begin(115200);     // define to serial interface with the boud rate

    delay(1000);
    pinMode(0, INPUT);
    configMode = !digitalRead(0);

    Serial.println(configMode);
    if (configMode) {
        Serial.println("Configmode");
    } else {
        delay(1000);

        pinMode(LED_BUILTIN, OUTPUT);   // define the pin of the onboard led as an output
        // blue led a the esp8266 chip
        digitalWrite(LED_BUILTIN, LOW); // turn to led on, active low

        manager.setupEverything();      // setup the wifi and the mqtt connection

        requestTicker.attach(REQUESTDELAY, []() { // define the softwaretimer to request periodically
            digitalWrite(LED_BUILTIN, LOW);   // turn to led on to indicate a running request
            manager.request();                // request data
            digitalWrite(LED_BUILTIN, HIGH);  // turn of the led to indicate the end of the request
        });
        digitalWrite(LED_BUILTIN, HIGH);      // turn of the led at the and of the initialization routine
    }
}

// loop() executed endless
void loop() {
    if (configMode) {

    } else {
        manager.stayAlive(); // keep the wifi and mqtt connections alive
        // reconnect when dropped
    }
}
