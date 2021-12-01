#include "display.h"

#include <cstdio>
#include <ctime>

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
    pinMode(DISPLAY_POWER_SWITCH_PIN, OUTPUT);
    digitalWrite(DISPLAY_POWER_SWITCH_PIN, DISPLAY_POWER_OFF);
    xTaskCreatePinnedToCore(Display::taskWrapper, "Display", 8000, this, tskIDLE_PRIORITY + 1,
                            &this->taskHandle, 0);
    this->eventQueue = xQueueCreate(4, sizeof(eventRecord));
}

void Display::enableDisplay() {
    digitalWrite(DISPLAY_POWER_SWITCH_PIN, DISPLAY_POWER_ON);  // turn on power to display

    // Init happens in the task so we don't mess up the thread safety in the display and wire
    // libraries

    delay(100);
    // Wire.begin(DISPLAY_I2C_SDA,DISPLAY_I2C_SCL,400000ul);

    ESP_LOGI("display", "Display size is: %d x %d", this->u8g2.getDisplayWidth(),
             this->u8g2.getDisplayHeight());
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

    this->u8g2.clearBuffer();
    this->u8g2.drawXBM(103, 0, batteryIconWidth, batteryIconHeight, batteryIcon);

    // draw battery level
    this->u8g2.drawBox(106, 3, this->batteryLevel, 10);

    // draw clock
    time_t now;
    struct tm timeInfo;

    time(&now);
    localtime_r(&now, &timeInfo);

    strftime(this->timeBuffer, sizeof(this->timeBuffer), "%a %b %d", &timeInfo);

    this->u8g2.setFont(u8g2_font_7x14B_mr);
    this->u8g2.drawStr(0, 14, this->timeBuffer);

    strftime(this->timeBuffer, sizeof(this->timeBuffer), "%H:%M", &timeInfo);
    this->u8g2.setFont(u8g2_font_inb24_mn);
    int width = u8g2.getStrWidth(this->timeBuffer);
    this->u8g2.drawStr(64 - width / 2, 44, this->timeBuffer);

    this->u8g2.drawHLine(0, 48, 128);

    if (this->eventCount == 0) {
        this->u8g2.setFont(u8g2_font_10x20_mf);
        this->u8g2.drawStr(0, 68, "No events");
    } else {
        int baseline = 80;
        int eventIndex = this->lastEventIndex;
        for (int i = 0; i < eventCount; i++) {
            // draw name of the event;
            uint8_t nameVOffset = (i == 0 ? 12 : 2);
            if (i == 0) {
                this->u8g2.setFont(u8g2_font_10x20_mf);
            } else {
                this->u8g2.setFont(u8g2_font_7x14B_mr);
            }
            this->u8g2.drawStr(0, baseline - nameVOffset, getName(this->events[eventIndex].name));
            this->u8g2.setFont(u8g2_font_7x14B_mr);
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
            int timeWidth = u8g2.getStrWidth(this->timeBuffer);
            this->u8g2.drawStr(127 - timeWidth, baseline - 2, this->timeBuffer);

            this->u8g2.drawHLine(0, baseline, 128);

            baseline += 16;

            eventIndex--;
            if (eventIndex < 0) {
                eventIndex = eventBufferSize - 1;
            }
        }
    }

    // write results to display
    this->u8g2.sendBuffer();
}

void Display::setBatteryLevel(uint8_t level) { this->batteryLevel = level; }

void Display::disableDisplay() { digitalWrite(DISPLAY_POWER_SWITCH_PIN, DISPLAY_POWER_OFF); }

void Display::addEvent(const eventName name) {
    time_t timestamp;
    time(&timestamp);

    eventRecord event{name, timestamp};

    xQueueSendToFront(this->eventQueue, &event, pdMS_TO_TICKS(1000));
}

void Display::taskWrapper(void* display_context) {
    reinterpret_cast<Display*>(display_context)->u8g2.begin();
    reinterpret_cast<Display*>(display_context)->updateDisplay();

    while (true) {
        if (display_context != nullptr) {
            reinterpret_cast<Display*>(display_context)->updateDisplay();
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}