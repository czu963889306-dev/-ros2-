#include <string.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_now.h"
#include "system_globals.h"
#include "msg/motion_msg.h"

static const char *TAG = "ESPNOW_RECV";

// 必须与发送端保持完全一致的数据结构
typedef struct {
    int joy1_x;
    int joy1_y;
    int joy2_x;
    int joy2_y;
} control_data_t;

// 将摇杆 ADC 值 (0~4095, 中值 2048) 映射为实际速度
// 带有死区(Deadzone)处理，防止摇杆不回中导致机器人漂移
static float map_joystick_to_velocity(int raw_adc, float max_velocity) {
    int offset = raw_adc - 2048;
    
    // 设置摇杆死区，比如偏移量在 150 以内算作中心点
    if (abs(offset) < 200) {
        return 0.0f;
    }
    
    // 线性映射到 -max_velocity ~ +max_velocity
    return (float)offset / 2048.0f * max_velocity;
}

// ESP-NOW 接收回调函数 (注意：此函数在 Wi-Fi 任务上下文中运行，需保持简短)
static void on_espnow_recv(const esp_now_recv_info_t *esp_now_info, const uint8_t *data, int data_len) {
    if (data_len == sizeof(control_data_t)) {
        control_data_t recv_data;
        memcpy(&recv_data, data, sizeof(control_data_t));

        MotionMsg cmd_msg = {};
        cmd_msg.source = MOTION_SRC_ESPNOW;
        
        // 【重要】你需要根据实际物理摇杆的方向，在这里决定是否加负号
        // 假设最大线速度为 1.0 m/s，最大角速度为 3.14 rad/s (可按需修改)
        // 假设 Joy1_Y 控制前后 (X轴)，Joy1_X 控制左右平移 (Y轴)，Joy2_X 控制自转 (Wz)
        cmd_msg.target_vx = map_joystick_to_velocity(recv_data.joy1_y, 100.0f); 
        // cmd_msg.target_vy = -map_joystick_to_velocity(recv_data.joy1_x, 100.0f); 

        // 注意：角速度 (wz) 的单位通常是 rad/s。
        // 如果你的底盘运动学解算也将自转 (wz) 当作类似 RPM 的缩放系数来处理，
        // 你可以把它也改为 100.0f；如果依然是弧度制，就保持 3.14f 不变。
        cmd_msg.target_wz = -map_joystick_to_velocity(recv_data.joy2_y, 100.0f);

        // 将指令覆盖写入全局运动控制队列
        if (q_motion_cmd != nullptr) {
            xQueueOverwrite(q_motion_cmd, &cmd_msg);
        }
    } else {
        ESP_LOGW(TAG, "收到未知大小数据包: %d 字节", data_len);
    }
}

void espnow_task(void *p) {
    // 初始化 ESP-NOW
    esp_err_t ret = esp_now_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "ESP-NOW 初始化失败: %s", esp_err_to_name(ret));
        vTaskDelete(NULL);
        return;
    }

    // 注册接收回调函数
    ESP_ERROR_CHECK(esp_now_register_recv_cb(on_espnow_recv));
    ESP_LOGI(TAG, "ESP-NOW 接收端初始化成功，等待遥控器信号...");

    // 任务可以挂起，因为接收逻辑在回调函数中触发
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(5000)); // 随便给个长延时，不占用 CPU
    }
}
