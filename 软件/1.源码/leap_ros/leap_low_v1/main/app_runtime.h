#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void app_runtime_startup(void);
void app_runtime_restart_after_delay_ms(uint32_t delay_ms);

#ifdef __cplusplus
}
#endif
