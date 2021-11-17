// stdlib Includes
#include <algorithm>
#include <array>
#include <stdio.h>

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
#include "ulp_mic.h"

extern const uint8_t bin_start[] asm("_binary_ulp_mic_bin_start");
extern const uint8_t bin_end[] asm("_binary_ulp_mic_bin_end");

uint32_t* mic_buffer = &ulp_mic_buffer;

void start_ulp_program() {
    // ESP_ERROR_CHECK(
    ulp_load_binary(0 /* load address, set to 0 when using default linker scripts */, bin_start,
                    (bin_end - bin_start) / sizeof(uint32_t));
    // );

    ulp_run(&ulp_entry - RTC_SLOW_MEM);
}

static std::array<uint16_t, 40000> buf{};
static size_t i{0};
void record_sample(void*) {
    if (i < buf.size()) {
        buf[i] = adc1_get_raw(ADC1_GPIO35_CHANNEL);
        i++;
    }
}

void scale_buffer(std::array<uint16_t, 40000>& buf) {
    auto max = *std::max_element(buf.begin(), buf.end());
    auto min = *std::min_element(buf.begin(), buf.end());

    for (auto& samp : buf) {
        uint32_t scratch = samp;
        scratch -= min;
        scratch *= 256;
        scratch /= (max - min);
        samp = static_cast<uint16_t>(scratch);
    }
}

extern "C" void app_main(void) {
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

    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_GPIO35_CHANNEL, ADC_ATTEN_DB_6);
    // static_assert(ADC1_CHANNEL_6_GPIO_NUM == PIN_INTERNAL_MICROPHONE);

    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &record_sample,
        /* name is optional, but may help identify the timer when debugging */
        .name = "mic"};
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));

    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, 1000000 / 8000));

    vTaskDelay(pdMS_TO_TICKS(6000));
    scale_buffer(buf);
    for (auto samp : buf) {
        printf("%u\n", samp);
    }

    while (true) {
        vTaskDelay(1);
    }
}
