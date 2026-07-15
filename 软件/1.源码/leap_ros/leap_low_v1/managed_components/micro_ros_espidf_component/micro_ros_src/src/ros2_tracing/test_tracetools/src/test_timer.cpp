// Copyright 2019 Robert Bosch GmbH
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <memory>
#include <chrono>

#include "rclcpp/rclcpp.hpp"

using namespace std::chrono_literals;

#define NODE_NAME "test_timer"
#define TIMER_PERIOD 1ms

class TimerNode : public rclcpp::Node
{
public:
  explicit TimerNode(rclcpp::NodeOptions options)
  : Node(NODE_NAME, options)
  {
    is_done_ = false;
    timer_ = this->create_wall_timer(
      TIMER_PERIOD,
      std::bind(&TimerNode::timer_callback, this));
  }

private:
  void timer_callback()
  {
    if (is_done_) {
      rclcpp::shutdown();
    } else {
      is_done_ = true;
    }
  }

  rclcpp::TimerBase::SharedPtr timer_;
  bool is_done_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);

  rclcpp::executors::SingleThreadedExecutor exec;
  auto timer_node = std::make_shared<TimerNode>(rclcpp::NodeOptions());
  exec.add_node(timer_node);

  printf("spinning\n");
  exec.spin();

  // Will actually be called inside the timer's callback
  rclcpp::shutdown();
  return 0;
}
