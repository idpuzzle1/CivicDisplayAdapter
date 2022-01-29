#include <Arduino.h>

#include "ButtonSet.h"

static const uint16_t refreshThreshold = 1;

ButtonSet::ButtonSet(uint8_t pinNumber, uint8_t tolerance, uint8_t levelsCount, const uint16_t *levels)
        : pinNumber(pinNumber),
          tolerance(tolerance),
          levelsCount(levelsCount),
          levels(levels) {
    pinMode(pinNumber, INPUT);
}

void ButtonSet::updateButtons() {
    static auto prevMillis = millis();
    auto currentMillis = millis();
    if (currentMillis - prevMillis < refreshThreshold) {
        return;
    }
    prevMillis = currentMillis;

    auto adcValue = analogRead(pinNumber);
    auto rawButton = getRawPressedButton(adcValue);
    if (rawButton == 0 || rawButton > 0 && state.pressedButton == 0) {
        if (rawButton != state.pressedButton) {
            if (rawButton > 0) {
                state.startPressMillis = millis();
                state.endPressMillis = 0;
            } else {
                state.endPressMillis = millis();
            }
            
        }
        state.pressedButton = rawButton;
    }
    state.currentAdcValue = adcValue;
}

int ButtonSet::getAdcValue() {
    return state.currentAdcValue;
}

uint16_t ButtonSet::openAdcValue() {
    auto openValue = pgm_read_word(&levels[0]);
    return openValue;
};

uint8_t ButtonSet::getPressedButton() {
    return state.pressedButton;
}

uint16_t ButtonSet::getPressedMillis() {
    if (state.startPressMillis == 0) {
        return 0;
    }
    if (state.endPressMillis > 0) {
        return state.endPressMillis - state.startPressMillis;
    }
    return millis() - state.startPressMillis;
}

int8_t ButtonSet::getRawPressedButton() {
    auto adcValue = analogRead(pinNumber);
    return getRawPressedButton(adcValue);
}

int8_t ButtonSet::getRawPressedButton(int adcValue) {
    for (uint8_t i = 0; i < levelsCount; ++i) {
        auto candidateValue = pgm_read_word(levels + i);
        if (abs(adcValue - (int16_t) candidateValue) <= tolerance) {
            return i;
        }
    }
    return -1;
}
