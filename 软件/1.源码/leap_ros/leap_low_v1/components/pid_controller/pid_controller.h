// Copyright 2026. All rights reserved.
//
// 描述: 适用于 ESP32-S3 的统一 PID 控制器实现（支持位置式和增量式）。
// 遵循 Google C++ 编程规范。

#ifndef PID_CONTROLLER_H_
#define PID_CONTROLLER_H_

// PID 控制器工作模式
enum class PidMode {
  kPositional,  // 位置式：计算绝对输出量，带抗积分饱和
  kIncremental  // 增量式：计算输出变化量，天然抗积分饱和
};

class PidController {
 public:
  // 构造函数
  PidController(PidMode mode, float kp, float ki, float kd, 
                float min_output, float max_output);

  ~PidController() = default;

  // 计算 PID 输出
  // 根据当前模式返回限制在 [min_output, max_output] 范围内的 绝对输出值。
  // 参数:
  //   setpoint: 目标设定值
  //   measured_value: 当前实际测量值
  //   dt: 距离上次计算的时间间隔（秒）
  // 返回:
  //   控制器的绝对输出值 U(k)
  float Calculate(float setpoint, float measured_value, float dt);

  // 获取上一次计算得出的输出增量 ΔU
  // 仅在 kIncremental 模式下对某些执行器（如步进电机）有用
  float GetLastDelta() const;

  // 重置控制器的内部所有历史状态（积分器、历史误差、当前输出缓存）
  void Reset();

  // 动态更新 PID 参数
  void SetGains(float kp, float ki, float kd);

  // 动态更新输出限幅
  void SetOutputLimits(float min_output, float max_output);

  // 动态切换 PID 模式
  void SetMode(PidMode mode);

 private:
  PidMode mode_;

  // PID 参数
  float kp_;
  float ki_;
  float kd_;

  // 输出限幅
  float min_output_;
  float max_output_;

  // 内部状态 - 共享
  float previous_error_;           // e(k-1)
  float current_output_;           // 缓存的绝对输出值 U(k)
  float last_delta_;               // 上一次的输出增量 ΔU

  // 内部状态 - 仅位置式使用
  float integral_;                 // 误差积分累加值

  // 内部状态 - 仅增量式使用
  float previous_previous_error_;  // e(k-2)
};

#endif  // PID_CONTROLLER_H_