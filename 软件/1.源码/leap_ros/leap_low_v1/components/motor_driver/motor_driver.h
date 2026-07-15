#ifndef MOTOR_DRIVER_H_
#define MOTOR_DRIVER_H_

#include <stdint.h>
#include "driver/gpio.h"
#include "driver/ledc.h"

class At8236Motor {
 public:
  At8236Motor(gpio_num_t in1_pin, gpio_num_t in2_pin);

  void Init();
  void SetSpeed(int16_t speed);
  void Brake();

 private:
  gpio_num_t in1_pin_;
  gpio_num_t in2_pin_;
  ledc_channel_t channel_in1_;
  ledc_channel_t channel_in2_;
  bool initialized_;
  bool usable_;
};

#endif  // MOTOR_DRIVER_H_
