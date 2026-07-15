#include "button_driver.h"

ButtonDriver::ButtonDriver(gpio_num_t pin, bool active_low, uint32_t debounce_ms)
    : pin_(pin),
      active_low_(active_low),
      debounce_ms_(debounce_ms),
      raw_state_(false),
      stable_state_(false),
      last_change_tick_(0) {}

void ButtonDriver::Init() {
  if (pin_ == GPIO_NUM_NC) {
    raw_state_ = false;
    stable_state_ = false;
    last_change_tick_ = xTaskGetTickCount();
    return;
  }

  gpio_config_t io_conf = {};
  io_conf.pin_bit_mask = 1ULL << pin_;
  io_conf.mode = GPIO_MODE_INPUT;
  io_conf.pull_up_en = active_low_ ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE;
  io_conf.pull_down_en = active_low_ ? GPIO_PULLDOWN_DISABLE : GPIO_PULLDOWN_ENABLE;
  io_conf.intr_type = GPIO_INTR_DISABLE;
  gpio_config(&io_conf);

  raw_state_ = ReadRawState();
  stable_state_ = raw_state_;
  last_change_tick_ = xTaskGetTickCount();
}

bool ButtonDriver::Update() {
  const bool current_state = ReadRawState();
  const TickType_t now = xTaskGetTickCount();

  if (current_state != raw_state_) {
    raw_state_ = current_state;
    last_change_tick_ = now;
  }

  if (stable_state_ != raw_state_) {
    const TickType_t debounce_ticks = pdMS_TO_TICKS(debounce_ms_);
    if ((now - last_change_tick_) >= debounce_ticks) {
      stable_state_ = raw_state_;
      return true;
    }
  }

  return false;
}

bool ButtonDriver::IsPressed() const {
  return stable_state_;
}

bool ButtonDriver::ReadRawState() const {
  if (pin_ == GPIO_NUM_NC) {
    return false;
  }

  const int level = gpio_get_level(pin_);
  return active_low_ ? (level == 0) : (level != 0);
}
