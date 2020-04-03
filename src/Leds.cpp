#include <Adafruit_NeoPixel.h>
#include <Ticker.h>
#include "Leds.h"

// definition of the required parameters to control the leds
#define PIN 4
#define NUMPIXELS 4

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800); // abstraction of the led pixels
boolean fadeNC; // flag to set the client is not configured
boolean fadeRT; // flag to set that the request timed out (not answer from the server)
boolean fadeWT; // flag to set that the wifi connection dropped or timed out
Ticker fade;    // fading ticker
int j = 0;
int xxx = 0;

/**
 * set led to show a not configured client
 */
void Leds::setNotConfigured() {
    fadeRT = false; // disable the request timeout ticker
    clearLed(1); // clear the fadeRT led
    fadeWT = false; // disable the wifi timeout ticker
    clearLed(2); // clear the fadeWT led
    fadeNC = true;
}

/**
 * set led to show a request timeout of the request
 */
void Leds::setRequestTimeout() {
    fadeNC = false; // disable the not connected timeout ticker
    clearLed(0); // clear the fadeNC led
    fadeRT = true;
}

/**
 * set led to show a dropped wifi connection/wifi timeout
 */
void Leds::setWifiTimeout() {
    fadeNC = false; // deactivate the not connected timeout ticker
    clearLed(0); // clear the fadeNC led
    fadeWT = true;
}

void Leds::disableWifiTimeout() {
    if (fadeWT) { // if one of the leds is fading
        // disable wifi fading
        fadeWT = false;
        clearLed(2); // clear the fadeWT led
    }
}

/**
 * set led to show the pickup of a chosen waste type
 */
void Leds::setNotificationLed(int wasteType, boolean state) {
    if (fadeRT || fadeWT || fadeNC) { // if one of the leds is fading
        // disable fading
        fadeNC = false;
        fadeRT = false;
        fadeWT = false;
        clearLeds();
    }

    if (state) { // if the leds should switched on
        if (wasteType == 0) {
            clearLeds();
        } else if (wasteType == 1) { // plastic
            pixels.setPixelColor(0, pixels.Color(colorPlastic));

        } else if (wasteType == 2) { // metal
            pixels.setPixelColor(1, pixels.Color(colorMetal));

        } else if (wasteType == 3) { // residual waste
            pixels.setPixelColor(2, pixels.Color(colorResidual));

        } else if (wasteType == 4) { // biowaste
            pixels.setPixelColor(3, pixels.Color(colorBiowaste));

        }
        pixels.show();
    } else { // switch off the chosen led
        clearLed(wasteType - 1);
    }

}

/**
 * function to initialize the leds
 *
 * @param brightness global maximum brightness
 */
void Leds::init(int brightness) {
    Serial.println("Init pixels");
    pixels.begin();
    pixels.clear();
    pixels.setBrightness(brightness);
    fade.attach_ms(5,[](){
        if (j < 256) { // fade up
            xxx = j;
        } else { // fade down
            int c = 511 - j; // calculate fading value
            xxx = c;
        }
        j++; // increment the fade counter
        if (j > 511) { // reset the fade counter
            j = 0;
        }

        if (fadeNC){
            pixels.setPixelColor(0, pixels.Color(xxx, xxx, xxx));
        } if (fadeRT){
            pixels.setPixelColor(1, pixels.Color(xxx, 0, 0));
        } if (fadeWT){
        pixels.setPixelColor(2, pixels.Color(xxx, 0, xxx));
        }
        pixels.show(); // eluminate led
    });
}

void Leds::begin(){
    pixels.begin();
}

/**
 * function to clear all leds
 */
void Leds::clearLeds() {
    for (int i = 0; i < NUMPIXELS; i++) { // loop all leds
        clearLed(i);
    }
}

/**
 * function to clear a single led
 *
 * @param wasteType led address
 */
void Leds::clearLed(int wasteType) {
    pixels.setPixelColor(wasteType, pixels.Color(0, 0, 0));
    pixels.show();
}
