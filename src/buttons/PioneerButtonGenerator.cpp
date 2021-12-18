#include <Arduino.h>
#include <SPI.h>

#include "PioneerButtonGenerator.h"

#define csPin PioneerButtonGenerator::csPin
#define tipPin PioneerButtonGenerator::tipPin
#define ringPin PioneerButtonGenerator::ringPin

// Set the wiper addresses for the digipot
int wiperTip = B00000000;
int wiperRing = B00010000;


// Set the digitpot resistor value for no button
int ground = 0;

bool isRing = false;
int holdMs = 0;
long startHoldMs = 0;

void holdStart(bool _isRing, int _holdMs) {
    isRing = _isRing;
    holdMs = _holdMs;
    startHoldMs = millis();
}

void PioneerButtonGenerator::updateTimer() {
    if (holdMs > 0 && millis() - startHoldMs < 0) {
        wrRelease();
    }
}

void PioneerButtonGenerator::wrTip(int digiValue, int delayMs) {
    wrTipHold(digiValue);
    holdStart(false, delayMs);
}

void PioneerButtonGenerator::wrTipHold(int digiValue) {
    if (holdMs > 0) {
        wrRelease();
    }
    SPI.beginTransaction(SPISettings(250000, MSBFIRST, SPI_MODE0));
    csPin.writeLow();
    SPI.transfer(wiperTip);
    SPI.transfer(digiValue);
//      Serial.println(" Hold"); // for debug
    csPin.writeHigh();
    SPI.endTransaction();
}

void PioneerButtonGenerator::wrRing(int digiValue, int delayMs) {
    wrRingHold(digiValue);
    holdStart(true, delayMs);
}

void PioneerButtonGenerator::wrRingHold(int digiValue) {
    if (holdMs > 0) {
        wrRelease();
    }
    SPI.beginTransaction(SPISettings(250000, MSBFIRST, SPI_MODE0));
    csPin.writeLow();
    SPI.transfer(wiperRing);
    SPI.transfer(ground);
//      Serial.println(" Select Ring, ground"); // for debug
    SPI.transfer(wiperTip);
    SPI.transfer(digiValue);
    csPin.writeHigh();
    SPI.endTransaction();
}

void PioneerButtonGenerator::wrRelease() {
    holdMs = 0;
    startHoldMs = 0;
    SPI.beginTransaction(SPISettings(250000, MSBFIRST, SPI_MODE0));
    csPin.writeLow();
    SPI.transfer(wiperTip);
    SPI.transfer(ground);
    if (isRing) {
        SPI.transfer(wiperRing);
        SPI.transfer(0); // float
    }
    //      Serial.println(" Button Release"); // for debug
    csPin.writeHigh();
    isRing = false;
    SPI.endTransaction();
}