#pragma once

#include <Adafruit_NeoPixel.h>

#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

#define colorBiowaste 0,255,0 // brown
#define colorPlastic 255,255,0 // yellow
#define colorResidual 255,255,255 // white
#define colorMetal 0,0,255 // blue

#define colorPapier 255,0,0 // red

class Leds {
private:


public:

    void setNotConfigured();

    void setRequestTimeout();

    void setWifiTimeout();

    void disableWifiTimeout();

    void setNotificationLed(int wasteType, boolean state);

    void init(int brightness);

    void clearLeds();

    void clearLed(int wasteType);


};
