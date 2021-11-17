#ifndef CONSOLE_H
#define CONSOLE_H

#include <array>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

class Command {
   public:
    virtual const char* name() = 0;
    virtual void command() = 0;
};

class Console final {
   public:
    static Console& get_instance();
    void init();
    bool register_command(Command& command);
    static void task_wrapper(void*);

   private:
    Console() = default;
    void task();
    void read_string();

    xTaskHandle task_handle{};
    static constexpr size_t STACK_SIZE{4096};
    static constexpr UBaseType_t TASK_PRIORITY{tskIDLE_PRIORITY + 1};
    StaticTask_t xTaskBuffer{};
    StackType_t xStack[STACK_SIZE]{};

    static constexpr size_t NUM_COMMANDS{1};

    std::array<Command*, NUM_COMMANDS> commands{};

    static constexpr size_t BUF_LEN{16};
    char buffer[BUF_LEN]{};
};

#endif