#pragma once

// #include "Arduino.h"



class VibrationMotor {
    public:
        void init();
        void setIntensity(int intensity);
        void enable();
        void stop();
    
    private:
        int intensity = 5;
        
};