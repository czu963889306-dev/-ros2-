#ifndef MOTION_CONTROLLER_H_
#define MOTION_CONTROLLER_H_

#include "motor_driver.h"
#include "encoder_driver.h"
#include "pid_controller.h"

enum class ControlMode { kVelocity, kPosition, kRelativeSeq };
enum class SeqState { kIdle, kForward, kSettle, kTurn };

enum class MotorID {
  kLeft = 0,
  kRight = 1,
};

class MotionController {
 public:
  MotionController(At8236Motor& left_motor, At8236Motor& right_motor,
                   QuadratureEncoder& left_encoder, QuadratureEncoder& right_encoder);
                   
  int16_t ApplyDeadband(float target_vel, float pid_output);
  void SetMaxAcceleration(float accel_mms2);
  void MoveToPosition(float target_x, float target_y, float target_yaw);
  void MoveRelative(float distance_mm, float angle_deg);
  bool IsBusy() const { return control_mode_ == ControlMode::kRelativeSeq && seq_state_ != SeqState::kIdle; }
  
  void GetOdometry(float* x, float* y, float* yaw) const;
  void GetOdometryQuaternion(float* qw, float* qx, float* qy, float* qz) const;
  float GetTotalDistance() const; // 新增：获取小车真实走过的总轨迹路程
  void ResetOdometry();
  
  void SetPositionPidGains(float kp, float ki, float kd);

  void SetMotorTargetVelocity(MotorID motor_id, float target_mms);
  void SetAllMotorTargetsVelocity(float left_mms, float right_mms);
  float GetMotorVelocity(MotorID motor_id) const;
  void GetAllMotorVelocities(float* left_mms, float* right_mms) const;

  void Drive(float linear_x, float linear_y, float angular_z);
  void GetVelocity(float* linear_x, float* linear_y, float* angular_z) const;
  void SetVelocityPidGains(float kp, float ki, float kd);
  void Update(float dt, float current_imu_yaw_rad);
  void Stop();

 private:
  void CalculateKinematics(float linear_x, float linear_y, float angular_z);
  static float NormalizeAngle(float angle);

  static constexpr int kNumWheels = 2;
  At8236Motor* motors_[kNumWheels];
  QuadratureEncoder* encs_[kNumWheels];
  PidController pid_vel_[kNumWheels];
  
  float target_vel_[kNumWheels] = {0};
  float final_target_vel_[kNumWheels] = {0};
  int32_t last_count_[kNumWheels] = {0};
  float filtered_rpm_[kNumWheels] = {0};

  ControlMode control_mode_ = ControlMode::kVelocity;

  PidController pid_pos_x_;
  PidController pid_pos_y_;
  PidController pid_pos_yaw_;

  float pos_x_ = 0.0f, pos_y_ = 0.0f, odom_yaw_ = 0.0f;
  float imu_yaw_rel_ = 0.0f;
  float total_distance_ = 0.0f; // 新增：累计轨迹路程(包含曲线)
  float target_pos_x_ = 0.0f, target_pos_y_ = 0.0f, target_pos_yaw_ = 0.0f;
  float yaw_offset_ = 0.0f;
  bool need_reset_yaw_offset_ = true;

  SeqState seq_state_ = SeqState::kIdle;
  float seq_target_dist_ = 0.0f, seq_target_angle_ = 0.0f;
  float seq_start_x_ = 0.0f, seq_start_y_ = 0.0f, seq_start_imu_yaw_ = 0.0f;

  float max_accel_rpm_s_ = 200.0f; 
  int seq_stable_count_ = 0; 
  int settle_count_ = 0;     
};

#endif // MOTION_CONTROLLER_H_
