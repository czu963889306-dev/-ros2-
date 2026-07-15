#pragma once

#include <stdint.h>

struct BatteryMsg {
    float adc_voltage_v = 0.0f;
    float voltage_v = 0.0f;
    uint8_t percentage = 0;
    int raw = 0;
    bool valid = false;
};
