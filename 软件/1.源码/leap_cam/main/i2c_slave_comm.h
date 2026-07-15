#pragma once

#include "esp_err.h"

#define I2C_SLAVE_COMM_ADDR 0x42
#define I2C_SLAVE_COMM_SCL_GPIO 47
#define I2C_SLAVE_COMM_SDA_GPIO 48

esp_err_t i2c_slave_comm_start(void);
