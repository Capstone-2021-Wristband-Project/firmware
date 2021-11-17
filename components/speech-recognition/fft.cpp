#include "fft.h"

// #include "tensorflow/lite/experimental/microfrontend/lib/frontend.h"
// #include "tensorflow/lite/experimental/microfrontend/lib/frontend_util.h"

#define FIXED_POINT 16
#include <algorithm>
#include <cmath>

#include "tensorflow/lite/experimental/microfrontend/lib/kiss_fft_int16.h"

constexpr int samps_to_ms(int samps) { return 1000 * samps / SAMPLE_RATE; }

// void transform(const std::array<int16_t, BUFFER_SIZE>& samples) {
//     FrontendState state;
//     FrontendConfig config;
//     FrontendFillConfigWithDefaults(&config);
//     config.window.size_ms = samps_to_ms(255);
//     config.window.step_size_ms = samps_to_ms(128);

//     int res = FrontendPopulateState(&config, &state, SAMPLE_RATE);
//     printf("res: %d\n", res);
//     assert(res);

//     size_t num_samples_read;

//     size_t processed = 0;
//     while (processed < samples.size()) {
//         FrontendOutput frontend_output =
//             FrontendProcessSamples(&state, samples.data(), samples.size(), &num_samples_read);
//         printf("Processed %d / %d samples.\n", num_samples_read, samples.size());
//         processed += num_samples_read;
//         printf("Total processed samples: %u\n", processed);
//         printf("Output size: %u\n", frontend_output.size);
//     }

// }

static std::array<uint8_t, 4096> fft_space{};

std::array<int8_t, FFT_OUTPUT_SIZE> fft_window(const int16_t* time_data) {
    const int inverse_fft = 0;
    size_t fft_size = fft_space.size();
    kissfft_fixed16::kiss_fftr_cfg fft_config =
        kissfft_fixed16::kiss_fftr_alloc(FFT_WINDOW_SIZE, inverse_fft, fft_space.data(), &fft_size);

    std::fill(fft_space.begin(), fft_space.end(), 0x00);

    assert(static_cast<void*>(fft_config) == static_cast<void*>(fft_space.data()));
    if (fft_config == nullptr) {
        assert(!"Failed to configure FFT");
    }

    printf("Configured FFT successfully with size %u\n", fft_size);
    // Bug????
    auto* fft_data = new std::array<kissfft_fixed16::kiss_fft_cpx, FFT_OUTPUT_SIZE>{};

    kissfft_fixed16::kiss_fftr(fft_config, time_data, fft_data->data());

    std::array<int8_t, FFT_OUTPUT_SIZE> magnitude;

    for (size_t i = 0; i < (*fft_data).size(); i++) {
        // Bug????
        auto x = (*fft_data)[i];
        magnitude[i] = sqrtf((x.i * x.i) + (x.r * x.r));
    }

    delete fft_data;

    return magnitude;
}

std::unique_ptr<std::array<std::array<int8_t, FFT_OUTPUT_SIZE>, FFT_WINDOW_COUNT>> kiss_transform(
    const std::array<int16_t, BUFFER_SIZE>& samples) {
    auto spectogram =
        std::unique_ptr<std::array<std::array<int8_t, FFT_OUTPUT_SIZE>, FFT_WINDOW_COUNT>>(
            new std::array<std::array<int8_t, FFT_OUTPUT_SIZE>, FFT_WINDOW_COUNT>);
            
    for (size_t slice = 0; slice < FFT_WINDOW_COUNT; slice++) {
        size_t offset = slice * FFT_STEP_SIZE;

        auto slice_start = &samples[offset];
        (*spectogram)[slice] = fft_window(slice_start);
    }

    return spectogram;
}
