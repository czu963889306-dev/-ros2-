#include "motor_driver.h"

// 静态变量分配唯一的 LEDC 通道，S3 有 8 个通道 (LEDC_CHANNEL_0 ~ LEDC_CHANNEL_7)
static ledc_channel_t next_ledc_channel = LEDC_CHANNEL_0;
static bool is_ledc_timer_init = false;

At8236Motor::At8236Motor(gpio_num_t in1_pin, gpio_num_t in2_pin)
    : in1_pin_(in1_pin),
      in2_pin_(in2_pin),
      initialized_(false),
      usable_(in1_pin != GPIO_NUM_NC && in2_pin != GPIO_NUM_NC) {
  // 为当前电机的 IN1 和 IN2 分配独立的 PWM 通道
  channel_in1_ = next_ledc_channel;
  next_ledc_channel = static_cast<ledc_channel_t>(next_ledc_channel + 1);
  
  channel_in2_ = next_ledc_channel;
  next_ledc_channel = static_cast<ledc_channel_t>(next_ledc_channel + 1);
}

void At8236Motor::Init() {
  if (!usable_) {
    return;
  }
  // 只初始化一次定时器 (频率 5000Hz, 8位分辨率 0-255)
  if (!is_ledc_timer_init) {
    ledc_timer_config_t ledc_timer = {};
    ledc_timer.speed_mode      = LEDC_LOW_SPEED_MODE;
    ledc_timer.duty_resolution = LEDC_TIMER_8_BIT;
    ledc_timer.timer_num       = LEDC_TIMER_0;
    ledc_timer.freq_hz         = 20000;
    ledc_timer.clk_cfg         = LEDC_AUTO_CLK;
    ledc_timer_config(&ledc_timer);
    is_ledc_timer_init = true;
  }

  // 配置 IN1 通道
  ledc_channel_config_t ledc_channel_1 = {};
  ledc_channel_1.speed_mode = LEDC_LOW_SPEED_MODE;
  ledc_channel_1.channel    = channel_in1_;
  ledc_channel_1.timer_sel  = LEDC_TIMER_0;
  ledc_channel_1.intr_type  = LEDC_INTR_DISABLE;
  ledc_channel_1.gpio_num   = in1_pin_;
  ledc_channel_1.duty       = 0;
  ledc_channel_1.hpoint     = 0;
  ledc_channel_config(&ledc_channel_1);

  // 配置 IN2 通道
  ledc_channel_config_t ledc_channel_2 = ledc_channel_1;
  ledc_channel_2.channel  = channel_in2_;
  ledc_channel_2.gpio_num = in2_pin_;
  ledc_channel_config(&ledc_channel_2);
  initialized_ = true;
}

void At8236Motor::SetSpeed(int16_t speed) {
  if (!initialized_) {
    return;
  }

  if (speed > 255) speed = 255;
  if (speed < -255) speed = -255;

  if (speed > 0) {
    // 正转：IN2 输出 0，IN1 输出 PWM
    ledc_set_duty(LEDC_LOW_SPEED_MODE, channel_in2_, 0);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, channel_in2_);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, channel_in1_, speed);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, channel_in1_);
  } else if (speed < 0) {
    // 反转：IN1 输出 0，IN2 输出 PWM
    ledc_set_duty(LEDC_LOW_SPEED_MODE, channel_in1_, 0);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, channel_in1_);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, channel_in2_, -speed);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, channel_in2_);
  } else {
    // 停止：双低电平
    ledc_set_duty(LEDC_LOW_SPEED_MODE, channel_in1_, 0);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, channel_in1_);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, channel_in2_, 0);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, channel_in2_);
  }
}

void At8236Motor::Brake() {
  if (!initialized_) {
    return;
  }

  // AT8236 双高电平为刹车
  ledc_set_duty(LEDC_LOW_SPEED_MODE, channel_in1_, 255);
  ledc_update_duty(LEDC_LOW_SPEED_MODE, channel_in1_);
  ledc_set_duty(LEDC_LOW_SPEED_MODE, channel_in2_, 255);
  ledc_update_duty(LEDC_LOW_SPEED_MODE, channel_in2_);
}
