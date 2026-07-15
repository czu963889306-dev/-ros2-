idf_component_register(
    SRCS "servo_driver.cpp"
    INCLUDE_DIRS "."
    REQUIRES driver esp_driver_ledc
)
