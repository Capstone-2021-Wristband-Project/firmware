// stdlib Includes
#include <algorithm>
#include <array>
#include <stdio.h>
#include <string>

// ESP-IDF Includes
#include "driver/adc.h"
#include "driver/gpio.h"
#include "esp32/ulp.h"
#include "soc/adc_channel.h"

// FreeRTOS Includes
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Project Includes
#include "pindefs.h"
#include "speech-recognition.h"
#include "ulp_mic.h"

// Arduino because it's easy
#include "Arduino.h"

// display
#include "display.h"
#include "time_utils.h"

extern const uint8_t bin_start[] asm("_binary_ulp_mic_bin_start");
extern const uint8_t bin_end[] asm("_binary_ulp_mic_bin_end");

uint32_t* mic_buffer = &ulp_mic_buffer;

const std::string dateStr = __DATE__;
const std::string timeStr = __TIME__;

Display display;



void start_ulp_program() {
    // ESP_ERROR_CHECK(
    ulp_load_binary(0 /* load address, set to 0 when using default linker scripts */, bin_start,
                    (bin_end - bin_start) / sizeof(uint32_t));
    // );

    ulp_run(&ulp_entry - RTC_SLOW_MEM);
}

extern "C" void app_main(void) {
    initArduino();

    gpio_config_t gpio_config_data{};
    gpio_config_data.pin_bit_mask = 1ULL << PIN_TESTPOINT_1;
    gpio_config_data.mode = GPIO_MODE_OUTPUT;
    gpio_config_data.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpio_config_data.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config_data.intr_type = GPIO_INTR_DISABLE;

    gpio_config(&gpio_config_data);

    // adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_11);
    // adc1_config_width(ADC_WIDTH_BIT_12);
    // adc1_ulp_enable();

    // start_ulp_program();

    // vTaskDelay(1000 / portTICK_PERIOD_MS);

    // for (size_t i = 0; i < 8000; i += 10) {
    //     for (size_t j = 0; j < 10; j++) {
    //         printf("%u ", mic_buffer[i + j] & 0xFFFF);
    //     }
    //     fputc('\n', stdout);
    // }

    display.enableDisplay();

    //init_speech_recognition();
    
    setTimeFromTimeStrings(dateStr, timeStr);

    long count = 0;

    while (true) {
        //run_speech_recognition();

        // TODO: implement this properly.
        display.setBatteryLevel(map(analogRead(PIN_BATTERY_SENSE), 0 ,4096, 0, 15));
        
        display.updateDisplay();
        time_t now;
        char strftime_buf[64];
        struct tm timeinfo;

        time(&now);
        localtime_r(&now, &timeinfo);
        strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
        ESP_LOGI("time", "The current date/time is: %s", strftime_buf);

        if(count == 30) {
            display.addEvent("up");
        }

        if(count == 40) {
            display.addEvent("down");
        }

        if(count == 50) {
            display.addEvent("left");
        }

        if(count == 60) {
            display.addEvent("right");
        }

        if(count == 80) {
            display.addEvent("stop");
        }

        count ++;

        vTaskDelay(100);
    }
}
