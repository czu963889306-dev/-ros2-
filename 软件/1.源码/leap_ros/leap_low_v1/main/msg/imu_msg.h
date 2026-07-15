#pragma once

struct ImuMsg {
    float qw = 1.0f, qx = 0.0f, qy = 0.0f, qz = 0.0f;
    float roll = 0.0f, pitch = 0.0f, yaw = 0.0f;
    float acc_x = 0.0f, acc_y = 0.0f, acc_z = 0.0f;
    float gyro_x = 0.0f, gyro_y = 0.0f, gyro_z = 0.0f;
};