#pragma once

#include "Arduino.h"
#include "U8g2lib.h"
#include "Wire.h"
#include <ctime>
#include <string>

const uint8_t DISPLAY_I2C_SCL = 27;
const uint8_t DISPLAY_I2C_SDA = 32;
const uint8_t DISPLAY_POWER_SWITCH_PIN = 26;
const uint8_t DISPLAY_POWER_ON = LOW;
const uint8_t DISPLAY_POWER_OFF = HIGH;

const unsigned char batteryIcon [] =
{
   0x00, 0x00, 0x00, 0xfe, 0xff, 0x1f, 0x02, 0x00, 0x10, 0x02, 0x00, 0x10,
   0x02, 0x00, 0x70, 0x02, 0x00, 0x50, 0x02, 0x00, 0x50, 0x02, 0x00, 0x50,
   0x02, 0x00, 0x50, 0x02, 0x00, 0x50, 0x02, 0x00, 0x50, 0x02, 0x00, 0x70,
   0x02, 0x00, 0x10, 0x02, 0x00, 0x10, 0xfe, 0xff, 0x1f, 0x00, 0x00, 0x00 
};

const uint8_t batteryIconWidth = 24;
const uint8_t batteryIconHeight = 16;

struct eventRecord {
    std::string name;
    time_t timestamp;
};

const unsigned int eventBufferSize = 5;

class Display{
    protected:
        //U8G2_SSD1327_EA_W128128_F_HW_I2C u8g2 = U8G2_SSD1327_EA_W128128_F_HW_I2C(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* scl=*/DISPLAY_I2C_SCL, /* sda=*/DISPLAY_I2C_SDA);
        U8G2_SSD1327_WS_128X128_F_HW_I2C u8g2 = U8G2_SSD1327_WS_128X128_F_HW_I2C(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* scl=*/DISPLAY_I2C_SCL, /* sda=*/DISPLAY_I2C_SDA);
        uint8_t batteryLevel = 0;
        char timeBuffer[64];
        eventRecord events[eventBufferSize];
        int lastEventIndex = 0;
        int eventCount = 0;
    public:
        Display();
        void enableDisplay();
        void setBatteryLevel(uint8_t level);
        void updateDisplay();
        void disableDisplay();
        void addEvent(const std::string &name);
};