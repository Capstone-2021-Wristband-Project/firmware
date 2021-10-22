#include <stdio.h>

#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main(void) {
    while (true) {
        printf("Hello world!\n");

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
