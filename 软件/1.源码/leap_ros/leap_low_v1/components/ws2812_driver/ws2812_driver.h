#ifndef WS2812_DRIVER_H_
#define WS2812_DRIVER_H_

#include <stdint.h>
#include "driver/gpio.h"
#include "led_strip.h"

class Ws2812Driver {
 public:
  // 构造函数
  // pin: WS2812 数据引脚
  // num_leds: 灯珠的数量 (如果只有 1 颗板载 LED，填 1)
  Ws2812Driver(gpio_num_t pin, uint32_t num_leds);

  void Init();

  // 设置指定序号灯珠的颜色 (0 <= index < num_leds)
  // red, green, blue 范围均是 0-255
  void SetPixel(uint32_t index, uint8_t red, uint8_t green, uint8_t blue);

  // 将设置好的颜色推送到实体灯珠上显示
  void Refresh();

  // 熄灭所有灯珠
  void Clear();

 private:
  gpio_num_t pin_;
  uint32_t num_leds_;
  led_strip_handle_t led_strip_;
};

#endif  // WS2812_DRIVER_H_