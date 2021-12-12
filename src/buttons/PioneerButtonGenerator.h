#pragma once

#include <Arduino.h>

#include "../util/Pin.h"

class PioneerButtonGenerator {

    protected:

    Pin csPin;

    public: 

    static const uint8_t CLICK_TIME = 50;
    static const uint8_t LONG_CLICK_TIME = 1000;

    //digipot values

    static const uint8_t SOURCE = 2; //1,2k	1 - 5
    static const uint8_t TRACK_NEXT = 19; //8,2k	17 - 22
    static const uint8_t TRACK_PREV = 27; //11,25k	23 -32
    static const uint8_t VOL_UP = 42; //16k	33 - 46
    static const uint8_t VOL_DOWN = 55; //24k	47 - 72
    static const uint8_t BAND = 100; //62,75k	88 - 212 //Play/Pause
    static const uint8_t DISP = 180; //5,75	12 - 16
    static const uint8_t MUTE = 8; //3,5	6 - 11
    static const uint8_t VOICE = 170; //6,7k	98 - 232
    //static const uint8_t VOICE = 232; //38-88k
    static const uint8_t HANG_UP = 14; //5,5k	14

    PioneerButtonGenerator(uint8_t _csPin): csPin(_csPin){
        csPin.doOutput(true);
    };

    void updateTimer();

    /* In the function for the tip commands we only change resistance for the 
     wiper where the tip is connected. The ring floats and is ignored by the 
     head unit */
    void wrTip(int digiValue, int delayMs);

    void wrTipHold(int digiValue);
    //void wrTipRelease();

    /* For ring functions the wiper for the tip is where resistance is applied
     in combination with taking the ring to ground */
    void wrRing(int digiValue, int delayMs); 

    void wrRingHold(int digiValue);
    //void wrRingRelease();

    void wrRelease();
};