#pragma once

#include <stdint.h>

typedef struct {
    int8_t left_x;
    int8_t left_y;
    int8_t right_x;
    int8_t right_y;
    uint8_t left_stick_btn;
    uint8_t right_stick_btn;
    uint8_t buttons;
    uint16_t button_mask;
    uint32_t update_ms;
    uint8_t connected;
} GamepadMsg;
