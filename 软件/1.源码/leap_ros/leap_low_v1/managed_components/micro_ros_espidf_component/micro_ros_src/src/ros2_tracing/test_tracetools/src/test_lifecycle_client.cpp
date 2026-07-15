// Copyright 2020 Christophe Bedard
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

// Based on:
// https://github.com/ros2/demos/blob/fab4fc38d5ea589c019659d6a49da4ba9c20594b/lifecycle/src/lifecycle_service_client.cpp

#include <chrono>
#include <memory>
#include <thread>

#include "lifecycle_msgs/msg/state.hpp"
#include "lifecycle_msgs/msg/transition.hpp"
#include "lifecycle_msgs/srv/change_state.hpp"
#include "lifecycle_msgs/srv/get_state.hpp"
#include "rclcpp/rclcpp.hpp"
#include "rcutils/logging_macros.h"

using namespace std::chrono_literals;

static constexpr char const * lifecycle_node_name = "test_lifecycle_node";
static constexpr char const * lifecycle_node_get_state_topic =
  "test_lifecycle_node/get_state";
static constexpr char const * lifecycle_node_change_state_topic =
  "test_lifecycle_node/change_state";

template<typename FutureT, typename WaitTimeT>
std::future_status
wait_for_result(
  FutureT & future,
  WaitTimeT time_to_wait)
{
  auto end = std::chrono::steady_clock::now() + time_to_wait;
  std::chrono::milliseconds wait_period(100);
  std::future_status status = std::future_status::timeout;
  do {
    auto now = std::chrono::steady_clock::now();
    auto time_left = end - now;
    if (time_left <= std::chrono::seconds(0)) {break;}
    status = future.wait_for((time_left < wait_period) ? time_left : wait_period);
  } while (rclcpp::ok() && status != std::future_status::ready);
  return status;
}

class LifecycleNodeClient : public rclcpp::Node
{
public:
  explicit LifecycleNodeClient(rclcpp::NodeOptions options)
  : Node("test_lifecycle_client", options)
  {}

  void init()
  {
    client_get_state_ = this->create_client<lifecycle_msgs::srv::GetState>(
      lifecycle_node_get_state_topic);
    client_change_state_ = this->create_client<lifecycle_msgs::srv::ChangeState>(
      lifecycle_node_change_state_topic);
  }

  unsigned int get_state(std::chrono::seconds time_out = 3s)
  {
    auto request = std::make_shared<lifecycle_msgs::srv::GetState::Request>();

    if (!client_get_state_->wait_for_service(time_out)) {
      RCLCPP_ERROR(
        get_logger(),
        "Service %s is not available.",
        client_get_state_->get_service_name());
      return lifecycle_msgs::msg::State::PRIMARY_STATE_UNKNOWN;
    }

    // We send the service request for asking the current
    // state of the lc_talker node.
    auto future_result = client_get_state_->async_send_request(request).future.share();

    // Let's wait until we have the answer from the node.
    // If the request times out, we return an unknown state.
    auto future_status = wait_for_result(future_result, time_out);

    if (future_status != std::future_status::ready) {
      RCLCPP_ERROR(
        get_logger(),
        "Server time out while getting current state for node %s",
        lifecycle_node_name);
      return lifecycle_msgs::msg::State::PRIMARY_STATE_UNKNOWN;
    }

    // We have an succesful answer. So let's print the current state.
    if (future_result.get()) {
      RCLCPP_INFO(
        get_logger(), "Node %s has current state %s.",
        lifecycle_node_name, future_result.get()->current_state.label.c_str());
      return future_result.get()->current_state.id;
    } else {
      RCLCPP_ERROR(
        get_logger(), "Failed to get current state for node %s", lifecycle_node_name);
      return lifecycle_msgs::msg::State::PRIMARY_STATE_UNKNOWN;
    }
  }

  bool change_state(
    std::uint8_t transition,
    const bool ignore_response = false,
    std::chrono::seconds time_out = 3s)
  {
    auto request = std::make_shared<lifecycle_msgs::srv::ChangeState::Request>();
    request->transition.id = transition;

    if (!client_change_state_->wait_for_service(time_out)) {
      RCLCPP_ERROR(
        get_logger(),
        "Service %s is not available.",
        client_change_state_->get_service_name());
      return false;
    }

    // We send the request with the transition we want to invoke.
    auto future_result = client_change_state_->async_send_request(request).future.share();

    if (ignore_response) {
      return true;
    }

    // Let's wait until we have the answer from the node.
    // If the request times out, we return an unknown state.
    auto future_status = wait_for_result(future_result, time_out);

    if (future_status != std::future_status::ready) {
      RCLCPP_ERROR(
        get_logger(),
        "Server time out while getting current state for node %s",
        lifecycle_node_name);
      return false;
    }

    // We have an answer, let's print our success.
    if (future_result.get()->success) {
      RCLCPP_INFO(
        get_logger(), "Transition %d successfully triggered.", static_cast<int>(transition));
      return true;
    } else {
      RCLCPP_WARN(
        get_logger(), "Failed to trigger transition %u", static_cast<unsigned int>(transition));
      return false;
    }
  }

private:
  std::shared_ptr<rclcpp::Client<lifecycle_msgs::srv::GetState>> client_get_state_;
  std::shared_ptr<rclcpp::Client<lifecycle_msgs::srv::ChangeState>> client_change_state_;
};

void cycle_through_states(std::shared_ptr<LifecycleNodeClient> client_node)
{
  rclcpp::WallRate time_between_state_changes(1);

  {
    if (!client_node->change_state(lifecycle_msgs::msg::Transition::TRANSITION_CONFIGURE)) {
      return;
    }
    if (!client_node->get_state()) {
      return;
    }
  }

  {
    time_between_state_changes.sleep();
    if (!rclcpp::ok()) {
      return;
    }
    if (!client_node->change_state(lifecycle_msgs::msg::Transition::TRANSITION_ACTIVATE)) {
      return;
    }
    if (!client_node->get_state()) {
      return;
    }
  }

  {
    time_between_state_changes.sleep();
    if (!rclcpp::ok()) {
      return;
    }
    if (!client_node->change_state(lifecycle_msgs::msg::Transition::TRANSITION_DEACTIVATE)) {
      return;
    }
    if (!client_node->get_state()) {
      return;
    }
  }

  {
    time_between_state_changes.sleep();
    if (!rclcpp::ok()) {
      return;
    }
    if (!client_node->change_state(lifecycle_msgs::msg::Transition::TRANSITION_ACTIVATE)) {
      return;
    }
    if (!client_node->get_state()) {
      return;
    }
  }

  {
    time_between_state_changes.sleep();
    if (!rclcpp::ok()) {
      return;
    }
    if (!client_node->change_state(lifecycle_msgs::msg::Transition::TRANSITION_DEACTIVATE)) {
      return;
    }
    if (!client_node->get_state()) {
      return;
    }
  }

  {
    time_between_state_changes.sleep();
    if (!rclcpp::ok()) {
      return;
    }
    if (!client_node->change_state(lifecycle_msgs::msg::Transition::TRANSITION_CLEANUP)) {
      return;
    }
    if (!client_node->get_state()) {
      return;
    }
  }

  {
    time_between_state_changes.sleep();
    if (!rclcpp::ok()) {
      return;
    }
    // Do not wait for response or check state because the lifecycle node exits on shutdown
    if (!client_node->change_state(
        lifecycle_msgs::msg::Transition::TRANSITION_UNCONFIGURED_SHUTDOWN, true))
    {
      return;
    }
  }

  rclcpp::shutdown();
}

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);

  rclcpp::executors::SingleThreadedExecutor exec;
  auto client_node = std::make_shared<LifecycleNodeClient>(rclcpp::NodeOptions());
  client_node->init();
  exec.add_node(client_node);

  std::shared_future<void> script = std::async(
    std::launch::async, std::bind(cycle_through_states, client_node));
  exec.spin_until_future_complete(script);

  rclcpp::shutdown();

  return 0;
}
