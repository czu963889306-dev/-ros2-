#pragma once

struct PidMsg {
    // 全局/统一的速度环 PID 参数
    float kp = 0.0f;
    float ki = 0.0f;
    float kd = 0.0f;

    // (预留) 如果未来需要针对单个轮子独立调参，可以解除注释
    /*
    float fl_kp = 0.0f, fl_ki = 0.0f, fl_kd = 0.0f;
    float fr_kp = 0.0f, fr_ki = 0.0f, fr_kd = 0.0f;
    float bl_kp = 0.0f, bl_ki = 0.0f, bl_kd = 0.0f;
    float br_kp = 0.0f, br_ki = 0.0f, br_kd = 0.0f;
    */
};