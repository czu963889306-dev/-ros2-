#include "system_globals.h"
#include "mavlink_common.h"
#include "msg/motion_msg.h"
#include "msg/rgb_msg.h"
#include "msg/servo_msg.h"
#include "msg/imu_msg.h"
#include "msg/ultrasonic_msg.h"
#include "msg/lidar_msg.h"
#include "msg/pid_msg.h"
#include "msg/temperature_msg.h"
#include "msg/battery_msg.h"
#include "board.h"

#include "esp_heap_caps.h"
#include "esp_flash.h"
#include "esp_partition.h"
#include "esp_wifi.h"
#include "freertos/task.h"
#include <stdio.h>
#include <string.h>
#include <math.h> 
#include <stdint.h>
#include <stddef.h>
#include "my_robot/mavlink.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif
#define DEG2RAD(x) ((x) * M_PI / 180.0f)

#define MATURO_WHEEL_DIAMETER_MM 65.0f
#define MATURO_OBSTACLE_DISTANCE_BINS 72
#define MATURO_OBSTACLE_BIN_WIDTH_DEG 5
#define MATURO_OBSTACLE_MIN_CM 2
#define MATURO_OBSTACLE_MAX_CM 1200

#define MATURO_MAV_CMD_MOVE_RELATIVE MAV_CMD_USER_2
#define MATURO_MAV_CMD_SET_PID MAV_CMD_USER_4
#define MATURO_DEBUG_MOTION_BUSY 1

enum MaturoPidTarget : uint8_t {
    MATURO_PID_SPEED = 1,
    MATURO_PID_POSITION = 2,
};

enum MaturoMotionMode : uint8_t {
    MATURO_MOTION_VELOCITY = 0,
    MATURO_MOTION_POSITION = 1,
    MATURO_MOTION_RELATIVE = 2,
    MATURO_MOTION_WHEEL_SPEED = 6,
};

enum MaturoParamTarget : uint8_t {
    MATURO_PARAM_SPEED_PID = 1,
    MATURO_PARAM_POSITION_PID = 2,
};

enum MaturoParamField : uint8_t {
    MATURO_PARAM_FIELD_KP = 1,
    MATURO_PARAM_FIELD_KI = 2,
    MATURO_PARAM_FIELD_KD = 3,
    MATURO_PARAM_FIELD_BOOL = 4,
};

struct MaturoParamDef {
    const char *name;
    uint8_t target;
    uint8_t field;
    uint8_t type;
};

static uint8_t s_mavlink_motion_mode = MATURO_MOTION_VELOCITY;

struct MavlinkTxRoute {
    const MavlinkTransport *transports;
    size_t transport_count;
};

static constexpr MaturoParamDef kMaturoParams[] = {
    {"SPD_KP", MATURO_PARAM_SPEED_PID, MATURO_PARAM_FIELD_KP, MAV_PARAM_TYPE_REAL32},
    {"SPD_KI", MATURO_PARAM_SPEED_PID, MATURO_PARAM_FIELD_KI, MAV_PARAM_TYPE_REAL32},
    {"SPD_KD", MATURO_PARAM_SPEED_PID, MATURO_PARAM_FIELD_KD, MAV_PARAM_TYPE_REAL32},
    {"POS_KP", MATURO_PARAM_POSITION_PID, MATURO_PARAM_FIELD_KP, MAV_PARAM_TYPE_REAL32},
    {"POS_KI", MATURO_PARAM_POSITION_PID, MATURO_PARAM_FIELD_KI, MAV_PARAM_TYPE_REAL32},
    {"POS_KD", MATURO_PARAM_POSITION_PID, MATURO_PARAM_FIELD_KD, MAV_PARAM_TYPE_REAL32},
};

static constexpr uint16_t kMaturoParamCount =
    static_cast<uint16_t>(sizeof(kMaturoParams) / sizeof(kMaturoParams[0]));

static float WheelMmsToRpm(float velocity_mms) {
    return velocity_mms * 60.0f / (M_PI * MATURO_WHEEL_DIAMETER_MM);
}

static uint16_t ClampDistanceCm(float distance_cm) {
    if (distance_cm < 0.0f) {
        return 0;
    }
    if (distance_cm > 65535.0f) {
        return 65535;
    }
    return static_cast<uint16_t>(distance_cm);
}

static uint8_t ClampUint8(float value) {
    if (value < 0.0f) {
        return 0;
    }
    if (value > 255.0f) {
        return 255;
    }
    return static_cast<uint8_t>(value);
}

static int8_t ClampTemperatureInt8(float value) {
    if (!isfinite(value)) {
        return INT8_MAX;
    }
    if (value > 126.0f) {
        return 126;
    }
    if (value < -128.0f) {
        return -128;
    }
    return static_cast<int8_t>(lroundf(value));
}

static uint32_t BytesToKiB(uint64_t bytes) {
    return static_cast<uint32_t>(bytes / 1024ULL);
}

static uint32_t KbpsToKibPerSecond(uint32_t kbps) {
    return (kbps * 1000UL) / 8UL / 1024UL;
}

static uint16_t MillimetersToObstacleCm(uint16_t distance_mm) {
    if (distance_mm == 0) {
        return MATURO_OBSTACLE_MAX_CM + 1;
    }

    uint32_t distance_cm = (static_cast<uint32_t>(distance_mm) + 5U) / 10U;
    if (distance_cm < MATURO_OBSTACLE_MIN_CM) {
        return MATURO_OBSTACLE_MIN_CM;
    }
    if (distance_cm > MATURO_OBSTACLE_MAX_CM) {
        return MATURO_OBSTACLE_MAX_CM + 1;
    }
    return static_cast<uint16_t>(distance_cm);
}

static void FillObstacleDistanceBins(const LidarMsg &lidar, uint16_t *distances_cm) {
    for (int bin = 0; bin < MATURO_OBSTACLE_DISTANCE_BINS; ++bin) {
        distances_cm[bin] = MATURO_OBSTACLE_MAX_CM + 1;
        const int start_deg = bin * MATURO_OBSTACLE_BIN_WIDTH_DEG;
        const int end_deg = start_deg + MATURO_OBSTACLE_BIN_WIDTH_DEG;

        for (int deg = start_deg; deg < end_deg; ++deg) {
            const uint16_t distance_cm = MillimetersToObstacleCm(lidar.distances[deg]);
            if (distance_cm < distances_cm[bin]) {
                distances_cm[bin] = distance_cm;
            }
        }
    }
}

static uint32_t GetDefaultHeapTotalAsKib() {
    return BytesToKiB(heap_caps_get_total_size(MALLOC_CAP_DEFAULT));
}

static uint32_t GetDefaultHeapUsedAsKib() {
    const size_t total = heap_caps_get_total_size(MALLOC_CAP_DEFAULT);
    const size_t free = heap_caps_get_free_size(MALLOC_CAP_DEFAULT);
    return BytesToKiB(total > free ? total - free : 0);
}

static uint32_t GetFlashTotalAsKib() {
    uint32_t flash_size = 0;
    if (esp_flash_get_size(nullptr, &flash_size) != ESP_OK) {
        return UINT32_MAX;
    }
    return BytesToKiB(flash_size);
}

static uint32_t GetPartitionUsedAsKib() {
    uint64_t used_bytes = 0;
    esp_partition_iterator_t it = esp_partition_find(
        ESP_PARTITION_TYPE_ANY,
        ESP_PARTITION_SUBTYPE_ANY,
        nullptr);
    while (it != nullptr) {
        const esp_partition_t *partition = esp_partition_get(it);
        if (partition != nullptr) {
            used_bytes += partition->size;
        }
        it = esp_partition_next(it);
    }
    esp_partition_iterator_release(it);
    return BytesToKiB(used_bytes);
}

static bool GetWifiLinkStats(uint32_t *link_type, uint32_t *tx_rate, uint32_t *rx_rate, uint32_t *tx_max, uint32_t *rx_max) {
    wifi_ap_record_t ap_info = {};
    if (esp_wifi_sta_get_ap_info(&ap_info) != ESP_OK) {
        return false;
    }

    uint32_t max_kbps = 0;
    if (ap_info.phy_11ax) {
        max_kbps = 143400;
    } else if (ap_info.phy_11n) {
        max_kbps = ap_info.second == WIFI_SECOND_CHAN_NONE ? 72200 : 150000;
    } else if (ap_info.phy_11g) {
        max_kbps = 54000;
    } else if (ap_info.phy_11b) {
        max_kbps = 11000;
    } else if (ap_info.phy_lr) {
        max_kbps = 1000;
    }

    if (max_kbps == 0) {
        return false;
    }

    const uint32_t max_kib_s = KbpsToKibPerSecond(max_kbps);
    if (link_type) {
        *link_type = 20;
    }
    if (tx_rate) {
        *tx_rate = max_kib_s;
    }
    if (rx_rate) {
        *rx_rate = max_kib_s;
    }
    if (tx_max) {
        *tx_max = max_kib_s;
    }
    if (rx_max) {
        *rx_max = max_kib_s;
    }
    return true;
}

static int16_t ClampInt16Rounded(float value) {
    if (!isfinite(value)) {
        return 0;
    }
    if (value > 32767.0f) {
        return 32767;
    }
    if (value < -32768.0f) {
        return -32768;
    }
    return static_cast<int16_t>(lroundf(value));
}

static bool IsFieldUsed(uint16_t type_mask, uint16_t ignore_flag) {
    return (type_mask & ignore_flag) == 0;
}

static bool IsFiniteMotionMode(uint8_t control_mode) {
    return control_mode == MATURO_MOTION_RELATIVE;
}

static bool IsMavlinkMotionLocked() {
    if (IsFiniteMotionMode(s_mavlink_motion_mode) && !g_motion_busy) {
        s_mavlink_motion_mode = MATURO_MOTION_VELOCITY;
    }
    return s_mavlink_motion_mode != MATURO_MOTION_VELOCITY;
}

static bool IsTargetMatched(uint8_t target_system, uint8_t target_component, uint8_t sys_id, uint8_t comp_id) {
    const bool system_match = target_system == 0 || target_system == sys_id;
    const bool component_match = target_component == 0 || target_component == comp_id;
    return system_match && component_match;
}

static void SendMavlinkBytes(const MavlinkTxRoute &route, const uint8_t *buf, uint16_t len) {
    if (len == 0) {
        return;
    }

    for (size_t i = 0; i < route.transport_count; ++i) {
        const MavlinkTransport &transport = route.transports[i];
        if (transport.write != nullptr) {
            transport.write(buf, len, transport.ctx);
        }
    }
}

static void SendMavlinkMessage(const MavlinkTxRoute &route, const mavlink_message_t &msg) {
    uint8_t buf[MAVLINK_MAX_PACKET_LEN];
    const uint16_t len = mavlink_msg_to_send_buffer(buf, &msg);
    SendMavlinkBytes(route, buf, len);
}

static void CopyMavParamId(char *dst, const char *src) {
    memcpy(dst, src, 16);
    dst[16] = '\0';
}

static const MaturoParamDef *FindParamByName(const char *name, uint16_t *index_out) {
    for (uint16_t i = 0; i < kMaturoParamCount; ++i) {
        if (strncmp(name, kMaturoParams[i].name, 16) == 0) {
            if (index_out) {
                *index_out = i;
            }
            return &kMaturoParams[i];
        }
    }
    return nullptr;
}

static const MaturoParamDef *FindParamByIndex(int16_t index) {
    if (index < 0 || index >= static_cast<int16_t>(kMaturoParamCount)) {
        return nullptr;
    }
    return &kMaturoParams[index];
}

static float ReadPidField(const PidMsg &state, uint8_t field) {
    switch (field) {
        case MATURO_PARAM_FIELD_KP:
            return state.kp;
        case MATURO_PARAM_FIELD_KI:
            return state.ki;
        case MATURO_PARAM_FIELD_KD:
            return state.kd;
        default:
            return 0.0f;
    }
}

static void WritePidField(PidMsg *state, uint8_t field, float value) {
    if (!state) {
        return;
    }
    switch (field) {
        case MATURO_PARAM_FIELD_KP:
            state->kp = value;
            break;
        case MATURO_PARAM_FIELD_KI:
            state->ki = value;
            break;
        case MATURO_PARAM_FIELD_KD:
            state->kd = value;
            break;
        default:
            break;
    }
}

static PidMsg *GetPidState(uint8_t target) {
    switch (target) {
        case MATURO_PARAM_SPEED_PID:
            return &g_speed_pid_state;
        case MATURO_PARAM_POSITION_PID:
            return &g_position_pid_state;
        default:
            return nullptr;
    }
}

static QueueHandle_t GetPidQueue(uint8_t target) {
    switch (target) {
        case MATURO_PARAM_SPEED_PID:
            return q_speedpid_cmd;
        case MATURO_PARAM_POSITION_PID:
            return q_postionpid_cmd;
        default:
            return nullptr;
    }
}

static float ReadParamValue(const MaturoParamDef &param) {
    const PidMsg *state = GetPidState(param.target);
    return state ? ReadPidField(*state, param.field) : 0.0f;
}

static bool WriteParamValue(const MaturoParamDef &param, float value) {
    if (!isfinite(value)) {
        return false;
    }

    PidMsg *state = GetPidState(param.target);
    QueueHandle_t queue = GetPidQueue(param.target);
    if (!state || !queue) {
        return false;
    }

    PidMsg updated = *state;
    WritePidField(&updated, param.field, value);
    *state = updated;
    xQueueOverwrite(queue, &updated);
    return true;
}

static void SendParamValue(
    const MavlinkTxRoute &route,
    uint8_t sys_id,
    uint8_t comp_id,
    uint16_t index) {
    if (index >= kMaturoParamCount) {
        return;
    }

    mavlink_message_t param_msg;
    uint8_t param_buf[MAVLINK_MAX_PACKET_LEN];
    const MaturoParamDef &param = kMaturoParams[index];
    mavlink_msg_param_value_pack(
        sys_id,
        comp_id,
        &param_msg,
        param.name,
        ReadParamValue(param),
        param.type,
        kMaturoParamCount,
        index);
    const uint16_t param_len = mavlink_msg_to_send_buffer(param_buf, &param_msg);
    SendMavlinkBytes(route, param_buf, param_len);
}

static void SendAllParamValues(
    const MavlinkTxRoute &route,
    uint8_t sys_id,
    uint8_t comp_id) {
    for (uint16_t i = 0; i < kMaturoParamCount; ++i) {
        SendParamValue(route, sys_id, comp_id, i);
    }
}

static void SendRawRpm(
    const MavlinkTxRoute &route,
    uint8_t sys_id,
    uint8_t comp_id,
    uint8_t index,
    float rpm) {
    mavlink_message_t rpm_msg;
    uint8_t rpm_buf[MAVLINK_MAX_PACKET_LEN];
    mavlink_msg_raw_rpm_pack(sys_id, comp_id, &rpm_msg, index, rpm);
    const uint16_t rpm_len = mavlink_msg_to_send_buffer(rpm_buf, &rpm_msg);
    SendMavlinkBytes(route, rpm_buf, rpm_len);
}

static uint16_t ClampBatteryVoltageMv(float voltage_v) {
    if (!isfinite(voltage_v) || voltage_v <= 0.0f) {
        return UINT16_MAX;
    }
    const float mv = voltage_v * 1000.0f;
    if (mv >= static_cast<float>(UINT16_MAX)) {
        return UINT16_MAX - 1;
    }
    return static_cast<uint16_t>(lroundf(mv));
}

static int8_t ClampBatteryPercent(uint8_t percentage, bool valid) {
    if (!valid) {
        return -1;
    }
    return static_cast<int8_t>(percentage > 100 ? 100 : percentage);
}

static uint8_t BatteryChargeState(const BatteryMsg &battery) {
    if (!battery.valid) {
        return MAV_BATTERY_CHARGE_STATE_UNDEFINED;
    }
    if (battery.percentage <= 10) {
        return MAV_BATTERY_CHARGE_STATE_CRITICAL;
    }
    if (battery.percentage <= 20) {
        return MAV_BATTERY_CHARGE_STATE_LOW;
    }
    return MAV_BATTERY_CHARGE_STATE_OK;
}

static void SendSysStatus(
    const MavlinkTxRoute &route,
    uint8_t sys_id,
    uint8_t comp_id,
    const BatteryMsg &battery,
    bool has_battery) {
    mavlink_message_t sys_msg;
    uint8_t sys_buf[MAVLINK_MAX_PACKET_LEN];
    mavlink_msg_sys_status_pack(
        sys_id,
        comp_id,
        &sys_msg,
        0,
        0,
        0,
        0,
        has_battery ? ClampBatteryVoltageMv(battery.voltage_v) : UINT16_MAX,
        -1,
        ClampBatteryPercent(battery.percentage, has_battery),
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0);
    const uint16_t sys_len = mavlink_msg_to_send_buffer(sys_buf, &sys_msg);
    SendMavlinkBytes(route, sys_buf, sys_len);
}

static void SendBatteryStatus(
    const MavlinkTxRoute &route,
    uint8_t sys_id,
    uint8_t comp_id,
    const BatteryMsg &battery,
    bool has_battery) {
    uint16_t voltages[10];
    uint16_t voltages_ext[4];
    for (size_t i = 0; i < sizeof(voltages) / sizeof(voltages[0]); ++i) {
        voltages[i] = UINT16_MAX;
    }
    for (size_t i = 0; i < sizeof(voltages_ext) / sizeof(voltages_ext[0]); ++i) {
        voltages_ext[i] = 0;
    }
    if (has_battery) {
        voltages[0] = ClampBatteryVoltageMv(battery.voltage_v);
    }
    const uint8_t charge_state = has_battery
        ? BatteryChargeState(battery)
        : static_cast<uint8_t>(MAV_BATTERY_CHARGE_STATE_UNDEFINED);

    mavlink_message_t battery_msg;
    uint8_t battery_buf[MAVLINK_MAX_PACKET_LEN];
    mavlink_msg_battery_status_pack(
        sys_id,
        comp_id,
        &battery_msg,
        0,
        MAV_BATTERY_FUNCTION_ALL,
        MAV_BATTERY_TYPE_LIPO,
        INT16_MAX,
        voltages,
        -1,
        -1,
        -1,
        ClampBatteryPercent(battery.percentage, has_battery),
        0,
        charge_state,
        voltages_ext,
        MAV_BATTERY_MODE_UNKNOWN,
        0);
    const uint16_t battery_len = mavlink_msg_to_send_buffer(battery_buf, &battery_msg);
    SendMavlinkBytes(route, battery_buf, battery_len);
}

static void SendOnboardComputerStatus(
    const MavlinkTxRoute &route,
    uint8_t sys_id,
    uint8_t comp_id,
    uint32_t time_boot_ms,
    float board_temperature_c,
    bool has_temperature) {
    const uint64_t time_usec = static_cast<uint64_t>(time_boot_ms) * 1000ULL;
    const uint8_t type = 3;
    const int8_t temperature = has_temperature ? ClampTemperatureInt8(board_temperature_c) : INT8_MAX;

    uint8_t cpu_cores[8];
    uint8_t cpu_combined[10];
    uint8_t gpu_cores[4];
    uint8_t gpu_combined[10];
    int8_t temperature_core[8];
    int16_t fan_speed[4];
    uint32_t storage_type[4];
    uint32_t storage_usage[4];
    uint32_t storage_total[4];
    uint32_t link_type[6];
    uint32_t link_tx_rate[6];
    uint32_t link_rx_rate[6];
    uint32_t link_tx_max[6];
    uint32_t link_rx_max[6];

    memset(cpu_cores, UINT8_MAX, sizeof(cpu_cores));
    memset(cpu_combined, UINT8_MAX, sizeof(cpu_combined));
    memset(gpu_cores, UINT8_MAX, sizeof(gpu_cores));
    memset(gpu_combined, UINT8_MAX, sizeof(gpu_combined));
    memset(temperature_core, INT8_MAX, sizeof(temperature_core));
    for (size_t i = 0; i < sizeof(fan_speed) / sizeof(fan_speed[0]); ++i) {
        fan_speed[i] = INT16_MAX;
    }
    for (size_t i = 0; i < sizeof(storage_type) / sizeof(storage_type[0]); ++i) {
        storage_type[i] = UINT32_MAX;
        storage_usage[i] = UINT32_MAX;
        storage_total[i] = UINT32_MAX;
    }
    for (size_t i = 0; i < sizeof(link_type) / sizeof(link_type[0]); ++i) {
        link_type[i] = UINT32_MAX;
        link_tx_rate[i] = UINT32_MAX;
        link_rx_rate[i] = UINT32_MAX;
        link_tx_max[i] = UINT32_MAX;
        link_rx_max[i] = UINT32_MAX;
    }

    storage_usage[0] = GetPartitionUsedAsKib();
    storage_total[0] = GetFlashTotalAsKib();
    GetWifiLinkStats(&link_type[0], &link_tx_rate[0], &link_rx_rate[0], &link_tx_max[0], &link_rx_max[0]);
    temperature_core[0] = temperature;

    mavlink_message_t status_msg;
    uint8_t status_buf[MAVLINK_MAX_PACKET_LEN];
    mavlink_msg_onboard_computer_status_pack(
        sys_id,
        comp_id,
        &status_msg,
        time_usec,
        time_boot_ms,
        type,
        cpu_cores,
        cpu_combined,
        gpu_cores,
        gpu_combined,
        temperature,
        temperature_core,
        fan_speed,
        GetDefaultHeapUsedAsKib(),
        GetDefaultHeapTotalAsKib(),
        storage_type,
        storage_usage,
        storage_total,
        link_type,
        link_tx_rate,
        link_rx_rate,
        link_tx_max,
        link_rx_max,
        0);
    const uint16_t status_len = mavlink_msg_to_send_buffer(status_buf, &status_msg);
    SendMavlinkBytes(route, status_buf, status_len);
}

static void SendCommandAck(
    const MavlinkTxRoute &route,
    uint8_t sys_id,
    uint8_t comp_id,
    uint8_t target_system,
    uint8_t target_component,
    uint16_t command,
    uint8_t result) {
    mavlink_message_t ack_msg;
    uint8_t ack_buf[MAVLINK_MAX_PACKET_LEN];
    mavlink_msg_command_ack_pack(
        sys_id,
        comp_id,
        &ack_msg,
        command,
        result,
        UINT8_MAX,
        0,
        target_system,
        target_component);
    const uint16_t ack_len = mavlink_msg_to_send_buffer(ack_buf, &ack_msg);
    SendMavlinkBytes(route, ack_buf, ack_len);
}

static void HandleParamRequestRead(
    const MavlinkTxRoute &route,
    uint8_t sys_id,
    uint8_t comp_id,
    const mavlink_message_t *msg) {
    mavlink_param_request_read_t request;
    mavlink_msg_param_request_read_decode(msg, &request);
    if (!IsTargetMatched(request.target_system, request.target_component, sys_id, comp_id)) {
        return;
    }

    if (request.param_index >= 0) {
        if (FindParamByIndex(request.param_index)) {
            SendParamValue(route, sys_id, comp_id, static_cast<uint16_t>(request.param_index));
        }
        return;
    }

    char param_id[17] = {};
    CopyMavParamId(param_id, request.param_id);
    uint16_t index = 0;
    if (FindParamByName(param_id, &index)) {
        SendParamValue(route, sys_id, comp_id, index);
    }
}

static void HandleParamRequestList(
    const MavlinkTxRoute &route,
    uint8_t sys_id,
    uint8_t comp_id,
    const mavlink_message_t *msg) {
    mavlink_param_request_list_t request;
    mavlink_msg_param_request_list_decode(msg, &request);
    if (!IsTargetMatched(request.target_system, request.target_component, sys_id, comp_id)) {
        return;
    }

    SendAllParamValues(route, sys_id, comp_id);
}

static void HandleParamSet(
    const MavlinkTxRoute &route,
    uint8_t sys_id,
    uint8_t comp_id,
    const mavlink_message_t *msg) {
    mavlink_param_set_t set;
    mavlink_msg_param_set_decode(msg, &set);
    if (!IsTargetMatched(set.target_system, set.target_component, sys_id, comp_id)) {
        return;
    }

    char param_id[17] = {};
    CopyMavParamId(param_id, set.param_id);
    uint16_t index = 0;
    const MaturoParamDef *param = FindParamByName(param_id, &index);
    if (!param) {
        return;
    }

    if (WriteParamValue(*param, set.param_value)) {
        SendParamValue(route, sys_id, comp_id, index);
    }
}

static void SendMotionCommand(const MotionMsg &cmd) {
    s_mavlink_motion_mode = cmd.control_mode;
    xQueueOverwrite(q_motion_cmd, &cmd);
    g_emergency_stop = false;
}

static uint8_t HandleSetPositionTargetLocalNed(const mavlink_message_t *msg, uint8_t motion_source) {
    mavlink_set_position_target_local_ned_t set_target;
    mavlink_msg_set_position_target_local_ned_decode(msg, &set_target);

    if (IsMavlinkMotionLocked()) {
        return MAV_RESULT_DENIED;
    }

    const bool use_x = IsFieldUsed(set_target.type_mask, POSITION_TARGET_TYPEMASK_X_IGNORE);
    const bool use_y = IsFieldUsed(set_target.type_mask, POSITION_TARGET_TYPEMASK_Y_IGNORE);
    const bool use_vx = IsFieldUsed(set_target.type_mask, POSITION_TARGET_TYPEMASK_VX_IGNORE);
    const bool use_vy = IsFieldUsed(set_target.type_mask, POSITION_TARGET_TYPEMASK_VY_IGNORE);
    const bool use_yaw = IsFieldUsed(set_target.type_mask, POSITION_TARGET_TYPEMASK_YAW_IGNORE);
    const bool use_yaw_rate = IsFieldUsed(set_target.type_mask, POSITION_TARGET_TYPEMASK_YAW_RATE_IGNORE);

    MotionMsg m_cmd = {};
    m_cmd.source = motion_source;

    if (use_x || use_y || use_yaw) {
        m_cmd.control_mode = MATURO_MOTION_POSITION;
        m_cmd.target_x = use_x ? set_target.x * 1000.0f : 0.0f;
        m_cmd.target_y = use_y ? set_target.y * 1000.0f : 0.0f;
        m_cmd.target_yaw = use_yaw ? set_target.yaw : 0.0f;
        SendMotionCommand(m_cmd);
        return MAV_RESULT_ACCEPTED;
    }

    if (use_vx || use_vy || use_yaw_rate) {
        m_cmd.control_mode = MATURO_MOTION_VELOCITY;
        m_cmd.target_vx = use_vx ? set_target.vx * 1000.0f : 0.0f;
        m_cmd.target_vy = use_vy ? set_target.vy * 1000.0f : 0.0f;
        m_cmd.target_wz = use_yaw_rate ? set_target.yaw_rate : 0.0f;
        SendMotionCommand(m_cmd);
        return MAV_RESULT_ACCEPTED;
    }

    return MAV_RESULT_DENIED;
}

static uint8_t HandleCommandLong(const mavlink_message_t *msg, uint8_t motion_source) {
    mavlink_command_long_t command;
    mavlink_msg_command_long_decode(msg, &command);

    switch (command.command) {
        case MAV_CMD_COMPONENT_ARM_DISARM: {
            if (command.param1 < 0.5f) {
                MotionMsg stop_cmd = {};
                stop_cmd.source = motion_source;
                stop_cmd.control_mode = MATURO_MOTION_VELOCITY;
                SendMotionCommand(stop_cmd);
                g_motion_busy = false;
                return MAV_RESULT_ACCEPTED;
            }
            g_emergency_stop = false;
            return MAV_RESULT_ACCEPTED;
        }
        case MAV_CMD_PREFLIGHT_SET_SENSOR_OFFSETS: {
            robot.ResetOdometry();
            return MAV_RESULT_ACCEPTED;
        }
        case MAV_CMD_DO_SET_SERVO: {
            ServoMsg servo_cmd = {};
            servo_cmd.angle = command.param2;
            xQueueOverwrite(q_servo_cmd, &servo_cmd);
            return MAV_RESULT_ACCEPTED;
        }
        case MAV_CMD_DO_SET_ACTUATOR: {
            if (IsMavlinkMotionLocked()) {
                return MAV_RESULT_DENIED;
            }
            MotionMsg motor_cmd = {};
            motor_cmd.source = motion_source;
            motor_cmd.control_mode = MATURO_MOTION_WHEEL_SPEED;
            motor_cmd.target_vx = command.param1;
            motor_cmd.target_vy = command.param2;
            SendMotionCommand(motor_cmd);
            return MAV_RESULT_ACCEPTED;
        }
        case MATURO_MAV_CMD_MOVE_RELATIVE: {
            if (IsMavlinkMotionLocked()) {
                return MAV_RESULT_DENIED;
            }
            MotionMsg relative_cmd = {};
            relative_cmd.source = motion_source;
            relative_cmd.control_mode = MATURO_MOTION_RELATIVE;
            relative_cmd.target_x = command.param1;
            relative_cmd.target_yaw = command.param2;
            g_motion_busy = true;
            SendMotionCommand(relative_cmd);
            return MAV_RESULT_ACCEPTED;
        }
        case MATURO_MAV_CMD_SET_PID: {
            PidMsg pid_msg = {};
            pid_msg.kp = command.param2;
            pid_msg.ki = command.param3;
            pid_msg.kd = command.param4;

            switch (static_cast<int>(command.param1)) {
                case MATURO_PID_SPEED:
                    xQueueOverwrite(q_speedpid_cmd, &pid_msg);
                    return MAV_RESULT_ACCEPTED;
                case MATURO_PID_POSITION:
                    xQueueOverwrite(q_postionpid_cmd, &pid_msg);
                    return MAV_RESULT_ACCEPTED;
                default:
                    return MAV_RESULT_DENIED;
            }
        }
        default:
            return MAV_RESULT_UNSUPPORTED;
    }
}

void mavlink_common_handle_message(
    const mavlink_message_t *msg,
    const MavlinkTransport *reply_transport,
    MavlinkTransportSource source) {
    const MavlinkTxRoute route = {
        .transports = reply_transport,
        .transport_count = reply_transport != nullptr ? 1U : 0U,
    };
    const uint8_t sys_id = 1;
    const uint8_t comp_id = 1;
    (void)source;
    const uint8_t motion_source = MOTION_SRC_UDP;

    switch (msg->msgid) {
        case MAVLINK_MSG_ID_SET_POSITION_TARGET_LOCAL_NED: {
            HandleSetPositionTargetLocalNed(msg, motion_source);
            break;
        }
        case MAVLINK_MSG_ID_COMMAND_LONG: {
            mavlink_command_long_t command;
            mavlink_msg_command_long_decode(msg, &command);
            const uint8_t result = HandleCommandLong(msg, motion_source);
            SendCommandAck(
                route,
                sys_id,
                comp_id,
                msg->sysid,
                msg->compid,
                command.command,
                result);
            break;
        }
        case MAVLINK_MSG_ID_SET_RGB_LED: {
            mavlink_set_rgb_led_t rgb;
            mavlink_msg_set_rgb_led_decode(msg, &rgb);
            RgbMsg rgb_cmd = {
                .r = rgb.r,
                .g = rgb.g,
                .b = rgb.b,
            };
            xQueueOverwrite(q_rgb_cmd, &rgb_cmd);
            break;
        }
        case MAVLINK_MSG_ID_SET_ACTUATOR_CONTROL_TARGET: {
            mavlink_set_actuator_control_target_t actuator;
            mavlink_msg_set_actuator_control_target_decode(msg, &actuator);
            RgbMsg rgb_cmd = {
                .r = ClampUint8(actuator.controls[0] * 255.0f),
                .g = ClampUint8(actuator.controls[1] * 255.0f),
                .b = ClampUint8(actuator.controls[2] * 255.0f),
            };
            xQueueOverwrite(q_rgb_cmd, &rgb_cmd);
            break;
        }
        case MAVLINK_MSG_ID_PARAM_REQUEST_READ: {
            HandleParamRequestRead(route, sys_id, comp_id, msg);
            break;
        }
        case MAVLINK_MSG_ID_PARAM_REQUEST_LIST: {
            HandleParamRequestList(route, sys_id, comp_id, msg);
            break;
        }
        case MAVLINK_MSG_ID_PARAM_SET: {
            HandleParamSet(route, sys_id, comp_id, msg);
            break;
        }
        default:
            break;
    }
}

void mavlink_common_publish(
    const MavlinkTransport *transports,
    size_t transport_count,
    MavlinkPublishState *state) {
    if (transports == nullptr || transport_count == 0 || state == nullptr) {
        return;
    }

    const uint8_t sys_id = 1;
    const uint8_t comp_id = 1;
    mavlink_message_t tx_msg;
    const MavlinkTxRoute tx_route = {
        .transports = transports,
        .transport_count = transport_count,
    };

        uint32_t time_boot_ms = xTaskGetTickCount() * portTICK_PERIOD_MS;
        state->loop_counter++;

        ImuMsg imu_state;
        if (xQueuePeek(q_imu_state, &imu_state, 0) == pdTRUE) {
            float roll_rad = DEG2RAD(imu_state.roll);
            float pitch_rad = DEG2RAD(imu_state.pitch);
            float yaw_rad = DEG2RAD(imu_state.yaw);
            float gyro_x_rad = DEG2RAD(imu_state.gyro_x);
            float gyro_y_rad = DEG2RAD(imu_state.gyro_y);
            float gyro_z_rad = DEG2RAD(imu_state.gyro_z);

            mavlink_msg_attitude_pack(sys_id, comp_id, &tx_msg, time_boot_ms, roll_rad, pitch_rad, yaw_rad, gyro_x_rad, gyro_y_rad, gyro_z_rad);
            SendMavlinkMessage(tx_route, tx_msg);

            float repr_offset[4] = {0};
            mavlink_msg_attitude_quaternion_pack(sys_id, comp_id, &tx_msg, time_boot_ms, imu_state.qw, imu_state.qx, imu_state.qy, imu_state.qz, gyro_x_rad, gyro_y_rad, gyro_z_rad, repr_offset);
            SendMavlinkMessage(tx_route, tx_msg);

            mavlink_msg_scaled_imu_pack(
                sys_id,
                comp_id,
                &tx_msg,
                time_boot_ms,
                ClampInt16Rounded(imu_state.acc_x * 1000.0f),
                ClampInt16Rounded(imu_state.acc_y * 1000.0f),
                ClampInt16Rounded(imu_state.acc_z * 1000.0f),
                ClampInt16Rounded(gyro_x_rad * 1000.0f),
                ClampInt16Rounded(gyro_y_rad * 1000.0f),
                ClampInt16Rounded(gyro_z_rad * 1000.0f),
                0,
                0,
                0,
                0);
            SendMavlinkMessage(tx_route, tx_msg);
        }

        MotionMsg motion_state;
        if (xQueuePeek(q_motion_state, &motion_state, 0) == pdTRUE) {
            const uint64_t time_usec = static_cast<uint64_t>(time_boot_ms) * 1000ULL;

            float q[4] = {
                motion_state.qw,
                motion_state.qx,
                motion_state.qy,
                motion_state.qz
            };

            float pose_covariance[21] = { NAN };
            float velocity_covariance[21] = { NAN };

            mavlink_msg_odometry_pack(
                sys_id,
                comp_id,
                &tx_msg,
                time_usec,
                MAV_FRAME_LOCAL_FLU,
                MAV_FRAME_BODY_FRD,
                motion_state.x / 1000.0f,
                motion_state.y / 1000.0f,
                0.0f,
                q,
                motion_state.vx / 1000.0f,
                motion_state.vy / 1000.0f,
                0.0f,
                0.0f,
                0.0f,
                motion_state.wz,
                pose_covariance,
                velocity_covariance,
                0,
                MAV_ESTIMATOR_TYPE_NAIVE,
                0
            );

            SendMavlinkMessage(tx_route, tx_msg);
        }

        if (state->loop_counter % 50 == 0) {
            mavlink_msg_heartbeat_pack(sys_id, comp_id, &tx_msg, MAV_TYPE_GROUND_ROVER, MAV_AUTOPILOT_GENERIC, 0, 0, MAV_STATE_ACTIVE);
            SendMavlinkMessage(tx_route, tx_msg);

            char device_text[50] = {0};
            snprintf(device_text, sizeof(device_text), "DEVICE_NAME:%s", g_device_name);
            mavlink_msg_statustext_pack(sys_id, comp_id, &tx_msg, MAV_SEVERITY_INFO, device_text, 0, 0);
            SendMavlinkMessage(tx_route, tx_msg);

            TemperatureMsg temperature_state = {};
            const bool has_temperature =
                xQueuePeek(q_temperature_state, &temperature_state, 0) == pdTRUE &&
                temperature_state.valid;
            SendOnboardComputerStatus(
                tx_route,
                sys_id,
                comp_id,
                time_boot_ms,
                temperature_state.celsius,
                has_temperature);

            BatteryMsg battery_state = {};
            const bool has_battery =
                xQueuePeek(q_battery_state, &battery_state, 0) == pdTRUE &&
                battery_state.valid;
            SendSysStatus(tx_route, sys_id, comp_id, battery_state, has_battery);
            SendBatteryStatus(tx_route, sys_id, comp_id, battery_state, has_battery);

            if (!state->component_info_sent) {
                char vendor_name[32] = {0};
                char model_name[32] = {0};
                char software_version[24] = {0};
                char hardware_version[24] = {0};
                char serial_number[32] = {0};

                snprintf(vendor_name, sizeof(vendor_name), "Maturo");
                snprintf(model_name, sizeof(model_name), "Driver");
                snprintf(software_version, sizeof(software_version), "2026.05");
                snprintf(hardware_version, sizeof(hardware_version), "ESP32");
                snprintf(serial_number, sizeof(serial_number), "%s", g_device_name);

                mavlink_msg_component_information_basic_pack(
                    sys_id,
                    comp_id,
                    &tx_msg,
                    time_boot_ms,
                    0,
                    0,
                    vendor_name,
                    model_name,
                    software_version,
                    hardware_version,
                    serial_number);
                SendMavlinkMessage(tx_route, tx_msg);
                state->component_info_sent = true;
            }
        }

        if (state->loop_counter % 5 == 0) {
            UltrasonicMsg ultrasonic_state;
            if (xQueuePeek(q_ultrasonic_state, &ultrasonic_state, 0) == pdTRUE) {
                const float q[4] = {0.0f, 0.0f, 0.0f, 0.0f};
                const uint16_t distance_cm = ClampDistanceCm(ultrasonic_state.distance_cm);

                mavlink_msg_distance_sensor_pack(
                    sys_id,
                    comp_id,
                    &tx_msg,
                    time_boot_ms,
                    2,
                    400,
                    distance_cm,
                    MAV_DISTANCE_SENSOR_ULTRASOUND,
                    0,
                    MAV_SENSOR_ROTATION_NONE,
                    UINT8_MAX,
                    0.0f,
                    0.0f,
                    q,
                    0);
                SendMavlinkMessage(tx_route, tx_msg);
            }

            mavlink_msg_debug_pack(
                sys_id,
                comp_id,
                &tx_msg,
                time_boot_ms,
                MATURO_DEBUG_MOTION_BUSY,
                g_motion_busy ? 1.0f : 0.0f);
            SendMavlinkMessage(tx_route, tx_msg);

            MotionMsg wheel_state;
            if (xQueuePeek(q_motion_state, &wheel_state, 0) == pdTRUE) {
                SendRawRpm(
                    tx_route,
                    sys_id,
                    comp_id,
                    0,
                    WheelMmsToRpm(wheel_state.vel_left));
                SendRawRpm(
                    tx_route,
                    sys_id,
                    comp_id,
                    1,
                    WheelMmsToRpm(wheel_state.vel_right));
            }
        }

        if (state->loop_counter % 5 == 0) {
            LidarMsg lidar_state;
            if (xQueuePeek(q_lidar_state, &lidar_state, 0) == pdTRUE) {
                uint16_t obstacle_distances[MATURO_OBSTACLE_DISTANCE_BINS] = {};
                FillObstacleDistanceBins(lidar_state, obstacle_distances);
                mavlink_msg_obstacle_distance_pack(
                    sys_id,
                    comp_id,
                    &tx_msg,
                    static_cast<uint64_t>(time_boot_ms) * 1000ULL,
                    MAV_DISTANCE_SENSOR_LASER,
                    obstacle_distances,
                    MATURO_OBSTACLE_BIN_WIDTH_DEG,
                    MATURO_OBSTACLE_MIN_CM,
                    MATURO_OBSTACLE_MAX_CM,
                    static_cast<float>(MATURO_OBSTACLE_BIN_WIDTH_DEG),
                    0.0f,
                    MAV_FRAME_BODY_FRD);
                SendMavlinkMessage(tx_route, tx_msg);
            }
        }
        
}
