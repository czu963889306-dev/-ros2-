#include "camera_init.h"

#include "esp_camera.h"
#include "esp_log.h"

#define CAM_PIN_SIOD     4
#define CAM_PIN_SIOC     5
#define CAM_PIN_VSYNC    6
#define CAM_PIN_HREF     7
#define CAM_PIN_XCLK     15
#define CAM_PIN_PCLK     13
#define CAM_PIN_RESET    -1
#define CAM_PIN_PWDN     -1
#define CAM_PIN_D0       11
#define CAM_PIN_D1       9
#define CAM_PIN_D2       8
#define CAM_PIN_D3       10
#define CAM_PIN_D4       12
#define CAM_PIN_D5       18
#define CAM_PIN_D6       17
#define CAM_PIN_D7       16

static const char *TAG = "APP_CAMERA";

static esp_err_t camera_capture_self_test(void)
{
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
        ESP_LOGE(TAG, "Camera self-test capture failed");
        return ESP_FAIL;
    }

    esp_err_t err = ESP_OK;
    if (fb->format != PIXFORMAT_JPEG) {
        ESP_LOGE(TAG, "Camera self-test frame is not JPEG: format=%d", fb->format);
        err = ESP_FAIL;
    } else if (!fb->buf || fb->len < 2 || fb->buf[0] != 0xff || fb->buf[1] != 0xd8) {
        ESP_LOGE(TAG, "Camera self-test invalid JPEG frame: len=%u", (unsigned int)fb->len);
        err = ESP_FAIL;
    } else {
        ESP_LOGI(
            TAG,
            "Camera self-test passed: captured %ux%u JPEG, %u bytes",
            (unsigned int)fb->width,
            (unsigned int)fb->height,
            (unsigned int)fb->len
        );
    }

    esp_camera_fb_return(fb);
    return err;
}

esp_err_t camera_init(void)
{
    camera_config_t config = {
        .pin_pwdn       = CAM_PIN_PWDN,
        .pin_reset      = CAM_PIN_RESET,
        .pin_xclk       = CAM_PIN_XCLK,
        .pin_sccb_sda   = CAM_PIN_SIOD,
        .pin_sccb_scl   = CAM_PIN_SIOC,

        .pin_d7         = CAM_PIN_D7,
        .pin_d6         = CAM_PIN_D6,
        .pin_d5         = CAM_PIN_D5,
        .pin_d4         = CAM_PIN_D4,
        .pin_d3         = CAM_PIN_D3,
        .pin_d2         = CAM_PIN_D2,
        .pin_d1         = CAM_PIN_D1,
        .pin_d0         = CAM_PIN_D0,

        .pin_vsync      = CAM_PIN_VSYNC,
        .pin_href       = CAM_PIN_HREF,
        .pin_pclk       = CAM_PIN_PCLK,

        .xclk_freq_hz   = 20000000,
        .ledc_timer     = LEDC_TIMER_0,
        .ledc_channel   = LEDC_CHANNEL_0,

        .pixel_format   = PIXFORMAT_JPEG,
        .frame_size     = FRAMESIZE_SVGA,
        .jpeg_quality   = 10,
        .fb_count       = 2,
        .fb_location    = CAMERA_FB_IN_PSRAM,
        .grab_mode      = CAMERA_GRAB_WHEN_EMPTY,
    };

    // ESP_LOGI(TAG, "Camera pin map:");
    // ESP_LOGI(TAG, "PWDN=%d RESET=%d XCLK=%d",
    //          config.pin_pwdn,
    //          config.pin_reset,
    //          config.pin_xclk);
    // ESP_LOGI(TAG, "SDA=%d SCL=%d VSYNC=%d HREF=%d PCLK=%d",
    //          config.pin_sccb_sda,
    //          config.pin_sccb_scl,
    //          config.pin_vsync,
    //          config.pin_href,
    //          config.pin_pclk);
    // ESP_LOGI(TAG, "D0=%d D1=%d D2=%d D3=%d D4=%d D5=%d D6=%d D7=%d",
    //          config.pin_d0,
    //          config.pin_d1,
    //          config.pin_d2,
    //          config.pin_d3,
    //          config.pin_d4,
    //          config.pin_d5,
    //          config.pin_d6,
    //          config.pin_d7);

    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_camera_init failed: 0x%x", err);
        return err;
    }

    sensor_t *sensor = esp_camera_sensor_get();
    if (sensor) {
        ESP_LOGI(TAG, "Camera sensor detected successfully");
        sensor->set_vflip(sensor, 1);
        sensor->set_brightness(sensor, 0);
        sensor->set_ae_level(sensor, 0);
        sensor->set_exposure_ctrl(sensor, 1);
        sensor->set_gain_ctrl(sensor, 1);
        sensor->set_contrast(sensor, 1);
        sensor->set_saturation(sensor, 1);
    }

    ESP_LOGI(TAG, "Camera init success");
    return camera_capture_self_test();
}
