#include "wifi_manager.h"

#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"

#include "esp_event.h"
#include "esp_check.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_mac.h"
#include "esp_wifi.h"
#include "nvs.h"

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1
#define WIFI_CONFIG_BIT    BIT2

#define WIFI_CONNECT_TIMEOUT_MS 15000
#define WIFI_MAX_RETRY          5

#define WIFI_NVS_NAMESPACE "wifi_cfg"
#define WIFI_NVS_SSID_KEY  "ssid"
#define WIFI_NVS_PASS_KEY  "pass"

#define CONFIG_AP_CHANNEL  1
#define CONFIG_AP_MAX_CONN 4
#define DEVICE_NAME_FALLBACK "Maturo_CAM_UNKNOWN"
#define WIFI_STATUS_LOG_STACK_SIZE 4096

static const char *TAG = "WIFI_MANAGER";

static EventGroupHandle_t s_wifi_event_group;
static esp_netif_t *s_sta_netif;
static esp_netif_t *s_ap_netif;
static esp_ip4_addr_t s_current_ip;
static bool s_ap_mode;
static bool s_connected;
static bool s_wifi_initialized;
static bool s_starting;
static bool s_config_mode_requested;
static int s_retry_num;
static char s_device_name[32] = DEVICE_NAME_FALLBACK;
static TaskHandle_t s_status_log_task_handle;

static esp_err_t clear_sta_config_nvs(void);
static esp_err_t start_ap(void);

static void wifi_status_log_task(void *arg)
{
    while (true) {
        esp_ip4_addr_t ip = s_current_ip;
        ESP_LOGI(TAG,
                 "设备名称: %s, IP地址: " IPSTR,
                 s_device_name,
                 IP2STR(&ip));
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

static void init_device_name(void)
{
    uint8_t mac[6] = {0};
    if (esp_read_mac(mac, ESP_MAC_WIFI_STA) != ESP_OK) {
        strlcpy(s_device_name, DEVICE_NAME_FALLBACK, sizeof(s_device_name));
        ESP_LOGW(TAG, "Failed to read STA MAC, using fallback device name");
        return;
    }

    snprintf(
        s_device_name,
        sizeof(s_device_name),
        "Maturo_CAM_%02X%02X%02X%02X%02X%02X",
        mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]
    );
}

static void wifi_event_handler(
    void *arg,
    esp_event_base_t event_base,
    int32_t event_id,
    void *event_data
)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        ESP_LOGI(TAG, "Wi-Fi STA started, connecting...");
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        s_connected = false;
        xEventGroupClearBits(s_wifi_event_group, WIFI_CONNECTED_BIT);

        if (!s_ap_mode && !s_config_mode_requested && s_retry_num < WIFI_MAX_RETRY) {
            s_retry_num++;
            ESP_LOGW(TAG, "Wi-Fi disconnected, retrying %d/%d",
                     s_retry_num,
                     WIFI_MAX_RETRY);
            esp_wifi_connect();
        } else if (!s_ap_mode) {
            ESP_LOGW(TAG, "Wi-Fi STA connect failed");
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;

        s_current_ip = event->ip_info.ip;
        s_retry_num = 0;

        ESP_LOGI(TAG, "Wi-Fi connected");
        ESP_LOGI(TAG, "STA IP: " IPSTR, IP2STR(&s_current_ip));

        wifi_ap_record_t ap_info;
        if (esp_wifi_sta_get_ap_info(&ap_info) == ESP_OK) {
            ESP_LOGI(TAG, "AP RSSI=%d dBm, primary channel=%u",
                     ap_info.rssi,
                     ap_info.primary);
        }

        s_connected = true;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

static bool load_sta_config(wifi_config_t *wifi_config)
{
    char ssid[sizeof(wifi_config->sta.ssid)] = {0};
    char password[sizeof(wifi_config->sta.password)] = {0};
    size_t ssid_len = sizeof(ssid);
    size_t pass_len = sizeof(password);

    nvs_handle_t nvs;
    esp_err_t err = nvs_open(WIFI_NVS_NAMESPACE, NVS_READONLY, &nvs);
    if (err == ESP_OK) {
        err = nvs_get_str(nvs, WIFI_NVS_SSID_KEY, ssid, &ssid_len);
        if (err == ESP_OK) {
            esp_err_t pass_err = nvs_get_str(nvs, WIFI_NVS_PASS_KEY, password, &pass_len);
            if (pass_err != ESP_OK && pass_err != ESP_ERR_NVS_NOT_FOUND) {
                err = pass_err;
            }
        }
        nvs_close(nvs);
    }

    if (err != ESP_OK || ssid[0] == '\0') {
        ESP_LOGI(TAG, "No saved STA config");
        memset(wifi_config, 0, sizeof(*wifi_config));
        return false;
    }

    ESP_LOGI(TAG, "Using saved STA config");
    memset(wifi_config, 0, sizeof(*wifi_config));
    strlcpy((char *)wifi_config->sta.ssid, ssid, sizeof(wifi_config->sta.ssid));
    strlcpy((char *)wifi_config->sta.password, password, sizeof(wifi_config->sta.password));
    return wifi_config->sta.ssid[0] != '\0';
}

static esp_err_t wifi_init_common(void)
{
    if (s_wifi_initialized) {
        return ESP_OK;
    }

    s_wifi_event_group = xEventGroupCreate();
    if (!s_wifi_event_group) {
        return ESP_ERR_NO_MEM;
    }

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    init_device_name();

    if (!s_status_log_task_handle) {
        BaseType_t task_ok = xTaskCreate(
            wifi_status_log_task,
            "wifi_status_log",
            WIFI_STATUS_LOG_STACK_SIZE,
            NULL,
            5,
            &s_status_log_task_handle
        );
        if (task_ok != pdPASS) {
            s_status_log_task_handle = NULL;
            return ESP_ERR_NO_MEM;
        }
    }

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_register(
        WIFI_EVENT,
        ESP_EVENT_ANY_ID,
        &wifi_event_handler,
        NULL
    ));

    ESP_ERROR_CHECK(esp_event_handler_register(
        IP_EVENT,
        IP_EVENT_STA_GOT_IP,
        &wifi_event_handler,
        NULL
    ));

    s_wifi_initialized = true;
    return ESP_OK;
}

static void destroy_sta_netif(void)
{
    if (s_sta_netif) {
        esp_netif_destroy(s_sta_netif);
        s_sta_netif = NULL;
    }
}

static esp_err_t stop_wifi_if_started(void)
{
    esp_err_t err = esp_wifi_stop();
    if (err != ESP_OK && err != ESP_ERR_WIFI_NOT_STARTED) {
        return err;
    }

    s_connected = false;
    if (s_wifi_event_group) {
        xEventGroupClearBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }

    vTaskDelay(pdMS_TO_TICKS(300));
    return ESP_OK;
}

static esp_err_t start_sta(wifi_config_t *wifi_config)
{
    s_sta_netif = esp_netif_create_default_wifi_sta();
    if (!s_sta_netif) {
        return ESP_FAIL;
    }

    s_ap_mode = false;
    s_connected = false;
    s_retry_num = 0;
    xEventGroupClearBits(s_wifi_event_group,
                         WIFI_CONNECTED_BIT | WIFI_FAIL_BIT | WIFI_CONFIG_BIT);

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));
    ESP_ERROR_CHECK(esp_wifi_set_bandwidth(WIFI_IF_STA, WIFI_BW_HT20));

    ESP_LOGI(TAG, "Waiting for STA connection...");
    EventBits_t bits = xEventGroupWaitBits(
        s_wifi_event_group,
        WIFI_CONNECTED_BIT | WIFI_FAIL_BIT | WIFI_CONFIG_BIT,
        pdFALSE,
        pdFALSE,
        pdMS_TO_TICKS(WIFI_CONNECT_TIMEOUT_MS)
    );

    if ((bits & WIFI_CONNECTED_BIT) && !s_config_mode_requested) {
        return ESP_OK;
    }

    ESP_LOGW(TAG, "STA unavailable, falling back to AP config mode");
    ESP_ERROR_CHECK_WITHOUT_ABORT(stop_wifi_if_started());
    destroy_sta_netif();
    return ESP_FAIL;
}

static esp_err_t start_ap(void)
{
    if (s_ap_mode) {
        return ESP_OK;
    }

    if (!s_ap_netif) {
        s_ap_netif = esp_netif_create_default_wifi_ap();
        if (!s_ap_netif) {
            return ESP_FAIL;
        }
    }

    esp_netif_ip_info_t ip_info;
    memset(&ip_info, 0, sizeof(ip_info));
    ESP_ERROR_CHECK(esp_netif_str_to_ip4(WIFI_MANAGER_AP_IP, &ip_info.ip));
    ESP_ERROR_CHECK(esp_netif_str_to_ip4(WIFI_MANAGER_AP_IP, &ip_info.gw));
    ESP_ERROR_CHECK(esp_netif_str_to_ip4("255.255.255.0", &ip_info.netmask));

    ESP_ERROR_CHECK(esp_netif_dhcps_stop(s_ap_netif));
    ESP_ERROR_CHECK(esp_netif_set_ip_info(s_ap_netif, &ip_info));
    ESP_ERROR_CHECK(esp_netif_dhcps_start(s_ap_netif));

    wifi_config_t ap_config = {
        .ap = {
            .channel = CONFIG_AP_CHANNEL,
            .max_connection = CONFIG_AP_MAX_CONN,
            .authmode = WIFI_AUTH_OPEN,
        },
    };
    strlcpy((char *)ap_config.ap.ssid, s_device_name, sizeof(ap_config.ap.ssid));
    ap_config.ap.ssid_len = strlen(s_device_name);

    s_current_ip = ip_info.ip;
    s_ap_mode = true;
    s_connected = false;

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "AP config mode started");
    ESP_LOGI(TAG, "AP SSID: %s", s_device_name);
    ESP_LOGI(TAG, "AP password: none");
    ESP_LOGI(TAG, "AP config URL: http://%s/", WIFI_MANAGER_AP_IP);
    return ESP_OK;
}

esp_err_t wifi_manager_start(void)
{
    ESP_RETURN_ON_ERROR(wifi_init_common(), TAG, "Wi-Fi common init failed");

    if (s_config_mode_requested) {
        ESP_RETURN_ON_ERROR(wifi_manager_clear_sta(), TAG, "Clear Wi-Fi config failed");
    }

    s_starting = true;

    wifi_config_t wifi_config;
    if (!s_config_mode_requested &&
        load_sta_config(&wifi_config) &&
        start_sta(&wifi_config) == ESP_OK) {
        s_starting = false;
        return ESP_OK;
    }

    esp_err_t err = start_ap();
    s_starting = false;
    return err;
}

esp_err_t wifi_manager_save_sta(const char *ssid, const char *password)
{
    size_t ssid_len = strlen(ssid);
    size_t pass_len = strlen(password);

    if (ssid_len == 0 || ssid_len > 32) {
        return ESP_ERR_INVALID_ARG;
    }

    if (pass_len > 0 && (pass_len < 8 || pass_len > 64)) {
        return ESP_ERR_INVALID_ARG;
    }

    nvs_handle_t nvs;
    ESP_RETURN_ON_ERROR(nvs_open(WIFI_NVS_NAMESPACE, NVS_READWRITE, &nvs),
                        TAG,
                        "Open Wi-Fi NVS failed");

    esp_err_t err = nvs_set_str(nvs, WIFI_NVS_SSID_KEY, ssid);
    if (err == ESP_OK) {
        err = nvs_set_str(nvs, WIFI_NVS_PASS_KEY, password);
    }
    if (err == ESP_OK) {
        err = nvs_commit(nvs);
    }

    nvs_close(nvs);
    return err;
}

esp_err_t wifi_manager_clear_sta(void)
{
    esp_err_t err = clear_sta_config_nvs();
    if (err != ESP_OK) {
        return err;
    }

    if (s_wifi_initialized) {
        esp_err_t restore_err = esp_wifi_restore();
        if (restore_err != ESP_OK && restore_err != ESP_ERR_WIFI_NOT_INIT) {
            ESP_LOGW(TAG, "Restore Wi-Fi persistent config failed: 0x%x", restore_err);
            return restore_err;
        }
    }

    return ESP_OK;
}

esp_err_t wifi_manager_enter_config_mode(void)
{
    s_config_mode_requested = true;

    if (s_wifi_event_group) {
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONFIG_BIT | WIFI_FAIL_BIT);
    }

    if (s_wifi_initialized && !s_starting && !s_ap_mode) {
        ESP_LOGI(TAG, "Switching to AP config mode");
        ESP_RETURN_ON_ERROR(stop_wifi_if_started(), TAG, "Stop Wi-Fi failed");
    }

    esp_err_t err = wifi_manager_clear_sta();
    if (err != ESP_OK) {
        return err;
    }

    if (!s_wifi_initialized || s_starting || s_ap_mode) {
        return ESP_OK;
    }

    destroy_sta_netif();
    return start_ap();
}

bool wifi_manager_is_ap_mode(void)
{
    return s_ap_mode;
}

bool wifi_manager_is_connected(void)
{
    return s_connected;
}

const char *wifi_manager_get_mode_name(void)
{
    return s_ap_mode ? "AP" : "STA";
}

const char *wifi_manager_get_device_name(void)
{
    return s_device_name;
}

esp_ip4_addr_t wifi_manager_get_ip(void)
{
    return s_current_ip;
}

static esp_err_t clear_sta_config_nvs(void)
{
    nvs_handle_t nvs;
    esp_err_t err = nvs_open(WIFI_NVS_NAMESPACE, NVS_READWRITE, &nvs);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        return ESP_OK;
    }
    if (err != ESP_OK) {
        return err;
    }

    err = nvs_erase_all(nvs);
    if (err == ESP_OK) {
        err = nvs_commit(nvs);
    }

    nvs_close(nvs);
    return err;
}
