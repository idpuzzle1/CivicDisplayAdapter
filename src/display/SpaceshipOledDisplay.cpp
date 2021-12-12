#include "SpaceshipOledDisplay.h"

// 'vent', 16x14px
const unsigned char bitmap_vent [] PROGMEM = {
	B00000000, B00001110,
  B00000000, B00011111,
  B10001110, B00011111,
  B10011111, B00011111,
  B11011111, B00001111,
  B11011111, B00000001,
  B01111110, B00000111,
  B00111100, B00011110,
  B01110000, B00111111,
  B11000000, B01111101,
  B11111000, B01111101,
  B11111100, B01111100,
  B11111100, B00111000,
  B01111000, B00000000,
  B01110000, B00000000,
};

//'wind_air', 32x21px
const unsigned char bitmap_wind_air [] PROGMEM = {
	B00000000, B11111000, B00000000, B00000000, 
  B11100000, B11111111, B01111111, B00000000, 
  B11111110, B10001111, B11111111, B00000011, 
  B00111111, B00000000, B11000000, B00000111, 
	B00000011, B00000000, B00000000, B00000110, 
  B00000011, B00000000, B00000000, B00000110, 
  B00000011, B00000000, B00000000, B00000110, 
  B10000111, B00100000, B00001000, B00000111, 
  B11000110, B01110001, B00011100, B00000011, 
  B11100110, B11111011, B00111110, B00000011, 
  B10000110, B00100000, B00001000, B00000011, 
  B10000110, B00100000, B00001000, B00000011, 
  B10000110, B00100000, B00001000, B00000011, 
  B10000110, B00100000, B00001000, B00000011, 
  B10001110, B00100000, B10001000, B00000011, 
  B01001100, B00010000, B10000100, B00000001, 
  B01001100, B00010000, B10000100, B00000001, 
  B01011100, B00010000, B11000100, B00000001, 
  B01011000, B11010111, B11110101, B00000000, 
  B01000000, B00010000, B00000100, B00000000, 
  B01000000, B00010000, B00000100, B00000000, 
};

// 'air_direction', 48x51px
const unsigned char bitmap_air_direction [] PROGMEM = {
	B00000000, B00000000, B00000000, B10000000, B00011111, B00000000,
  B00000000, B00000000, B00000000, B11100000, B01111111, B00000000,
  B00000000, B00000000, B00000000, B11110000, B11111111, B00000000,
  B00000000, B00000000, B00000000, B11110000, B11111111, B00000000,
  B00000000, B00000000, B00000000, B11111000, B11111111, B00000001,
  B00000000, B00000000, B00000000, B11111000, B11111111, B00000001,
  B00000000, B00000000, B00000000, B11111000, B11111111, B00000001,
  B00000000, B00000000, B00000000, B11111000, B11111111, B00000001,
  B00000000, B00000000, B00000000, B11111000, B11111111, B00000001,
  B00000000, B00000000, B00000000, B11110000, B11111111, B00000001,
  B00000000, B00000000, B00000000, B11110000, B11111111, B00000000,
  B00000000, B00000000, B00000000, B11110000, B11111111, B00000000,
  B00000000, B00000000, B00000000, B11100000, B01111111, B00000000,
  B00000000, B00000000, B00000000, B10000000, B00011111, B00000000,
  B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B11100000, B00000111, B00000000,
  B00000000, B00000000, B00000000, B11111000, B00011111, B00000000,
  B00000000, B00000000, B00000000, B11111100, B00111111, B00000000,
  B00000000, B00000000, B00000000, B11111100, B00111111, B00000000,
  B00000000, B00000000, B00000000, B11111110, B01111111, B00000000,
  B00000000, B00000000, B00000000, B11111110, B01111111, B00000000,
  B00000000, B00000000, B00000000, B11111111, B01111111, B00000000,
  B00000000, B00000000, B00000000, B11111111, B01111111, B00000000,
  B00000000, B00000000, B00000000, B11111111, B01111111, B00000000,
  B00000000, B00000000, B00000000, B11111111, B01111111, B00000000,
  B00000000, B00000000, B10000000, B11111111, B01111111, B00000000,
  B00000000, B00000000, B10000000, B11111111, B00111111, B00000000,
  B00000000, B00000000, B10000000, B11111111, B00111111, B00000000,
  B00000000, B00000000, B10000000, B11111111, B00011111, B00000000,
  B00000000, B00000000, B11000000, B11111111, B00011111, B00000000,
  B00000000, B00011111, B11000000, B11111111, B00001111, B00000000,
  B10000000, B01111111, B11000000, B11111111, B00001111, B00000000,
  B11000000, B11111111, B11000011, B11111111, B00001111, B00000000,
  B11100000, B11111111, B11111111, B11111111, B00000111, B00000000,
  B11100000, B11111111, B11111111, B11111111, B00000111, B00000000,
  B11110000, B11111111, B11111111, B11111111, B00000111, B00000000,
  B11110000, B11111111, B11111111, B11111111, B00000011, B00000000,
  B11111000, B11111111, B11111111, B11111111, B00000011, B00000000,
  B11111000, B11111111, B11111111, B11111111, B00000011, B00000000,
  B11111000, B11101111, B11111111, B11111111, B00000011, B00000000,
  B11111100, B10000111, B11111111, B11111111, B00000001, B00000000,
  B11111100, B00000111, B11111110, B11111111, B00000001, B00000000,
  B11111110, B00000111, B11111000, B11111111, B00000000, B00000000,
  B11111110, B00000011, B11100000, B11111111, B00000000, B00000000,
  B11111111, B00000011, B10000000, B01111111, B00000000, B00000000,
  B11111111, B00000001, B00000000, B00011110, B00000000, B00000000,
  B11111111, B00000001, B00000000, B00000000, B00000000, B00000000,
  B11111111, B00000000, B00000000, B00000000, B00000000, B00000000,
  B11111111, B00000000, B00000000, B00000000, B00000000, B00000000,
  B01111110, B00000000, B00000000, B00000000, B00000000, B00000000,
  B00111100, B00000000, B00000000, B00000000, B00000000, B00000000,
};

// 'air_air_front', 16x16px
const unsigned char bitmap_air_front [] PROGMEM = {
  B00000000, B00000000,
  B00000000, B00000000,
  B00000000, B00000110,
  B00000000, B00001110,
  B00000000, B00011110,
  B00000000, B00111110,
  B11111111, B01111111,
  B11111111, B11111111,
  B11111111, B11111111,
  B11111111, B01111111,
  B00000000, B00111110,
  B00000000, B00011110,
  B00000000, B00001110,
  B00000000, B00000110,
  B00000000, B00000000,
  B00000000, B00000000,
};

// 'air_air_down', 16x16px
const unsigned char bitmap_air_down [] PROGMEM = {
  B11000000, B00000011,
  B11000000, B00000011,
  B11000000, B00000011,
  B11000000, B00000011,
  B11000000, B00000011,
  B11000000, B00000011,
  B11000000, B00000011,
  B11000000, B00000011,
  B11000000, B00000011,
  B11111100, B00111111,
  B11111100, B00111111,
  B11111000, B00011111,
  B11110000, B00001111,
  B11100000, B00000111,
  B11000000, B00000011,
  B10000000, B00000001,
};

// Array of all bitmaps for convenience. (Total bytes used to store images in PROGMEM = 480)
const int bitmap_allArray_LEN = 3;
const unsigned char* bitmap_allArray[3] = {
	bitmap_air_direction,
	bitmap_vent,
	bitmap_wind_air
};

void SpaceshipOledDisplay::begin() {
    display.begin();
}

void SpaceshipOledDisplay::clearBuffer() {
    display.clearBuffer();
}

void SpaceshipOledDisplay::sendBuffer() {
    display.sendBuffer();
}

void SpaceshipOledDisplay::setPowerSaveMode(bool mode) {
    display.setPowerSave(mode);
}

void SpaceshipOledDisplay::setBrightness(uint8_t value) {
    display.setContrast(value);
}

void SpaceshipOledDisplay::leftTemp(int8_t value) {
    leftTemp(value,value != 0);
}

void SpaceshipOledDisplay::leftTemp(int8_t value, bool frame) {
    if (value != 0) {
        int textLeft = 6;
        int textBottom = 46;
        display.setFont(u8g2_font_logisoso32_tr);
        if (value == TEMP_HI || value > 99) {
            display.setCursor(textLeft,textBottom);
            display.print(F("Hi"));
        } else if (value == TEMP_LO || value < 0) {
            display.setCursor(textLeft,textBottom);
            display.print(F("Lo"));
        } else {
            char buffer[1];
            itoa(value,buffer,10); 
            display.drawStr(textLeft,textBottom,buffer);
        }
    }

    if (frame) {
        //Temp Frame
        display.drawRFrame(0,4,52,47,4);
        display.drawRFrame(1,5,50,45,3);

        //Temp text clear undercover
        display.setDrawColor(0);
        display.drawBox(9,5,35,2);
        display.setDrawColor(1);

        //Temp text
        display.setFont(u8g2_font_9x15B_mr);
        display.setCursor(8,10);
        display.print(F("TEMP"));
    }
}

void SpaceshipOledDisplay::leftAuto(bool value) {
    if (value) {
        display.setFont(u8g2_font_9x15B_mr);
        display.setCursor(8,64);
        display.print(F("AUTO"));
    }
}
    
void SpaceshipOledDisplay::rightTemp(int8_t value) {

}

void SpaceshipOledDisplay::rightTemp(int8_t value, bool frame) {

}

void SpaceshipOledDisplay::rightAuto(bool value) {

}

void SpaceshipOledDisplay::rightFull(bool value) {

}
    
void SpaceshipOledDisplay::fan(bool icon, uint8_t speed) {
    u8g2_uint_t startX = 56;
    if (icon) {
        display.drawXBMP(startX,24,16,14,bitmap_vent);
    }

    if (speed == 0) {
        return;
    }

    u8g2_uint_t max = 7;

    u8g2_uint_t startY = 30;
    u8g2_uint_t lowestY = 47;

    u8g2_uint_t minHeight = 7;
    u8g2_uint_t width = 3;
    u8g2_uint_t step = 2;
    u8g2_uint_t space = 1;

    for (size_t i = 0; i < speed; i++) {
        display.setDrawColor(1);

        u8g2_uint_t vOffset = (step * (max - i));

        u8g2_uint_t currentX = startX + (width * i) + (space * i);
        u8g2_uint_t currentY = startY + vOffset;

        u8g2_uint_t height = minHeight + (step * i);

        display.drawBox(currentX, currentY, width, height);

        display.setDrawColor(0);

        display.drawPixel(currentX,currentY);

        u8g2_uint_t currentBottom = currentY + height;
        display.drawBox(currentX, currentBottom - 3, width, 1);
    }
    display.setDrawColor(1);
}

void SpaceshipOledDisplay::ac(bool on, bool off) {
    if (on || off) {
        display.setFont(u8g2_font_9x15B_mr);
        display.setCursor(56,64);
    }
    if (on) {
        display.print(F("A/C ON"));
    } else if (off) {
        display.print(F("A/C OFF"));
    } 
}

void SpaceshipOledDisplay::mode(bool windshield, bool human, bool front, bool bottom) {
    if (human) {
        display.drawXBMP(86,0,48,51,bitmap_air_direction);
    }
    if (windshield) {
        display.drawXBMP(56,0,32,21,bitmap_wind_air);
    }
    if (front) {
        display.drawXBMP(95,3,16,16,bitmap_air_front);
    }
    if (bottom) {
        display.drawXBMP(84,14,16,16,bitmap_air_down);
    }
}