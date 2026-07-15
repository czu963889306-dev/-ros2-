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
#include "std_srvs/srv/empty.hpp"

using namespace std::chrono_literals;

#define NODE_NAME "test_service_ping"
#define SERVICE_NAME "pong"
#define CLIENT_NAME "ping"

class PingNode : public rclcpp::Node
{
public:
  explicit PingNode(rclcpp::NodeOptions options)
  : Node(NODE_NAME, options)
  {
    srv_ = this->create_service<std_srvs::srv::Empty>(
      SERVICE_NAME,
      std::bind(
        &PingNode::service_callback,
        this,
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3));
    client_ = this->create_client<std_srvs::srv::Empty>(
      CLIENT_NAME);
    timer_ = this->create_wall_timer(
      500ms,
      std::bind(&PingNode::timer_callback, this));
  }

private:
  void service_callback(
    const std::shared_ptr<rmw_request_id_t> request_header,
    const std::shared_ptr<std_srvs::srv::Empty::Request> request,
    const std::shared_ptr<std_srvs::srv::Empty::Response> response)
  {
    (void)request_header;
    (void)request;
    (void)response;
    RCLCPP_INFO(this->get_logger(), "got request");
    rclcpp::shutdown();
  }

  void timer_callback()
  {
    auto req = std::make_shared<std_srvs::srv::Empty::Request>();
    client_->async_send_request(req);
  }

  rclcpp::Service<std_srvs::srv::Empty>::SharedPtr srv_;
  rclcpp::Client<std_srvs::srv::Empty>::SharedPtr client_;
  rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);

  rclcpp::executors::SingleThreadedExecutor exec;
  auto ping_node = std::make_shared<PingNode>(rclcpp::NodeOptions());
  exec.add_node(ping_node);

  printf("spinning\n");
  exec.spin();

  // Will actually be called inside the node's service callback
  rclcpp::shutdown();
  return 0;
}
