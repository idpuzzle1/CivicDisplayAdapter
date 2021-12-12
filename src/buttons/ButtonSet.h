#pragma once

#include <stdint.h>

class ButtonSet {

public:

    ButtonSet(uint8_t pinNumber, uint8_t tolerance, uint8_t levelsCount, const uint16_t *levels);

    void updateButtons();

    const uint8_t pinNumber;
    const uint8_t tolerance;

    uint16_t openAdcValue();

    int getAdcValue();
    uint8_t getPressedButton();
    int8_t getRawPressedButton();
    uint16_t getPressedMillis();

private:

    struct State {
        uint8_t pressedButton;
        int currentAdcValue;
        uint16_t startPressMillis;
        uint16_t endPressMillis;
    };

    State getState() {
        return state;
    };

    
    const uint8_t levelsCount;
    const uint16_t *levels;

    State state;

    int8_t getRawPressedButton(int adcValue);
};
