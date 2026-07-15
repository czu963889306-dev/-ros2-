idf_component_register(
    SRCS "motor_driver.cpp"
    INCLUDE_DIRS "."
    REQUIRES driver esp_driver_ledc
)
