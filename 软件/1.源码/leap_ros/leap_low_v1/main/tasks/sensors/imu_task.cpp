#include "system_globals.h"
#include "board.h" // 假设 imu 实例在此声明
#include "msg/imu_msg.h"

#include "esp_log.h" // 确保包含了日志打印头文件

void imu_task(void *p) {
    ImuMsg msg;
    uint8_t print_counter = 0; // 【新增】用于降低串口打印频率的计数器

    while (1) {
        imu.Update();
        imu.GetQuaternion(msg.qw, msg.qx, msg.qy, msg.qz);
        msg.roll = imu.GetRoll();
        msg.pitch = imu.GetPitch();
        msg.yaw = imu.GetYaw();
        msg.acc_x = imu.GetAccX();
        msg.acc_y = imu.GetAccY();
        msg.acc_z = imu.GetAccZ();
        msg.gyro_x = imu.GetGyroX();
        msg.gyro_y = imu.GetGyroY();
        msg.gyro_z = imu.GetGyroZ();

        xQueueOverwrite(q_imu_state, &msg);

        // 【新增】：降频打印 Yaw 角 (25 * 20ms = 500ms 打印一次)
        // if (++print_counter >= 5) {
        //     // 如果你的 IMU 吐出的是弧度，可以自行加备注
        //     ESP_LOGI("IMU_TASK", "Current Yaw: %.2f", msg.yaw); 
        //     print_counter = 0; // 计数器清零
        // }

        vTaskDelay(pdMS_TO_TICKS(20)); 
    }
}