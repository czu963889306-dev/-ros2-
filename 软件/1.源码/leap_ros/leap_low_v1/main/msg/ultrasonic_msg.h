#pragma once

#pragma pack(push, 1) // 强制 1 字节对齐
struct UltrasonicMsg {
    float distance_cm = 0.0f;
    bool is_obstacle_detected = false;
};
#pragma pack(pop)     // 恢复默认对齐