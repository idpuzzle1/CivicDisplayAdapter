#include <avr/pgmspace.h>
#include "SpaceshipButtons.h"

static const uint8_t AUDIO_LEVELS_COUNT = 6;
//static const uint16_t AUDIO_MASTER_LEVELS[AUDIO_LEVELS_COUNT] PROGMEM = {931, 807, 644, 447, 269, 92};
//static const uint16_t AUDIO_MASTER_LEVELS[AUDIO_LEVELS_COUNT] PROGMEM = {880, 760, 614, 426, 255, 93};
static const uint16_t AUDIO_MASTER_LEVELS[AUDIO_LEVELS_COUNT] PROGMEM = {792, 697, 575, 426, 288, 155};
static const uint8_t AUDIO_MASTER_TOLERANCE = 10;
static const uint16_t AUDIO_SLAVE_LEVELS[AUDIO_LEVELS_COUNT] PROGMEM = {512, 430, 338, 227, 135, 43};
static const uint8_t AUDIO_SLAVE_TOLERANCE = 20;
static const uint8_t HFT_LEVELS_COUNT = 5;
//static const uint16_t HFT_MASTER_LEVELS[HFT_LEVELS_COUNT] PROGMEM = {931, 709, 403, 184, 49};
//static const uint16_t HFT_MASTER_LEVELS[HFT_LEVELS_COUNT] PROGMEM = {479, 410, 290, 163, 60}; //Combined two vcc wires in one
static const uint16_t HFT_MASTER_LEVELS[HFT_LEVELS_COUNT] PROGMEM = {645, 579, 435, 264, 89}; // separate vcc wires
//static const uint16_t HFT_MASTER_LEVELS[HFT_LEVELS_COUNT] PROGMEM = {931, 579, 435, 264, 89}; // separate vcc wires
static const uint8_t HFT_TOLERANCE = 10;

ButtonSet SpaceshipButtons::audioSwitchMaster(uint8_t pin) {
    return ButtonSet(pin, AUDIO_MASTER_TOLERANCE, AUDIO_LEVELS_COUNT, AUDIO_MASTER_LEVELS);
}

ButtonSet SpaceshipButtons::audioSwitchSlave(uint8_t pin) {
    return ButtonSet(pin, AUDIO_SLAVE_TOLERANCE, AUDIO_LEVELS_COUNT, AUDIO_SLAVE_LEVELS);
}

ButtonSet SpaceshipButtons::hft(uint8_t pin) {
    return ButtonSet(pin, HFT_TOLERANCE, HFT_LEVELS_COUNT, HFT_MASTER_LEVELS);
}
