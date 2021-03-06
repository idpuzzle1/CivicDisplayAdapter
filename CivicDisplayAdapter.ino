// Core
#include <Arduino.h>
#include <SPI.h>

// Display
#include <U8g2lib.h>

//Internal
#include "environment.h"
#include "src/util/Pin.h"
#include "src/hvac/SpaceshipHvac.h"
#include "src/display/SpaceshipOledDisplay.h"
#include "src/buttons/SpaceshipButtons.h"
#include "src/buttons/PioneerButtonGenerator.h"

const uint16_t LOOP_PERIOD_MS = 50;
const uint32_t LOOP_PERIOD_US = LOOP_PERIOD_MS * 1000L;

SpaceshipHvac hvac(HVAC_CLOCK, HVAC_DATA);
SpaceshipOledDisplay display(U8G2_SSD1309_128X64_NONAME0_F_4W_HW_SPI(U8G2_R0, DISP_CS, DISP_DC));

Pin lightSignal = Pin(HVAC_LIGHTS);
ButtonSet audioButtons = SpaceshipButtons::audioSwitchMaster(SW_AUDIO);
ButtonSet hftButtons = SpaceshipButtons::hft(SW_HFT);
PioneerButtonGenerator huButtons = PioneerButtonGenerator(HU_CS);

struct ButtonState {
  uint8_t lastReleasedButton = 0;
  uint8_t pressedButton = 0;
  int8_t rawButton = 0;
  uint16_t pressedMillis = 0;
  int adc = 0;
};

struct State
{
  SpaceshipHvac::State hvac;
  ButtonState audioButton;
  ButtonState hftButton;
};
State state;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  lightSignal.doInput();
  
  SPI.begin();
  Serial.begin(SERIAL_SPEED);
  display.begin();

  setupTestState(state.hvac);
}

void loop() {
  auto startTimeUs = micros();
  auto prevState = state;
  
  // Update Timers
  huButtons.updateTimer();
  audioButtons.updateButtons();
  hftButtons.updateButtons();

  // Display
  //hvac.readState(state.hvac);

  drawDisplay(state.hvac);
  //printHvacSerial(state.hvac);

  if (lightSignal.read()) {
    display.setBrightness(255);
  } else {
    display.setBrightness(0);
  }

  // Buttons
  getPressedButtons(state, prevState);
  //pressButton(prevState);
  printButtonSerial(state, prevState);

  updateTestState(state);

  static const uint32_t LOOP_TIME_ADJUST_US = 50;
  auto loopTimeUs = micros() - startTimeUs + LOOP_TIME_ADJUST_US;
  if (loopTimeUs < LOOP_PERIOD_US) {
    auto delayTotal = LOOP_PERIOD_US - loopTimeUs;
    uint16_t delayMs = delayTotal / 1000;
    uint16_t delayUs = delayTotal % 1000;
    delay(delayMs);
    delayMicroseconds(delayUs);
  }
}

void setupTestState(SpaceshipHvac::State &state) {
  state.connected = true;
  state.leftWheel = true;
  state.acOff = false;
  state.acOn = true;
  state.autoMode = true;

  state.temp1 = SpaceshipHvac::TEMP_HI;
  state.temp2 = SpaceshipHvac::TEMP_LO;
  state.fanSpeed = 5;
  state.airDirection = 4;
}

void printHvacSerial(SpaceshipHvac::State hvac) {
  if (!hvac.connected) {
    Serial.print("<");
    Serial.print("OFF");
    Serial.println(">");
    return;
  }

  Serial.print("<");
  Serial.print("T1:");
  Serial.print(hvac.temp1,DEC);
  Serial.print("-");
  Serial.print("Auto:");
  Serial.print(hvac.autoMode,DEC);
  Serial.print("-");
  Serial.print("AC:");
  Serial.print(hvac.acOn + (hvac.acOff << 1),DEC);
  Serial.print("-");
  Serial.print("Fan:");
  Serial.print(hvac.fanSpeed,DEC);
  Serial.print("-");
  Serial.print("Dir:");
  Serial.print(hvac.airDirection,DEC);
  Serial.print("-");
  Serial.print("T2:");
  Serial.print(hvac.temp2,DEC);
  Serial.println(">");
}

void printButtonSerial(State state, State prevState) {
  bool pressedAudio = state.audioButton.pressedButton != prevState.audioButton.pressedButton;
  bool pressedHft = state.hftButton.pressedButton != prevState.hftButton.pressedButton;

  if (!pressedAudio && !pressedHft) {
    auto audioAdcOpenDiff = audioButtons.openAdcValue() - state.audioButton.adc;
    auto hftAdcOpenDiff = hftButtons.openAdcValue() - state.hftButton.adc;
    bool unknownAudio = state.audioButton.pressedButton == 0 && abs(audioAdcOpenDiff) > audioButtons.tolerance;
    bool unknownHft = state.hftButton.pressedButton == 0 && abs(hftAdcOpenDiff) > hftButtons.tolerance;

    if (!unknownAudio && !unknownHft) {
      return;
    }

    Serial.print("<");
    if (unknownAudio) {
      Serial.print("Audio Missed:");
      Serial.print("ADC=");
      Serial.print(state.audioButton.adc, DEC);
    }
    if (unknownAudio && unknownHft) {
      Serial.print("; ");
    }
    if (unknownHft) {
      Serial.print("HFT Missed:");
      Serial.print("ADC=");
      Serial.print(state.hftButton.adc, DEC);
    }
    Serial.println(">");
    
    return;
  }

  Serial.print("<");
  if (pressedAudio) {
    Serial.print("Audio:");
    Serial.print("[");
    Serial.print(state.audioButton.adc,DEC);
    Serial.print("] ");
    Serial.print(prevState.audioButton.pressedButton,DEC);
    Serial.print("->");
    Serial.print(state.audioButton.pressedButton,DEC);
    Serial.print(" (");
    Serial.print(state.audioButton.pressedMillis,DEC);
    Serial.print("ms)");
  }
  if (pressedAudio && pressedHft) {
    Serial.print("; ");
  }
  if (pressedHft) {
    Serial.print("HFT:");
    Serial.print("[");
    Serial.print(state.hftButton.adc,DEC);
    Serial.print("] ");
    Serial.print(prevState.hftButton.pressedButton,DEC);
    Serial.print("->");
    Serial.print(state.hftButton.pressedButton,DEC);
    Serial.print(" (");
    Serial.print(state.hftButton.pressedMillis,DEC);
    Serial.print("ms)");
  }
  
  Serial.println(">");
}

void drawDisplay(SpaceshipHvac::State hvac) {
  if (!hvac.connected) {
    display.setPowerSaveMode(true);
    return;
  }
  display.setPowerSaveMode(false);
  display.clearBuffer();

  int8_t leftTemp;
  int8_t rightTemp;
  if (hvac.leftWheel) {
    leftTemp = convertedHvacTemp(hvac.temp1);
    rightTemp = convertedHvacTemp(hvac.temp2);
  } else {
    leftTemp = convertedHvacTemp(hvac.temp2);
    rightTemp = convertedHvacTemp(hvac.temp1);
  }
  display.leftTemp(leftTemp);
  display.leftAuto(hvac.autoMode && leftTemp);
  display.leftTemp(leftTemp);
  display.rightAuto(hvac.autoMode && rightTemp);
  display.rightTemp(rightTemp);
  display.fan(hvac.fanSpeed != 0, hvac.fanSpeed);
  display.ac(hvac.acOn, hvac.acOff);
        
  bool windshield = false;
  bool front = false;
  bool bottom = false;
  switch (hvac.airDirection) {
    case 1:
      front = true;
      break;
    case 2:
      front = true;
      bottom = true;
      break;
    case 3:
      bottom = true;
      break;
    case 4:
      windshield = true;
      bottom = true;
      break;
    case 5:
      windshield = true;
      break;
  }
  display.mode(windshield, front || bottom, front, bottom);

  display.sendBuffer();
}

int8_t convertedHvacTemp(uint8_t value) {
  if (value == SpaceshipHvac::TEMP_HI) {
    return SpaceshipOledDisplay::TEMP_HI;
  } else if (value == SpaceshipHvac::TEMP_LO) {
    return SpaceshipOledDisplay::TEMP_LO;
  } else {
    return value;
  }
}

void getPressedButtons(State &state, State prevState) {
  getButtonsState(state.audioButton, prevState.audioButton, audioButtons);
  getButtonsState(state.hftButton, prevState.hftButton, hftButtons);
}

void getButtonsState(ButtonState &state, ButtonState prevState, ButtonSet buttonsSet) {
  state.adc = buttonsSet.getAdcValue();
  state.rawButton = buttonsSet.getRawPressedButton();
  state.pressedButton = buttonsSet.getPressedButton();
  state.pressedMillis = buttonsSet.getPressedMillis();

  if (state.pressedButton > 0 && state.pressedButton != prevState.pressedButton) {
    state.lastReleasedButton = prevState.pressedButton;
  }
}

void pressButton(State state) {
  if (state.audioButton.pressedButton > 0) {
  switch (state.audioButton.pressedButton)
  {
  case SpaceshipButtons::AUDIO_MODE:
    huButtons.wrTipHold(PioneerButtonGenerator::SOURCE);
    break;
  case SpaceshipButtons::AUDIO_CH_MINUS:
    huButtons.wrTipHold(PioneerButtonGenerator::TRACK_PREV);
    break;
    case SpaceshipButtons::AUDIO_CH_PLUS:
    huButtons.wrTipHold(PioneerButtonGenerator::TRACK_NEXT);
    break;
    case SpaceshipButtons::AUDIO_VOL_MINUS:
    huButtons.wrTipHold(PioneerButtonGenerator::VOL_DOWN);
    break;
    case SpaceshipButtons::AUDIO_VOL_PLUS:
    huButtons.wrTipHold(PioneerButtonGenerator::VOL_UP);
    break;
    default:
    break;
  }
  }
  if (state.hftButton.pressedButton > 0) {
  switch (state.hftButton.pressedButton) {
    case SpaceshipButtons::HFT_BACK:
    huButtons.wrRingHold(PioneerButtonGenerator::MUTE);
    break;
    case SpaceshipButtons::HFT_TALK:
    huButtons.wrRingHold(PioneerButtonGenerator::VOICE);
    break;
    case SpaceshipButtons::HFT_HOOK_OFF:
    huButtons.wrTipHold(PioneerButtonGenerator::BAND);
    break;
    case SpaceshipButtons::HFT_HOOK_ON:
    huButtons.wrRingHold(PioneerButtonGenerator::HANG_UP);
    break;
    default:
    break;
  }
  }

  if ( state.audioButton.pressedButton == 0 && state.hftButton.pressedButton == 0) {
    huButtons.wrRelease();
  }
}

void updateTestState(State state) {
  uint16_t pressThreshold = 20;
  if (state.audioButton.pressedButton == SpaceshipButtons::AUDIO_VOL_MINUS && state.audioButton.pressedMillis < pressThreshold) {
    if (state.hvac.temp1 > SpaceshipHvac::TEMP_LO) {
      state.hvac.temp1--;
    } else {
      state.hvac.temp1 = SpaceshipHvac::TEMP_LO;
    }
  }
  if (state.audioButton.pressedButton == SpaceshipButtons::AUDIO_VOL_PLUS && state.audioButton.pressedMillis < pressThreshold) {
    if (state.hvac.temp1 < SpaceshipHvac::TEMP_HI) {
      state.hvac.temp1++;
    } else {
      state.hvac.temp1 = SpaceshipHvac::TEMP_HI;
    }
  }
  if (state.audioButton.pressedButton == SpaceshipButtons::AUDIO_CH_MINUS && state.audioButton.pressedMillis < pressThreshold) {
    if (state.hvac.fanSpeed > 0) {
      state.hvac.fanSpeed--;
    } else {
      state.hvac.fanSpeed = 0;
    }
  }
  if (state.audioButton.pressedButton == SpaceshipButtons::AUDIO_CH_PLUS && state.audioButton.pressedMillis < pressThreshold) {
    if (state.hvac.fanSpeed < 7) {
      state.hvac.fanSpeed++;
    } else {
      state.hvac.fanSpeed = 0;
    }
  }
  if (state.audioButton.pressedButton == SpaceshipButtons::AUDIO_MODE && state.audioButton.pressedMillis < pressThreshold) {
    if (state.hvac.airDirection < 5) {
      state.hvac.airDirection++;
    } else {
      state.hvac.airDirection = 0;
    }
  }
}