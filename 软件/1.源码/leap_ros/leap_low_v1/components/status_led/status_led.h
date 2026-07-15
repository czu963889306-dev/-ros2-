#ifndef STATUS_LED_H_
#define STATUS_LED_H_

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"

enum class StatusLedMode {
  kOff = 0,
  kOn,
  kSlowBlink,
  kFastBlink,
};

class StatusLed {
 public:
  StatusLed(gpio_num_t pin, bool active_high = true);

  void Init();
  void SetMode(StatusLedMode mode);
  StatusLedMode GetMode() const;
  void SetOn(bool on);
  void Update(TickType_t now);

 private:
  void ApplyOutput(bool on);
  TickType_t GetBlinkInterval() const;

  gpio_num_t pin_;
  bool active_high_;
  bool output_on_;
  StatusLedMode mode_;
  TickType_t last_toggle_tick_;
};

#endif  // STATUS_LED_H_
