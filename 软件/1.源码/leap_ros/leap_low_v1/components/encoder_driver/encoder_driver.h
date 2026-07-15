// Copyright 2026. All rights reserved.

#ifndef ENCODER_DRIVER_H_
#define ENCODER_DRIVER_H_

#include <stdint.h>
#include "driver/gpio.h"
#include "driver/pulse_cnt.h" // 引入 ESP-IDF v5+ 的 PCNT 驱动

class QuadratureEncoder {
 public:
  QuadratureEncoder(gpio_num_t pin_a, gpio_num_t pin_b);
  ~QuadratureEncoder();

  void Init();
  
  // 获取当前累计的绝对脉冲数 (32位无溢出)
  int32_t GetCount();
  
  // 重置脉冲计数
  void ResetCount();

  bool IsInitialized() const;

 private:
  gpio_num_t pin_a_;
  gpio_num_t pin_b_;

  pcnt_unit_handle_t pcnt_unit_;

  // 用于将硬件 16 位计数平滑扩展为 32 位计数的变量
  int16_t last_hw_count_;
  int32_t accumulated_count_;
};

#endif  // ENCODER_DRIVER_H_
