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
#include "src/buttons/PioneerSimpleButtons.h"

const uint16_t LOOP_PERIOD_MS = 50;
const uint32_t LOOP_PERIOD_US = LOOP_PERIOD_MS * 1000L;

SpaceshipHvac hvac(HVAC_CLOCK, HVAC_DATA);
SpaceshipOledDisplay display(U8G2_SSD1309_128X64_NONAME0_F_4W_HW_SPI(U8G2_R0, DISP_CS, DISP_DC));

Pin lightSignal = Pin(HVAC_LIGHTS);
ButtonSet audioButtons = SpaceshipButtons::audioSwitchMaster(SW_AUDIO);
PioneerSimpleButtons huButtons = PioneerSimpleButtons(HU_CS);

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
  //huButtons.updateTimer();
  audioButtons.updateButtons();

  // Display
  //hvac.readState(state.hvac);

  drawDisplay(state.hvac);
  printHvacSerial(state.hvac, prevState.hvac);

  if (lightSignal.read()) {
    display.setBrightness(255);
  } else {
    display.setBrightness(0);
  }

  // Buttons
  getPressedButtons(state, prevState);
  //pressTestButton(state);
  pressButton(state, prevState);
  //printButtonSerial(state, prevState);

  //updateTestState(state);

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

  state.temp1 = SpaceshipHvac::TEMP_LO;
  state.temp2 = SpaceshipHvac::TEMP_HI;
  state.fanSpeed = 5;
  state.airDirection = 4;
}

void printHvacSerial(SpaceshipHvac::State hvac, SpaceshipHvac::State prevHvac) {
  if (
    hvac.connected == prevHvac.connected &&
    hvac.temp1 == prevHvac.temp1 &&
    hvac.temp2 == prevHvac.temp2 &&
    hvac.autoMode == prevHvac.autoMode &&
    hvac.acOn == prevHvac.acOn &&
    hvac.acOff == prevHvac.acOff &&
    hvac.fanSpeed == prevHvac.fanSpeed &&
    hvac.airDirection == prevHvac.airDirection
  ) {
    return;
  }

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

  if (!pressedAudio) {
    auto audioAdcOpenDiff = audioButtons.openAdcValue() - state.audioButton.adc;
    bool unknownAudio = state.audioButton.pressedButton == 0 && abs(audioAdcOpenDiff) > audioButtons.tolerance;

    
    if (unknownAudio) {
      Serial.print("<");
      Serial.print("Audio Missed:");
      Serial.print("ADC=");
      Serial.print(state.audioButton.adc, DEC);
      Serial.println(">");
    }
    return;
  } else {
    Serial.print("<");
    Serial.print("Audio:");

    if (state.audioButton.pressedButton == SpaceshipButtons::AUDIO_MODE) {
      Serial.print(" MODE");
    } else if (state.audioButton.pressedButton == SpaceshipButtons::AUDIO_CH_PLUS) {
      Serial.print(" CH+");
    } else if (state.audioButton.pressedButton == SpaceshipButtons::AUDIO_CH_MINUS) {
      Serial.print(" CH-");
    } else if (state.audioButton.pressedButton == SpaceshipButtons::AUDIO_VOL_PLUS) {
      Serial.print(" VOL+");
    } else if (state.audioButton.pressedButton == SpaceshipButtons::AUDIO_VOL_MINUS) {
      Serial.print(" VOL-");
    } 

    Serial.print(" [");
    Serial.print(state.audioButton.adc,DEC);
    Serial.print("] ");
    Serial.print(prevState.audioButton.pressedButton,DEC);
    Serial.print("->");
    Serial.print(state.audioButton.pressedButton,DEC);
    Serial.print(" (");
    Serial.print(state.audioButton.pressedMillis,DEC);
    Serial.print("ms)");
    Serial.println(">");
  }
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

uint8_t huValue = 0;
void pressTestButton(State state) {
  switch (state.audioButton.pressedButton)
  {
    case SpaceshipButtons::AUDIO_VOL_PLUS:
    Serial.print("Output Resistance: ");
    Serial.println(huValue, DEC);
    huButtons.sendValue(++huValue);
    break;
    case SpaceshipButtons::AUDIO_VOL_MINUS:
    Serial.print("Output Resistance: ");
    Serial.println(huValue, DEC);
    huButtons.sendValue(--huValue);
    break;
    default:
    //huButtons.releaseButton();
    break;
  }
}

void pressButton(State state, State prevState) {
  if (state.audioButton.pressedButton == prevState.audioButton.pressedButton) {
    return;
  }

  switch (state.audioButton.pressedButton)
  {
    case SpaceshipButtons::AUDIO_MODE:
    Serial.println("Pioneer: Source");
    huButtons.pressButton(PioneerSimpleButtons::SOURCE);
    break;
    case SpaceshipButtons::AUDIO_CH_MINUS:
    Serial.println("Pioneer: Track -");
    huButtons.pressButton(PioneerSimpleButtons::TRACK_PREV);
    break;
    case SpaceshipButtons::AUDIO_CH_PLUS:
    Serial.println("Pioneer: Track +");
    huButtons.pressButton(PioneerSimpleButtons::TRACK_NEXT);
    break;
    case SpaceshipButtons::AUDIO_VOL_MINUS:
    Serial.println("Pioneer: Volume -");
    huButtons.pressButton(PioneerSimpleButtons::VOL_DOWN);
    break;
    case SpaceshipButtons::AUDIO_VOL_PLUS:
    Serial.println("Pioneer: Volume +");
    huButtons.pressButton(PioneerSimpleButtons::VOL_UP);
    break;
    default:
    huButtons.releaseButton();
    break;
  }
}

void updateTestState(State &state) {
  uint16_t pressThreshold = 120;

  // if (state.audioButton.pressedButton > 0) {
  //   Serial.print("Pressed Button: ");
  //   Serial.print(state.audioButton.pressedButton, DEC);
  //   Serial.print(" [");
  //   Serial.print(state.audioButton.pressedMillis, DEC);
  //   Serial.println(" ms]");
  // }

  if (state.audioButton.pressedButton == SpaceshipButtons::AUDIO_VOL_MINUS && state.audioButton.pressedMillis < pressThreshold) {
    Serial.print("STATE: TEMP- - ");
    Serial.println(state.hvac.temp1, DEC);
    if (state.hvac.temp1 == SpaceshipHvac::TEMP_HI) {
      state.hvac.temp1 = 28;
    } else if (state.hvac.temp1 > SpaceshipHvac::TEMP_LO) {
      state.hvac.temp1--;
    } else {
      state.hvac.temp1 = SpaceshipHvac::TEMP_LO;
    }
  }
  if (state.audioButton.pressedButton == SpaceshipButtons::AUDIO_VOL_PLUS && state.audioButton.pressedMillis < pressThreshold) {
    Serial.print("STATE: TEMP+ - ");
    Serial.println(state.hvac.temp1, DEC);
    if (state.hvac.temp1 == SpaceshipHvac::TEMP_LO) {
      state.hvac.temp1 = 16;
    } else if (state.hvac.temp1 < SpaceshipHvac::TEMP_HI) {
      state.hvac.temp1++;
    } else {
      state.hvac.temp1 = SpaceshipHvac::TEMP_HI;
    }
  }
  if (state.audioButton.pressedButton == SpaceshipButtons::AUDIO_CH_MINUS && state.audioButton.pressedMillis < pressThreshold) {
    Serial.println("STATE: FAN-");
    if (state.hvac.fanSpeed > 0) {
      state.hvac.fanSpeed--;
    } else {
      state.hvac.fanSpeed = 0;
    }
  }
  if (state.audioButton.pressedButton == SpaceshipButtons::AUDIO_CH_PLUS && state.audioButton.pressedMillis < pressThreshold) {
    Serial.println("STATE: FAN+");
    if (state.hvac.fanSpeed < 7) {
      state.hvac.fanSpeed++;
    } else {
      state.hvac.fanSpeed = 0;
    }
  }
  if (state.audioButton.pressedButton == SpaceshipButtons::AUDIO_MODE && state.audioButton.pressedMillis < pressThreshold) {
    Serial.println("STATE: AIR MODE");
    if (state.hvac.airDirection < 5) {
      state.hvac.airDirection++;
    } else {
      state.hvac.airDirection = 0;
    }
  }
}