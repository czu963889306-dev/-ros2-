#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "driver/i2c.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CAMERA_I2C_DEFAULT_ADDRESS 0x42
#define CAMERA_I2C_STATUS_JSON_MAX 256

typedef enum {
    CAMERA_I2C_PROVISION_WIFI = 0,
    CAMERA_I2C_PROVISION_SET_WIFI,
    CAMERA_I2C_PROVISION_PROV,
    CAMERA_I2C_PROVISION_JSON,
} camera_i2c_provision_format_t;

typedef struct {
    bool valid;
    bool reachable;
    esp_err_t last_error;
    uint32_t last_update_ms;
    char raw_json[CAMERA_I2C_STATUS_JSON_MAX];
    char ip[40];
    char mode[24];
    char name[40];
    char config_url[128];
    char stream_url[160];
} CameraI2cStatus;

esp_err_t camera_i2c_client_init(i2c_port_t port, uint8_t address);
void camera_task(void *arg);
esp_err_t camera_i2c_client_get_cached_status(CameraI2cStatus *status);
esp_err_t camera_i2c_client_read_status(CameraI2cStatus *status);
esp_err_t camera_i2c_client_refresh_status(CameraI2cStatus *status);
esp_err_t camera_i2c_client_refresh_status_with_command(const char *command, CameraI2cStatus *status);
esp_err_t camera_i2c_client_write_command(const char *command);
esp_err_t camera_i2c_client_provision_wifi(
    const char *ssid,
    const char *password,
    camera_i2c_provision_format_t format);
esp_err_t camera_i2c_client_provision_wifi_and_read(
    const char *ssid,
    const char *password,
    camera_i2c_provision_format_t format,
    CameraI2cStatus *status);

#ifdef __cplusplus
}
#endif
