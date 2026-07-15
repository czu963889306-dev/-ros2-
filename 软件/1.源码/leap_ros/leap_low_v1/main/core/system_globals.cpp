#include "system_globals.h"

volatile bool g_emergency_stop = false;
volatile bool g_motion_busy = false;
volatile WifiCommMode g_wifi_comm_mode = WifiCommMode::kMicroRos;
char g_microros_agent_ip[16] = "192.168.31.214";
uint16_t g_microros_agent_port = 8888;
char g_device_name[32] = "Maturo_UNKNOWN";

QueueHandle_t q_imu_state = nullptr;
QueueHandle_t q_motion_state = nullptr;
QueueHandle_t q_ultrasonic_state = nullptr;
QueueHandle_t q_lidar_state = nullptr;   
QueueHandle_t q_gamepad_state = nullptr;
QueueHandle_t q_temperature_state = nullptr;
QueueHandle_t q_battery_state = nullptr;


QueueHandle_t q_motion_cmd = nullptr;
QueueHandle_t q_rgb_cmd = nullptr;
QueueHandle_t q_servo_cmd = nullptr;
QueueHandle_t q_speedpid_cmd = nullptr;
QueueHandle_t q_postionpid_cmd = nullptr;

PidMsg g_speed_pid_state = {
    .kp = 1.0f,
    .ki = 6.0f,
    .kd = 0.0f,
};

PidMsg g_position_pid_state = {
    .kp = 2.1f,
    .ki = 0.0f,
    .kd = 0.5f,
};
