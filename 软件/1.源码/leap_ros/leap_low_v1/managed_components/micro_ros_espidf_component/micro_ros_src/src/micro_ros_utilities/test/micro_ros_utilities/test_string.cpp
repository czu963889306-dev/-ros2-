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

#include <gtest/gtest.h>

#include <micro_ros_utilities/string_utilities.h>

#include <string>

TEST(Test, micro_ros_utilities_strings)
{
  size_t size = 10;
  rosidl_runtime_c__String str_with_size = micro_ros_string_utilities_init_with_size(size);
  ASSERT_FALSE(str_with_size.data == NULL);
  ASSERT_EQ(0UL, strlen(str_with_size.data));
  ASSERT_EQ(0UL, str_with_size.size);
  ASSERT_EQ(size + 1, str_with_size.capacity);

  std::string data("Test string");
  rosidl_runtime_c__String str = micro_ros_string_utilities_init(data.c_str());

  ASSERT_FALSE(str.data == NULL);
  ASSERT_EQ(strcmp(data.c_str(), str.data), 0);
  ASSERT_EQ(data.length(), strlen(str.data));
  ASSERT_EQ(data.length(), str.size);
  ASSERT_EQ(data.length() + 1, str.capacity);

  data.append(".");
  str = micro_ros_string_utilities_append(str, ".");

  ASSERT_FALSE(str.data == NULL);
  ASSERT_EQ(strcmp(data.c_str(), str.data), 0);
  ASSERT_EQ(data.length(), strlen(str.data));
  ASSERT_EQ(data.length(), str.size);
  ASSERT_EQ(data.length() + 1, str.capacity);

  micro_ros_string_utilities_destroy(&str);

  data = "Test string";
  str = micro_ros_string_utilities_init(data.c_str());

  ASSERT_FALSE(str.data == NULL);
  ASSERT_EQ(strcmp(data.c_str(), str.data), 0);
  ASSERT_EQ(data.length(), str.size);
  ASSERT_EQ(data.length() + 1, str.capacity);

  std::string append("more text");
  std::string complete = data;
  complete.append(append);

  str = micro_ros_string_utilities_append(str, append.c_str());

  ASSERT_FALSE(str.data == NULL);
  ASSERT_EQ(strcmp(complete.c_str(), str.data), 0);
  ASSERT_EQ(complete.length(), str.size);
  ASSERT_EQ(complete.length() + 1, str.capacity);

  str = micro_ros_string_utilities_remove_tail_chars(str, append.length());

  ASSERT_FALSE(str.data == NULL);
  ASSERT_EQ(strcmp(data.c_str(), str.data), 0);
  ASSERT_EQ(data.length(), str.size);
  ASSERT_EQ(complete.length() + 1, str.capacity);

  std::string append_2("more");
  std::string complete_2 = data;
  complete_2.append(append_2);

  str = micro_ros_string_utilities_append(str, append_2.c_str());

  ASSERT_FALSE(str.data == NULL);
  ASSERT_EQ(strcmp(complete_2.c_str(), str.data), 0);
  ASSERT_EQ(complete_2.length(), str.size);
  ASSERT_EQ(complete.length() + 1, str.capacity);

  micro_ros_string_utilities_destroy(&str);

  ASSERT_TRUE(str.data == NULL);
  ASSERT_EQ(str.size, 0UL);
  ASSERT_EQ(str.capacity, 0UL);

  rosidl_runtime_c__String empty = micro_ros_string_utilities_init("");

  ASSERT_FALSE(empty.data == NULL);
  ASSERT_EQ(empty.data[0], '\0');
  ASSERT_EQ(empty.size, 0UL);
  ASSERT_EQ(empty.capacity, 1UL);
}
