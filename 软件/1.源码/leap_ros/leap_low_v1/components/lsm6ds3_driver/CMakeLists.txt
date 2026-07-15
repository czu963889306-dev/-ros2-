idf_component_register(
    SRCS "lsm6ds3_driver.cpp"
    INCLUDE_DIRS "."
    REQUIRES driver i2c_bus_lock esp_timer
)
