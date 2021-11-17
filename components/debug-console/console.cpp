#include "console.h"

#include <cstdio>
#include <cstring>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

Console& Console::get_instance() {
    static Console instance{};
    return instance;
}

void Console::init() {
    if (this->task_handle) {
        assert(!"Error: Console task reinitialized");
    } else {
        puts("console init");
        this->task_handle = xTaskCreateStaticPinnedToCore(
            this->task_wrapper, "Console", this->STACK_SIZE, nullptr, this->TASK_PRIORITY,
            this->xStack, &this->xTaskBuffer, 0 /* core */);
        printf("console handle: %p\n", this->task_handle);
    }
}

bool Console::register_command(Command& command) {
    for (auto & entry : this->commands) {
        if (entry == nullptr) {
            entry = &command;
            return true;
        }
    }
    return false;
}

void Console::task_wrapper(void*) {
    for (;;) {
        Console::get_instance().task();

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

void Console::task() {
    if (!this->task_handle) {
        assert(!"Error: Console task not initialized");
    } else {
        this->read_string();
        if (strncmp(this->buffer, "audio\n", BUF_LEN) == 0) {
            this->commands[0]->command();
        }
    }
}

void Console::read_string() {
    memset(this->buffer, 0, this->BUF_LEN);

    size_t i = 0;
    while (i < this->BUF_LEN - 1) {
        char c = fgetc(stdin);

        if (c == 0xFF) {
            // No input, ignore
        } else {
            fputc(c, stdout);
            fflush(stdout);

            // printf("%d\n", c);
            this->buffer[i] = c;
            i++;

            if (c == '\n') {
                // Reached end of command
                break;
            }
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }

    fputs(this->buffer, stdout);
    return;
}