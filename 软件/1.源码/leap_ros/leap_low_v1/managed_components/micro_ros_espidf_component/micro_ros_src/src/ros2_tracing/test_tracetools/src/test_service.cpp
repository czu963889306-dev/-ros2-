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

#include "rclcpp/rclcpp.hpp"
#include "std_srvs/srv/empty.hpp"

#define NODE_NAME "test_service"
#define SERVICE_NAME "the_service"

class ServiceNode : public rclcpp::Node
{
public:
  explicit ServiceNode(rclcpp::NodeOptions options)
  : Node(NODE_NAME, options)
  {
    srv_ = this->create_service<std_srvs::srv::Empty>(
      SERVICE_NAME,
      std::bind(
        &ServiceNode::service_callback,
        this,
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3));
  }

private:
  void service_callback(
    const std::shared_ptr<rmw_request_id_t> request_header,
    const std::shared_ptr<std_srvs::srv::Empty::Request> request,
    const std::shared_ptr<std_srvs::srv::Empty::Response> response)
  {
    // Nothing
    (void)request_header;
    (void)request;
    (void)response;
  }

  rclcpp::Service<std_srvs::srv::Empty>::SharedPtr srv_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);

  rclcpp::executors::SingleThreadedExecutor exec;
  auto service_node = std::make_shared<ServiceNode>(rclcpp::NodeOptions());
  exec.add_node(service_node);

  printf("spinning once\n");
  exec.spin_once();

  rclcpp::shutdown();
  return 0;
}
