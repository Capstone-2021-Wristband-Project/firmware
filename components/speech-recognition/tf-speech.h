#ifndef TF_SPEECH_H
#define TF_SPEECH_H

#include <array>
#include <cstdint>

#include "fft.h"

void init_tf_speech();
void run_tf_speech(const std::unique_ptr<std::array<std::array<int8_t, FFT_OUTPUT_SIZE>, FFT_WINDOW_COUNT>> spectogram);

#endif