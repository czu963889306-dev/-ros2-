#include "camsense_lidar.h"
#include "esp_log.h"
#include <string.h>

static const char* TAG = "CamsenseLidar";

CamsenseLidar::CamsenseLidar(uart_port_t uart_num, gpio_num_t tx_pin, gpio_num_t rx_pin, int baud_rate)
    : uart_num_(uart_num),
      tx_pin_(tx_pin),
      rx_pin_(rx_pin),
      baud_rate_(baud_rate),
      state_(0), cInfo_(0), N_(0), last_angle_(0.0f),
      current_point_count_(0), payload_idx_(0), expected_payload_len_(0) {
}

void CamsenseLidar::Init() {
    uart_config_t uart_config = {};
    uart_config.baud_rate = baud_rate_;
    uart_config.data_bits = UART_DATA_8_BITS;
    uart_config.parity    = UART_PARITY_DISABLE;
    uart_config.stop_bits = UART_STOP_BITS_1;
    uart_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
    uart_config.source_clk = UART_SCLK_DEFAULT;

    // 适当加大 RX 缓冲
    ESP_ERROR_CHECK(uart_driver_install(uart_num_, 2048, 0, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(uart_num_, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(uart_num_, tx_pin_, rx_pin_, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    ESP_LOGI(TAG, "Lidar UART initialized on Port %d, Baud %d", uart_num_, baud_rate_);
}

bool CamsenseLidar::Poll(LidarPoint* out_scan, uint16_t* out_count) {
    size_t available = 0;
    uart_get_buffered_data_len(uart_num_, &available);
    
    if (available == 0) {
        return false;
    }

    // 限制单次最大读取量，确保不会霸占 CPU（留时间给系统喂狗）
    if (available > 512) {
        available = 512; 
    }

    uint8_t rx_buf[512];
    int read_len = uart_read_bytes(uart_num_, rx_buf, available, 0);
    
    bool circle_completed = false;
    
    // 在内存中极速跑状态机
    for (int i = 0; i < read_len; i++) {
        if (ProcessByte(rx_buf[i], out_scan, out_count)) {
            circle_completed = true;
        }
    }
    
    return circle_completed;
}

bool CamsenseLidar::ProcessByte(uint8_t rx_byte, LidarPoint* out_scan, uint16_t* out_count) {
    bool circle_completed = false;

    if (state_ == 0) {
        if (rx_byte == 0x55) state_ = 1;
    } else if (state_ == 1) {
        if (rx_byte == 0xAA) state_ = 2;
        else if (rx_byte == 0x55) state_ = 1; 
        else state_ = 0;
    } else if (state_ == 2) {
        cInfo_ = rx_byte;
        state_ = 3;
    } else if (state_ == 3) {
        N_ = rx_byte;
        expected_payload_len_ = N_ * cInfo_ + 8;
        
        // 【终极安全校验】：彻底封死脏数据！
        // Camsense协议 cInfo 必须是 2 (部分型号是3)，且单包点数 N_ 不可能超过 60
        if (expected_payload_len_ > MAX_LIDAR_PACKET_SIZE || cInfo_ == 0 || N_ > 60 || N_ == 0) {
            state_ = 0; // 只要有一点不合常理，直接当成乱码丢弃
        } else {
            payload_idx_ = 0;
            state_ = 4;
        }
    } else if (state_ == 4) {
        payload_buffer_[payload_idx_++] = rx_byte;
        
        if (payload_idx_ >= expected_payload_len_) {
            // 将数组稍微开大一点留足余量
            LidarPoint packet_points[80];
            uint8_t valid_points = ParsePacket(payload_buffer_, cInfo_, N_, packet_points);

            for (uint8_t i = 0; i < valid_points; i++) {
                const LidarPoint& pt = packet_points[i];

                if (pt.angle < last_angle_ - 100.0f && current_point_count_ > 50) {
                    if (out_scan != nullptr && out_count != nullptr) {
                        memcpy(out_scan, current_scan_, current_point_count_ * sizeof(LidarPoint));
                        *out_count = current_point_count_;
                    }
                    circle_completed = true;
                    current_point_count_ = 0; 
                }

                if (current_point_count_ < MAX_LIDAR_POINTS_PER_SCAN) {
                    current_scan_[current_point_count_] = pt;
                    current_point_count_++;
                }
                
                last_angle_ = pt.angle;
            }
            state_ = 0; 
        }
    }
    
    return circle_completed;
}

uint8_t CamsenseLidar::ParsePacket(const uint8_t* buffer, uint8_t cInfo, uint8_t N, LidarPoint* packet_points) {
    float FA = (buffer[3] - 0xA0 + buffer[2] / 256.0f) * 4.0f;
    int LA_start = 4 + N * cInfo;
    float LA = (buffer[LA_start + 1] - 0xA0 + buffer[LA_start] / 256.0f) * 4.0f;

    if (LA < FA) LA += 360.0f;
    float dAngle = (N > 1) ? (LA - FA) / (N - 1) : 0.0f;

    uint8_t valid_count = 0;

    for (int i = 0; i < N; i++) {
        int idx = 4 + i * cInfo;
        uint16_t dist = 0;
        uint8_t flag = 1; 

        if (cInfo == 0x02) { 
            dist = ((buffer[idx + 1] & 0x3F) << 8) | buffer[idx];
            flag = (buffer[idx + 1] >> 7) & 0x01;
        }

        if (dist == 0) flag = 1; 

        if (flag == 0) {  
            float angle = FA + dAngle * i + center_base_angle_;
            while (angle < 0.0f) angle += 360.0f;
            while (angle >= 360.0f) angle -= 360.0f;
            
            // 【硬性边界保护】：防止哪怕逃过前面校验的脏数据越界
            if (valid_count < 80) {
                packet_points[valid_count].angle = angle;
                packet_points[valid_count].distance = static_cast<float>(dist);
                valid_count++;
            }
        }
    }
    return valid_count;
}