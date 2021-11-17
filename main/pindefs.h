#ifndef PINDEFS_H
#define PINDEFS_H

#include <driver/gpio.h>

static constexpr gpio_num_t PIN_INTERNAL_MICROPHONE{GPIO_NUM_34};
static constexpr gpio_num_t PIN_EXTERNAL_MICROPHONE{GPIO_NUM_35};
static constexpr gpio_num_t PIN_VIBRATION_MOTOR{GPIO_NUM_33};
static constexpr gpio_num_t PIN_I2C_SCL{GPIO_NUM_27};
static constexpr gpio_num_t PIN_I2C_SDA{GPIO_NUM_32};
static constexpr gpio_num_t PIN_MONITOR_OFF{GPIO_NUM_26};
static constexpr gpio_num_t PIN_BATTERY_SENSE{GPIO_NUM_25};
static constexpr gpio_num_t PIN_SENSE_ENABLE{GPIO_NUM_23};
static constexpr gpio_num_t PIN_TESTPOINT_1{GPIO_NUM_5};
static constexpr gpio_num_t PIN_TESTPOINT_2{GPIO_NUM_4};
static constexpr gpio_num_t PIN_TESTPOINT_3{GPIO_NUM_2};

#endif