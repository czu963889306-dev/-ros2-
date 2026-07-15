#include "servo_driver.h"

ServoDriver::ServoDriver(gpio_num_t pin) 
    : pin_(pin), comparator_(nullptr) {}

void ServoDriver::Init() {
  // 1. 创建并配置 MCPWM 定时器 (频率 50Hz, 也就是周期 20ms)
  mcpwm_timer_handle_t timer = nullptr;
  mcpwm_timer_config_t timer_config = {};
  timer_config.group_id = 0; // 使用 MCPWM 组 0
  timer_config.clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT;
  timer_config.resolution_hz = 1000000; // 1MHz 分辨率，意味着 1个tick = 1微秒
  timer_config.count_mode = MCPWM_TIMER_COUNT_MODE_UP;
  timer_config.period_ticks = 20000;    // 周期 20000 微秒 (50Hz)
  mcpwm_new_timer(&timer_config, &timer);

  // 2. 创建操作器并连接到定时器
  mcpwm_oper_handle_t oper = nullptr;
  mcpwm_operator_config_t oper_config = {};
  oper_config.group_id = 0;
  mcpwm_new_operator(&oper_config, &oper);
  mcpwm_operator_connect_timer(oper, timer);

  // 3. 创建比较器 (负责产生我们需要的脉宽)
  mcpwm_comparator_config_t cmpr_config = {};
  cmpr_config.flags.update_cmp_on_tez = true;
  mcpwm_new_comparator(oper, &cmpr_config, &comparator_);

  // 4. 创建生成器并绑定到舵机的 GPIO 引脚
  mcpwm_gen_handle_t gen = nullptr;
  mcpwm_generator_config_t gen_config = {};
  gen_config.gen_gpio_num = pin_;
  mcpwm_new_generator(oper, &gen_config, &gen);

  // 5. 设置波形生成规则 (重点)
  // 当定时器归零时，引脚输出高电平
  mcpwm_generator_set_action_on_timer_event(
      gen, 
      MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH));
  // 当定时器计数到我们设置的脉宽时间时，引脚输出低电平
  mcpwm_generator_set_action_on_compare_event(
      gen, 
      MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, comparator_, MCPWM_GEN_ACTION_LOW));

  // 6. 启动硬件定时器
  mcpwm_timer_enable(timer);
  mcpwm_timer_start_stop(timer, MCPWM_TIMER_START_NO_STOP);
}

void ServoDriver::SetAngle(float angle) {
  if (!comparator_) return; // 确保初始化成功

  if (angle < 0.0f) angle = 0.0f;
  if (angle > kMaxAngle) angle = kMaxAngle;

  // 根据角度计算需要的脉宽时间 (单位: 微秒)
  uint32_t pulse_width_us = kMinPulseWidthUs + (angle / kMaxAngle) * (kMaxPulseWidthUs - kMinPulseWidthUs);

  // 直接更新比较器的值，MCPWM 硬件会自动帮我们调整输出脉宽
  mcpwm_comparator_set_compare_value(comparator_, pulse_width_us);
}