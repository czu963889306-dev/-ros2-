#ifndef IMU_LSM6DS3_H_
#define IMU_LSM6DS3_H_

#include <stdint.h>
#include "driver/i2c.h"
#include "driver/gpio.h"
#include "esp_err.h"

class Lsm6ds3Imu {
 public:
  // 构造函数：指定 I2C 端口号以及 SDA、SCL 引脚
  Lsm6ds3Imu(i2c_port_t port, gpio_num_t sda_pin, gpio_num_t scl_pin);

  // 初始化 IMU 并配置 I2C (修改点：内部会自动执行上电零偏校准)
  bool Init();

  // 读取传感器数据并执行互补滤波更新姿态 (修改点：内部增加了零偏补偿和死区)
  esp_err_t Update();
  
  // 一次性获取四元数 (w, x, y, z)
  void GetQuaternion(float &w, float &x, float &y, float &z) const {
    w = q_w_; x = q_x_; y = q_y_; z = q_z_;
  }
  // 获取欧拉角 (度)
  float GetRoll() const { return roll_; }
  float GetPitch() const { return pitch_; }
  float GetYaw() const { return yaw_; }

  // 获取原始加速度 (g) 和 陀螺仪角速度 (dps)
  float GetAccX() const { return acc_x_; }
  float GetAccY() const { return acc_y_; }
  float GetAccZ() const { return acc_z_; }
  float GetGyroX() const { return gyro_x_; }
  float GetGyroY() const { return gyro_y_; }
  float GetGyroZ() const { return gyro_z_; }
  i2c_port_t GetPort() const { return port_; }

 private:
  i2c_port_t port_;
  gpio_num_t sda_pin_;
  gpio_num_t scl_pin_;

  float acc_x_, acc_y_, acc_z_;
  float gyro_x_, gyro_y_, gyro_z_;
  float roll_, pitch_, yaw_;
  float q_w_, q_x_, q_y_, q_z_;
  uint64_t last_us_;

  // ---------- 新增：零偏补偿相关内部变量与函数 ----------
  float gyro_bias_x_;
  float gyro_bias_y_;
  float gyro_bias_z_;
  void CalibrateGyro();
  // -----------------------------------------------------

  // 内部 I2C 读写封装
  esp_err_t WriteRegister8(uint8_t reg, uint8_t val);
  esp_err_t ReadRegisters(uint8_t reg, uint8_t *data, size_t len);
};

#endif  // IMU_LSM6DS3_H_
