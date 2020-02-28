#include <Adafruit_NeoPixel.h>
#include <Ticker.h>
#include "Leds.h"

// definition of the required parameters to control the leds
#define PIN 4
#define NUMPIXELS 4

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800); // abstraction of the led pixels
Ticker fadeNC; // ticker to show that the client is not configured
Ticker fadeRT; // ticker to show that the request timed out (not answer from the server)
Ticker fadeWT; // ticker to show that the wifi connection dropped or timed out
boolean fadeing = false; // flag to set if the leds at in fading mode or eliminating permanently

/**
 * set led to show a not configured client
 */
void Leds::setNotConfigured() {
    Serial.println("Set LEDs");

    fadeRT.detach(); // disable the request timeout ticker
    clearLed(1); // clear the second led
    fadeWT.detach(); // disable the wifi timeout ticker
    clearLed(2); // clear the third led

    static int j = 0; // variable for fading

    fadeing = true; // set the fading flag
    fadeNC.attach_ms(5, []() { // enable fading
        if (j < 256) { // fade up
            pixels.setPixelColor(0, pixels.Color(j, j, j)); // fade up the first led
        } else { // fade down
            int c = 511 - j; // calculate fading value
            pixels.setPixelColor(0, pixels.Color(c, c, c)); // fade down the led
        }
        pixels.show(); // eluminate led
        j++; // increment the fade counter
        if (j > 511) { // reset the fade counter
            j = 0;
        }
    });
}

/**
 * set led to show a request timeout of the request
 */
void Leds::setRequestTimeout() {
    Serial.println("Set LEDs");

    fadeNC.detach(); // disable the not connected timeout ticker
    clearLed(0); // clear the first led

    static int j = 0; // variable for fading

    fadeing = true; // set the fading flag
    fadeRT.attach_ms(5, []() { // enable fading
        if (j < 256) { // fade up
            pixels.setPixelColor(1, pixels.Color(j, 0, 0)); // fade up the first led
        } else { // fade down
            int c = 511 - j; // calculate fading value
            pixels.setPixelColor(1, pixels.Color(c, 0, 0)); // fade down the led
        }
        pixels.show(); // eluminate led
        j++; // increment the fade counter
        if (j > 511) { // reset the fade counter
            j = 0;
        }
    });
}

/**
 * set led to show a dropped wifi connection/wifi timeout
 */
void Leds::setWifiTimeout() {
    Serial.println("Set LEDs");

    fadeNC.detach();
    clearLed(0);

    static int j = 0;

    fadeing = true;
    fadeWT.attach_ms(5, []() {
        if (j < 256) {
            pixels.setPixelColor(2, pixels.Color(j, 0, j));
        } else {
            int c = 511 - j;
            pixels.setPixelColor(2, pixels.Color(c, 0, c));
        }
        pixels.show();
        j++;
        if (j > 511) {
            j = 0;
        }
    });
}

/**
 * set led to show the pickup of a chosen waste type
 */
void Leds::setNotificationLed(int wasteType, boolean state) {
    Serial.println("Set LEDs");
    if (fadeing) {
        // disable fading
        fadeNC.detach();
        fadeRT.detach();
        fadeWT.detach();
        clearLeds();
        fadeing = false;
    }

    if (state) {
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
    } else {
        clearLed(wasteType - 1);
    }

}

void Leds::init(int brightness) {
    Serial.println("Init pixels");
    pixels.begin();
    pixels.clear();
    pixels.setBrightness(brightness);
}

void Leds::clearLeds() {
    for (int i = 0; i < NUMPIXELS; i++) {
        clearLed(i);
    }
}

void Leds::clearLed(int wasteType) {
    pixels.setPixelColor(wasteType, pixels.Color(0, 0, 0));
    pixels.show();
}
