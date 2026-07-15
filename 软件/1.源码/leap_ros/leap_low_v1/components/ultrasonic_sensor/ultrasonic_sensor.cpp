#include "ultrasonic_sensor.h"
#include "esp_timer.h"
#include "esp_rom_sys.h"

UltrasonicSensor::UltrasonicSensor(gpio_num_t io_pin) : 
    io_pin_(io_pin), cap_timer_(NULL), cap_chan_(NULL), 
    timer_res_(0), cap_val_begin_of_sample_(0), task_to_notify_(NULL) {}

bool UltrasonicSensor::Init() {
  // 1. 初始化引脚为输入，启用内部弱下拉，防止悬空杂波被误捕获
  gpio_config_t io_conf = {};
  io_conf.intr_type = GPIO_INTR_DISABLE;
  io_conf.mode = GPIO_MODE_INPUT;
  io_conf.pin_bit_mask = (1ULL << io_pin_);
  io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE; 
  io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
  gpio_config(&io_conf);

  // 2. 配置 MCPWM 捕获定时器
  mcpwm_capture_timer_config_t cap_conf = {};
  cap_conf.clk_src = MCPWM_CAPTURE_CLK_SRC_DEFAULT;
  cap_conf.group_id = 0;
  if (mcpwm_new_capture_timer(&cap_conf, &cap_timer_) != ESP_OK) return false;

  // 获取定时器实际分辨率(Hz)，用于后续在中断里快速换算微秒
  mcpwm_capture_timer_get_resolution(cap_timer_, &timer_res_);

// 3. 配置 MCPWM 捕获通道（直接绑定到你的 io_pin）
  mcpwm_capture_channel_config_t cap_ch_conf = {};
  cap_ch_conf.gpio_num = io_pin_;
  cap_ch_conf.prescale = 1;
  cap_ch_conf.flags.pos_edge = true;
  cap_ch_conf.flags.neg_edge = true;
  cap_ch_conf.flags.pull_down = true;
  cap_ch_conf.flags.pull_up = false;
  // 结构体经过 {} 初始化后，其余 flags 默认皆为 0，无需手动赋值，以此兼容所有 IDF v5.x 版本
  
  if (mcpwm_new_capture_channel(cap_timer_, &cap_ch_conf, &cap_chan_) != ESP_OK) return false;

  // 4. 注册硬件边沿中断回调 (硬件会自动同时监听上升沿和下降沿)
  mcpwm_capture_event_callbacks_t cbs = {};
  cbs.on_cap = mcpwm_isr_handler;
  if (mcpwm_capture_channel_register_event_callbacks(cap_chan_, &cbs, this) != ESP_OK) return false;

  // 5. 启动硬件捕获
  mcpwm_capture_channel_enable(cap_chan_);
  mcpwm_capture_timer_enable(cap_timer_);
  mcpwm_capture_timer_start(cap_timer_);

  return true;
}

bool IRAM_ATTR UltrasonicSensor::mcpwm_isr_handler(
    mcpwm_cap_channel_handle_t cap_chan,
    const mcpwm_capture_event_data_t *edata,
    void *user_data) {
  UltrasonicSensor *sensor = static_cast<UltrasonicSensor *>(user_data);
  BaseType_t high_task_wakeup = pdFALSE;

  if (!sensor->measurement_armed_ ||
      sensor->task_to_notify_ == NULL ||
      sensor->result_sent_) {
    return false;
  }

  if (edata->cap_edge == MCPWM_CAP_EDGE_POS) {
    // 只接受第一个回波上升沿
    if (!sensor->echo_rise_seen_) {
      sensor->cap_val_begin_of_sample_ = edata->cap_value;
      sensor->echo_rise_seen_ = true;
    }
  } 
  else if (edata->cap_edge == MCPWM_CAP_EDGE_NEG) {
    if (sensor->echo_rise_seen_) {
      uint32_t pulse_ticks =
          edata->cap_value - sensor->cap_val_begin_of_sample_;

      uint32_t duration_us =
          (uint32_t)(((uint64_t)pulse_ticks * 1000000) / sensor->timer_res_);

      // HC-SR04 典型有效量程约从 2 cm 起，
      // 2 cm 对应约 116 us，因此 100 us 可作为更合理的下限
      if (duration_us > 100 && duration_us < kTimeoutUs) {
        sensor->result_sent_ = true;
        sensor->measurement_armed_ = false;

        xTaskNotifyFromISR(
            sensor->task_to_notify_,
            duration_us,
            eSetValueWithoutOverwrite,
            &high_task_wakeup
        );
      } else {
        // 当前边沿对无效，允许下一组重新配对
        sensor->echo_rise_seen_ = false;
      }
    }
  }

  return high_task_wakeup == pdTRUE;
}

float UltrasonicSensor::GetDistanceCm(float temperature) {
  task_to_notify_ = xTaskGetCurrentTaskHandle();

  uint32_t dummy = 0;
  xTaskNotifyWait(0x00, 0xFFFFFFFFUL, &dummy, 0);

  measurement_armed_ = false;
  echo_rise_seen_ = false;
  result_sent_ = false;

  gpio_set_level(io_pin_, 0);
  gpio_set_direction(io_pin_, GPIO_MODE_OUTPUT);
  esp_rom_delay_us(2);

  gpio_set_level(io_pin_, 1);
  esp_rom_delay_us(15);

  gpio_set_level(io_pin_, 0);

  gpio_set_direction(io_pin_, GPIO_MODE_INPUT);

  // 触发脉冲结束后，才正式允许捕获回波
  measurement_armed_ = true;

  uint32_t duration_us = 0;
  TickType_t timeout_ticks = pdMS_TO_TICKS(kTimeoutUs / 1000) + 1;

  if (xTaskNotifyWait(0x00, 0xFFFFFFFFUL, &duration_us, timeout_ticks) == pdTRUE) {
    float sound_speed_m_s = 331.45f + 0.61f * temperature;
    float distance_cm = (duration_us * sound_speed_m_s) / 20000.0f;

    task_to_notify_ = NULL;
    measurement_armed_ = false;
    return distance_cm;
  }

  task_to_notify_ = NULL;
  measurement_armed_ = false;
  return -1.0f;
}