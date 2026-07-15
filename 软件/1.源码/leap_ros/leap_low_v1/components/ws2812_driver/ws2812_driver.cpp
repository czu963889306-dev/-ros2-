#include "ws2812_driver.h"
#include "esp_log.h"

Ws2812Driver::Ws2812Driver(gpio_num_t pin, uint32_t num_leds)
    : pin_(pin), num_leds_(num_leds), led_strip_(NULL) {}

void Ws2812Driver::Init() {
  // 1. 配置 LED 灯带的基本参数
  led_strip_config_t strip_config = {};
  strip_config.strip_gpio_num = pin_;
  strip_config.max_leds = num_leds_;
  strip_config.led_pixel_format = LED_PIXEL_FORMAT_GRB; // WS2812 通常是 GRB 格式
  strip_config.led_model = LED_MODEL_WS2812;
  strip_config.flags.invert_out = false;

  // 2. 配置 RMT 底层硬件参数
  led_strip_rmt_config_t rmt_config = {};
  rmt_config.clk_src = RMT_CLK_SRC_DEFAULT; // 默认时钟源
  rmt_config.resolution_hz = 10 * 1000 * 1000; // 10MHz 的 RMT 解析度，足够生成精确时序
  rmt_config.flags.with_dma = false;

  // 3. 安装并初始化设备
  ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip_));
  
  // 初始化后默认清空（熄灭）
  Clear();
}

void Ws2812Driver::SetPixel(uint32_t index, uint8_t red, uint8_t green, uint8_t blue) {
  if (led_strip_ && index < num_leds_) {
    led_strip_set_pixel(led_strip_, index, red, green, blue);
  }
}

void Ws2812Driver::Refresh() {
  if (led_strip_) {
    led_strip_refresh(led_strip_);
  }
}

void Ws2812Driver::Clear() {
  if (led_strip_) {
    led_strip_clear(led_strip_);
  }
}