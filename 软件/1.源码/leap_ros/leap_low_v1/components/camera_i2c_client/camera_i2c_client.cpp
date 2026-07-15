#include "camera_i2c_client.h"

#include <stdio.h>
#include <string.h>

#include "cJSON.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "i2c_bus_lock.h"

namespace {

constexpr char kTag[] = "CAMERA_I2C";
constexpr TickType_t kClientMutexTimeout = pdMS_TO_TICKS(1000);
constexpr TickType_t kBusMutexTimeout = pdMS_TO_TICKS(200);
constexpr TickType_t kI2cTransactionTimeout = pdMS_TO_TICKS(120);
constexpr TickType_t kCommandQueueTimeout = pdMS_TO_TICKS(250);
constexpr TickType_t kPollPeriod = pdMS_TO_TICKS(1000);
constexpr TickType_t kProvisionResultTimeout = pdMS_TO_TICKS(1500);
constexpr TickType_t kProvisionResultPoll = pdMS_TO_TICKS(50);
constexpr uint32_t kReadAfterCommandDelayMs = 60;
constexpr size_t kCameraTaskCommandMax = 192;
constexpr UBaseType_t kCameraTaskQueueLen = 4;

SemaphoreHandle_t s_client_mutex = nullptr;
QueueHandle_t s_command_queue = nullptr;
bool s_camera_task_running = false;
i2c_port_t s_i2c_port = I2C_NUM_0;
uint8_t s_i2c_address = CAMERA_I2C_DEFAULT_ADDRESS;
bool s_initialized = false;
CameraI2cStatus s_status = {};

typedef struct {
    char command[kCameraTaskCommandMax];
} CameraTaskCommand;

bool ensure_mutex() {
    if (s_client_mutex != nullptr) {
        return true;
    }

    s_client_mutex = xSemaphoreCreateMutex();
    return s_client_mutex != nullptr;
}

void copy_string(char *dst, size_t dst_size, const char *src) {
    if (dst == nullptr || dst_size == 0) {
        return;
    }
    if (src == nullptr) {
        dst[0] = '\0';
        return;
    }

    strncpy(dst, src, dst_size - 1);
    dst[dst_size - 1] = '\0';
}

void copy_status(CameraI2cStatus *out) {
    if (out != nullptr) {
        *out = s_status;
    }
}

void reset_dynamic_status_locked(esp_err_t err) {
    s_status.valid = false;
    s_status.reachable = false;
    s_status.last_error = err;
    s_status.last_update_ms = static_cast<uint32_t>(esp_timer_get_time() / 1000ULL);
    s_status.raw_json[0] = '\0';
    s_status.ip[0] = '\0';
    s_status.mode[0] = '\0';
    s_status.name[0] = '\0';
    s_status.config_url[0] = '\0';
    s_status.stream_url[0] = '\0';
}

esp_err_t i2c_write_command(const char *command) {
    if (command == nullptr || command[0] == '\0') {
        return ESP_ERR_INVALID_ARG;
    }

    const size_t command_len = strlen(command);
    if (command_len > 255) {
        return ESP_ERR_INVALID_SIZE;
    }
    if (!shared_i2c_bus_lock_take(kBusMutexTimeout)) {
        return ESP_ERR_TIMEOUT;
    }

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    if (cmd == nullptr) {
        shared_i2c_bus_lock_give();
        return ESP_ERR_NO_MEM;
    }

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, static_cast<uint8_t>((s_i2c_address << 1) | I2C_MASTER_WRITE), true);
    i2c_master_write(
        cmd,
        const_cast<uint8_t *>(reinterpret_cast<const uint8_t *>(command)),
        command_len,
        true);
    i2c_master_stop(cmd);

    const esp_err_t err = i2c_master_cmd_begin(s_i2c_port, cmd, kI2cTransactionTimeout);
    i2c_cmd_link_delete(cmd);
    shared_i2c_bus_lock_give();
    return err;
}

esp_err_t i2c_read_raw(uint8_t *buffer, size_t buffer_size) {
    if (buffer == nullptr || buffer_size < 2) {
        return ESP_ERR_INVALID_ARG;
    }
    if (!shared_i2c_bus_lock_take(kBusMutexTimeout)) {
        return ESP_ERR_TIMEOUT;
    }

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    if (cmd == nullptr) {
        shared_i2c_bus_lock_give();
        return ESP_ERR_NO_MEM;
    }

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, static_cast<uint8_t>((s_i2c_address << 1) | I2C_MASTER_READ), true);
    i2c_master_read(cmd, buffer, buffer_size - 1, I2C_MASTER_ACK);
    i2c_master_read_byte(cmd, buffer + buffer_size - 1, I2C_MASTER_NACK);
    i2c_master_stop(cmd);

    const esp_err_t err = i2c_master_cmd_begin(s_i2c_port, cmd, kI2cTransactionTimeout);
    i2c_cmd_link_delete(cmd);
    shared_i2c_bus_lock_give();
    return err;
}

bool normalize_json_payload(const uint8_t *raw, size_t raw_len, char *out, size_t out_size) {
    if (raw == nullptr || out == nullptr || out_size == 0) {
        return false;
    }

    size_t start = raw_len;
    for (size_t i = 0; i < raw_len; ++i) {
        if (raw[i] == '{') {
            start = i;
            break;
        }
    }
    if (start == raw_len) {
        out[0] = '\0';
        return false;
    }

    size_t end = start;
    for (size_t i = start; i < raw_len; ++i) {
        if (raw[i] == '}') {
            end = i + 1;
        } else if (raw[i] == '\0' && end > start) {
            break;
        }
    }
    if (end <= start) {
        out[0] = '\0';
        return false;
    }

    size_t copy_len = end - start;
    if (copy_len >= out_size) {
        copy_len = out_size - 1;
    }
    memcpy(out, raw + start, copy_len);
    out[copy_len] = '\0';
    return true;
}

void copy_json_string(cJSON *root, const char *key, char *dst, size_t dst_size) {
    cJSON *item = cJSON_GetObjectItemCaseSensitive(root, key);
    if (cJSON_IsString(item) && item->valuestring != nullptr) {
        copy_string(dst, dst_size, item->valuestring);
    } else if (dst != nullptr && dst_size > 0) {
        dst[0] = '\0';
    }
}

esp_err_t parse_status_json_locked(const char *json) {
    cJSON *root = cJSON_Parse(json);
    if (root == nullptr || !cJSON_IsObject(root)) {
        if (root != nullptr) {
            cJSON_Delete(root);
        }
        reset_dynamic_status_locked(ESP_FAIL);
        copy_string(s_status.raw_json, sizeof(s_status.raw_json), json);
        return ESP_FAIL;
    }

    s_status.valid = true;
    s_status.reachable = true;
    s_status.last_error = ESP_OK;
    s_status.last_update_ms = static_cast<uint32_t>(esp_timer_get_time() / 1000ULL);
    copy_string(s_status.raw_json, sizeof(s_status.raw_json), json);
    copy_json_string(root, "ip", s_status.ip, sizeof(s_status.ip));
    copy_json_string(root, "mode", s_status.mode, sizeof(s_status.mode));
    copy_json_string(root, "name", s_status.name, sizeof(s_status.name));
    copy_json_string(root, "config_url", s_status.config_url, sizeof(s_status.config_url));
    copy_json_string(root, "stream_url", s_status.stream_url, sizeof(s_status.stream_url));

    cJSON_Delete(root);
    return ESP_OK;
}

esp_err_t read_status_locked(CameraI2cStatus *status) {
    uint8_t raw[CAMERA_I2C_STATUS_JSON_MAX] = {};
    char json[CAMERA_I2C_STATUS_JSON_MAX] = {};

    esp_err_t err = i2c_read_raw(raw, sizeof(raw));
    if (err != ESP_OK) {
        reset_dynamic_status_locked(err);
        copy_status(status);
        return err;
    }

    if (!normalize_json_payload(raw, sizeof(raw), json, sizeof(json))) {
        reset_dynamic_status_locked(ESP_FAIL);
        copy_status(status);
        return ESP_FAIL;
    }

    err = parse_status_json_locked(json);
    copy_status(status);
    return err;
}

esp_err_t refresh_with_command_locked(const char *command, CameraI2cStatus *status) {
    esp_err_t err = i2c_write_command(command);
    if (err != ESP_OK) {
        reset_dynamic_status_locked(err);
        copy_status(status);
        return err;
    }

    vTaskDelay(pdMS_TO_TICKS(kReadAfterCommandDelayMs));
    return read_status_locked(status);
}

esp_err_t build_json_provision_command(const char *ssid, const char *password, char *out, size_t out_size) {
    cJSON *root = cJSON_CreateObject();
    if (root == nullptr) {
        return ESP_ERR_NO_MEM;
    }

    cJSON_AddStringToObject(root, "ssid", ssid);
    cJSON_AddStringToObject(root, "password", password);
    char *printed = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    if (printed == nullptr) {
        return ESP_ERR_NO_MEM;
    }

    const size_t printed_len = strlen(printed);
    if (printed_len >= out_size) {
        cJSON_free(printed);
        return ESP_ERR_INVALID_SIZE;
    }

    memcpy(out, printed, printed_len + 1);
    cJSON_free(printed);
    return ESP_OK;
}

esp_err_t build_provision_command(
    const char *ssid,
    const char *password,
    camera_i2c_provision_format_t format,
    char *out,
    size_t out_size) {
    if (ssid == nullptr || ssid[0] == '\0' || password == nullptr || out == nullptr || out_size == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    int written = 0;
    switch (format) {
        case CAMERA_I2C_PROVISION_WIFI:
            written = snprintf(out, out_size, "WIFI:%s,%s", ssid, password);
            break;
        case CAMERA_I2C_PROVISION_SET_WIFI:
            written = snprintf(out, out_size, "SET_WIFI:%s,%s", ssid, password);
            break;
        case CAMERA_I2C_PROVISION_PROV:
            written = snprintf(out, out_size, "PROV:%s|%s", ssid, password);
            break;
        case CAMERA_I2C_PROVISION_JSON:
            return build_json_provision_command(ssid, password, out, out_size);
        default:
            return ESP_ERR_INVALID_ARG;
    }

    if (written < 0 || static_cast<size_t>(written) >= out_size) {
        return ESP_ERR_INVALID_SIZE;
    }
    return ESP_OK;
}

esp_err_t execute_refresh_command(const char *command, CameraI2cStatus *status) {
    if (!ensure_mutex()) {
        return ESP_ERR_NO_MEM;
    }
    if (xSemaphoreTake(s_client_mutex, kClientMutexTimeout) != pdTRUE) {
        return ESP_ERR_TIMEOUT;
    }
    if (!s_initialized) {
        reset_dynamic_status_locked(ESP_ERR_INVALID_STATE);
        copy_status(status);
        xSemaphoreGive(s_client_mutex);
        return ESP_ERR_INVALID_STATE;
    }

    const esp_err_t err = refresh_with_command_locked(command, status);
    xSemaphoreGive(s_client_mutex);
    return err;
}

esp_err_t submit_task_command(const char *command) {
    if (command == nullptr || command[0] == '\0') {
        return ESP_ERR_INVALID_ARG;
    }
    if (s_command_queue == nullptr || !s_camera_task_running) {
        return ESP_ERR_INVALID_STATE;
    }

    CameraTaskCommand task_command = {};
    copy_string(task_command.command, sizeof(task_command.command), command);
    if (xQueueSend(s_command_queue, &task_command, kCommandQueueTimeout) != pdTRUE) {
        return ESP_ERR_TIMEOUT;
    }
    return ESP_OK;
}

}  // namespace

void camera_task(void *arg) {
    (void)arg;
    CameraTaskCommand task_command = {};
    TickType_t last_poll = xTaskGetTickCount() - kPollPeriod;

    if (!ensure_mutex()) {
        ESP_LOGE(kTag, "camera_task failed to create client mutex");
        vTaskDelete(nullptr);
        return;
    }
    if (xSemaphoreTake(s_client_mutex, kClientMutexTimeout) != pdTRUE) {
        ESP_LOGE(kTag, "camera_task failed to take client mutex");
        vTaskDelete(nullptr);
        return;
    }
    if (!s_initialized) {
        xSemaphoreGive(s_client_mutex);
        ESP_LOGE(kTag, "camera_task started before camera_i2c_client_init");
        vTaskDelete(nullptr);
        return;
    }
    if (s_command_queue == nullptr) {
        s_command_queue = xQueueCreate(kCameraTaskQueueLen, sizeof(CameraTaskCommand));
        if (s_command_queue == nullptr) {
            xSemaphoreGive(s_client_mutex);
            ESP_LOGE(kTag, "camera_task failed to create command queue");
            vTaskDelete(nullptr);
            return;
        }
    }
    s_camera_task_running = true;
    xSemaphoreGive(s_client_mutex);
    ESP_LOGI(kTag, "camera_task started");

    while (true) {
        const TickType_t now = xTaskGetTickCount();
        const TickType_t elapsed = now - last_poll;
        const TickType_t wait_ticks = (elapsed >= kPollPeriod) ? 0 : (kPollPeriod - elapsed);

        if (s_command_queue != nullptr &&
            xQueueReceive(s_command_queue, &task_command, wait_ticks) == pdTRUE) {
            (void)execute_refresh_command(task_command.command, nullptr);
            last_poll = xTaskGetTickCount();
            continue;
        }

        (void)execute_refresh_command("STATUS", nullptr);
        last_poll = xTaskGetTickCount();
    }
}

esp_err_t camera_i2c_client_init(i2c_port_t port, uint8_t address) {
    if (address == 0 || address > 0x7F) {
        return ESP_ERR_INVALID_ARG;
    }
    if (!ensure_mutex()) {
        return ESP_ERR_NO_MEM;
    }

    if (xSemaphoreTake(s_client_mutex, kClientMutexTimeout) != pdTRUE) {
        return ESP_ERR_TIMEOUT;
    }

    s_i2c_port = port;
    s_i2c_address = address;
    memset(&s_status, 0, sizeof(s_status));
    s_status.last_error = ESP_ERR_INVALID_STATE;
    s_initialized = true;

    xSemaphoreGive(s_client_mutex);
    ESP_LOGI(kTag, "Camera I2C client initialized on port %d, address 0x%02X", port, address);
    return ESP_OK;
}

esp_err_t camera_i2c_client_get_cached_status(CameraI2cStatus *status) {
    if (status == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }
    if (!ensure_mutex()) {
        return ESP_ERR_NO_MEM;
    }
    if (xSemaphoreTake(s_client_mutex, kClientMutexTimeout) != pdTRUE) {
        return ESP_ERR_TIMEOUT;
    }

    copy_status(status);
    xSemaphoreGive(s_client_mutex);
    return ESP_OK;
}

esp_err_t camera_i2c_client_read_status(CameraI2cStatus *status) {
    if (!ensure_mutex()) {
        return ESP_ERR_NO_MEM;
    }
    if (xSemaphoreTake(s_client_mutex, kClientMutexTimeout) != pdTRUE) {
        return ESP_ERR_TIMEOUT;
    }
    if (!s_initialized) {
        reset_dynamic_status_locked(ESP_ERR_INVALID_STATE);
        copy_status(status);
        xSemaphoreGive(s_client_mutex);
        return ESP_ERR_INVALID_STATE;
    }

    const esp_err_t err = read_status_locked(status);
    xSemaphoreGive(s_client_mutex);
    return err;
}

esp_err_t camera_i2c_client_refresh_status(CameraI2cStatus *status) {
    return camera_i2c_client_refresh_status_with_command("STATUS", status);
}

esp_err_t camera_i2c_client_refresh_status_with_command(const char *command, CameraI2cStatus *status) {
    if (command == nullptr || command[0] == '\0') {
        return ESP_ERR_INVALID_ARG;
    }
    return execute_refresh_command(command, status);
}

esp_err_t camera_i2c_client_write_command(const char *command) {
    if (command == nullptr || command[0] == '\0') {
        return ESP_ERR_INVALID_ARG;
    }
    if (s_camera_task_running && s_command_queue != nullptr) {
        return submit_task_command(command);
    }

    if (!ensure_mutex()) {
        return ESP_ERR_NO_MEM;
    }
    if (xSemaphoreTake(s_client_mutex, kClientMutexTimeout) != pdTRUE) {
        return ESP_ERR_TIMEOUT;
    }
    if (!s_initialized) {
        reset_dynamic_status_locked(ESP_ERR_INVALID_STATE);
        xSemaphoreGive(s_client_mutex);
        return ESP_ERR_INVALID_STATE;
    }

    const esp_err_t err = refresh_with_command_locked(command, nullptr);
    xSemaphoreGive(s_client_mutex);
    return err;
}

esp_err_t camera_i2c_client_provision_wifi(
    const char *ssid,
    const char *password,
    camera_i2c_provision_format_t format) {
    char command[192] = {};
    const esp_err_t build_err = build_provision_command(
        ssid,
        password != nullptr ? password : "",
        format,
        command,
        sizeof(command));
    if (build_err != ESP_OK) {
        return build_err;
    }
    return camera_i2c_client_write_command(command);
}

esp_err_t camera_i2c_client_provision_wifi_and_read(
    const char *ssid,
    const char *password,
    camera_i2c_provision_format_t format,
    CameraI2cStatus *status) {
    char command[192] = {};
    const esp_err_t build_err = build_provision_command(
        ssid,
        password != nullptr ? password : "",
        format,
        command,
        sizeof(command));
    if (build_err != ESP_OK) {
        return build_err;
    }
    CameraI2cStatus before = {};
    (void)camera_i2c_client_get_cached_status(&before);

    esp_err_t err = camera_i2c_client_write_command(command);
    if (err != ESP_OK) {
        return err;
    }

    const TickType_t start = xTaskGetTickCount();
    CameraI2cStatus current = {};
    while ((xTaskGetTickCount() - start) < kProvisionResultTimeout) {
        err = camera_i2c_client_get_cached_status(&current);
        if (err != ESP_OK) {
            return err;
        }
        if (current.last_update_ms != before.last_update_ms) {
            if (status != nullptr) {
                *status = current;
            }
            return current.last_error;
        }
        vTaskDelay(kProvisionResultPoll);
    }

    err = camera_i2c_client_get_cached_status(&current);
    if (status != nullptr && err == ESP_OK) {
        *status = current;
    }
    return err == ESP_OK ? ESP_ERR_TIMEOUT : err;
}
