#include "display.h"

#include <ctime>
#include <cstdio>

Display::Display(){
    pinMode(DISPLAY_POWER_SWITCH_PIN, OUTPUT);
    digitalWrite(DISPLAY_POWER_SWITCH_PIN, DISPLAY_POWER_OFF);
}

void Display::enableDisplay() {
    digitalWrite(DISPLAY_POWER_SWITCH_PIN, DISPLAY_POWER_ON); // turn on power to display

    delay(100);
    // Wire.begin(DISPLAY_I2C_SDA,DISPLAY_I2C_SCL,400000ul);
    this->u8g2.begin();
    this->updateDisplay();

    ESP_LOGI("display", "Display size is: %d x %d", this->u8g2.getDisplayWidth(), this->u8g2.getDisplayHeight());
}

void Display::updateDisplay(){
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
            if(i == 0) {
               this->u8g2.setFont(u8g2_font_10x20_mf); 
            } else {
               this->u8g2.setFont(u8g2_font_7x14B_mr);
            }
            this->u8g2.drawStr(0, baseline - nameVOffset, this->events[eventIndex].name.c_str());
            this->u8g2.setFont(u8g2_font_7x14B_mr);
            //draw time;
            time_t secondsAgo = now - this->events[eventIndex].timestamp;
            if (secondsAgo < 60) {
                // the ns ago case
                snprintf(timeBuffer, sizeof(timeBuffer), "%lds ago", secondsAgo);
            } else if (secondsAgo < 660) {
                // the nm ago case
                snprintf(timeBuffer, sizeof(timeBuffer), "%ldm ago", secondsAgo/60);
            } else {
                // the HH:MM case
                localtime_r(&(this->events[eventIndex].timestamp), &timeInfo);
                strftime(this->timeBuffer, sizeof(this->timeBuffer), "%H:%M", &timeInfo);
            }

            // draw the time to the screen
            int timeWidth = u8g2.getStrWidth(this->timeBuffer);
            this->u8g2.drawStr(127 - timeWidth, baseline-2, this->timeBuffer);

            this->u8g2.drawHLine(0, baseline, 128);

            baseline += 16;
            
            eventIndex --;
            if (eventIndex < 0) {
                eventIndex = eventBufferSize - 1;
            }
        }
    }

    // write results to display
    this->u8g2.sendBuffer();
}

void Display::setBatteryLevel(uint8_t level){
    this->batteryLevel = level;
}

void Display::disableDisplay() {
    digitalWrite(DISPLAY_POWER_SWITCH_PIN, DISPLAY_POWER_OFF);
}

void Display::addEvent(const std::string &name) {
    time_t timestamp;
    time(&timestamp);

    this->lastEventIndex ++;
    if (this->lastEventIndex >= eventBufferSize) {
        this->lastEventIndex = 0;
    }

    if (this->eventCount < eventBufferSize) {
        this->eventCount++;
    }

    this->events[this->lastEventIndex].name = name;
    this->events[this->lastEventIndex].timestamp = timestamp;
}