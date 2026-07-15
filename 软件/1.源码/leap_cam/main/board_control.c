#include "board_control.h"

#include <stdbool.h>

#include "driver/gpio.h"
#include "esp_check.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "wifi_manager.h"

#define BOOT_BUTTON_GPIO GPIO_NUM_0
#define WIFI_LED_GPIO    GPIO_NUM_2
#define WIFI_LED_ON      0
#define WIFI_LED_OFF     1

#define BUTTON_POLL_MS       50
#define BUTTON_LONG_PRESS_MS 3000
#define LED_FAST_BLINK_MS    120

static const char *TAG = "BOARD_CONTROL";

static TaskHandle_t s_control_task;

static void board_control_task(void *arg)
{
    bool led_on = false;
    uint32_t pressed_ms = 0;
    uint32_t blink_elapsed_ms = 0;
    bool long_press_handled = false;
    TickType_t last_wake = xTaskGetTickCount();

    while (true) {
        bool button_pressed = gpio_get_level(BOOT_BUTTON_GPIO) == 0;

        if (button_pressed) {
            if (pressed_ms < BUTTON_LONG_PRESS_MS) {
                pressed_ms += BUTTON_POLL_MS;
            }

            if (!long_press_handled && pressed_ms >= BUTTON_LONG_PRESS_MS) {
                long_press_handled = true;
                ESP_LOGI(TAG, "BOOT held for %d ms, clearing Wi-Fi config", BUTTON_LONG_PRESS_MS);

                esp_err_t err = wifi_manager_enter_config_mode();
                if (err != ESP_OK) {
                    ESP_LOGW(TAG, "Enter Wi-Fi config mode failed: 0x%x", err);
                }
            }
        } else {
            pressed_ms = 0;
            long_press_handled = false;
        }

        if (wifi_manager_is_connected() && !wifi_manager_is_ap_mode()) {
            led_on = true;
            blink_elapsed_ms = 0;
        } else {
            blink_elapsed_ms += BUTTON_POLL_MS;
            if (blink_elapsed_ms >= LED_FAST_BLINK_MS) {
                blink_elapsed_ms = 0;
                led_on = !led_on;
            }
        }
        gpio_set_level(WIFI_LED_GPIO, led_on ? WIFI_LED_ON : WIFI_LED_OFF);

        vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(BUTTON_POLL_MS));
    }
}

esp_err_t board_control_start(void)
{
    if (s_control_task) {
        return ESP_OK;
    }

    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << BOOT_BUTTON_GPIO),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    ESP_RETURN_ON_ERROR(gpio_config(&io_conf), TAG, "Configure BOOT button failed");

    io_conf.pin_bit_mask = (1ULL << WIFI_LED_GPIO);
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    ESP_RETURN_ON_ERROR(gpio_config(&io_conf), TAG, "Configure Wi-Fi LED failed");
    ESP_RETURN_ON_ERROR(gpio_set_level(WIFI_LED_GPIO, WIFI_LED_OFF), TAG, "Set Wi-Fi LED failed");

    BaseType_t ok = xTaskCreate(
        board_control_task,
        "board_control",
        3072,
        NULL,
        5,
        &s_control_task
    );
    return ok == pdPASS ? ESP_OK : ESP_ERR_NO_MEM;
}
