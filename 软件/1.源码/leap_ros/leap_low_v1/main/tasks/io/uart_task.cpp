#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "driver/uart.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "system_globals.h"
#include "wifi_app.h"

static const char *TAG = "UART_FS";

#define UART_PORT_NUM UART_NUM_0
#define BUF_SIZE 8192

static constexpr const char *kWifiStartTag = "[WIFI_CONFIG]";
static constexpr const char *kWifiEndTag = "[WIFI_CONFIG_END]";

static void restart_after_delay_task(void *pvParameters) {
    vTaskDelay(pdMS_TO_TICKS(1200));
    esp_restart();
}

static void uart_send_response(const char *message) {
    uart_write_bytes(UART_PORT_NUM, message, strlen(message));
}

static void trim_in_place(char *text) {
    if (text == nullptr) {
        return;
    }

    size_t start = 0;
    size_t len = strlen(text);
    while (start < len && isspace(static_cast<unsigned char>(text[start]))) {
        ++start;
    }

    size_t end = len;
    while (end > start && isspace(static_cast<unsigned char>(text[end - 1]))) {
        --end;
    }

    if (start > 0) {
        memmove(text, text + start, end - start);
    }
    text[end - start] = '\0';
}

static bool extract_wifi_field(const char *payload, const char *key, char *out, size_t out_size) {
    if (out == nullptr || out_size == 0) {
        return false;
    }

    out[0] = '\0';
    const size_t key_len = strlen(key);
    const char *cursor = payload;

    while (cursor != nullptr && *cursor != '\0') {
        while (*cursor == '\r' || *cursor == '\n') {
            ++cursor;
        }
        if (*cursor == '\0') {
            break;
        }

        const char *line_end = strpbrk(cursor, "\r\n");
        const size_t line_len =
            (line_end == nullptr) ? strlen(cursor) : static_cast<size_t>(line_end - cursor);

        if (line_len > key_len + 1 && strncmp(cursor, key, key_len) == 0 &&
            cursor[key_len] == '=') {
            const char *value_start = cursor + key_len + 1;
            size_t value_len = line_len - key_len - 1;
            if (value_len >= out_size) {
                value_len = out_size - 1;
            }
            memcpy(out, value_start, value_len);
            out[value_len] = '\0';
            trim_in_place(out);
            return true;
        }

        cursor = (line_end == nullptr) ? nullptr : line_end + 1;
    }

    return false;
}

static void consume_buffer_prefix(char *buffer, int *buffer_len, int consume_len) {
    if (consume_len <= 0 || buffer == nullptr || buffer_len == nullptr || *buffer_len <= 0) {
        return;
    }

    if (consume_len >= *buffer_len) {
        *buffer_len = 0;
        buffer[0] = '\0';
        return;
    }

    memmove(buffer, buffer + consume_len, *buffer_len - consume_len);
    *buffer_len -= consume_len;
    buffer[*buffer_len] = '\0';
}

static void process_wifi_packet(char *packet_start, char *packet_end) {
    char *payload_start = packet_start + strlen(kWifiStartTag);
    while (*payload_start == '\r' || *payload_start == '\n') {
        ++payload_start;
    }
    const int payload_len = static_cast<int>(packet_end - payload_start);

    if (payload_len <= 0 || payload_len >= 256) {
        ESP_LOGW(TAG, "Invalid Wi-Fi config packet length: %d", payload_len);
        uart_send_response("[WIFI_CONFIG] ERROR: invalid payload\r\n");
        return;
    }

    char payload[256] = {0};
    memcpy(payload, payload_start, payload_len);
    payload[payload_len] = '\0';

    char ssid[33] = {0};
    char password[65] = {0};
    extract_wifi_field(payload, "ssid", ssid, sizeof(ssid));
    extract_wifi_field(payload, "password", password, sizeof(password));

    if (ssid[0] == '\0') {
        ESP_LOGW(TAG, "Wi-Fi config packet missing SSID");
        uart_send_response("[WIFI_CONFIG] ERROR: ssid required\r\n");
        return;
    }
    if (password[0] != '\0' && strlen(password) < 8) {
        ESP_LOGW(TAG, "Wi-Fi password too short for SSID: %s", ssid);
        uart_send_response("[WIFI_CONFIG] ERROR: password too short\r\n");
        return;
    }

    const esp_err_t err = wifi_save_sta_credentials(ssid, password);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to save Wi-Fi credentials: %s", esp_err_to_name(err));
        uart_send_response("[WIFI_CONFIG] ERROR: save failed\r\n");
        return;
    }

    ESP_LOGI(TAG, "Saved Wi-Fi credentials from UART for SSID: %s", ssid);
    uart_send_response("[WIFI_CONFIG] OK: saved, restarting\r\n");
    xTaskCreate(restart_after_delay_task, "uart_wifi_restart", 2048, NULL, 3, NULL);
}

void uart_cmd_task(void *pvParameters) {
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 0,
        .source_clk = UART_SCLK_DEFAULT,
        .flags = {},
    };
    uart_driver_install(UART_PORT_NUM, BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(UART_PORT_NUM, &uart_config);

    char *rx_buffer = static_cast<char *>(malloc(BUF_SIZE));
    if (rx_buffer == NULL) {
        ESP_LOGE(TAG, "Failed to allocate UART RX buffer");
        vTaskDelete(NULL);
        return;
    }

    int rx_length = 0;
    rx_buffer[0] = '\0';

    while (1) {
        int len = uart_read_bytes(
            UART_PORT_NUM,
            reinterpret_cast<uint8_t *>(rx_buffer) + rx_length,
            BUF_SIZE - rx_length - 1,
            pdMS_TO_TICKS(20));
        if (len <= 0) {
            continue;
        }

        rx_length += len;
        rx_buffer[rx_length] = '\0';

        bool processed_packet = true;
        while (processed_packet) {
            processed_packet = false;

            char *wifi_start = strstr(rx_buffer, kWifiStartTag);
            if (wifi_start == nullptr) {
                break;
            }

            const int prefix_len = static_cast<int>(wifi_start - rx_buffer);
            if (prefix_len > 0) {
                consume_buffer_prefix(rx_buffer, &rx_length, prefix_len);
                processed_packet = true;
                continue;
            }

            char *packet_end = strstr(wifi_start, kWifiEndTag);
            if (packet_end == nullptr) {
                break;
            }

            process_wifi_packet(wifi_start, packet_end);
            const int consume_len =
                static_cast<int>((packet_end + strlen(kWifiEndTag)) - rx_buffer);
            consume_buffer_prefix(rx_buffer, &rx_length, consume_len);
            processed_packet = true;
        }

        if (rx_length >= BUF_SIZE - 1) {
            ESP_LOGW(TAG, "Buffer overflow, clearing...");
            rx_length = 0;
            rx_buffer[0] = '\0';
        }
    }
}
