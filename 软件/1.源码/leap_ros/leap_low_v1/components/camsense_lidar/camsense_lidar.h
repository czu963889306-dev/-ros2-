#ifndef CAMSENSE_LIDAR_H_
#define CAMSENSE_LIDAR_H_

#include <stdint.h>
#include "driver/uart.h"
#include "driver/gpio.h"

#define MAX_LIDAR_POINTS_PER_SCAN 500
#define MAX_LIDAR_PACKET_SIZE 128

struct LidarPoint {
    float angle;    
    float distance; 
};

class CamsenseLidar {
 public:
  CamsenseLidar(uart_port_t uart_num, gpio_num_t tx_pin, gpio_num_t rx_pin, int baud_rate = 115200);
  void Init();
  bool Poll(LidarPoint* out_scan, uint16_t* out_count);

 private:
  uart_port_t uart_num_;
  gpio_num_t tx_pin_;
  gpio_num_t rx_pin_;
  int baud_rate_;

  // 状态机变量
  int state_;
  uint8_t cInfo_;
  uint8_t N_;
  float last_angle_;
  
  const float center_base_angle_ = 18.5f; 

  LidarPoint current_scan_[MAX_LIDAR_POINTS_PER_SCAN];
  uint16_t current_point_count_;

  // 缓存区与逐字节解析状态
  uint8_t payload_buffer_[MAX_LIDAR_PACKET_SIZE];
  uint16_t payload_idx_;
  uint16_t expected_payload_len_;

  // 单字节处理状态机
  bool ProcessByte(uint8_t rx_byte, LidarPoint* out_scan, uint16_t* out_count);
  uint8_t ParsePacket(const uint8_t* buffer, uint8_t cInfo, uint8_t N, LidarPoint* packet_points);
};

#endif  // CAMSENSE_LIDAR_H_