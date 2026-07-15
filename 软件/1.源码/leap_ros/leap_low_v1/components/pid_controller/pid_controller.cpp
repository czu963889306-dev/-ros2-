// Copyright 2026. All rights reserved.

#include "pid_controller.h"

#include <algorithm>

PidController::PidController(PidMode mode, float kp, float ki, float kd,
                             float min_output, float max_output)
    : mode_(mode),
      kp_(kp),
      ki_(ki),
      kd_(kd),
      min_output_(min_output),
      max_output_(max_output),
      previous_error_(0.0f),
      current_output_(0.0f),
      last_delta_(0.0f),
      integral_(0.0f),
      previous_previous_error_(0.0f) {}

float PidController::Calculate(float setpoint, float measured_value, float dt) {
  // 防止 dt 为 0 或负数导致除零错误
  if (dt <= 0.0f) {
    return current_output_;
  }

  float error = setpoint - measured_value;
  float output = 0.0f;

  if (mode_ == PidMode::kPositional) {
    // ==========================================
    // 位置式 PID 计算
    // ==========================================
    float p_term = kp_ * error;
    float d_term = kd_ * (error - previous_error_) / dt;

    // 抗积分饱和处理
    float provisional_output = p_term + (ki_ * integral_) + d_term;
    bool is_saturated_high = (provisional_output >= max_output_ && error > 0.0f);
    bool is_saturated_low = (provisional_output <= min_output_ && error < 0.0f);

    if (!is_saturated_high && !is_saturated_low) {
      integral_ += error * dt;
    }

    float i_term = ki_ * integral_;

    output = p_term + i_term + d_term;
    
    // 计算对于位置式而言的等效 delta（方便统一接口调用）
    last_delta_ = output - current_output_;
    previous_error_ = error;

  } else {
    // ==========================================
    // 增量式 PID 计算
    // ==========================================
    float p_term = kp_ * (error - previous_error_);
    float i_term = ki_ * error * dt;
    float d_term = kd_ * (error - 2.0f * previous_error_ + previous_previous_error_) / dt;

    last_delta_ = p_term + i_term + d_term;
    output = current_output_ + last_delta_;

    previous_previous_error_ = previous_error_;
    previous_error_ = error;
  }

  // 统一进行输出限幅
  current_output_ = std::clamp(output, min_output_, max_output_);

  // 限幅后修正 last_delta_，确保获取的增量与实际受限输出匹配
  if (mode_ == PidMode::kIncremental) {
      // 增量修正：实际输出变化量 = 限幅后的输出 - 之前的输出
      last_delta_ = current_output_ - (output - last_delta_); 
  }

  return current_output_;
}

float PidController::GetLastDelta() const {
  return last_delta_;
}

void PidController::Reset() {
  previous_error_ = 0.0f;
  current_output_ = 0.0f;
  last_delta_ = 0.0f;
  integral_ = 0.0f;
  previous_previous_error_ = 0.0f;
}

void PidController::SetGains(float kp, float ki, float kd) {
  kp_ = kp;
  ki_ = ki;
  kd_ = kd;
  Reset(); 
}

void PidController::SetOutputLimits(float min_output, float max_output) {
  min_output_ = min_output;
  max_output_ = max_output;
}

void PidController::SetMode(PidMode mode) {
  if (mode_ != mode) {
    mode_ = mode;
    Reset();  // 切换模式时必须重置状态，防止历史数据污染
  }
}