#include "speech-recognition.h"

#include <algorithm>

#include "driver/adc.h"
#include "fft.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/adc_channel.h"
#include "tf-speech.h"

static std::array<int16_t, 16000> buf{};
static volatile size_t i{0};
void record_sample(void*) {
    if (i < buf.size()) {
        buf[i] = adc1_get_raw(ADC1_GPIO35_CHANNEL);
        i++;
    }
}

// void scale_buffer(std::array<int16_t, 16000>& buf) {
//     auto max = *std::max_element(buf.begin(), buf.end());
//     auto min = *std::min_element(buf.begin(), buf.end());

//     for (auto& samp : buf) {
//         uint32_t scratch = samp;
//         scratch -= min;
//         scratch *= 256;
//         scratch /= (max - min);
//         samp = static_cast<uint16_t>(scratch);
//     }
// }

void init_mic() {
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_GPIO35_CHANNEL, ADC_ATTEN_DB_6);
    // static_assert(ADC1_CHANNEL_6_GPIO_NUM == PIN_INTERNAL_MICROPHONE);

    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &record_sample,
        /* name is optional, but may help identify the timer when debugging */
        .name = "mic"};
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));

    puts("Recording");
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, 1000000 / 8000));

    // vTaskDelay(pdMS_TO_TICKS(6000));
    // scale_buffer(buf);
    // for (auto samp : buf) {
    //     printf("%u\n", samp);
    // }
}

void init_speech_recognition() {
    init_mic();
    init_tf_speech();
}

void run_speech_recognition() {
    if (i >= 16000) {
        puts("Done recording");
        auto spectogram = kiss_transform(buf);
        run_tf_speech(std::move(spectogram));
    }
}
