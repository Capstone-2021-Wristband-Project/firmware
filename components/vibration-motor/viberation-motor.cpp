#include "vibration-motor.h"

void VibrationMotor::init() {
    pinMode(vibrationMotorPin, OUTPUT);
    ledcSetup(vibrationMotorPWMChannel, 5000, 8);
    ledcAttachPin(vibrationMotorPin, vibrationMotorPWMChannel);
    ledcWrite(vibrationMotorPWMChannel, 0);
}

void VibrationMotor::setIntensity(int intensity) {
    this->intensity = intensity;
}

void VibrationMotor::enable() {
    ledcWrite(vibrationMotorPWMChannel, vibrationDutyCycleMapping[this->intensity]);
}

void VibrationMotor::stop() {
    ledcWrite(vibrationMotorPWMChannel, 0);
}