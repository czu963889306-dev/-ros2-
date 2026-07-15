#include "board.h"

#include "esp_log.h"

static const char *TAG = "BOARD_INIT";

// ================= 引脚定义 (私有) =================
const gpio_num_t kMotorLeftIn1 = GPIO_NUM_46;
const gpio_num_t kMotorLeftIn2 = GPIO_NUM_9;
const gpio_num_t kMotorRightIn1 = GPIO_NUM_16;
const gpio_num_t kMotorRightIn2 = GPIO_NUM_15;

const gpio_num_t kEncoderLeftA = GPIO_NUM_10;
const gpio_num_t kEncoderLeftB = GPIO_NUM_11;
const gpio_num_t kEncoderRightA = GPIO_NUM_17;
const gpio_num_t kEncoderRightB = GPIO_NUM_18;

const gpio_num_t kUltrasonicPin = GPIO_NUM_21;
const gpio_num_t kRgbPin = GPIO_NUM_47;
const gpio_num_t kStatusLedPin = GPIO_NUM_14;
const gpio_num_t kServoPin = GPIO_NUM_48;
const adc_unit_t kBatteryAdcUnit = ADC_UNIT_1;
const adc_channel_t kBatteryAdcChannel = ADC_CHANNEL_2; // GPIO3 on ESP32-S3.

// GPIO0/BOOT is a strapping/download pin. Using it as a runtime button is risky,
// but it is enabled here intentionally for the BOOT long-press recovery action.
const gpio_num_t kIo0ButtonPin = GPIO_NUM_0;

// ================= 对象实例化 =================
Lsm6ds3Imu imu(I2C_NUM_0, GPIO_NUM_12, GPIO_NUM_13);

At8236Motor motor_left(kMotorLeftIn1, kMotorLeftIn2);
At8236Motor motor_right(kMotorRightIn1, kMotorRightIn2);

QuadratureEncoder encoder_left(kEncoderLeftA, kEncoderLeftB);
QuadratureEncoder encoder_right(kEncoderRightA, kEncoderRightB);

MotionController robot(motor_left, motor_right, encoder_left, encoder_right);
UltrasonicSensor ultrasonic(kUltrasonicPin);
Ws2812Driver rgb_led(kRgbPin, 6);
StatusLed status_led(kStatusLedPin);
ButtonDriver io0_button(kIo0ButtonPin, true);
ServoDriver my_servo(kServoPin);
BatteryAdc battery_adc(kBatteryAdcUnit, kBatteryAdcChannel);

// ================= 统一初始化逻辑 =================
void board_init(void) {
    ESP_LOGI(TAG, "Initializing two-wheel differential hardware...");

    motor_left.Init();
    encoder_left.Init();
    motor_right.Init();
    encoder_right.Init();

    ultrasonic.Init();
    rgb_led.Init();
    status_led.Init();
    io0_button.Init();
    ESP_LOGW(TAG, "BOOT/IO0 is used as a runtime communication-mode button; holding it during reset may enter download mode.");
    my_servo.Init();

    if (imu.Init()) {
        ESP_LOGI(TAG, "IMU (LSM6DS3) initialized.");
    } else {
        ESP_LOGE(TAG, "IMU initialization failed.");
    }

    ESP_LOGI(TAG, "Two-wheel differential board ready.");
}
