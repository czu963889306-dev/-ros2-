#include "app_runtime.h"

#include <stdint.h>
#include <stdio.h>

#include "board.h"
#include "camera_i2c_client.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "i2c_bus_lock.h"
#include "msg/gamepad_msg.h"
#include "msg/imu_msg.h"
#include "msg/battery_msg.h"
#include "msg/lidar_msg.h"
#include "msg/motion_msg.h"
#include "msg/pid_msg.h"
#include "msg/rgb_msg.h"
#include "msg/servo_msg.h"
#include "msg/temperature_msg.h"
#include "msg/ultrasonic_msg.h"
#include "nvs_flash.h"
#include "system_globals.h"
#include "wifi_app.h"

static const char *TAG = "APP_RUNTIME";

static bool s_runtime_started = false;
static TaskHandle_t s_wifi_start_task_handle = nullptr;

static void delayed_restart_task(void *p) {
    const uint32_t delay_ms = reinterpret_cast<uintptr_t>(p);
    vTaskDelay(pdMS_TO_TICKS(delay_ms));
    esp_restart();
}

static void start_comm_task(void) {
    if (g_wifi_comm_mode == WifiCommMode::kMavlinkUdp) {
        if (xTaskCreate(mavlink_udp_task, "mavlink_udp", 8192, NULL, 3, NULL) == pdPASS) {
            ESP_LOGI(TAG, "MAVLink UDP task started");
        } else {
            ESP_LOGE(TAG, "Failed to start MAVLink UDP task");
        }
        return;
    }

    if (xTaskCreate(microros_task, "microros", 12288, NULL, 4, NULL) == pdPASS) {
        ESP_LOGI(TAG, "micro-ROS task started");
    } else {
        ESP_LOGE(TAG, "Failed to start micro-ROS task");
    }
}

static void wifi_start_task(void *p) {
    const bool sta_connected = wifi_init_sta();
    if (!sta_connected) {
        wifi_init_softap();
        status_led.SetMode(StatusLedMode::kFastBlink);
    }

    xTaskCreate(wifi_provision_task, "wifi_provision", 6144, NULL, 3, NULL);
    if (sta_connected) {
        start_comm_task();
    } else {
        ESP_LOGW(TAG, "STA not connected; communication task is not started");
    }

    s_wifi_start_task_handle = nullptr;
    vTaskDelete(NULL);
}

static void init_device_name(void) {
    uint8_t mac[6] = {0};
    if (esp_read_mac(mac, ESP_MAC_WIFI_STA) != ESP_OK) {
        snprintf(g_device_name, sizeof(g_device_name), "Maturo_UNKNOWN");
        ESP_LOGW(TAG, "Failed to read STA MAC, using fallback device name");
        return;
    }

    snprintf(
        g_device_name,
        sizeof(g_device_name),
        "Maturo_%02X%02X%02X%02X%02X%02X",
        mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

static void init_runtime_config(void) {
    WifiRuntimeConfig config = {};
    esp_err_t err = wifi_load_runtime_config(&config);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGW(TAG, "Failed to load runtime config, using defaults: %s",
                 esp_err_to_name(err));
    }

    g_wifi_comm_mode = config.comm_mode;
    snprintf(g_microros_agent_ip, sizeof(g_microros_agent_ip), "%s",
             config.microros_agent_ip);
    g_microros_agent_port = config.microros_agent_port;

    ESP_LOGI(TAG, "Runtime config: comm=%s, micro-ROS agent=%s:%u",
             g_wifi_comm_mode == WifiCommMode::kMicroRos ? "micro_ros" : "mavlink_udp",
             g_microros_agent_ip,
             static_cast<unsigned>(g_microros_agent_port));
}

static void create_runtime_queues(void) {
    q_imu_state = xQueueCreate(1, sizeof(ImuMsg));
    q_motion_state = xQueueCreate(1, sizeof(MotionMsg));
    q_ultrasonic_state = xQueueCreate(1, sizeof(UltrasonicMsg));
    q_lidar_state = xQueueCreate(1, sizeof(LidarMsg));
    q_gamepad_state = xQueueCreate(1, sizeof(GamepadMsg));
    q_temperature_state = xQueueCreate(1, sizeof(TemperatureMsg));
    q_battery_state = xQueueCreate(1, sizeof(BatteryMsg));
    q_motion_cmd = xQueueCreate(1, sizeof(MotionMsg));
    q_rgb_cmd = xQueueCreate(1, sizeof(RgbMsg));
    q_servo_cmd = xQueueCreate(1, sizeof(ServoMsg));
    q_speedpid_cmd = xQueueCreate(1, sizeof(PidMsg));
    q_postionpid_cmd = xQueueCreate(1, sizeof(PidMsg));
}

void app_runtime_startup(void) {
    if (s_runtime_started) {
        return;
    }

    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    init_device_name();
    init_runtime_config();
    shared_i2c_bus_lock_init();
    board_init();
    err = camera_i2c_client_init(I2C_NUM_0, CAMERA_I2C_DEFAULT_ADDRESS);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Camera I2C client init failed: %s", esp_err_to_name(err));
    }
    status_led.SetMode(StatusLedMode::kOn);

    create_runtime_queues();

    xTaskCreate(imu_task, "imu", 4096, NULL, 5, NULL);
    xTaskCreate(motion_task, "motion", 4096, NULL, 5, NULL);
    // xTaskCreate(ultrasonic_task, "ultrasonic", 4096, NULL, 4, NULL);
    xTaskCreate(battery_task, "battery", 4096, NULL, 4, NULL);
    xTaskCreate(peripheral_task, "peripheral", 4096, NULL, 3, NULL);
    xTaskCreate(lidar_task, "lidar", 8192, NULL, 4, NULL);
    xTaskCreate(uart_cmd_task, "uart_cmd", 4096, NULL, 3, NULL);
    xTaskCreate(camera_task, "camera_task", 4096, NULL, 3, NULL);
    xTaskCreate(wifi_start_task, "wifi_start", 4096, NULL, 3, &s_wifi_start_task_handle);

    s_runtime_started = true;
    ESP_LOGI(TAG, "Leap low runtime started");
}

void app_runtime_restart_after_delay_ms(uint32_t delay_ms) {
    xTaskCreate(
        delayed_restart_task,
        "app_restart",
        2048,
        reinterpret_cast<void *>(static_cast<uintptr_t>(delay_ms)),
        3,
        NULL);
}
