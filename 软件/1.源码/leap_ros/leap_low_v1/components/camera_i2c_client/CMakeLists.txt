idf_component_register(
    SRCS "camera_i2c_client.cpp"
    INCLUDE_DIRS "."
    REQUIRES driver freertos esp_timer i2c_bus_lock json
)
