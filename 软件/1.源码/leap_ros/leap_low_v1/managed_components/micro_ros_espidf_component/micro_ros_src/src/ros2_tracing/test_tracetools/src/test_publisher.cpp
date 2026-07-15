// Copyright 2019 Robert Bosch GmbH
// Copyright 2021 Christophe Bedard
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
#include "std_msgs/msg/string.hpp"

#define NODE_NAME "test_publisher"
#define TOPIC_NAME "the_topic"
#define QUEUE_DEPTH 10

class PubNode : public rclcpp::Node
{
public:
  explicit PubNode(rclcpp::NodeOptions options)
  : Node(NODE_NAME, options)
  {
    pub_ = this->create_publisher<std_msgs::msg::String>(
      TOPIC_NAME,
      rclcpp::QoS(QUEUE_DEPTH));

    auto msg = std::make_shared<std_msgs::msg::String>();
    msg->data = "some random string";
    pub_->publish(*msg);
  }

private:
  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr pub_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);

  rclcpp::executors::SingleThreadedExecutor exec;
  auto pub_node = std::make_shared<PubNode>(rclcpp::NodeOptions());
  exec.add_node(pub_node);

  printf("spinning once\n");
  exec.spin_once();

  rclcpp::shutdown();
  return 0;
}
