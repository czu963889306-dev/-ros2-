// Copyright (c) 2021 - for information on the respective copyright owner
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

#include "test_memory.hpp"

#include <std_msgs/msg/int32.h>
#include <std_msgs/msg/multi_array_layout.h>
#include <trajectory_msgs/msg/joint_trajectory.h>

#include <string>
#include <map>
#include <utility>

TEST_F(MicroROSTypeUtilities, check_size) {
  typesupport_ = ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, MultiArrayLayout);

  size_t size = micro_ros_utilities_get_dynamic_size(
    typesupport_,
    conf_
  );

  // Size should be:
  //   Introspection for std_msgs__msg/MultiArrayLayout - 2 members, 32 B
  //  - dim [Sequence of ROS2 type std_msgs__msg/MultiArrayDimension] <- This should be inited
  //          - label [string] <- This should be inited
  //          - size [uint32]
  //          - stride [uint32]
  //  - data_offset [uint32]
  //
  //  dim.label.size -> 0
  //  dim.label.capacity -> 0
  //  dim.label.data -> 20 chars = 20 B -> We have 5 dim -> 20 B x 5 = 100 B

  //  dim.size -> 0
  //  dim.stride -> 0
  //  dim.data -> 5 std_msgs__msg/MultiArrayDimension = 5 * (aligned) 32 B = 160 B

  ASSERT_EQ(size, 260UL);
}

TEST_F(MicroROSTypeUtilities, default_config) {
  typesupport_ = ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, MultiArrayLayout);

  std::array<std_msgs__msg__MultiArrayLayout, 2> msgs;

  ASSERT_TRUE(create_memory_dynamic(&msgs[0]));
  ASSERT_TRUE(create_memory_static(&msgs[1]));

  // Check message members
  for (auto & msg : msgs) {
    ASSERT_EQ(msg.data_offset, 0UL);
    ASSERT_EQ(msg.dim.size, 0UL);
    ASSERT_EQ(
      msg.dim.capacity,
      micro_ros_utilities_memory_conf_default.max_ros2_type_sequence_capacity);
    ASSERT_NE(msg.dim.data, nullptr);

    for (size_t i = 0; i < msg.dim.capacity; i++) {
      ASSERT_EQ(msg.dim.data[i].size, 0UL);
      ASSERT_EQ(msg.dim.data[i].stride, 0UL);
      ASSERT_NE(msg.dim.data[i].label.data, nullptr);
      ASSERT_EQ(msg.dim.data[i].label.size, 0UL);
      ASSERT_EQ(
        msg.dim.data[i].label.capacity,
        micro_ros_utilities_memory_conf_default.max_string_capacity);
    }
  }

  destroy_memory_dynamic();
}

TEST_F(MicroROSTypeUtilities, custom_config) {
  typesupport_ = ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, MultiArrayLayout);

  add_rule("dim", 3);
  add_rule("dim.label", 100);

  std::array<std_msgs__msg__MultiArrayLayout, 2> msgs;

  ASSERT_TRUE(create_memory_dynamic(&msgs[0]));
  ASSERT_TRUE(create_memory_static(&msgs[1]));

  // Check message members
  for (auto & msg : msgs) {
    ASSERT_EQ(msg.data_offset, 0UL);
    ASSERT_EQ(msg.dim.size, 0UL);
    ASSERT_EQ(msg.dim.capacity, get_rule_size("dim"));
    ASSERT_NE(msg.dim.data, nullptr);

    for (size_t i = 0; i < msg.dim.capacity; i++) {
      ASSERT_EQ(msg.dim.data[i].size, 0UL);
      ASSERT_EQ(msg.dim.data[i].stride, 0UL);
      ASSERT_NE(msg.dim.data[i].label.data, nullptr);
      ASSERT_EQ(msg.dim.data[i].label.size, 0UL);
      ASSERT_EQ(msg.dim.data[i].label.capacity, get_rule_size("dim.label"));
    }
  }

  destroy_memory_dynamic();
}

TEST_F(MicroROSTypeUtilities, string_sequence_regression) {
  typesupport_ = ROSIDL_GET_MSG_TYPE_SUPPORT(trajectory_msgs, msg, JointTrajectory);

  std::array<trajectory_msgs__msg__JointTrajectory, 2> msgs;

  ASSERT_TRUE(create_memory_dynamic(&msgs[0]));
  ASSERT_TRUE(create_memory_static(&msgs[1]));

  for (auto & msg : msgs) {
    ASSERT_EQ(msg.joint_names.size, 0UL);
    ASSERT_EQ(
      msg.joint_names.capacity,
      micro_ros_utilities_memory_conf_default.max_ros2_type_sequence_capacity);
    ASSERT_NE(msg.joint_names.data, nullptr);

    for (size_t i = 0; i < msg.joint_names.capacity; i++) {
      ASSERT_EQ(msg.joint_names.data[i].size, 0UL);
      ASSERT_EQ(
        msg.joint_names.data[i].capacity,
        micro_ros_utilities_memory_conf_default.max_string_capacity);
      ASSERT_NE(msg.joint_names.data[i].data, nullptr);
    }
  }

  destroy_memory_dynamic();
}

TEST_F(MicroROSTypeUtilities, string_sequence_regression_custom) {
  typesupport_ = ROSIDL_GET_MSG_TYPE_SUPPORT(trajectory_msgs, msg, JointTrajectory);

  add_rule("joint_names", 16);
  add_rule("joint_names.data", 5);

  std::array<trajectory_msgs__msg__JointTrajectory, 2> msgs;

  ASSERT_TRUE(create_memory_dynamic(&msgs[0]));
  ASSERT_TRUE(create_memory_static(&msgs[1]));

  for (auto & msg : msgs) {
    ASSERT_EQ(msg.joint_names.size, 0UL);
    ASSERT_EQ(msg.joint_names.capacity, get_rule_size("joint_names"));
    ASSERT_NE(msg.joint_names.data, nullptr);

    for (size_t i = 0; i < msg.joint_names.capacity; i++) {
      ASSERT_EQ(msg.joint_names.data[i].size, 0UL);
      ASSERT_EQ(msg.joint_names.data[i].capacity, get_rule_size("joint_names.data"));
      ASSERT_NE(msg.joint_names.data[i].data, nullptr);
    }
  }

  destroy_memory_dynamic();
}

TEST_F(MicroROSTypeUtilities, wrong_allocator) {
  typesupport_ = ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, MultiArrayLayout);

  rcutils_allocator_t wrong_allocator = {
    [](size_t, void *) -> void * {return nullptr;},
    deallocate,
    reallocate,
    zero_allocate,
    static_cast<void *>(this)
  };

  std::array<std_msgs__msg__MultiArrayLayout, 2> msgs;

  ASSERT_FALSE(create_memory_dynamic(&msgs[0], &wrong_allocator));
  ASSERT_TRUE(create_memory_static(&msgs[1]));
}
