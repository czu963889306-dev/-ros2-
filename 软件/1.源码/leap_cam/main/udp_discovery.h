#pragma once

#include "esp_err.h"

#define UDP_DISCOVERY_PORT 33333

esp_err_t udp_discovery_start(void);
