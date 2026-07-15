#include "i2c_bus_lock.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

static SemaphoreHandle_t s_i2c_bus_mutex = NULL;
static portMUX_TYPE s_i2c_bus_mux = portMUX_INITIALIZER_UNLOCKED;

void shared_i2c_bus_lock_init(void) {
    if (s_i2c_bus_mutex != NULL) {
        return;
    }

    portENTER_CRITICAL(&s_i2c_bus_mux);
    if (s_i2c_bus_mutex == NULL) {
        s_i2c_bus_mutex = xSemaphoreCreateMutex();
    }
    portEXIT_CRITICAL(&s_i2c_bus_mux);
}

bool shared_i2c_bus_lock_take(TickType_t ticks_to_wait) {
    shared_i2c_bus_lock_init();
    return s_i2c_bus_mutex != NULL && xSemaphoreTake(s_i2c_bus_mutex, ticks_to_wait) == pdTRUE;
}

void shared_i2c_bus_lock_give(void) {
    if (s_i2c_bus_mutex != NULL) {
        xSemaphoreGive(s_i2c_bus_mutex);
    }
}
