// Copyright 2026. All rights reserved.

#include "encoder_driver.h"
#include "esp_log.h"

static const char* TAG = "ENCODER_PCNT";

QuadratureEncoder::QuadratureEncoder(gpio_num_t pin_a, gpio_num_t pin_b)
    : pin_a_(pin_a),
      pin_b_(pin_b),
      pcnt_unit_(nullptr),
      last_hw_count_(0),
      accumulated_count_(0) {}

QuadratureEncoder::~QuadratureEncoder() {
  if (pcnt_unit_) {
    pcnt_unit_stop(pcnt_unit_);
    pcnt_unit_disable(pcnt_unit_);
    pcnt_del_unit(pcnt_unit_);
  }
}

void QuadratureEncoder::Init() {
  if (pin_a_ == GPIO_NUM_NC || pin_b_ == GPIO_NUM_NC) {
    ESP_LOGI(TAG, "Skipping PCNT init on pins %d/%d because encoder is not populated", pin_a_, pin_b_);
    pcnt_unit_ = nullptr;
    last_hw_count_ = 0;
    accumulated_count_ = 0;
    return;
  }

  ESP_LOGI(TAG, "Initializing PCNT on pins %d and %d", pin_a_, pin_b_);

  // 1. 配置 PCNT 单元 (Unit)
  pcnt_unit_config_t unit_config = {};
  unit_config.low_limit = -30000;   // 【修改这里】改为对称下限
  unit_config.high_limit = 30000;   // 【修改这里】改为对称上限
  
  ESP_ERROR_CHECK(pcnt_new_unit(&unit_config, &pcnt_unit_));

  // 2. 设置硬件毛刺滤波器 (极大提升抗干扰能力)
  pcnt_glitch_filter_config_t filter_config = {};
  filter_config.max_glitch_ns = 1000; // 过滤 1微秒 (1000ns) 以下的短跳变
  ESP_ERROR_CHECK(pcnt_unit_set_glitch_filter(pcnt_unit_, &filter_config));

  // 3. 实例化两个通道 (Channel A 和 Channel B)
  pcnt_chan_config_t chan_a_config = {};
  chan_a_config.edge_gpio_num = pin_a_;
  chan_a_config.level_gpio_num = pin_b_;
  pcnt_channel_handle_t pcnt_chan_a = nullptr;
  ESP_ERROR_CHECK(pcnt_new_channel(pcnt_unit_, &chan_a_config, &pcnt_chan_a));

  pcnt_chan_config_t chan_b_config = {};
  chan_b_config.edge_gpio_num = pin_b_;
  chan_b_config.level_gpio_num = pin_a_;
  pcnt_channel_handle_t pcnt_chan_b = nullptr;
  ESP_ERROR_CHECK(pcnt_new_channel(pcnt_unit_, &chan_b_config, &pcnt_chan_b));

  // 4. 设置硬件四倍频 (4X Quadrature) 逻辑
  // A相：当A跳变时，根据B相的电平决定加还是减
  ESP_ERROR_CHECK(pcnt_channel_set_edge_action(pcnt_chan_a, 
      PCNT_CHANNEL_EDGE_ACTION_DECREASE, PCNT_CHANNEL_EDGE_ACTION_INCREASE));
  ESP_ERROR_CHECK(pcnt_channel_set_level_action(pcnt_chan_a, 
      PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE));

  // B相：当B跳变时，根据A相的电平决定加还是减
  ESP_ERROR_CHECK(pcnt_channel_set_edge_action(pcnt_chan_b, 
      PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_DECREASE));
  ESP_ERROR_CHECK(pcnt_channel_set_level_action(pcnt_chan_b, 
      PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE));

  // 5. 启用、清零并启动 PCNT 硬件
  ESP_ERROR_CHECK(pcnt_unit_enable(pcnt_unit_));
  ESP_ERROR_CHECK(pcnt_unit_clear_count(pcnt_unit_));
  ESP_ERROR_CHECK(pcnt_unit_start(pcnt_unit_));
}

int32_t QuadratureEncoder::GetCount() {
  if (pcnt_unit_ == nullptr) {
    return 0;
  }

  int hw_count = 0;
  pcnt_unit_get_count(pcnt_unit_, &hw_count);

  // 【关键修复 1】必须用 32 位整数来算差值，避免被 16位 截断
  int32_t delta = hw_count - last_hw_count_;

  // 【关键修复 2】硬件归零跃变补偿逻辑
  // 当硬件计数器到达 30000 时，下一个脉冲会把它变成 0，跨度为 30001
  if (delta < -15000) {
    // 如果算出来的差值是个极大的负数（比如从 29990 突然变成 10），说明发生了正向溢出归零
    delta += 30001; 
  } else if (delta > 15000) {
    // 如果算出来的差值是个极大的正数（比如从 -29990 突然变成 -10），说明发生了反向溢出归零
    delta -= 30001; 
  }

  // 累加真实的物理脉冲增量
  accumulated_count_ += delta;
  last_hw_count_ = hw_count;

  return accumulated_count_;
}

void QuadratureEncoder::ResetCount() {
  if (pcnt_unit_ == nullptr) {
    last_hw_count_ = 0;
    accumulated_count_ = 0;
    return;
  }

  pcnt_unit_clear_count(pcnt_unit_);
  last_hw_count_ = 0;
  accumulated_count_ = 0;
}

bool QuadratureEncoder::IsInitialized() const {
  return pcnt_unit_ != nullptr;
}
