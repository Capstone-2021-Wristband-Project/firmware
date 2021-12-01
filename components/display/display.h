#pragma once

#include <array>
#include <ctime>
#include <string>
#include <type_traits>


#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

#include "u8g2.h"


const gpio_num_t DISPLAY_I2C_SCL = GPIO_NUM_27;
const gpio_num_t DISPLAY_I2C_SDA = GPIO_NUM_32;
const gpio_num_t DISPLAY_POWER_SWITCH_PIN = GPIO_NUM_26;
const uint8_t DISPLAY_POWER_ON = 0;
const uint8_t DISPLAY_POWER_OFF = 1;

const unsigned char batteryIcon[] = {
    0x00, 0x00, 0x00, 0xfe, 0xff, 0x1f, 0x02, 0x00, 0x10, 0x02, 0x00, 0x10, 0x02, 0x00, 0x70, 0x02,
    0x00, 0x50, 0x02, 0x00, 0x50, 0x02, 0x00, 0x50, 0x02, 0x00, 0x50, 0x02, 0x00, 0x50, 0x02, 0x00,
    0x50, 0x02, 0x00, 0x70, 0x02, 0x00, 0x10, 0x02, 0x00, 0x10, 0xfe, 0xff, 0x1f, 0x00, 0x00, 0x00};

const uint8_t batteryIconWidth = 24;
const uint8_t batteryIconHeight = 16;

enum class eventName {
    Yes,
    No,
};

struct eventRecord {
    eventName name;
    time_t timestamp;
};

const unsigned int eventBufferSize = 5;

class Display {
   protected:
    // U8G2_SSD1327_EA_W128128_F_HW_I2C u8g2 = U8G2_SSD1327_EA_W128128_F_HW_I2C(U8G2_R0, /* reset=*/
    // U8X8_PIN_NONE, /* scl=*/DISPLAY_I2C_SCL, /* sda=*/DISPLAY_I2C_SDA);

    

    // U8G2_SSD1327_WS_128X128_F_HW_I2C u8g2 = U8G2_SSD1327_WS_128X128_F_HW_I2C(
    //     U8G2_R0, /* reset=*/U8X8_PIN_NONE, /* scl=*/DISPLAY_I2C_SCL, /* sda=*/DISPLAY_I2C_SDA);
    u8g2_t u8g2;
    uint8_t batteryLevel = 0;
    char timeBuffer[64];
    eventRecord events[eventBufferSize];
    int lastEventIndex = 0;
    int eventCount = 0;
    QueueHandle_t eventQueue = nullptr;
    TaskHandle_t taskHandle{};

    // Not thread-safe! Only call from the display task
    void updateDisplay();

   public:
    Display();
    void enableDisplay();
    void setBatteryLevel(uint8_t level);
    void disableDisplay();
    void addEvent(const eventName name);
    static void taskWrapper(void* display_context);
};