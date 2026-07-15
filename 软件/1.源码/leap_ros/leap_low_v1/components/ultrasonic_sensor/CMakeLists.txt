idf_component_register(
    SRCS "ultrasonic_sensor.cpp"
    INCLUDE_DIRS "."
    REQUIRES driver freertos esp_timer
)
