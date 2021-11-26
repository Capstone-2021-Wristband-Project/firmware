#include "display.h"

#include <ctime>

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
    // TODO: actually implement getting the battery level.
    this->u8g2.drawBox(106, 3, this->batteryLevel, 10);

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

    // write results to display
    this->u8g2.sendBuffer();
}

void Display::setBatteryLevel(uint8_t level){
    this->batteryLevel = level;
}

void Display::disableDisplay() {
    digitalWrite(DISPLAY_POWER_SWITCH_PIN, DISPLAY_POWER_OFF);
}