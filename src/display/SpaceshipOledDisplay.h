#pragma once

#include <U8g2lib.h>

class SpaceshipOledDisplay {
    U8G2 display;

public:

    static constexpr int8_t TEMP_HI = -1;
    static constexpr int8_t TEMP_LO = -2;

    SpaceshipOledDisplay(U8G2 display) : display(display) {
        
    };

    void begin();

    void clearBuffer();
    void sendBuffer();
    void setPowerSaveMode(bool mode);

    void setBrightness(uint8_t value);

    void leftTemp(int8_t value);

    void leftTemp(int8_t value, bool frame);

    void leftAuto(bool value);
    void rightTemp(int8_t value);
    void rightTemp(int8_t value, bool frame);
    void rightAuto(bool value);
    void rightFull(bool value);
    void fan(bool icon, uint8_t speed);
    void ac(bool on, bool off);
    void mode(bool windshield, bool human, bool front, bool bottom);
};