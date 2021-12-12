#pragma once

#define SERIAL_SPEED 115200

// the pin must support external interrupts
#define DISPLAY_EMULATOR_CE 2

//Дисплей
#define DISP_CLK SCK
#define DISP_DATA MOSI
#define DISP_CS 10
#define DISP_DC 9

//Разьем E (5P)
#define HVAC_ILL 5 //Синий E1. Регулятор яркости подсветки панели управления (ILL-) - ШИМ
#define HVAC_LIGHTS 4 //Серый E2. Сигнал включенных приборов освещения
#define HVAC_CLOCK 2 //Красный E4. Блок климат-контроля (DISP CLK) 
#define HVAC_DATA 3 // Жёлтый E5. Блок климат-контроля (AC SO) 

//Разьем B (20P)
#define SW_AUDIO A3 //Розовый B7. Выключатель радиоприемника на рулевом колесе (AUDIO REMOTE SW) 
#define SW_HFT A4 // Голубой B8. Блок управления HandsFree (HFT ICON) - Любой свободный провод
//#define SW_AUDIO_GND VCC //Коричневый B6. Выключатель радиоприемника на рулевом колесе (AUDIO REMOTE GND) 

//SWC Pioneer
#define HU_CS 8
#define HU_CLK SCK
#define HU_DATA MOSI


