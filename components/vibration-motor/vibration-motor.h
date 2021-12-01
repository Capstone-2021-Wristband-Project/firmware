#pragma once

// #include "Arduino.h"

const int vibrationMotorPin = 33;
const int vibrationMotorPWMChannel = 0;
const int vibrationDutyCycleMapping[] = {0, 12, 24, 36, 48, 60, 72, 84, 96, 108, 120};

class VibrationMotor {
    public:
        void init();
        void setIntensity(int intensity);
        void enable();
        void stop();
    
    private:
        int intensity = 5;
        
};