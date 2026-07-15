#pragma once
#include <stdint.h>

#pragma pack(push, 1)
struct RgbMsg {
    uint8_t r = 0;
    uint8_t g = 255;
    uint8_t b = 0;
};
#pragma pack(pop)