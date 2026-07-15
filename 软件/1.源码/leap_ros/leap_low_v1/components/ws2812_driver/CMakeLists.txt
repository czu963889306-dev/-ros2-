idf_component_register(
    SRCS "ws2812_driver.cpp"
    INCLUDE_DIRS "."
    REQUIRES led_strip esp_driver_gpio
)
