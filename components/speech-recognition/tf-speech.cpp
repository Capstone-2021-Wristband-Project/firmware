#include "tf-speech.h"

#include <cstdio>

#include "fft.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "model.h"
#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/system_setup.h"
#include "tensorflow/lite/schema/schema_generated.h"

// Globals, used for compatibility with Arduino-style sketches.
namespace {

tflite::ErrorReporter* error_reporter = nullptr;
const tflite::Model* model = nullptr;
tflite::MicroInterpreter* interpreter = nullptr;

TfLiteTensor* input = nullptr;
TfLiteTensor* output = nullptr;

constexpr int kTensorArenaSize = 64000;
uint8_t tensor_arena[kTensorArenaSize];
}  // namespace

void init_tf_speech() {
    // Set up logging. Google style is to avoid globals or statics because of
    // lifetime uncertainty, but since this has a trivial destructor it's okay.
    // NOLINTNEXTLINE(runtime-global-variables)
    static tflite::MicroErrorReporter micro_error_reporter;
    error_reporter = &micro_error_reporter;

    // Map the model into a usable data structure. This doesn't involve any
    // copying or parsing, it's a very lightweight operation.
    model = tflite::GetModel(get_model());
    if (model->version() != TFLITE_SCHEMA_VERSION) {
        TF_LITE_REPORT_ERROR(error_reporter,
                             "Model provided is schema version %d not equal "
                             "to supported version %d.",
                             model->version(), TFLITE_SCHEMA_VERSION);
        return;
    }

    // This pulls in all the operation implementations we need.
    // NOLINTNEXTLINE(runtime-global-variables)
    static tflite::AllOpsResolver resolver;

    // Build an interpreter to run the model with.
    static tflite::MicroInterpreter static_interpreter(model, resolver, tensor_arena,
                                                       kTensorArenaSize, error_reporter);
    interpreter = &static_interpreter;

    // Allocate memory from the tensor_arena for the model's tensors.
    TfLiteStatus allocate_status = interpreter->AllocateTensors();
    if (allocate_status != kTfLiteOk) {
        TF_LITE_REPORT_ERROR(error_reporter, "AllocateTensors() failed");
        return;
    }

    // Obtain pointers to the model's input and output tensors.
    input = interpreter->input(0);
    output = interpreter->output(0);

    TfLiteType type = input->type;
    int dim_count = input->dims->size;
    int dim_0 = input->dims->data[0];
    int dim_1 = input->dims->data[1];
    int dim_2 = input->dims->data[2];
    int dim_3 = input->dims->data[3];

    printf("type: %s, dimensions: %d {%d, %d, %d, %d} \n", TfLiteTypeGetName(type), dim_count,
           dim_0, dim_1, dim_2, dim_3);
}

void run_tf_speech(
    const std::unique_ptr<std::array<std::array<int8_t, FFT_OUTPUT_SIZE>, FFT_WINDOW_COUNT>>
        spectogram) {
    printf("Input tensor size: %d", input->bytes);
    vTaskDelay(100);

    for (int window = 0; window < spectogram->size(); window++) {
        for (int bin = 0; bin < (*spectogram)[window].size(); bin++) {
            input->data.int8[window * (*spectogram)[window].size() + bin] =
                (*spectogram)[window][bin];
        }
    }

    // Run inference, and report any error
    TfLiteStatus invoke_status = interpreter->Invoke();
    if (invoke_status != kTfLiteOk) {
        TF_LITE_REPORT_ERROR(error_reporter, "Invoke failed");
        return;
    }

    printf("Output dims: %u", output->dims->size);

    // Obtain the quantized output from model's output tensor
    // int8_t y_quantized = output->data.int8[0];
    // Dequantize the output from integer to floating-point
    // float y = (y_quantized - output->params.zero_point) * output->params.scale;

    puts("done");
}
