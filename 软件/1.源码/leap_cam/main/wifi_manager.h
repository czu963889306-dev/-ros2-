#pragma once

#include <stdbool.h>

#include "esp_err.h"
#include "esp_netif_ip_addr.h"

#define WIFI_MANAGER_AP_IP "192.168.1.88"

esp_err_t wifi_manager_start(void);
esp_err_t wifi_manager_save_sta(const char *ssid, const char *password);
esp_err_t wifi_manager_clear_sta(void);
esp_err_t wifi_manager_enter_config_mode(void);
bool wifi_manager_is_ap_mode(void);
bool wifi_manager_is_connected(void);
const char *wifi_manager_get_mode_name(void);
const char *wifi_manager_get_device_name(void);
esp_ip4_addr_t wifi_manager_get_ip(void);
