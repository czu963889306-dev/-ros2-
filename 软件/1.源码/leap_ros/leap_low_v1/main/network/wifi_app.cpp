#include "wifi_app.h"

#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "nvs.h"
#include "system_globals.h"

static const char *TAG = "WIFI_APP";
static constexpr EventBits_t WIFI_CONNECTED_BIT = BIT0;
static constexpr uint32_t kWifiConnectTimeoutMs = 15000;
static const char *kWifiCfgNamespace = "wifi_cfg";
static const char *kWifiCfgSsidKey = "sta_ssid";
static const char *kWifiCfgPasswordKey = "sta_pass";
static const char *kRuntimeCfgNamespace = "runtime_cfg";
static const char *kRuntimeCfgCommModeKey = "comm_mode";
static const char *kRuntimeCfgMicroRosAgentIpKey = "uros_ip";
static const char *kRuntimeCfgMicroRosAgentPortKey = "uros_port";
static const char *kDefaultStaSsid = "Maturo";
static const char *kDefaultStaPassword = "maturo2026";
static const char *kDefaultMicroRosAgentIp = "192.168.31.214";
static constexpr uint16_t kDefaultMicroRosAgentPort = 8888;

static EventGroupHandle_t s_wifi_event_group = nullptr;
static esp_netif_t *s_sta_netif = nullptr;
static esp_netif_t *s_ap_netif = nullptr;
static bool s_event_handlers_registered = false;
static bool s_wifi_driver_initialized = false;
static bool s_sta_auto_connect_enabled = false;

static esp_err_t load_saved_sta_credentials(char *ssid, size_t ssid_size,
                                            char *password, size_t password_size) {
    nvs_handle_t handle;
    esp_err_t err = nvs_open(kWifiCfgNamespace, NVS_READONLY, &handle);
    if (err != ESP_OK) {
        return err;
    }

    size_t actual_ssid_size = ssid_size;
    size_t actual_password_size = password_size;
    err = nvs_get_str(handle, kWifiCfgSsidKey, ssid, &actual_ssid_size);
    if (err == ESP_OK) {
        err = nvs_get_str(handle, kWifiCfgPasswordKey, password, &actual_password_size);
        if (err == ESP_ERR_NVS_NOT_FOUND) {
            password[0] = '\0';
            err = ESP_OK;
        }
    }

    nvs_close(handle);
    return err;
}

static void fill_sta_config(wifi_config_t *wifi_config, const char *ssid, const char *password) {
    memset(wifi_config, 0, sizeof(*wifi_config));
    strncpy(reinterpret_cast<char *>(wifi_config->sta.ssid), ssid, sizeof(wifi_config->sta.ssid) - 1);
    strncpy(reinterpret_cast<char *>(wifi_config->sta.password), password, sizeof(wifi_config->sta.password) - 1);
    wifi_config->sta.threshold.authmode =
        (password[0] == '\0') ? WIFI_AUTH_OPEN : WIFI_AUTH_WPA2_PSK;
}

void wifi_get_default_runtime_config(WifiRuntimeConfig *config) {
    if (config == NULL) {
        return;
    }

    config->comm_mode = WifiCommMode::kMicroRos;
    strncpy(config->microros_agent_ip, kDefaultMicroRosAgentIp,
            sizeof(config->microros_agent_ip) - 1);
    config->microros_agent_ip[sizeof(config->microros_agent_ip) - 1] = '\0';
    config->microros_agent_port = kDefaultMicroRosAgentPort;
}

static bool is_valid_runtime_config(const WifiRuntimeConfig *config) {
    if (config == NULL || config->microros_agent_ip[0] == '\0' ||
        config->microros_agent_port == 0) {
        return false;
    }

    return config->comm_mode == WifiCommMode::kMavlinkUdp ||
           config->comm_mode == WifiCommMode::kMicroRos;
}

static void ensure_wifi_stack_ready() {
    static bool s_netif_initialized = false;
    static bool s_event_loop_initialized = false;

    if (!s_netif_initialized) {
        esp_err_t err = esp_netif_init();
        if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
            ESP_ERROR_CHECK(err);
        }
        s_netif_initialized = true;
    }

    if (!s_event_loop_initialized) {
        esp_err_t err = esp_event_loop_create_default();
        if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
            ESP_ERROR_CHECK(err);
        }
        s_event_loop_initialized = true;
    }

    if (!s_wifi_driver_initialized) {
        wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
        ESP_ERROR_CHECK(esp_wifi_init(&cfg));
        s_wifi_driver_initialized = true;
    }

    if (s_wifi_event_group == nullptr) {
        s_wifi_event_group = xEventGroupCreate();
    }
}

static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        if (s_sta_auto_connect_enabled) {
            esp_wifi_connect();
        }
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_sta_auto_connect_enabled) {
            ESP_LOGW(TAG, "STA disconnected, retrying...");
            esp_wifi_connect();
        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = static_cast<ip_event_got_ip_t *>(event_data);
        ESP_LOGI(TAG, "STA connected, got IP: " IPSTR, IP2STR(&event->ip_info.ip));
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

void wifi_init_softap(void) {
    ensure_wifi_stack_ready();
    s_sta_auto_connect_enabled = false;

    if (s_ap_netif == nullptr) {
        s_ap_netif = esp_netif_create_default_wifi_ap();
    }

    wifi_config_t wifi_config = {};
    strncpy(reinterpret_cast<char *>(wifi_config.ap.ssid), g_device_name,
            sizeof(wifi_config.ap.ssid) - 1);
    wifi_config.ap.ssid_len = strlen(g_device_name);
    wifi_config.ap.password[0] = 0;
    wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    wifi_config.ap.max_connection = 4;
    wifi_config.ap.channel = 1;

    esp_wifi_stop();
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));

    ESP_LOGW(TAG, "Provisioning AP started, SSID: %s", wifi_config.ap.ssid);
}

bool wifi_init_sta(void) {
    ensure_wifi_stack_ready();
    s_sta_auto_connect_enabled = true;

    if (s_sta_netif == nullptr) {
        s_sta_netif = esp_netif_create_default_wifi_sta();
    }

    if (!s_event_handlers_registered) {
        ESP_ERROR_CHECK(esp_event_handler_instance_register(
            WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL));
        ESP_ERROR_CHECK(esp_event_handler_instance_register(
            IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, NULL));
        s_event_handlers_registered = true;
    }

    char ssid[33] = {0};
    char password[65] = {0};
    esp_err_t creds_err = load_saved_sta_credentials(ssid, sizeof(ssid), password, sizeof(password));
    if (creds_err == ESP_OK) {
        ESP_LOGI(TAG, "Using saved STA credentials for SSID: %s", ssid);
    } else {
        strncpy(ssid, kDefaultStaSsid, sizeof(ssid) - 1);
        strncpy(password, kDefaultStaPassword, sizeof(password) - 1);
        ESP_LOGW(TAG, "Saved STA credentials not found, trying default SSID: %s", ssid);
    }

    wifi_config_t wifi_config = {};
    fill_sta_config(&wifi_config, ssid, password);

    xEventGroupClearBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    esp_wifi_stop();
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));

    ESP_LOGI(TAG, "Waiting for STA connection...");
    const EventBits_t bits = xEventGroupWaitBits(
        s_wifi_event_group,
        WIFI_CONNECTED_BIT,
        pdFALSE,
        pdFALSE,
        pdMS_TO_TICKS(kWifiConnectTimeoutMs));

    if ((bits & WIFI_CONNECTED_BIT) != 0) {
        ESP_LOGI(TAG, "STA connection established.");
        return true;
    }

    ESP_LOGW(TAG, "STA connect timeout after %lu ms.",
             static_cast<unsigned long>(kWifiConnectTimeoutMs));
    esp_wifi_disconnect();
    ESP_ERROR_CHECK(esp_wifi_stop());
    return false;
}

void wifi_init_espnow() {
    ensure_wifi_stack_ready();

    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE));
}

esp_err_t wifi_save_sta_credentials(const char *ssid, const char *password) {
    if (ssid == NULL || ssid[0] == '\0' || password == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    nvs_handle_t handle;
    esp_err_t err = nvs_open(kWifiCfgNamespace, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        return err;
    }

    err = nvs_set_str(handle, kWifiCfgSsidKey, ssid);
    if (err == ESP_OK) {
        err = nvs_set_str(handle, kWifiCfgPasswordKey, password);
    }
    if (err == ESP_OK) {
        err = nvs_commit(handle);
    }

    nvs_close(handle);
    return err;
}

esp_err_t wifi_load_runtime_config(WifiRuntimeConfig *config) {
    if (config == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    wifi_get_default_runtime_config(config);

    nvs_handle_t handle;
    esp_err_t err = nvs_open(kRuntimeCfgNamespace, NVS_READONLY, &handle);
    if (err != ESP_OK) {
        return err;
    }

    uint8_t comm_mode = static_cast<uint8_t>(config->comm_mode);
    esp_err_t read_err = nvs_get_u8(handle, kRuntimeCfgCommModeKey, &comm_mode);
    if (read_err == ESP_OK) {
        if (comm_mode == static_cast<uint8_t>(WifiCommMode::kMavlinkUdp) ||
            comm_mode == static_cast<uint8_t>(WifiCommMode::kMicroRos)) {
            config->comm_mode = static_cast<WifiCommMode>(comm_mode);
        }
    } else if (read_err != ESP_ERR_NVS_NOT_FOUND) {
        err = read_err;
    }

    size_t ip_len = sizeof(config->microros_agent_ip);
    read_err = nvs_get_str(handle, kRuntimeCfgMicroRosAgentIpKey,
                           config->microros_agent_ip, &ip_len);
    if (read_err != ESP_OK && read_err != ESP_ERR_NVS_NOT_FOUND) {
        err = read_err;
    }

    uint16_t port = config->microros_agent_port;
    read_err = nvs_get_u16(handle, kRuntimeCfgMicroRosAgentPortKey, &port);
    if (read_err == ESP_OK) {
        config->microros_agent_port = port;
    } else if (read_err != ESP_ERR_NVS_NOT_FOUND) {
        err = read_err;
    }

    nvs_close(handle);

    if (!is_valid_runtime_config(config)) {
        wifi_get_default_runtime_config(config);
        return ESP_ERR_INVALID_ARG;
    }
    return err;
}

esp_err_t wifi_save_runtime_config(const WifiRuntimeConfig *config) {
    if (!is_valid_runtime_config(config)) {
        return ESP_ERR_INVALID_ARG;
    }

    nvs_handle_t handle;
    esp_err_t err = nvs_open(kRuntimeCfgNamespace, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        return err;
    }

    err = nvs_set_u8(handle, kRuntimeCfgCommModeKey,
                     static_cast<uint8_t>(config->comm_mode));
    if (err == ESP_OK) {
        err = nvs_set_str(handle, kRuntimeCfgMicroRosAgentIpKey,
                          config->microros_agent_ip);
    }
    if (err == ESP_OK) {
        err = nvs_set_u16(handle, kRuntimeCfgMicroRosAgentPortKey,
                          config->microros_agent_port);
    }
    if (err == ESP_OK) {
        err = nvs_commit(handle);
    }

    nvs_close(handle);
    return err;
}

esp_err_t wifi_save_comm_mode(WifiCommMode mode) {
    WifiRuntimeConfig config = {};
    (void)wifi_load_runtime_config(&config);
    config.comm_mode = mode;
    return wifi_save_runtime_config(&config);
}

esp_err_t wifi_get_softap_ip_info(esp_netif_ip_info_t *ip_info) {
    if (ip_info == NULL || s_ap_netif == nullptr) {
        return ESP_ERR_INVALID_STATE;
    }
    return esp_netif_get_ip_info(s_ap_netif, ip_info);
}
