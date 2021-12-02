#include "display.h"

#include <cstdio>
#include <ctime>

#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/task.h"
#include "u8g2_esp32_hal.h"

const char* getName(eventName event) {
    switch (event) {
        case eventName::Yes:
            return "Yes";
        case eventName::No:
            return "No";
        default:
            return "Unknown";
    }
}

Display::Display() {
    gpio_set_direction(DISPLAY_POWER_SWITCH_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(DISPLAY_POWER_SWITCH_PIN, DISPLAY_POWER_OFF);
    // pinMode(DISPLAY_POWER_SWITCH_PIN, OUTPUT);
    // digitalWrite(DISPLAY_POWER_SWITCH_PIN, DISPLAY_POWER_OFF);

    u8g2_Setup_ssd1327_i2c_ws_128x128_f(&this->u8g2, U8G2_R0, u8g2_esp32_i2c_byte_cb,
                                        u8g2_esp32_gpio_and_delay_cb);

    xTaskCreatePinnedToCore(Display::taskWrapper, "Display", 8000, this, tskIDLE_PRIORITY + 1,
                            &this->taskHandle, 0);
    this->eventQueue = xQueueCreate(4, sizeof(eventRecord));
}

void Display::enableDisplay() {
    gpio_set_level(DISPLAY_POWER_SWITCH_PIN, DISPLAY_POWER_ON);  // turn on power to display

    // Init happens in the task so we don't mess up the thread safety in the display and wire
    // libraries

    vTaskDelay(pdMS_TO_TICKS(100));
    // Wire.begin(DISPLAY_I2C_SDA,DISPLAY_I2C_SCL,400000ul);

    ESP_LOGI("display", "Display size is: %d x %d", this->u8g2.width, this->u8g2.height);
}

void Display::updateDisplay() {
    eventRecord event{};
    if (xQueueReceive(this->eventQueue, &event, 0)) {
        this->lastEventIndex++;
        if (this->lastEventIndex >= eventBufferSize) {
            this->lastEventIndex = 0;
        }

        if (this->eventCount < eventBufferSize) {
            this->eventCount++;
        }

        this->events[this->lastEventIndex] = event;
    }

    u8g2_ClearBuffer(&(this->u8g2));
    u8g2_DrawXBM(&(this->u8g2), 103, 0, batteryIconWidth, batteryIconHeight, batteryIcon);

    // draw battery level
    u8g2_DrawBox(&(this->u8g2), 106, 3, this->batteryLevel, 10);

    // draw clock
    time_t now;
    struct tm timeInfo;

    time(&now);
    localtime_r(&now, &timeInfo);

    strftime(this->timeBuffer, sizeof(this->timeBuffer), "%a %b %d", &timeInfo);

    u8g2_SetFont(&(this->u8g2), u8g2_font_7x14B_mr);
    u8g2_DrawStr(&(this->u8g2), 0, 14, this->timeBuffer);

    strftime(this->timeBuffer, sizeof(this->timeBuffer), "%H:%M", &timeInfo);
    u8g2_SetFont(&(this->u8g2), u8g2_font_inb24_mn);
    int width = u8g2_GetStrWidth(&(this->u8g2), this->timeBuffer);
    u8g2_DrawStr(&(this->u8g2), 64 - width / 2, 44, this->timeBuffer);

    u8g2_DrawHLine(&(this->u8g2), 0, 48, 128);

    if (this->eventCount == 0) {
        u8g2_SetFont(&(this->u8g2), u8g2_font_10x20_mf);
        u8g2_DrawStr(&(this->u8g2), 0, 68, "No events");
    } else {
        int baseline = 80;
        int eventIndex = this->lastEventIndex;
        for (int i = 0; i < eventCount; i++) {
            // draw name of the event;
            uint8_t nameVOffset = (i == 0 ? 12 : 2);
            if (i == 0) {
                u8g2_SetFont(&(this->u8g2), u8g2_font_10x20_mf);
            } else {
                u8g2_SetFont(&(this->u8g2), u8g2_font_7x14B_mr);
            }
            u8g2_DrawStr(&(this->u8g2), 0, baseline - nameVOffset,
                         getName(this->events[eventIndex].name));
            u8g2_SetFont(&(this->u8g2), u8g2_font_7x14B_mr);
            // draw time;
            time_t secondsAgo = now - this->events[eventIndex].timestamp;
            if (secondsAgo < 60) {
                // the ns ago case
                snprintf(timeBuffer, sizeof(timeBuffer), "%lds ago", secondsAgo);
            } else if (secondsAgo < 660) {
                // the nm ago case
                snprintf(timeBuffer, sizeof(timeBuffer), "%ldm ago", secondsAgo / 60);
            } else {
                // the HH:MM case
                localtime_r(&(this->events[eventIndex].timestamp), &timeInfo);
                strftime(this->timeBuffer, sizeof(this->timeBuffer), "%H:%M", &timeInfo);
            }

            // draw the time to the screen
            int timeWidth = u8g2_GetStrWidth(&(this->u8g2), this->timeBuffer);
            u8g2_DrawStr(&(this->u8g2), 127 - timeWidth, baseline - 2, this->timeBuffer);

            u8g2_DrawHLine(&(this->u8g2), 0, baseline, 128);

            baseline += 16;

            eventIndex--;
            if (eventIndex < 0) {
                eventIndex = eventBufferSize - 1;
            }
        }
    }

    // write results to display
    u8g2_SendBuffer(&(this->u8g2));
}

void Display::setBatteryLevel(uint8_t level) { this->batteryLevel = level; }

void Display::disableDisplay() { gpio_set_level(DISPLAY_POWER_SWITCH_PIN, DISPLAY_POWER_OFF); }

void Display::addEvent(const eventName name) {
    time_t timestamp;
    time(&timestamp);

    eventRecord event{name, timestamp};

    xQueueSendToFront(this->eventQueue, &event, pdMS_TO_TICKS(1000));
}

void Display::taskWrapper(void* display_context) {
    u8g2_t* u8g2 = &(reinterpret_cast<Display*>(display_context)->u8g2);

    u8g2_esp32_hal_t u8g2_hal{
        GPIO_NUM_NC,     GPIO_NUM_NC,
        DISPLAY_I2C_SDA,  // data for I²C
        DISPLAY_I2C_SCL,  // clock for I²C
        GPIO_NUM_NC,     GPIO_NUM_NC, GPIO_NUM_NC,
    };

    u8g2_esp32_hal_init(u8g2_hal);
    vTaskDelay(pdMS_TO_TICKS(100));
    u8x8_SetI2CAddress(&(u8g2->u8x8), 0x3C << 1);

    u8g2_InitDisplay(u8g2);
    u8g2_ClearDisplay(u8g2);
    u8g2_SetPowerSave(u8g2, 0);

    reinterpret_cast<Display*>(display_context)->updateDisplay();

    while (true) {
        if (display_context != nullptr) {
            reinterpret_cast<Display*>(display_context)->updateDisplay();
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}