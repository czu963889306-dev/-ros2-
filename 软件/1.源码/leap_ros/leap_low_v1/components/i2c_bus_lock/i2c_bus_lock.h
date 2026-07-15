#pragma once

#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#ifdef __cplusplus
extern "C" {
#endif

void shared_i2c_bus_lock_init(void);
bool shared_i2c_bus_lock_take(TickType_t ticks_to_wait);
void shared_i2c_bus_lock_give(void);

#ifdef __cplusplus
}
#endif
