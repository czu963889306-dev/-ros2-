#include "status_led.h"

#include "freertos/task.h"

StatusLed::StatusLed(gpio_num_t pin, bool active_high)
    : pin_(pin),
      active_high_(active_high),
      output_on_(false),
      mode_(StatusLedMode::kOff),
      last_toggle_tick_(0) {}

void StatusLed::Init() {
  gpio_config_t config = {};
  config.pin_bit_mask = 1ULL << pin_;
  config.mode = GPIO_MODE_OUTPUT;
  config.pull_up_en = GPIO_PULLUP_DISABLE;
  config.pull_down_en = GPIO_PULLDOWN_DISABLE;
  config.intr_type = GPIO_INTR_DISABLE;
  gpio_config(&config);

  last_toggle_tick_ = xTaskGetTickCount();
  ApplyOutput(false);
}

void StatusLed::SetMode(StatusLedMode mode) {
  mode_ = mode;
  last_toggle_tick_ = xTaskGetTickCount();

  switch (mode_) {
    case StatusLedMode::kOff:
      ApplyOutput(false);
      break;
    case StatusLedMode::kOn:
      ApplyOutput(true);
      break;
    case StatusLedMode::kSlowBlink:
    case StatusLedMode::kFastBlink:
      ApplyOutput(true);
      break;
  }
}

StatusLedMode StatusLed::GetMode() const {
  return mode_;
}

void StatusLed::SetOn(bool on) {
  mode_ = on ? StatusLedMode::kOn : StatusLedMode::kOff;
  ApplyOutput(on);
  last_toggle_tick_ = xTaskGetTickCount();
}

void StatusLed::Update(TickType_t now) {
  if (mode_ != StatusLedMode::kSlowBlink && mode_ != StatusLedMode::kFastBlink) {
    return;
  }

  const TickType_t interval = GetBlinkInterval();
  if ((now - last_toggle_tick_) < interval) {
    return;
  }

  last_toggle_tick_ = now;
  ApplyOutput(!output_on_);
}

void StatusLed::ApplyOutput(bool on) {
  output_on_ = on;
  const uint32_t level = (active_high_ == on) ? 1U : 0U;
  gpio_set_level(pin_, level);
}

TickType_t StatusLed::GetBlinkInterval() const {
  switch (mode_) {
    case StatusLedMode::kSlowBlink:
      return pdMS_TO_TICKS(500);
    case StatusLedMode::kFastBlink:
      return pdMS_TO_TICKS(120);
    case StatusLedMode::kOff:
    case StatusLedMode::kOn:
    default:
      return portMAX_DELAY;
  }
}
