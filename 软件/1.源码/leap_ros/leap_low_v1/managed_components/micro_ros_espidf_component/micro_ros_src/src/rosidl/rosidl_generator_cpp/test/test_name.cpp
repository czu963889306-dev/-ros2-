// Copyright 2020 Open Source Robotics Foundation, Inc.
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

#include <gtest/gtest.h>
#include "rosidl_generator_cpp/msg/empty.hpp"
#include "rosidl_generator_cpp/msg/strings.hpp"
#include "rosidl_generator_cpp/srv/basic_types.hpp"
#include "rosidl_generator_cpp/srv/empty.hpp"

TEST(Test_rosidl_generator_traits, check_msg_name) {
  ASSERT_STREQ(
    "rosidl_generator_cpp/msg/Strings",
    rosidl_generator_traits::name<rosidl_generator_cpp::msg::Strings>());
  ASSERT_STREQ(
    "rosidl_generator_cpp/msg/Empty",
    rosidl_generator_traits::name<rosidl_generator_cpp::msg::Empty>());
}

TEST(Test_rosidl_generator_traits, check_srv_name) {
  ASSERT_STREQ(
    "rosidl_generator_cpp/srv/BasicTypes",
    rosidl_generator_traits::name<rosidl_generator_cpp::srv::BasicTypes>());
  ASSERT_STREQ(
    "rosidl_generator_cpp/srv/Empty",
    rosidl_generator_traits::name<rosidl_generator_cpp::srv::Empty>());
}
