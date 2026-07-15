// Copyright 2017 Open Source Robotics Foundation, Inc.
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

#include <string>

#include "./allocator_testing_utils.h"
#include "rcutils/allocator.h"
#include "rcutils/strdup.h"

TEST(test_strdup, nominal) {
  {
    std::string typical = "typical";
    auto allocator = rcutils_get_default_allocator();
    char * duped = rcutils_strdup(typical.c_str(), allocator);
    EXPECT_STREQ(typical.c_str(), duped);
    allocator.deallocate(duped, allocator.state);
  }

  {
    std::string empty = "";
    auto allocator = rcutils_get_default_allocator();
    char * duped = rcutils_strdup(empty.c_str(), allocator);
    EXPECT_STREQ(empty.c_str(), duped);
    allocator.deallocate(duped, allocator.state);
  }
}

TEST(test_strndup, nominal) {
  {
    std::string typical = "typical";
    auto allocator = rcutils_get_default_allocator();
    char * duped = rcutils_strndup(typical.c_str(), 3, allocator);
    EXPECT_STREQ(typical.substr(0, 3).c_str(), duped);
    allocator.deallocate(duped, allocator.state);
  }

  {
    std::string typical = "typical";
    auto allocator = rcutils_get_default_allocator();
    char * duped = rcutils_strndup(typical.c_str(), 0, allocator);
    size_t expected = 0;
    EXPECT_EQ(expected, strlen(duped));
    EXPECT_STREQ("", duped);
    allocator.deallocate(duped, allocator.state);
  }

  {
    std::string empty = "";
    auto allocator = rcutils_get_default_allocator();
    char * duped = rcutils_strdup(empty.c_str(), allocator);
    EXPECT_STREQ(empty.c_str(), duped);
    allocator.deallocate(duped, allocator.state);
  }
}

TEST(test_strdup, invalid_arguments) {
  auto allocator = rcutils_get_default_allocator();
  auto failing_allocator = get_failing_allocator();
  EXPECT_EQ(NULL, rcutils_strdup(NULL, allocator));
  EXPECT_EQ(NULL, rcutils_strndup(NULL, 5, allocator));
  EXPECT_EQ(NULL, rcutils_strdup("something", failing_allocator));
}
