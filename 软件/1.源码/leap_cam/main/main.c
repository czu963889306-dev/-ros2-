#include <stdio.h>

#include "esp_err.h"
#include "esp_log.h"
#include "esp_netif_ip_addr.h"
#include "nvs_flash.h"

#include "board_control.h"
#include "camera_init.h"
#include "i2c_slave_comm.h"
#include "udp_discovery.h"
#include "web_config.h"
#include "wifi_manager.h"
#include "web_mjpeg.h"

static const char *TAG = "OV3660_STREAM";

void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_ERROR_CHECK(board_control_start());
    ESP_ERROR_CHECK(wifi_manager_start());
    ESP_ERROR_CHECK(i2c_slave_comm_start());
    ESP_ERROR_CHECK(camera_init());

    httpd_handle_t config_server = start_config_webserver();
    if (!config_server) {
        ESP_LOGW(TAG, "Wi-Fi config webserver not started");
    }

    httpd_handle_t stream_server = start_webserver();
    if (!stream_server) {
        ESP_LOGW(TAG, "HTTP MJPEG stream webserver not started");
    }

    ESP_ERROR_CHECK(udp_discovery_start());
}
