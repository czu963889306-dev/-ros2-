// Copyright 2026. All rights reserved.

#include "motion_controller.h"
#include "esp_log.h"
#include <algorithm>
#include <cmath>

#define DEBUG_PID 0
static const char* TAG = "PID_CTRL";

static const float kWheelDiameter = 65.0f; 
static const float kTrackWidth = 150.0f;   
static const float kLy = kTrackWidth / 2.0f; 
static const float kRpmToMms = (M_PI * kWheelDiameter) / 60.0f; 
static const float kMmsToRpm = 1.0f / kRpmToMms;

static const float kKp = 1.0f, kKi = 6.0f, kKd = 0.0f, kMaxPwm = 255.0f;
static const float kMaxPosVel = 250.0f;   
static const float kMaxPosYaw = 3.0f;     
static const int kRelativeTurnSettleMaxCycles = 5;
static const float kRelativeTurnSettleVxMms = 10.0f;

static float MoveTowards(float current, float target, float max_step) {
  return target > current ? std::min(current + max_step, target) : std::max(current - max_step, target);
}

static bool IsNearZero(float value) {
  return std::abs(value) < 0.001f;
}

float MotionController::NormalizeAngle(float angle) {
  while (angle > M_PI) angle -= 2.0f * M_PI;
  while (angle < -M_PI) angle += 2.0f * M_PI;
  return angle;
}

MotionController::MotionController(
    At8236Motor& left_motor, At8236Motor& right_motor,
    QuadratureEncoder& left_encoder, QuadratureEncoder& right_encoder)
    : motors_{&left_motor, &right_motor},
      encs_{&left_encoder, &right_encoder},
      pid_vel_{
        PidController(PidMode::kIncremental, kKp, kKi, kKd, -kMaxPwm, kMaxPwm),
        PidController(PidMode::kIncremental, kKp, kKi, kKd, -kMaxPwm, kMaxPwm)
      },
      pid_pos_x_(PidMode::kPositional, 2.1f, 0.0f, 0.5f, -kMaxPosVel, kMaxPosVel),
      pid_pos_y_(PidMode::kPositional, 2.1f, 0.0f, 0.5f, -kMaxPosVel, kMaxPosVel),
      pid_pos_yaw_(PidMode::kPositional, 2.5f, 0.0f, 0.5f, -kMaxPosYaw, kMaxPosYaw) {}

void MotionController::SetMaxAcceleration(float accel_mms2) { max_accel_rpm_s_ = accel_mms2 * kMmsToRpm; }

void MotionController::MoveToPosition(float target_x, float target_y, float target_yaw) {
  target_pos_x_ = target_x; target_pos_y_ = target_y; target_pos_yaw_ = target_yaw;
  control_mode_ = ControlMode::kPosition;
}

void MotionController::MoveRelative(float distance_mm, float angle_deg) {
  seq_stable_count_ = 0; 
  control_mode_ = ControlMode::kRelativeSeq;
  seq_target_dist_ = distance_mm;
  seq_target_angle_ = angle_deg * (M_PI / 180.0f); 
  
  seq_start_x_ = pos_x_;
  seq_start_y_ = pos_y_;
  seq_start_imu_yaw_ = imu_yaw_rel_;

  pid_pos_x_.Reset(); pid_pos_yaw_.Reset();
  for (int i = 0; i < kNumWheels; ++i) {
      pid_vel_[i].Reset();
      target_vel_[i] = 0.0f;
  }

  if (std::abs(distance_mm) > 1.0f) seq_state_ = SeqState::kForward;
  else if (std::abs(angle_deg) > 0.5f) seq_state_ = SeqState::kTurn;
  else seq_state_ = SeqState::kIdle;
}

void MotionController::GetOdometry(float* x, float* y, float* yaw) const {
  if (x) *x = pos_x_;
  if (y) *y = pos_y_;
  if (yaw) *yaw = odom_yaw_;
}

void MotionController::GetOdometryQuaternion(
    float* qw, float* qx, float* qy, float* qz) const {
  const float half_yaw = odom_yaw_ * 0.5f;

  if (qw) *qw = std::cos(half_yaw);
  if (qx) *qx = 0.0f;
  if (qy) *qy = 0.0f;
  if (qz) *qz = std::sin(half_yaw);
}

float MotionController::GetTotalDistance() const {
  return total_distance_;
}

void MotionController::ResetOdometry() {
  for(int i = 0; i < kNumWheels; ++i) {
      encs_[i]->ResetCount();
      last_count_[i] = 0;
      filtered_rpm_[i] = 0.0f;
      pid_vel_[i].Reset();
      final_target_vel_[i] = 0.0f;
      target_vel_[i] = 0.0f;
  }
  need_reset_yaw_offset_ = true; 
  total_distance_ = 0.0f; // 重置总路程
  pos_x_ = pos_y_ = odom_yaw_ = imu_yaw_rel_ = 0.0f;
  target_pos_x_ = target_pos_y_ = target_pos_yaw_ = 0.0f;
  pid_pos_x_.Reset(); pid_pos_y_.Reset(); pid_pos_yaw_.Reset();
}

void MotionController::CalculateKinematics(float linear_x, float linear_y, float angular_z) {
  (void)linear_y;
  final_target_vel_[0] = (linear_x - angular_z * kLy) * kMmsToRpm;
  final_target_vel_[1] = (linear_x + angular_z * kLy) * kMmsToRpm;
}

void MotionController::Drive(float linear_x, float linear_y, float angular_z) {
  if (IsNearZero(linear_x) && IsNearZero(linear_y) && IsNearZero(angular_z)) {
    Stop();
    return;
  }

  control_mode_ = ControlMode::kVelocity; 
  CalculateKinematics(linear_x, linear_y, angular_z);
}

int16_t MotionController::ApplyDeadband(float target_vel, float pid_output) {
  const float kDeadband = 150.0f; 
  if (std::abs(target_vel) > 0.1f && std::abs(pid_output) > 0.1f) {
    float out = pid_output + (pid_output > 0 ? kDeadband : -kDeadband);
    return static_cast<int16_t>(std::clamp(out, -kMaxPwm, kMaxPwm));
  }
  return 0; 
}

void MotionController::Update(float dt, float current_imu_yaw_rad) {
  if (dt <= 0.0f) return;

  // 1. 读取并计算转速
  const float kAlpha = 0.35f;
  const float kPulsesPerRev = 4680.0f; 
  const int sign[kNumWheels] = {1, -1}; // 右侧轮反向读取

  for (int i = 0; i < kNumWheels; ++i) {
    int32_t current_count = encs_[i]->GetCount() * sign[i];
    float pps = (current_count - last_count_[i]) / dt;
    last_count_[i] = current_count;
    float raw_rpm = (pps / kPulsesPerRev) * 60.0f;
    filtered_rpm_[i] = kAlpha * raw_rpm + (1.0f - kAlpha) * filtered_rpm_[i];
  }

  // 2. 里程计更新
  float local_vx, local_vy, local_vw;
  GetVelocity(&local_vx, &local_vy, &local_vw);

  // 累加计算小车真实走过的总路程 (用于精确定长循迹)
  float step_dist = std::sqrt(local_vx * local_vx + local_vy * local_vy) * dt;
  total_distance_ += step_dist;

  if (need_reset_yaw_offset_) {
      yaw_offset_ = current_imu_yaw_rad;
      need_reset_yaw_offset_ = false;
  }
  imu_yaw_rel_ = NormalizeAngle(current_imu_yaw_rad - yaw_offset_);
  odom_yaw_ += local_vw * dt;
  odom_yaw_ = NormalizeAngle(odom_yaw_);
  pos_x_ += (local_vx * std::cos(odom_yaw_) - local_vy * std::sin(odom_yaw_)) * dt;
  pos_y_ += (local_vx * std::sin(odom_yaw_) + local_vy * std::cos(odom_yaw_)) * dt;
// ESP_LOGI(TAG,
//          "local_vx=%.6f, local_vy=%.6f, local_vw=%.6f, yaw=%.6f, pos_y=%.6f",
//          local_vx, local_vy, local_vw, odom_yaw_, pos_y_);
  // 3. 执行外环控制逻辑
  if (control_mode_ == ControlMode::kPosition) {
    float dx = target_pos_x_ - pos_x_;
    float dy = target_pos_y_ - pos_y_;
    float distance = std::sqrt(dx * dx + dy * dy);

    float cmd_vx = 0.0f, vw_cmd = 0.0f;
    if (distance > 15.0f) {
      float angle_error = NormalizeAngle(std::atan2(dy, dx) - odom_yaw_);
      vw_cmd = pid_pos_yaw_.Calculate(angle_error, 0.0f, dt);
      float v_cmd_raw = pid_pos_x_.Calculate(distance, 0.0f, dt);
      cmd_vx = std::cos(angle_error) > 0.0f ? v_cmd_raw * std::cos(angle_error) : 0.0f;
    } else {
      float final_angle_error = NormalizeAngle(target_pos_yaw_ - odom_yaw_);
      vw_cmd = pid_pos_yaw_.Calculate(final_angle_error, 0.0f, dt);
    }
    CalculateKinematics(cmd_vx, 0.0f, vw_cmd);
  } else if (control_mode_ == ControlMode::kRelativeSeq) {
    float cmd_vx = 0.0f, vw_cmd = 0.0f;

    if (seq_state_ == SeqState::kForward) {
      float dx = pos_x_ - seq_start_x_, dy = pos_y_ - seq_start_y_;
      float moved_dist = std::sqrt(dx * dx + dy * dy) * (seq_target_dist_ < 0 ? -1 : 1);

      if (std::abs(seq_target_dist_ - moved_dist) < 10.0f) { 
        for (int i = 0; i < kNumWheels; ++i) {
          pid_vel_[i].Reset();
          target_vel_[i] = 0.0f;
        }
        if (std::abs(seq_target_angle_) > 0.01f) { seq_state_ = SeqState::kSettle; settle_count_ = 0; }
        else seq_state_ = SeqState::kIdle;
      } else {
        cmd_vx = pid_pos_x_.Calculate(seq_target_dist_, moved_dist, dt);
      }
    } else if (seq_state_ == SeqState::kSettle) {
      if (std::abs(local_vx) < kRelativeTurnSettleVxMms ||
          ++settle_count_ > kRelativeTurnSettleMaxCycles) {
        seq_start_imu_yaw_ = imu_yaw_rel_;
        pid_pos_yaw_.Reset();
        seq_stable_count_ = 0; 
        seq_state_ = SeqState::kTurn; 
      }
    } else if (seq_state_ == SeqState::kTurn) {
      float moved_angle = NormalizeAngle(imu_yaw_rel_ - seq_start_imu_yaw_);
      float err = NormalizeAngle(seq_target_angle_ - moved_angle);
      
      if (std::abs(err) < 0.05f && std::abs(local_vw) < 0.1f) {
        if (++seq_stable_count_ > 10) {
            seq_state_ = SeqState::kIdle;
        }
        vw_cmd = 0.0f; 
      } else {
        seq_stable_count_ = 0; 
        vw_cmd = pid_pos_yaw_.Calculate(seq_target_angle_, moved_angle, dt); 
        
        const float kMinVw = 0.15f; 
        if (vw_cmd > 0.01f && vw_cmd < kMinVw) vw_cmd = kMinVw;
        if (vw_cmd < -0.01f && vw_cmd > -kMinVw) vw_cmd = -kMinVw;
      }
    }
    CalculateKinematics(cmd_vx, 0.0f, vw_cmd);
  }

  // 4. 下发电机控制指令与PID计算
  float max_rpm_step = max_accel_rpm_s_ * dt;
  bool velocity_stop_command = (control_mode_ == ControlMode::kVelocity);
  for (int i = 0; i < kNumWheels && velocity_stop_command; ++i) {
    velocity_stop_command = IsNearZero(final_target_vel_[i]);
  }

  if (velocity_stop_command) {
    for (int i = 0; i < kNumWheels; ++i) {
      target_vel_[i] = 0.0f;
      final_target_vel_[i] = 0.0f;
      pid_vel_[i].Reset();
      motors_[i]->Brake();
    }
    return;
  }

  for (int i = 0; i < kNumWheels; ++i) {
    if (control_mode_ == ControlMode::kRelativeSeq) {
      if (std::abs(final_target_vel_[i]) > std::abs(target_vel_[i])) {
        target_vel_[i] = MoveTowards(target_vel_[i], final_target_vel_[i], max_rpm_step);
      } else {
        target_vel_[i] = final_target_vel_[i];
      }
    } else {
      target_vel_[i] = final_target_vel_[i];
    }

    if (std::abs(target_vel_[i]) < 0.1f) pid_vel_[i].Reset();
    
    float raw_pwm = pid_vel_[i].Calculate(target_vel_[i], filtered_rpm_[i], dt);
    int16_t final_pwm = ApplyDeadband(target_vel_[i], raw_pwm);
    
#if DEBUG_PID
    if (i == 0) {
      ESP_LOGI(TAG, "Mode:%d Pos:(%.1f, %.1f) Yaw:%.2f PWM_L:%d",
               (int)control_mode_, pos_x_, pos_y_, odom_yaw_, final_pwm);
    }
#endif
    motors_[i]->SetSpeed(final_pwm);
  }
}

void MotionController::GetVelocity(float* linear_x, float* linear_y, float* angular_z) const {
  if (!linear_x || !linear_y || !angular_z) return;

  float v[kNumWheels];
  for (int i = 0; i < kNumWheels; ++i) {
    v[i] = filtered_rpm_[i] * kRpmToMms;
  }

  *linear_x = (v[0] + v[1]) / 2.0f;
  *linear_y = 0.0f;
  *angular_z = (-v[0] + v[1]) / (2.0f * kLy);
}

void MotionController::Stop() {
  control_mode_ = ControlMode::kVelocity; 
  for(int i = 0; i < kNumWheels; ++i) {
      target_vel_[i] = 0.0f;
      final_target_vel_[i] = 0.0f;
      pid_vel_[i].Reset();
      motors_[i]->Brake();
  }
  pid_pos_x_.Reset(); pid_pos_y_.Reset(); pid_pos_yaw_.Reset();
  seq_state_ = SeqState::kIdle; 
}

void MotionController::SetVelocityPidGains(float kp, float ki, float kd) {
  for (int i = 0; i < kNumWheels; ++i) {
    pid_vel_[i].SetGains(kp, ki, kd);
  }
}

void MotionController::SetPositionPidGains(float kp, float ki, float kd) {
  pid_pos_x_.SetGains(kp, ki, kd); pid_pos_y_.SetGains(kp, ki, kd); pid_pos_yaw_.SetGains(kp, ki, kd);
}

void MotionController::SetMotorTargetVelocity(MotorID motor_id, float target_mms) {
  control_mode_ = ControlMode::kVelocity; 
  const int motor_index = static_cast<int>(motor_id);
  if (motor_index < 0 || motor_index >= kNumWheels) {
    return;
  }
  final_target_vel_[motor_index] = target_mms * kMmsToRpm;
  if (IsNearZero(target_mms)) {
    target_vel_[motor_index] = 0.0f;
    pid_vel_[motor_index].Reset();
    motors_[motor_index]->Brake();
  }
}

void MotionController::SetAllMotorTargetsVelocity(float left_mms, float right_mms) {
  if (IsNearZero(left_mms) && IsNearZero(right_mms)) {
    Stop();
    return;
  }

  control_mode_ = ControlMode::kVelocity;
  final_target_vel_[0] = left_mms * kMmsToRpm;
  final_target_vel_[1] = right_mms * kMmsToRpm;
}

float MotionController::GetMotorVelocity(MotorID motor_id) const {
  const int motor_index = static_cast<int>(motor_id);
  if (motor_index < 0 || motor_index >= kNumWheels) {
    return 0.0f;
  }
  return filtered_rpm_[motor_index] * kRpmToMms;
}

void MotionController::GetAllMotorVelocities(float* left_mms, float* right_mms) const {
  if (left_mms) *left_mms = filtered_rpm_[0] * kRpmToMms;
  if (right_mms) *right_mms = filtered_rpm_[1] * kRpmToMms;
}
