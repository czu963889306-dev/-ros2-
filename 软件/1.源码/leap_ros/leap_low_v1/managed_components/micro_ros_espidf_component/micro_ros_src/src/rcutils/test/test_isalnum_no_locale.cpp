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

#include <string>

#include "gtest/gtest.h"

#include "rcutils/isalnum_no_locale.h"

TEST(test_isalnum_no_locale, valid_characters_ok) {
  std::string valid("0123456789" "ABCDEFGHIJKLMNOPQRSTUVWXYZ" "abcdefghijklmnopqrstuvwxyz");
  for (auto & c : valid) {
    ASSERT_TRUE(rcutils_isalnum_no_locale(c));
  }
}

TEST(test_isalnum_no_locale, invalid_characters_fail) {
  std::string invalid("/" /*0-9*/ ":;<=>?@" /*A-Z*/ "[\\]^_`" /*a-z*/);
  for (auto & c : invalid) {
    ASSERT_FALSE(rcutils_isalnum_no_locale(c));
  }
}
