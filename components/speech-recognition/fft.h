#ifndef FFT_H
#define FFT_H

#include <array>
#include <cstdint>
#include <memory>

// #define FIXED_POINT 16
// #include "third_party/kissfft/tools/kiss_fftr.h"

static constexpr uint32_t SAMPLE_RATE{16000};
static constexpr uint32_t BUFFER_SIZE{SAMPLE_RATE};

static constexpr size_t FFT_INPUT_SIZE{128};
static constexpr size_t FFT_STEP_SIZE{FFT_INPUT_SIZE};
static constexpr size_t FFT_WINDOW_SIZE{FFT_INPUT_SIZE * 2};
static constexpr size_t FFT_OUTPUT_SIZE{FFT_WINDOW_SIZE / 2 + 1};
static constexpr size_t FFT_WINDOW_COUNT{124};

static_assert(BUFFER_SIZE / FFT_STEP_SIZE - 1 == FFT_WINDOW_COUNT);

// void transform(const std::array<int16_t, BUFFER_SIZE>& samples);

std::unique_ptr<std::array<std::array<int8_t, FFT_OUTPUT_SIZE>, FFT_WINDOW_COUNT>> kiss_transform(
    const std::array<int16_t, BUFFER_SIZE>& samples);

// void kiss_transform(const std::array<int16_t, BUFFER_SIZE>& samples) {
//     const int nfft = 1024;
//     const int inverse_fft = 0;
//     std::array<uint8_t, 2048> fft_space{};
//     size_t fft_size = fft_space.size();
//     kiss_fftr_cfg fft_config = kiss_fftr_alloc(nfft, inverse_fft, fft_space.data(), &fft_size);
//     assert(static_cast<void*>(fft_config) == static_cast<void*>(fft_space.data()));
//     if (fft_config == nullptr) {
//         puts("Failed to configure FFT");
//         return;
//     }

//     printf("Configured FFT successfully with size &u\n", fft_size);

//     std::array<kiss_fft_cpx, BUFFER_SIZE> fft_data;
//     kiss_fftr(fft_config, samples.data(), fft_data.data());
// }

#endif