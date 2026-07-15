#include "system_globals.h"
#include "board.h"
#include "wifi_app.h"
#include "msg/rgb_msg.h"
#include "msg/servo_msg.h"
#include "msg/temperature_msg.h"

#include "driver/temperature_sensor.h"
#include "esp_err.h"
#include "esp_log.h"

static const char *TAG = "PERIPHERAL";
static const TickType_t kCommModeToggleMinTicks = pdMS_TO_TICKS(50);
static const TickType_t kCommModeToggleMaxTicks = pdMS_TO_TICKS(1200);
static const TickType_t kTemperatureSampleTicks = pdMS_TO_TICKS(1000);

static const char *wifi_comm_mode_name(WifiCommMode mode) {
    return mode == WifiCommMode::kMicroRos ? "micro-ROS" : "MAVLink UDP";
}

static void toggle_wifi_comm_mode() {
    g_wifi_comm_mode = (g_wifi_comm_mode == WifiCommMode::kMavlinkUdp)
        ? WifiCommMode::kMicroRos
        : WifiCommMode::kMavlinkUdp;
    const esp_err_t err = wifi_save_comm_mode(static_cast<WifiCommMode>(g_wifi_comm_mode));
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "Failed to persist Wi-Fi communication mode: %s", esp_err_to_name(err));
    }

    status_led.SetMode(g_wifi_comm_mode == WifiCommMode::kMicroRos
        ? StatusLedMode::kSlowBlink
        : StatusLedMode::kOn);
    ESP_LOGI(TAG, "Wi-Fi communication mode switched to %s",
             wifi_comm_mode_name(g_wifi_comm_mode));
}

static temperature_sensor_handle_t init_temperature_sensor() {
    temperature_sensor_handle_t sensor = nullptr;
    temperature_sensor_config_t config = TEMPERATURE_SENSOR_CONFIG_DEFAULT(-10, 80);

    esp_err_t err = temperature_sensor_install(&config, &sensor);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "Failed to install internal temperature sensor: %s", esp_err_to_name(err));
        return nullptr;
    }

    err = temperature_sensor_enable(sensor);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "Failed to enable internal temperature sensor: %s", esp_err_to_name(err));
        temperature_sensor_uninstall(sensor);
        return nullptr;
    }

    return sensor;
}

static void sample_temperature_sensor(temperature_sensor_handle_t sensor) {
    if (sensor == nullptr || q_temperature_state == nullptr) {
        return;
    }

    TemperatureMsg msg = {};
    esp_err_t err = temperature_sensor_get_celsius(sensor, &msg.celsius);
    msg.valid = (err == ESP_OK);
    if (!msg.valid) {
        ESP_LOGW(TAG, "Failed to read internal temperature: %s", esp_err_to_name(err));
    }
    xQueueOverwrite(q_temperature_state, &msg);
}

void peripheral_task(void *p) {
    RgbMsg rgb_cmd = {0, 255, 0}; 
    ServoMsg servo_cmd = {45.0f};
    TickType_t press_start_tick = 0;
    TickType_t last_temperature_sample_tick = 0;
    temperature_sensor_handle_t temperature_sensor = init_temperature_sensor();

    my_servo.SetAngle(servo_cmd.angle);
    
    while (1) {
        if (xQueueReceive(q_rgb_cmd, &rgb_cmd, 0) == pdTRUE) {
            for (int i = 0; i < 6; i++) {
                rgb_led.SetPixel(i, rgb_cmd.r, rgb_cmd.g, rgb_cmd.b);
            }
            rgb_led.Refresh();
        }
        if (xQueueReceive(q_servo_cmd, &servo_cmd, 0) == pdTRUE) {
            my_servo.SetAngle(servo_cmd.angle);
        }

        const TickType_t now = xTaskGetTickCount();
        if ((now - last_temperature_sample_tick) >= kTemperatureSampleTicks) {
            last_temperature_sample_tick = now;
            sample_temperature_sensor(temperature_sensor);
        }

        status_led.Update(now);
        const bool button_state_changed = io0_button.Update();
        const bool button_pressed = io0_button.IsPressed();

        if (button_state_changed) {
            if (button_pressed) {
                press_start_tick = now;
            } else {
                const TickType_t held_ticks = now - press_start_tick;
                if (press_start_tick != 0 &&
                    held_ticks >= kCommModeToggleMinTicks &&
                    held_ticks <= kCommModeToggleMaxTicks) {
                    toggle_wifi_comm_mode();
                }
                press_start_tick = 0;
            }
        }

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}
