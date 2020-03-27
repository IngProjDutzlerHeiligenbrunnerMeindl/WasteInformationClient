#include <Ticker.h>
#include <EEPROM.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include "WifiMqttManager.h"

#define REQUESTDELAY 5 // delay between requests

WifiMqttManager manager; // generating a new manager for the mqtt connections
Ticker requestTicker;    // generating a software timer to request data periodically
boolean configMode = true;

const char *PARAM_INPUT_1 = "ssid";
const char *PARAM_INPUT_2 = "wifipassword";
const char *PARAM_INPUT_3 = "mqttip";
const char *PARAM_INPUT_4 = "mqttport";
const char *PARAM_INPUT_5 = "identificator";

ESP8266WebServer server(80);

void handleRoot();

void handleNotFound();

void handleGet();

// setup() only execute once for initialization
void setup() {
    Serial.begin(115200);     // define to serial interface with the boud rate

    delay(1000); // wait one second after start
    pinMode(0, INPUT); // set the flash button pin as input
    configMode = !digitalRead(0); // read the push button (flash button)

    if (configMode) { // config mode
        Serial.println("Configmode");
        WiFi.mode(WIFI_AP);
        EEPROM.begin(4096);
        int clientIdentifier = 0;
        EEPROM.get(EA_ID, clientIdentifier);
        Serial.println("Wasteinfoboard-" + String(clientIdentifier));
        WiFi.softAP("Wasteinfoboard-" + String(clientIdentifier), "waste123");
        EEPROM.end();
        delay(500);
        IPAddress IP = WiFi.softAPIP();
        Serial.print("AP IP address: ");
        Serial.println(IP);

        //server.on("/", handleRoot);      //Which routine to handle at root location

        server.on("/", HTTP_GET, handleRoot);
        server.on("/specificArgs", handleGet);
        server.onNotFound(handleNotFound);

        server.begin();
    } else { // request/normal mode
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
    if (configMode) { // config mode
        server.handleClient();
    } else { // request/normal mode
        manager.stayAlive(); // keep the wifi and mqtt connections alive
        // reconnect when connection dropped
    }
}

void handleRoot() { // When URI / is requested, send a web page with a button to toggle the LED
//    manager.initVars();
//    manager.setPort(1883);
//    manager.storeVars();
//    manager.setPort(3333);

    manager.initVars();
    manager.initID();
    const String index_html = "<!DOCTYPE HTML><html><head>\n"
                              "  <title>ESP Input Form</title>\n"
                              "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n"
                              "  </head><body>\n"
                              "  <form action=\"/get?" + String(PARAM_INPUT_1) + "\">\n"
                              "    " + String(PARAM_INPUT_1) + ": <input placeholder=" + manager.getSSID() + " type=\"text\" name=>\n"
                              "    <input type=\"submit\" value=\"Submit\">\n"
                              "  </form><br>\n"
                              "  <form action=\"/get?" + String(PARAM_INPUT_2) + "\">\n"
                              "    " + String(PARAM_INPUT_2) + ": <input placeholder=" + manager.getPassword() + " type=\"text\" name=>\n"
                              "    <input type=\"submit\" value=\"Submit\">\n"
                              "  </form><br>\n"
                              "  <form action=\"/get?" + String(PARAM_INPUT_2) + "\">\n"
                              "    " + String(PARAM_INPUT_3) + ": <input placeholder=" + manager.getServer() + " type=\"text\" name=>\n"
                              "    <input type=\"submit\" value=\"Submit\">\n"
                              "  </form><br>\n"
                              "  <form action=\"/get?" + String(PARAM_INPUT_3) + "\">\n"
                              "    " + String(PARAM_INPUT_4) + ": <input placeholder=" + manager.getPort() + " type=\"text\" name=>\n"
                              "    <input type=\"submit\" value=\"Submit\">\n"
                              "  </form><br>\n"
                              "  <form action=\"/get?" + String(PARAM_INPUT_4) + "\">\n"
                              "    " + String(PARAM_INPUT_5) + ": <input placeholder=" + manager.getID() + " type=\"text\" name=>\n"
                              "    <input type=\"submit\" value=\"Submit\">\n"
                              "  </form><br>\n"
                              "</body></html>";

    server.send(200, "text/html", index_html);
}

void handleNotFound() {
    server.send(404, "text/plain",
                "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}

void handleGet(){
    String message = "";

    if (server.arg("Temperature")== ""){     //Parameter not found

        message = "Temperature Argument not found";

    }else{     //Parameter found

        message = "Temperature Argument = ";
        message += server.arg("Temperature");     //Gets the value of the query parameter

    }
}