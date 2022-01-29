#include <Arduino.h>
#include <SPI.h>

#include "PioneerSimpleButtons.h"

void PioneerSimpleButtons::pressButton(uint8_t button) {
    sendValue(button);
}

void PioneerSimpleButtons::releaseButton() {
    csPin.writeLow();
    SPI.transfer(0);
    SPI.transfer(0);
    csPin.writeHigh();
    SPI.endTransaction();
}

void PioneerSimpleButtons::sendValue(uint8_t value) {
    csPin.writeLow();
    SPI.transfer(0);
    SPI.transfer(value);
    csPin.writeHigh();
    SPI.endTransaction();
}