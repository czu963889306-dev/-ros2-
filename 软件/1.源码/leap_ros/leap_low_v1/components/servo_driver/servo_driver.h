#ifndef SERVO_DRIVER_H_
#define SERVO_DRIVER_H_

#include "driver/gpio.h"
#include "driver/mcpwm_prelude.h" // 引入 MCPWM 库

class ServoDriver {
 public:
  // 构造函数只需要传入引脚即可
  ServoDriver(gpio_num_t pin);

  void Init();
  void SetAngle(float angle);

 private:
  gpio_num_t pin_;
  mcpwm_cmpr_handle_t comparator_; // 比较器句柄，用于调节占空比
  
  // 舵机脉宽参数
  const uint32_t kMinPulseWidthUs = 500;
  const uint32_t kMaxPulseWidthUs = 2500;
  const float kMaxAngle = 180.0f;
};

#endif  // SERVO_DRIVER_H_