#include "vibration-motor.h"

#include "driver/gpio.h"
#include "driver/ledc.h"

#include "ble.h"

constexpr int vibrationMotorPin = 33;
constexpr int vibrationMotorPWMChannel = 0;
constexpr int vibrationDutyCycleMapping[] = {0, 12, 24, 36, 48, 60, 72, 84, 96, 108, 120};

void VibrationMotor::init() {
    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel = {.gpio_num = vibrationMotorPin,
                                          .speed_mode = LEDC_LOW_SPEED_MODE,
                                          .channel = LEDC_CHANNEL_0,
                                          .intr_type = LEDC_INTR_DISABLE,
                                          .timer_sel = LEDC_TIMER_0,
                                          .duty = 0,  // Set duty to 0%
                                          .hpoint = 0};
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));

    // Prepare and then apply the LEDC PWM timer configuration
    ledc_timer_config_t ledc_timer = {.speed_mode = LEDC_LOW_SPEED_MODE,
                                      .duty_resolution = LEDC_TIMER_8_BIT,
                                      .timer_num = LEDC_TIMER_0,
                                      .freq_hz = 5000,  // Set output frequency at 5 kHz
                                      .clk_cfg = LEDC_AUTO_CLK};
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // pinMode(vibrationMotorPin, OUTPUT);
    // ledcSetup(vibrationMotorPWMChannel, 5000, 8);
    // ledcAttachPin(vibrationMotorPin, vibrationMotorPWMChannel);
    // ledcWrite(vibrationMotorPWMChannel, 0);
}

void VibrationMotor::setIntensity(int intensity) { this->intensity = intensity; }

void VibrationMotor::enable() {
    this->intensity = getMotorIntensity();

    ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0,
                                  vibrationDutyCycleMapping[this->intensity]));
    // Update duty to apply the new value
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0));
}

void VibrationMotor::stop() {
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0));
    // Update duty to apply the new value
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0));
}