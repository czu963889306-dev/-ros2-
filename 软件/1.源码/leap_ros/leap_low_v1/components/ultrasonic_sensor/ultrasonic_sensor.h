#ifndef ULTRASONIC_SENSOR_H_
#define ULTRASONIC_SENSOR_H_

#include <stdint.h>
#include "driver/gpio.h"
#include "driver/mcpwm_cap.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

class UltrasonicSensor {
 public:
  // 构造函数，传入单总线所在的 GPIO 引脚
  UltrasonicSensor(gpio_num_t io_pin);

  // 初始化引脚默认状态与 MCPWM 捕获硬件
  // 返回 true 表示初始化成功，false 表示失败
  bool Init();

  // 获取距离，返回值为厘米 (cm)
  // temperature: 当前环境温度（摄氏度），用于温度补偿声速，默认 20.0℃
  // 返回负数表示测量失败或超时
  float GetDistanceCm(float temperature = 20.0f);

 private:
  gpio_num_t io_pin_;
    volatile bool measurement_armed_ = false;
    volatile bool echo_rise_seen_ = false;
    volatile bool result_sent_ = false;
  // MCPWM 硬件捕获相关句柄
  mcpwm_cap_timer_handle_t cap_timer_;
  mcpwm_cap_channel_handle_t cap_chan_;
  uint32_t timer_res_;               // 定时器分辨率 (Hz)，用于换算微秒
  uint32_t cap_val_begin_of_sample_; // 记录上升沿的计数值
  TaskHandle_t task_to_notify_;      // 记录等待测量结果的 FreeRTOS 任务句柄

  // 超时时间设置：最大测量距离约为 5 米，对应的高电平时间约为 35000 微秒
  static const int64_t kTimeoutUs = 35000; 

  // 硬件中断回调函数 (必须是静态函数以匹配 C 风格回调)
  static bool IRAM_ATTR mcpwm_isr_handler(mcpwm_cap_channel_handle_t cap_chan, 
                                          const mcpwm_capture_event_data_t *edata, 
                                          void *user_data);
};

#endif  // ULTRASONIC_SENSOR_H_