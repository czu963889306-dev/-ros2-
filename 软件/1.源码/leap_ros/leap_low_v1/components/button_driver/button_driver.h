#ifndef BUTTON_DRIVER_H_
#define BUTTON_DRIVER_H_

#include <stdint.h>

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

class ButtonDriver {
 public:
  ButtonDriver(gpio_num_t pin, bool active_low = true, uint32_t debounce_ms = 30);

  void Init();
  bool Update();
  bool IsPressed() const;

 private:
  bool ReadRawState() const;

  gpio_num_t pin_;
  bool active_low_;
  uint32_t debounce_ms_;
  bool raw_state_;
  bool stable_state_;
  TickType_t last_change_tick_;
};

#endif  // BUTTON_DRIVER_H_
