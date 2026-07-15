#pragma once
#include <stdint.h>

#pragma pack(push, 1) // 强制 1 字节对齐
struct LidarMsg {
    // 存储 0~359 度的距离数据 (单位: mm)，索引(0~359)即代表对应的角度。
    // 数值为 0 表示该角度下无有效点或超出量程。
    uint16_t distances[360] = {0}; 
};
#pragma pack(pop)     // 恢复默认对齐