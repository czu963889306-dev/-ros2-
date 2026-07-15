# Leap Low v1 MAVLink and micro-ROS Protocols

This document describes the network protocol surface implemented by `leap_low_v1`.

## Runtime Mode

The firmware supports one active Wi-Fi communication mode at a time:

| Mode | Runtime value | Default |
| --- | --- | --- |
| micro-ROS | `micro_ros` | yes |
| MAVLink UDP | `mavlink_udp` | no |

The mode is stored in NVS under runtime configuration and can be changed from the web configuration page. The default micro-ROS agent is `192.168.31.214:8888`.

## Common Units

| Quantity | Internal unit | MAVLink unit | ROS unit |
| --- | --- | --- | --- |
| Linear position | mm | m | m |
| Linear velocity | mm/s | m/s | m/s |
| Yaw / angular velocity | rad / rad/s | rad / rad/s | rad / rad/s |
| IMU acceleration | g | mg in `SCALED_IMU`, m/s^2 in ROS | m/s^2 |
| IMU gyro | deg/s | rad/s or mrad/s | rad/s |
| Lidar distance | mm | cm in `OBSTACLE_DISTANCE` | m |
| Battery voltage | V | mV | V |

## micro-ROS

Transport: custom UDP transport over Wi-Fi.

| Item | Value |
| --- | --- |
| Node name | `leap_low_driver` |
| Local UDP port | `CONFIG_MICRO_ROS_LOCAL_PORT`, default `8888` |
| Agent address | runtime `g_microros_agent_ip:g_microros_agent_port`, default `192.168.31.214:8888` |
| Timer period | 20 ms |

### Subscribed Topics

| Topic | Type | QoS | Mapping |
| --- | --- | --- | --- |
| `/cmd_vel` | `geometry_msgs/msg/Twist` | sensor data | `linear.x/y` in m/s -> target `vx/vy` in mm/s; `angular.z` -> target `wz` in rad/s |

Receiving `/cmd_vel` writes a velocity command to `q_motion_cmd`, marks the source as micro-ROS, and clears `g_emergency_stop`.

### Published Topics

| Topic | Type | Rate | Frame | Notes |
| --- | --- | --- | --- | --- |
| `/odom` | `nav_msgs/msg/Odometry` | 50 Hz | `odom`, child `base_link` | position and velocity from motion state |
| `/imu` | `sensor_msgs/msg/Imu` | 50 Hz | `imu_link` | quaternion, gyro, acceleration |
| `/scan` | `sensor_msgs/msg/LaserScan` | 10 Hz | `laser_frame` | 360 points, 1 degree increment, 0.02-12.0 m |
| `/battery_state` | `sensor_msgs/msg/BatteryState` | 10 Hz | `battery` | voltage and percentage; LiPo, discharging |

`/battery_state` publishes:

| Field | Value |
| --- | --- |
| `voltage` | measured battery voltage in V |
| `percentage` | `0.0` to `1.0` |
| `power_supply_status` | `DISCHARGING` |
| `power_supply_health` | `GOOD` |
| `power_supply_technology` | `LIPO` |
| `present` | `true` |
| `temperature/current/charge/capacity/design_capacity` | `NaN` |

## MAVLink UDP

Transport: UDP socket bound to port `14550`.

| Item | Value |
| --- | --- |
| Local port | `14550` |
| Initial target | broadcast `255.255.255.255:14550` |
| Connected target | first MAVLink sender address |
| System ID | `1` |
| Component ID | `1` |
| Main loop period | 20 ms |
| Vehicle type | `MAV_TYPE_GROUND_ROVER` |
| Autopilot | `MAV_AUTOPILOT_GENERIC` |

### Received Messages

| Message | Effect |
| --- | --- |
| `SET_POSITION_TARGET_LOCAL_NED` | Position command if x/y/yaw fields are enabled; velocity command if vx/vy/yaw_rate fields are enabled |
| `COMMAND_LONG` | Handles arm/disarm, odometry reset, servo, direct wheel speed, relative move, and PID update commands |
| `SET_RGB_LED` | Sets RGB LED directly |
| `SET_ACTUATOR_CONTROL_TARGET` | Uses `controls[0..2] * 255` as RGB |
| `PARAM_REQUEST_READ` | Returns one PID parameter |
| `PARAM_REQUEST_LIST` | Returns all PID parameters |
| `PARAM_SET` | Updates one PID parameter and returns the new value |

### COMMAND_LONG Commands

| Command | Parameters | Result |
| --- | --- | --- |
| `MAV_CMD_COMPONENT_ARM_DISARM` | `param1 < 0.5`: stop; otherwise clear emergency stop | `ACCEPTED` |
| `MAV_CMD_PREFLIGHT_SET_SENSOR_OFFSETS` | none | reset odometry |
| `MAV_CMD_DO_SET_SERVO` | `param2`: servo angle | writes `q_servo_cmd` |
| `MAV_CMD_DO_SET_ACTUATOR` | `param1`: left wheel target, `param2`: right wheel target | wheel-speed mode |
| `MAV_CMD_USER_2` | `param1`: relative distance, `param2`: relative yaw | relative motion mode |
| `MAV_CMD_USER_4` | `param1`: PID target, `param2`: kp, `param3`: ki, `param4`: kd | updates speed or position PID |

Custom command IDs:

| Name | Value | Meaning |
| --- | --- | --- |
| `MATURO_MAV_CMD_MOVE_RELATIVE` | `MAV_CMD_USER_2` | relative movement |
| `MATURO_MAV_CMD_SET_PID` | `MAV_CMD_USER_4` | set PID values |

PID target values:

| Value | Target |
| --- | --- |
| `1` | speed PID |
| `2` | position PID |

### Parameters

MAVLink parameter protocol exposes:

| Parameter | Type | Meaning |
| --- | --- | --- |
| `SPD_KP` | `MAV_PARAM_TYPE_REAL32` | speed PID kp |
| `SPD_KI` | `MAV_PARAM_TYPE_REAL32` | speed PID ki |
| `SPD_KD` | `MAV_PARAM_TYPE_REAL32` | speed PID kd |
| `POS_KP` | `MAV_PARAM_TYPE_REAL32` | position PID kp |
| `POS_KI` | `MAV_PARAM_TYPE_REAL32` | position PID ki |
| `POS_KD` | `MAV_PARAM_TYPE_REAL32` | position PID kd |

### Published Messages

Published every 20 ms when source data is available:

| Message | Contents |
| --- | --- |
| `ATTITUDE` | roll/pitch/yaw and gyro in rad |
| `ATTITUDE_QUATERNION` | quaternion and gyro in rad/s |
| `SCALED_IMU` | acceleration in mg, gyro in mrad/s, magnetometer zero |
| `ODOMETRY` | local FLU pose and body FRD twist |

Published every 100 ms:

| Message | Contents |
| --- | --- |
| `DISTANCE_SENSOR` | ultrasonic distance, 2-400 cm |
| `DEBUG` | index `1`, motion busy as `0.0` or `1.0` |
| `RAW_RPM` | index `0`: left wheel RPM; index `1`: right wheel RPM |
| `OBSTACLE_DISTANCE` | lidar binned into 72 bins, 5 degrees per bin, 2-1200 cm |

Published every 1 s:

| Message | Contents |
| --- | --- |
| `HEARTBEAT` | rover heartbeat |
| `STATUSTEXT` | `DEVICE_NAME:<device_name>` |
| `ONBOARD_COMPUTER_STATUS` | heap, flash, Wi-Fi link estimate, board temperature if valid |
| `SYS_STATUS` | battery voltage in mV and remaining percentage |
| `BATTERY_STATUS` | LiPo battery voltage and remaining percentage |

Published once after first MAVLink client is seen:

| Message | Contents |
| --- | --- |
| `COMPONENT_INFORMATION_BASIC` | vendor `Maturo`, model `Driver`, software `2026.05`, hardware `ESP32`, serial set to device name |

MAVLink publishing follows the active Wi-Fi mode and target address behavior described above.

### Battery Reporting

Battery status is based on GPIO3 ADC sampling:

| Field | Value |
| --- | --- |
| Full voltage | `8.4 V` |
| Empty voltage estimate | `6.0 V` |
| Divider ratio | `8.4 / 2.6857` |
| Percentage | linear estimate from 6.0 V to 8.4 V |

MAVLink battery fields:

| Message | Field | Value |
| --- | --- | --- |
| `SYS_STATUS` | `voltage_battery` | battery voltage in mV, `UINT16_MAX` if invalid |
| `SYS_STATUS` | `current_battery` | `-1` unknown |
| `SYS_STATUS` | `battery_remaining` | `0..100`, `-1` if invalid |
| `BATTERY_STATUS` | `battery_function` | `MAV_BATTERY_FUNCTION_ALL` |
| `BATTERY_STATUS` | `type` | `MAV_BATTERY_TYPE_LIPO` |
| `BATTERY_STATUS` | `voltages[0]` | battery voltage in mV |
| `BATTERY_STATUS` | `current_battery/current_consumed/energy_consumed` | `-1` unknown |
| `BATTERY_STATUS` | `charge_state` | OK, LOW at <=20%, CRITICAL at <=10% |

## Mode Switching Notes

Only the active mode sends network traffic:

| Active mode | Behavior |
| --- | --- |
| `micro_ros` | micro-ROS entities are created and spun; MAVLink UDP socket does not publish or receive |
| `mavlink_udp` | MAVLink UDP task receives and publishes; micro-ROS entities are destroyed/inactive |

The web runtime configuration saves the selected mode and micro-ROS agent address in NVS, so the setting persists after reboot.
