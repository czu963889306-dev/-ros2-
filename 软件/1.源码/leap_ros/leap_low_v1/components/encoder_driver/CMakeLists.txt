idf_component_register(
    SRCS "encoder_driver.cpp"
    INCLUDE_DIRS "."
    REQUIRES driver esp_driver_pcnt
)
