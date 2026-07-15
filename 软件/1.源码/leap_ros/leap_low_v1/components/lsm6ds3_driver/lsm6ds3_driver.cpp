#include "lsm6ds3_driver.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "i2c_bus_lock.h"
#include <cmath>

static const char* TAG = "LSM6DS3_IMU";

// 寄存器地址常量定义
constexpr uint8_t kLsm6ds3Addr = 0x6A;
constexpr uint8_t kRegWhoAmI   = 0x0F;
constexpr uint8_t kValWhoAmI   = 0x6A;
constexpr uint8_t kRegCtrl1Xl  = 0x10;
constexpr uint8_t kRegCtrl2G   = 0x11;
constexpr uint8_t kRegCtrl3C   = 0x12;
constexpr uint8_t kRegOutxLG   = 0x22;

Lsm6ds3Imu::Lsm6ds3Imu(i2c_port_t port, gpio_num_t sda_pin, gpio_num_t scl_pin)
    : port_(port), sda_pin_(sda_pin), scl_pin_(scl_pin),
      acc_x_(0.0f), acc_y_(0.0f), acc_z_(0.0f),
      gyro_x_(0.0f), gyro_y_(0.0f), gyro_z_(0.0f),
      roll_(0.0f), pitch_(0.0f), yaw_(0.0f),
      q_w_(1.0f), q_x_(0.0f), q_y_(0.0f), q_z_(0.0f),
      last_us_(0),
      gyro_bias_x_(0.0f), gyro_bias_y_(0.0f), gyro_bias_z_(0.0f) { // 初始化零偏
}

// ---------------- 新增：陀螺仪静态校准函数 ----------------
void Lsm6ds3Imu::CalibrateGyro() {
  ESP_LOGI(TAG, "Starting Gyro Calibration. PLEASE KEEP SENSOR STILL...");
  float sum_gx = 0, sum_gy = 0, sum_gz = 0;
  uint8_t buffer[6];
  const uint16_t kSamples = 300; // 采样300次，每次10ms，总计约3秒

  for (uint16_t i = 0; i < kSamples; ++i) {
    if (ReadRegisters(kRegOutxLG, buffer, 6) == ESP_OK) {
      int16_t gx = (buffer[1] << 8) | buffer[0];
      int16_t gy = (buffer[3] << 8) | buffer[2];
      int16_t gz = (buffer[5] << 8) | buffer[4];

      // ±250 dps -> 8.75 mdps/LSB
      sum_gx += gx * 8.75f / 1000.0f; 
      sum_gy += gy * 8.75f / 1000.0f;
      sum_gz += gz * 8.75f / 1000.0f;
    }
    vTaskDelay(pdMS_TO_TICKS(10)); // 匹配 104Hz 的数据更新率
  }

  gyro_bias_x_ = sum_gx / kSamples;
  gyro_bias_y_ = sum_gy / kSamples;
  gyro_bias_z_ = sum_gz / kSamples;

  ESP_LOGI(TAG, "Calibration Done! Bias: X=%.3f, Y=%.3f, Z=%.3f", 
           gyro_bias_x_, gyro_bias_y_, gyro_bias_z_);
}
// ---------------------------------------------------------

bool Lsm6ds3Imu::Init() {
  // 配置 I2C 主机模式
  i2c_config_t conf = {};
  conf.mode = I2C_MODE_MASTER;
  conf.sda_io_num = sda_pin_;
  conf.scl_io_num = scl_pin_;
  conf.sda_pullup_en = true;
  conf.scl_pullup_en = true;
  conf.master.clk_speed = 400000;
  
  ESP_ERROR_CHECK(i2c_param_config(port_, &conf));
  
  // 忽略 ESP_ERR_INVALID_STATE 防止其他外设已经注册过该 I2C 端口导致崩溃
  esp_err_t err = i2c_driver_install(port_, conf.mode, 0, 0, 0);
  if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
      ESP_LOGE(TAG, "I2C driver install failed");
      return false;
  }

  // 验证 WHO_AM_I
  uint8_t who = 0;
  if (ReadRegisters(kRegWhoAmI, &who, 1) != ESP_OK || who != kValWhoAmI) {
    ESP_LOGE(TAG, "WHO_AM_I check failed. Expected: 0x%02X, Got: 0x%02X", kValWhoAmI, who);
    return false;
  }

  // 初始化陀螺仪和加速度计配置
  WriteRegister8(kRegCtrl1Xl, 0x40); // 加速度计：104Hz, ±2g
  // 【重要修改】：量程从 ±500dps 降至 ±250dps (0x40)，提升低速旋转时的分辨率
  WriteRegister8(kRegCtrl2G, 0x40);  
  WriteRegister8(kRegCtrl3C, 0x04);  // BDU=1, 地址自动递增

  // 执行上电零偏校准 (耗时约 3 秒)
  CalibrateGyro();

  // 状态复位
  roll_ = 0.0f;
  pitch_ = 0.0f;
  yaw_ = 0.0f;
  last_us_ = esp_timer_get_time();
  
  ESP_LOGI(TAG, "LSM6DS3 Initialized Successfully");
  return true;
}

esp_err_t Lsm6ds3Imu::Update() {
  uint8_t buffer[12];
  if (ReadRegisters(kRegOutxLG, buffer, 12) != ESP_OK) {
    return ESP_FAIL;
  }

  // 数据拼接 (LSM6DS3 是小端模式)
  int16_t gx = (buffer[1] << 8) | buffer[0];
  int16_t gy = (buffer[3] << 8) | buffer[2];
  int16_t gz = (buffer[5] << 8) | buffer[4];
  int16_t ax = (buffer[7] << 8) | buffer[6];
  int16_t ay = (buffer[9] << 8) | buffer[8];
  int16_t az = (buffer[11] << 8) | buffer[10];

  // 【重要修改】：转换为物理单位 (±250dps -> 8.75mdps/LSB) 并减去校准零偏
  gyro_x_ = (gx * 8.75f / 1000.0f) - gyro_bias_x_;  
  gyro_y_ = (gy * 8.75f / 1000.0f) - gyro_bias_y_;
  gyro_z_ = (gz * 8.75f / 1000.0f) - gyro_bias_z_;

  // 【重要修改】：引入死区，如果角速度极小 (<0.2 dps)，则视为未旋转，防止底噪被积分
  constexpr float kGyroDeadzone = 0.2f; 
  if (std::abs(gyro_x_) < kGyroDeadzone) gyro_x_ = 0.0f;
  if (std::abs(gyro_y_) < kGyroDeadzone) gyro_y_ = 0.0f;
  if (std::abs(gyro_z_) < kGyroDeadzone) gyro_z_ = 0.0f;

  acc_x_  = ax * 0.061f / 1000.0f; // ±2g -> 0.061 mg/LSB
  acc_y_  = ay * 0.061f / 1000.0f;
  acc_z_  = az * 0.061f / 1000.0f;

  // 获取积分时间 dt
  uint64_t now = esp_timer_get_time();
  float dt = (now - last_us_) / 1000000.0f;
  last_us_ = now;

  // 通过加速度计计算静态倾角 (弧度转角度)
  float roll_acc  = std::atan2(acc_y_, std::sqrt(acc_x_ * acc_x_ + acc_z_ * acc_z_)) * 57.29578f;
  float pitch_acc = std::atan2(-acc_x_, std::sqrt(acc_y_ * acc_y_ + acc_z_ * acc_z_)) * 57.29578f;

  // -------- 互补滤波算法 --------
  constexpr float kAlpha = 0.95f; // 信任陀螺仪的比例
  
  roll_  = kAlpha * (roll_  + gyro_x_ * dt) + (1.0f - kAlpha) * roll_acc;
  pitch_ = kAlpha * (pitch_ + gyro_y_ * dt) + (1.0f - kAlpha) * pitch_acc;
  yaw_  += gyro_z_ * dt;

  // 限制 Yaw 角在 [-180, 180] 之间
  if (yaw_ > 180.0f) yaw_ -= 360.0f;
  if (yaw_ < -180.0f) yaw_ += 360.0f;
  
  // 1. 将角度转换为弧度，并除以 2 
  constexpr float kDegToRadHalf = 0.01745329252f * 0.5f; 
  float cy = std::cos(yaw_ * kDegToRadHalf);
  float sy = std::sin(yaw_ * kDegToRadHalf);
  float cp = std::cos(pitch_ * kDegToRadHalf);
  float sp = std::sin(pitch_ * kDegToRadHalf);
  float cr = std::cos(roll_ * kDegToRadHalf);
  float sr = std::sin(roll_ * kDegToRadHalf);

  // 2. 根据 Z(Yaw) - Y(Pitch) - X(Roll) 的标准航空旋转顺序计算四元数
  q_w_ = cr * cp * cy + sr * sp * sy;
  q_x_ = sr * cp * cy - cr * sp * sy;
  q_y_ = cr * sp * cy + sr * cp * sy;
  q_z_ = cr * cp * sy - sr * sp * cy;

  return ESP_OK;
}

// ---------------- 内部 I2C 驱动封装 ----------------

esp_err_t Lsm6ds3Imu::WriteRegister8(uint8_t reg, uint8_t val) {
  if (!shared_i2c_bus_lock_take(pdMS_TO_TICKS(100))) {
    return ESP_ERR_TIMEOUT;
  }

  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (kLsm6ds3Addr << 1) | I2C_MASTER_WRITE, true);
  i2c_master_write_byte(cmd, reg, true);
  i2c_master_write_byte(cmd, val, true);
  i2c_master_stop(cmd);
  esp_err_t ret = i2c_master_cmd_begin(port_, cmd, pdMS_TO_TICKS(50));
  i2c_cmd_link_delete(cmd);
  shared_i2c_bus_lock_give();
  return ret;
}

esp_err_t Lsm6ds3Imu::ReadRegisters(uint8_t reg, uint8_t *data, size_t len) {
  if (!shared_i2c_bus_lock_take(pdMS_TO_TICKS(100))) {
    return ESP_ERR_TIMEOUT;
  }

  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (kLsm6ds3Addr << 1) | I2C_MASTER_WRITE, true);
  i2c_master_write_byte(cmd, reg, true);
  
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (kLsm6ds3Addr << 1) | I2C_MASTER_READ, true);
  if (len > 1) {
    i2c_master_read(cmd, data, len - 1, I2C_MASTER_ACK);
  }
  i2c_master_read_byte(cmd, data + len - 1, I2C_MASTER_NACK);
  i2c_master_stop(cmd);
  
  esp_err_t ret = i2c_master_cmd_begin(port_, cmd, pdMS_TO_TICKS(50));
  i2c_cmd_link_delete(cmd);
  shared_i2c_bus_lock_give();
  return ret;
}
