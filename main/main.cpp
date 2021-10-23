#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "wb-console.h"

extern "C" void app_main(void) {
    uint8_t i = 0;
    while (true) {
        printf("Hello world!\n");

        Console::print_array(std::array<uint8_t, 4>{i, 1, 2, 3});
        i++;
        
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
