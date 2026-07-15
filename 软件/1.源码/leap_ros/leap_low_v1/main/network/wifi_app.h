#ifndef WIFI_APP_H_
#define WIFI_APP_H_

#include <stddef.h>
#include <stdint.h>

#include "esp_err.h"
#include "esp_netif.h"
#include "system_globals.h"

static constexpr size_t kMicroRosAgentIpMaxLen = 16;

struct WifiRuntimeConfig {
    WifiCommMode comm_mode;
    char microros_agent_ip[kMicroRosAgentIpMaxLen];
    uint16_t microros_agent_port;
};

bool wifi_init_sta(void);
void wifi_init_softap(void);
void wifi_init_espnow();
esp_err_t wifi_save_sta_credentials(const char *ssid, const char *password);
esp_err_t wifi_get_softap_ip_info(esp_netif_ip_info_t *ip_info);
void wifi_get_default_runtime_config(WifiRuntimeConfig *config);
esp_err_t wifi_load_runtime_config(WifiRuntimeConfig *config);
esp_err_t wifi_save_runtime_config(const WifiRuntimeConfig *config);
esp_err_t wifi_save_comm_mode(WifiCommMode mode);

#endif
