#include "system_globals.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lwip/inet.h"
#include "lwip/sockets.h"
#include "msg/battery_msg.h"
#include "msg/imu_msg.h"
#include "msg/lidar_msg.h"
#include "msg/motion_msg.h"

#include "geometry_msgs/msg/twist.h"
#include "nav_msgs/msg/odometry.h"
#include "rcl/rcl.h"
#include "rcl/node_options.h"
#include "rcl/time.h"
#include "rcl/timer.h"
#include "rcl/wait.h"
#include "rmw/qos_profiles.h"
#include "rmw_microros/custom_transport.h"
#include "rmw_microros/ping.h"
#include "rmw_microros/rmw_microros.h"
#include "rmw_microros/time_sync.h"
#include "rosidl_runtime_c/primitives_sequence_functions.h"
#include "rosidl_runtime_c/string_functions.h"
#include "sensor_msgs/msg/battery_state.h"
#include "sensor_msgs/msg/imu.h"
#include "sensor_msgs/msg/laser_scan.h"
#include "uxr/client/profile/transport/custom/custom_transport.h"

static const char *TAG = "MICROROS";
static constexpr size_t kLaserScanPointCount = 360;
static constexpr double kDegToRad = 0.017453292519943295;
static constexpr double kGravity = 9.80665;

#ifndef CONFIG_MICRO_ROS_LOCAL_PORT
#define CONFIG_MICRO_ROS_LOCAL_PORT "8888"
#endif

struct UdpTransportContext {
    int fd;
    sockaddr_in remote;
    uint16_t local_port;
};

static UdpTransportContext s_udp_ctx = {};
static geometry_msgs__msg__Twist s_cmd_vel_msg = {};
static nav_msgs__msg__Odometry s_odom_msg = {};
static sensor_msgs__msg__Imu s_imu_msg = {};
static sensor_msgs__msg__LaserScan s_scan_msg = {};
static sensor_msgs__msg__BatteryState s_battery_msg = {};
static rcl_publisher_t s_odom_publisher = {};
static rcl_publisher_t s_imu_publisher = {};
static rcl_publisher_t s_scan_publisher = {};
static rcl_publisher_t s_battery_publisher = {};
static rcl_subscription_t s_cmd_vel_subscriber = {};
static rcl_init_options_t s_init_options = {};
static rcl_context_t s_context = {};
static rcl_allocator_t s_allocator = {};
static rcl_node_t s_node = {};
static rcl_timer_t s_publish_timer = {};
static rcl_wait_set_t s_wait_set = {};
static rcl_clock_t s_clock = {};
static bool s_ros_created = false;
static bool s_strings_initialized = false;
static bool s_scan_ranges_initialized = false;
static bool s_init_options_initialized = false;
static bool s_context_initialized = false;
static bool s_node_initialized = false;
static bool s_odom_publisher_initialized = false;
static bool s_imu_publisher_initialized = false;
static bool s_scan_publisher_initialized = false;
static bool s_battery_msg_initialized = false;
static bool s_battery_publisher_initialized = false;
static bool s_cmd_vel_subscriber_initialized = false;
static bool s_clock_initialized = false;
static bool s_timer_initialized = false;
static bool s_wait_set_initialized = false;
static uint32_t s_publish_tick = 0;

#define RCCHECK(fn) do { \
    rcl_ret_t rc = (fn); \
    if (rc != RCL_RET_OK) { \
        ESP_LOGW(TAG, "rcl status line %d: %d", __LINE__, static_cast<int>(rc)); \
        return false; \
    } \
} while (0)

#define RCSOFTCHECK(fn) do { \
    rcl_ret_t rc = (fn); \
    if (rc != RCL_RET_OK) { \
        ESP_LOGW(TAG, "rcl status line %d: %d", __LINE__, static_cast<int>(rc)); \
    } \
} while (0)

extern "C" bool transport_open_udp(uxrCustomTransport *transport) {
    auto *ctx = static_cast<UdpTransportContext *>(transport->args);
    ctx->fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (ctx->fd < 0) {
        return false;
    }

    sockaddr_in local = {};
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = htonl(INADDR_ANY);
    local.sin_port = htons(ctx->local_port);
    bind(ctx->fd, reinterpret_cast<sockaddr *>(&local), sizeof(local));
    return true;
}

extern "C" bool transport_close_udp(uxrCustomTransport *transport) {
    auto *ctx = static_cast<UdpTransportContext *>(transport->args);
    if (ctx->fd >= 0) {
        close(ctx->fd);
        ctx->fd = -1;
    }
    return true;
}

extern "C" size_t transport_write_udp(
    uxrCustomTransport *transport,
    const uint8_t *buf,
    size_t len,
    uint8_t *) {
    auto *ctx = static_cast<UdpTransportContext *>(transport->args);
    const int sent = sendto(
        ctx->fd,
        buf,
        len,
        0,
        reinterpret_cast<sockaddr *>(&ctx->remote),
        sizeof(ctx->remote));
    return sent > 0 ? static_cast<size_t>(sent) : 0;
}

extern "C" size_t transport_read_udp(
    uxrCustomTransport *transport,
    uint8_t *buf,
    size_t len,
    int timeout,
    uint8_t *) {
    auto *ctx = static_cast<UdpTransportContext *>(transport->args);
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(ctx->fd, &readfds);

    timeval tv = {};
    tv.tv_sec = timeout / 1000;
    tv.tv_usec = (timeout % 1000) * 1000;
    const int ret = select(ctx->fd + 1, &readfds, nullptr, nullptr, &tv);
    if (ret <= 0) {
        return 0;
    }

    const int received = recv(ctx->fd, buf, len, 0);
    return received > 0 ? static_cast<size_t>(received) : 0;
}

static void cleanup_result(rcl_ret_t ret) {
    if (ret != RCL_RET_OK) {
        ESP_LOGW(TAG, "cleanup status: %d", static_cast<int>(ret));
    }
}

static void reset_ros_handles() {
    s_odom_publisher = rcl_get_zero_initialized_publisher();
    s_imu_publisher = rcl_get_zero_initialized_publisher();
    s_scan_publisher = rcl_get_zero_initialized_publisher();
    s_battery_publisher = rcl_get_zero_initialized_publisher();
    s_cmd_vel_subscriber = rcl_get_zero_initialized_subscription();
    s_init_options = rcl_get_zero_initialized_init_options();
    s_context = rcl_get_zero_initialized_context();
    s_node = rcl_get_zero_initialized_node();
    s_publish_timer = rcl_get_zero_initialized_timer();
    s_wait_set = rcl_get_zero_initialized_wait_set();
    s_clock = {};
}

static void handle_cmd_vel(const geometry_msgs__msg__Twist *msg) {
    if (msg == nullptr || q_motion_cmd == nullptr) {
        return;
    }

    MotionMsg cmd = {};
    cmd.source = MOTION_SRC_MICROROS;
    cmd.control_mode = 0;
    cmd.target_vx = static_cast<float>(msg->linear.x * 1000.0);
    cmd.target_vy = static_cast<float>(msg->linear.y * 1000.0);
    cmd.target_wz = static_cast<float>(msg->angular.z);
    xQueueOverwrite(q_motion_cmd, &cmd);
    g_emergency_stop = false;
}

static void set_stamp(std_msgs__msg__Header *header, int64_t stamp_ms) {
    header->stamp.sec = static_cast<int32_t>(stamp_ms / 1000);
    header->stamp.nanosec = static_cast<uint32_t>((stamp_ms % 1000) * 1000000);
}

static void publish_odom(int64_t stamp_ms) {
    MotionMsg motion = {};
    if (q_motion_state == nullptr || xQueuePeek(q_motion_state, &motion, 0) != pdTRUE) {
        return;
    }

    set_stamp(&s_odom_msg.header, stamp_ms);
    s_odom_msg.pose.pose.position.x = motion.x / 1000.0;
    s_odom_msg.pose.pose.position.y = motion.y / 1000.0;
    s_odom_msg.pose.pose.position.z = 0.0;
    s_odom_msg.pose.pose.orientation.w = motion.qw;
    s_odom_msg.pose.pose.orientation.x = motion.qx;
    s_odom_msg.pose.pose.orientation.y = motion.qy;
    s_odom_msg.pose.pose.orientation.z = motion.qz;
    s_odom_msg.twist.twist.linear.x = motion.vx / 1000.0;
    s_odom_msg.twist.twist.linear.y = motion.vy / 1000.0;
    s_odom_msg.twist.twist.angular.z = motion.wz;
    RCSOFTCHECK(rcl_publish(&s_odom_publisher, &s_odom_msg, nullptr));
}

static void publish_imu(int64_t stamp_ms) {
    ImuMsg imu = {};
    if (q_imu_state == nullptr || xQueuePeek(q_imu_state, &imu, 0) != pdTRUE) {
        return;
    }

    set_stamp(&s_imu_msg.header, stamp_ms);
    s_imu_msg.orientation.w = imu.qw;
    s_imu_msg.orientation.x = imu.qx;
    s_imu_msg.orientation.y = imu.qy;
    s_imu_msg.orientation.z = imu.qz;
    s_imu_msg.angular_velocity.x = imu.gyro_x * kDegToRad;
    s_imu_msg.angular_velocity.y = imu.gyro_y * kDegToRad;
    s_imu_msg.angular_velocity.z = imu.gyro_z * kDegToRad;
    s_imu_msg.linear_acceleration.x = imu.acc_x * kGravity;
    s_imu_msg.linear_acceleration.y = imu.acc_y * kGravity;
    s_imu_msg.linear_acceleration.z = imu.acc_z * kGravity;
    RCSOFTCHECK(rcl_publish(&s_imu_publisher, &s_imu_msg, nullptr));
}

static void publish_scan(int64_t stamp_ms) {
    LidarMsg lidar = {};
    if (q_lidar_state == nullptr || xQueuePeek(q_lidar_state, &lidar, 0) != pdTRUE) {
        return;
    }

    set_stamp(&s_scan_msg.header, stamp_ms);
    for (size_t i = 0; i < kLaserScanPointCount; ++i) {
        s_scan_msg.ranges.data[i] = lidar.distances[i] > 0
            ? static_cast<float>(lidar.distances[i]) / 1000.0f
            : 0.0f;
    }
    RCSOFTCHECK(rcl_publish(&s_scan_publisher, &s_scan_msg, nullptr));
}

static void publish_battery(int64_t stamp_ms) {
    BatteryMsg battery = {};
    if (q_battery_state == nullptr || xQueuePeek(q_battery_state, &battery, 0) != pdTRUE ||
        !battery.valid) {
        return;
    }

    set_stamp(&s_battery_msg.header, stamp_ms);
    s_battery_msg.voltage = battery.voltage_v;
    s_battery_msg.temperature = NAN;
    s_battery_msg.current = NAN;
    s_battery_msg.charge = NAN;
    s_battery_msg.capacity = NAN;
    s_battery_msg.design_capacity = NAN;
    s_battery_msg.percentage = static_cast<float>(battery.percentage) / 100.0f;
    s_battery_msg.power_supply_status =
        sensor_msgs__msg__BatteryState__POWER_SUPPLY_STATUS_DISCHARGING;
    s_battery_msg.power_supply_health =
        sensor_msgs__msg__BatteryState__POWER_SUPPLY_HEALTH_GOOD;
    s_battery_msg.power_supply_technology =
        sensor_msgs__msg__BatteryState__POWER_SUPPLY_TECHNOLOGY_LIPO;
    s_battery_msg.present = true;
    RCSOFTCHECK(rcl_publish(&s_battery_publisher, &s_battery_msg, nullptr));
}

static void publish_state_timer(rcl_timer_t *timer, int64_t) {
    if (timer == nullptr || g_wifi_comm_mode != WifiCommMode::kMicroRos) {
        return;
    }

    const int64_t stamp_ms = rmw_uros_epoch_millis();
    publish_odom(stamp_ms);
    publish_imu(stamp_ms);

    if ((++s_publish_tick % 5) == 0) {
        publish_scan(stamp_ms);
        publish_battery(stamp_ms);
    }
}

static bool setup_udp_transport() {
    memset(&s_udp_ctx, 0, sizeof(s_udp_ctx));
    s_udp_ctx.fd = -1;
    s_udp_ctx.local_port = static_cast<uint16_t>(atoi(CONFIG_MICRO_ROS_LOCAL_PORT));
    s_udp_ctx.remote.sin_family = AF_INET;
    s_udp_ctx.remote.sin_port = htons(g_microros_agent_port);

    if (inet_pton(AF_INET, g_microros_agent_ip, &s_udp_ctx.remote.sin_addr.s_addr) != 1) {
        ESP_LOGE(TAG, "invalid micro-ROS agent IP: %s", g_microros_agent_ip);
        return false;
    }

    rmw_uros_set_custom_transport(
        false,
        &s_udp_ctx,
        transport_open_udp,
        transport_close_udp,
        transport_write_udp,
        transport_read_udp);
    return true;
}

static bool create_ros_entities() {
    s_publish_tick = 0;
    reset_ros_handles();

    rosidl_runtime_c__String__init(&s_odom_msg.header.frame_id);
    rosidl_runtime_c__String__init(&s_odom_msg.child_frame_id);
    rosidl_runtime_c__String__init(&s_imu_msg.header.frame_id);
    rosidl_runtime_c__String__init(&s_scan_msg.header.frame_id);
    s_strings_initialized = true;

    (void)rosidl_runtime_c__String__assign(&s_odom_msg.header.frame_id, "odom");
    (void)rosidl_runtime_c__String__assign(&s_odom_msg.child_frame_id, "base_link");
    (void)rosidl_runtime_c__String__assign(&s_imu_msg.header.frame_id, "imu_link");
    (void)rosidl_runtime_c__String__assign(&s_scan_msg.header.frame_id, "laser_frame");

    if (!rosidl_runtime_c__float32__Sequence__init(&s_scan_msg.ranges, kLaserScanPointCount)) {
        ESP_LOGE(TAG, "failed to allocate LaserScan ranges");
        return false;
    }
    s_scan_ranges_initialized = true;

    if (!sensor_msgs__msg__BatteryState__init(&s_battery_msg)) {
        ESP_LOGE(TAG, "failed to initialize BatteryState message");
        return false;
    }
    s_battery_msg_initialized = true;
    (void)rosidl_runtime_c__String__assign(&s_battery_msg.header.frame_id, "battery");
    (void)rosidl_runtime_c__String__assign(&s_battery_msg.location, "main");

    s_scan_msg.angle_min = 0.0f;
    s_scan_msg.angle_max = static_cast<float>((kLaserScanPointCount - 1) * kDegToRad);
    s_scan_msg.angle_increment = static_cast<float>(kDegToRad);
    s_scan_msg.time_increment = 0.0f;
    s_scan_msg.scan_time = 0.1f;
    s_scan_msg.range_min = 0.02f;
    s_scan_msg.range_max = 12.0f;

    s_imu_msg.orientation_covariance[0] = -1.0;
    s_imu_msg.angular_velocity_covariance[0] = -1.0;
    s_imu_msg.linear_acceleration_covariance[0] = -1.0;

    s_allocator = rcl_get_default_allocator();
    s_init_options = rcl_get_zero_initialized_init_options();
    RCCHECK(rcl_init_options_init(&s_init_options, s_allocator));
    s_init_options_initialized = true;

    const rmw_ret_t ping_ret = rmw_uros_ping_agent(300, 3);
    if (ping_ret != RMW_RET_OK) {
        ESP_LOGW(TAG, "micro-ROS agent unavailable: %s:%u",
                 g_microros_agent_ip, static_cast<unsigned>(g_microros_agent_port));
        return false;
    }

    s_context = rcl_get_zero_initialized_context();
    RCCHECK(rcl_init(0, nullptr, &s_init_options, &s_context));
    s_context_initialized = true;

    if (!rmw_uros_epoch_synchronized()) {
        (void)rmw_uros_sync_session(1000);
    }

    s_node = rcl_get_zero_initialized_node();
    rcl_node_options_t node_options = rcl_node_get_default_options();
    node_options.enable_rosout = false;
    const rcl_ret_t node_ret = rcl_node_init(&s_node, "leap_low_driver", "", &s_context, &node_options);
    cleanup_result(rcl_node_options_fini(&node_options));
    if (node_ret != RCL_RET_OK) {
        ESP_LOGW(TAG, "rcl status line %d: %d", __LINE__, static_cast<int>(node_ret));
        return false;
    }
    s_node_initialized = true;

    rcl_publisher_options_t pub_options = rcl_publisher_get_default_options();
    rcl_publisher_options_t sensor_pub_options = rcl_publisher_get_default_options();
    sensor_pub_options.qos = rmw_qos_profile_sensor_data;
    s_odom_publisher = rcl_get_zero_initialized_publisher();
    RCCHECK(rcl_publisher_init(
        &s_odom_publisher,
        &s_node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(nav_msgs, msg, Odometry),
        "odom",
        &pub_options));
    s_odom_publisher_initialized = true;

    s_imu_publisher = rcl_get_zero_initialized_publisher();
    RCCHECK(rcl_publisher_init(
        &s_imu_publisher,
        &s_node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, Imu),
        "imu",
        &sensor_pub_options));
    s_imu_publisher_initialized = true;

    s_scan_publisher = rcl_get_zero_initialized_publisher();
    RCCHECK(rcl_publisher_init(
        &s_scan_publisher,
        &s_node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, LaserScan),
        "scan",
        &pub_options));
    s_scan_publisher_initialized = true;

    s_battery_publisher = rcl_get_zero_initialized_publisher();
    RCCHECK(rcl_publisher_init(
        &s_battery_publisher,
        &s_node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, BatteryState),
        "battery_state",
        &sensor_pub_options));
    s_battery_publisher_initialized = true;

    rcl_subscription_options_t sub_options = rcl_subscription_get_default_options();
    sub_options.qos = rmw_qos_profile_sensor_data;
    s_cmd_vel_subscriber = rcl_get_zero_initialized_subscription();
    RCCHECK(rcl_subscription_init(
        &s_cmd_vel_subscriber,
        &s_node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(geometry_msgs, msg, Twist),
        "cmd_vel",
        &sub_options));
    s_cmd_vel_subscriber_initialized = true;

    s_clock = {};
    s_publish_timer = rcl_get_zero_initialized_timer();
    RCCHECK(rcl_steady_clock_init(&s_clock, &s_allocator));
    s_clock_initialized = true;
    RCCHECK(rcl_timer_init(
        &s_publish_timer,
        &s_clock,
        &s_context,
        RCL_MS_TO_NS(20),
        publish_state_timer,
        s_allocator));
    s_timer_initialized = true;

    s_wait_set = rcl_get_zero_initialized_wait_set();
    RCCHECK(rcl_wait_set_init(&s_wait_set, 1, 0, 1, 0, 0, 0, &s_context, s_allocator));
    s_wait_set_initialized = true;
    s_ros_created = true;
    return true;
}

static void destroy_ros_entities() {
    if (!s_ros_created &&
        !s_strings_initialized &&
        !s_scan_ranges_initialized &&
        !s_init_options_initialized &&
        !s_context_initialized) {
        return;
    }

    if (s_context_initialized) {
        rmw_context_t *rmw_context = rcl_context_get_rmw_context(&s_context);
        if (rmw_context) {
            (void)rmw_uros_set_context_entity_destroy_session_timeout(rmw_context, 0);
        }
    }

    if (s_wait_set_initialized) {
        cleanup_result(rcl_wait_set_fini(&s_wait_set));
        s_wait_set_initialized = false;
    }
    if (s_timer_initialized) {
        cleanup_result(rcl_timer_fini(&s_publish_timer));
        s_timer_initialized = false;
    }
    if (s_clock_initialized) {
        cleanup_result(rcl_clock_fini(&s_clock));
        s_clock_initialized = false;
    }
    if (s_scan_publisher_initialized) {
        cleanup_result(rcl_publisher_fini(&s_scan_publisher, &s_node));
        s_scan_publisher_initialized = false;
    }
    if (s_battery_publisher_initialized) {
        cleanup_result(rcl_publisher_fini(&s_battery_publisher, &s_node));
        s_battery_publisher_initialized = false;
    }
    if (s_imu_publisher_initialized) {
        cleanup_result(rcl_publisher_fini(&s_imu_publisher, &s_node));
        s_imu_publisher_initialized = false;
    }
    if (s_odom_publisher_initialized) {
        cleanup_result(rcl_publisher_fini(&s_odom_publisher, &s_node));
        s_odom_publisher_initialized = false;
    }
    if (s_cmd_vel_subscriber_initialized) {
        cleanup_result(rcl_subscription_fini(&s_cmd_vel_subscriber, &s_node));
        s_cmd_vel_subscriber_initialized = false;
    }
    if (s_node_initialized) {
        cleanup_result(rcl_node_fini(&s_node));
        s_node_initialized = false;
    }
    if (s_context_initialized) {
        cleanup_result(rcl_shutdown(&s_context));
        cleanup_result(rcl_context_fini(&s_context));
        s_context_initialized = false;
    }
    if (s_init_options_initialized) {
        cleanup_result(rcl_init_options_fini(&s_init_options));
        s_init_options_initialized = false;
    }
    if (s_strings_initialized) {
        rosidl_runtime_c__String__fini(&s_odom_msg.header.frame_id);
        rosidl_runtime_c__String__fini(&s_odom_msg.child_frame_id);
        rosidl_runtime_c__String__fini(&s_imu_msg.header.frame_id);
        rosidl_runtime_c__String__fini(&s_scan_msg.header.frame_id);
        s_strings_initialized = false;
    }
    if (s_scan_ranges_initialized) {
        rosidl_runtime_c__float32__Sequence__fini(&s_scan_msg.ranges);
        s_scan_ranges_initialized = false;
    }
    if (s_battery_msg_initialized) {
        sensor_msgs__msg__BatteryState__fini(&s_battery_msg);
        s_battery_msg_initialized = false;
    }

    s_ros_created = false;
    reset_ros_handles();
}

static bool spin_once(int timeout_ms) {
    if (!s_wait_set_initialized) {
        return false;
    }

    rcl_ret_t ret = rcl_wait_set_clear(&s_wait_set);
    if (ret != RCL_RET_OK) {
        ESP_LOGW(TAG, "rcl_wait_set_clear failed: %d", static_cast<int>(ret));
        return false;
    }
    ret = rcl_wait_set_add_subscription(&s_wait_set, &s_cmd_vel_subscriber, nullptr);
    if (ret != RCL_RET_OK) {
        ESP_LOGW(TAG, "rcl_wait_set_add_subscription failed: %d", static_cast<int>(ret));
        return false;
    }
    ret = rcl_wait_set_add_timer(&s_wait_set, &s_publish_timer, nullptr);
    if (ret != RCL_RET_OK) {
        ESP_LOGW(TAG, "rcl_wait_set_add_timer failed: %d", static_cast<int>(ret));
        return false;
    }

    ret = rcl_wait(&s_wait_set, RCL_MS_TO_NS(timeout_ms));
    if (ret == RCL_RET_TIMEOUT) {
        return true;
    }
    if (ret != RCL_RET_OK) {
        ESP_LOGW(TAG, "rcl_wait failed: %d", static_cast<int>(ret));
        return false;
    }

    if (s_wait_set.subscriptions[0] != nullptr) {
        const rcl_ret_t take_ret = rcl_take(&s_cmd_vel_subscriber, &s_cmd_vel_msg, nullptr, nullptr);
        if (take_ret == RCL_RET_OK) {
            handle_cmd_vel(&s_cmd_vel_msg);
        } else if (take_ret != RCL_RET_SUBSCRIPTION_TAKE_FAILED) {
            ESP_LOGW(TAG, "rcl_take failed: %d", static_cast<int>(take_ret));
        }
    }

    if (s_wait_set.timers[0] != nullptr) {
        RCSOFTCHECK(rcl_timer_call(&s_publish_timer));
    }
    return true;
}

void microros_task(void *p) {
    (void)p;

    while (1) {
        if (g_wifi_comm_mode == WifiCommMode::kMicroRos) {
            if (!s_ros_created) {
                if (setup_udp_transport() && create_ros_entities()) {
                    ESP_LOGI(TAG, "micro-ROS active: agent=%s:%u, pub=/odom,/imu,/scan, sub=/cmd_vel",
                             g_microros_agent_ip,
                             static_cast<unsigned>(g_microros_agent_port));
                } else {
                    destroy_ros_entities();
                    vTaskDelay(pdMS_TO_TICKS(1000));
                    continue;
                }
            }
            if (!spin_once(20)) {
                destroy_ros_entities();
                vTaskDelay(pdMS_TO_TICKS(1000));
            }
        } else {
            if (s_ros_created) {
                destroy_ros_entities();
                ESP_LOGI(TAG, "micro-ROS inactive");
            }
            vTaskDelay(pdMS_TO_TICKS(100));
        }
    }
}
