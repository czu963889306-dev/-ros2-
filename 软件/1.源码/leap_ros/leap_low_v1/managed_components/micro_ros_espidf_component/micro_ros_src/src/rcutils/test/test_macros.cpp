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

#include "rcutils/macros.h"

TEST(TestMacros, stringify) {
#define NUMBER 23
  EXPECT_STREQ(RCUTILS_STRINGIFY(foo), "foo");
  EXPECT_STREQ(RCUTILS_STRINGIFY(42), "42");
  EXPECT_STREQ(RCUTILS_STRINGIFY(NUMBER), "23");
#undef NUMBER
}

TEST(TestMacros, join) {
  EXPECT_STREQ(RCUTILS_STRINGIFY(RCUTILS_JOIN(foo, bar)), "foobar");
  EXPECT_STREQ(RCUTILS_STRINGIFY(RCUTILS_JOIN(RCUTILS_JOIN(fizz, buzz), 42)), "fizzbuzz42");
}
