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

#include "rcutils/strcasecmp.h"

// Tests the rcutils_strcasecmp() function.
TEST(TestStrcasecmp, test_strcasecmp) {
  int value;
  EXPECT_EQ(-1, rcutils_strcasecmp(NULL, NULL, NULL));
  EXPECT_EQ(-1, rcutils_strcasecmp(NULL, "", &value));
  EXPECT_EQ(-1, rcutils_strcasecmp("", NULL, &value));
  EXPECT_EQ(-1, rcutils_strcasecmp("", "", NULL));
  EXPECT_EQ(-1, rcutils_strcasecmp(NULL, NULL, &value));
  EXPECT_EQ(-1, rcutils_strcasecmp("", NULL, NULL));
  EXPECT_EQ(-1, rcutils_strcasecmp(NULL, "", NULL));

  EXPECT_EQ(0, rcutils_strcasecmp("", "", &value));
  EXPECT_EQ(0, value);
  EXPECT_EQ(0, rcutils_strcasecmp("abc", "abc", &value));
  EXPECT_EQ(0, value);
  EXPECT_EQ(0, rcutils_strcasecmp("ABC", "ABC", &value));
  EXPECT_EQ(0, value);
  EXPECT_EQ(0, rcutils_strcasecmp("1abc", "1abc", &value));
  EXPECT_EQ(0, value);
  EXPECT_EQ(0, rcutils_strcasecmp("abc1", "abc1", &value));
  EXPECT_EQ(0, value);
  EXPECT_EQ(0, rcutils_strcasecmp("1ABC", "1ABC", &value));
  EXPECT_EQ(0, value);
  EXPECT_EQ(0, rcutils_strcasecmp("ABC1", "ABC1", &value));
  EXPECT_EQ(0, value);
  EXPECT_EQ(0, rcutils_strcasecmp("ABC", "abc", &value));
  EXPECT_EQ(0, value);
  EXPECT_EQ(0, rcutils_strcasecmp("abc", "ABC", &value));
  EXPECT_EQ(0, value);
  EXPECT_EQ(0, rcutils_strcasecmp("Abc", "abc", &value));
  EXPECT_EQ(0, value);
  EXPECT_EQ(0, rcutils_strcasecmp("abc", "Abc", &value));
  EXPECT_EQ(0, value);
  EXPECT_EQ(0, rcutils_strcasecmp("Abc", "Abc", &value));
  EXPECT_EQ(0, value);
  EXPECT_EQ(0, rcutils_strcasecmp("aBc", "abc", &value));
  EXPECT_EQ(0, value);
  EXPECT_EQ(0, rcutils_strcasecmp("abc", "aBc", &value));
  EXPECT_EQ(0, value);
  EXPECT_EQ(0, rcutils_strcasecmp("aBc", "aBc", &value));
  EXPECT_EQ(0, value);
  EXPECT_EQ(0, rcutils_strcasecmp("abC", "abc", &value));
  EXPECT_EQ(0, value);
  EXPECT_EQ(0, rcutils_strcasecmp("abc", "abC", &value));
  EXPECT_EQ(0, value);
  EXPECT_EQ(0, rcutils_strcasecmp("abC", "abC", &value));
  EXPECT_EQ(0, value);

  EXPECT_EQ(0, rcutils_strcasecmp("", "abc", &value));
  EXPECT_LT(value, 0);
  EXPECT_EQ(0, rcutils_strcasecmp("abc", "", &value));
  EXPECT_GT(value, 0);
  EXPECT_EQ(0, rcutils_strcasecmp("abcd", "abc", &value));
  EXPECT_GT(value, 0);
  EXPECT_EQ(0, rcutils_strcasecmp("abc", "abcd", &value));
  EXPECT_LT(value, 0);
  EXPECT_EQ(0, rcutils_strcasecmp("abcD", "abc", &value));
  EXPECT_GT(value, 0);
  EXPECT_EQ(0, rcutils_strcasecmp("abc", "abcD", &value));
  EXPECT_LT(value, 0);
  EXPECT_EQ(0, rcutils_strcasecmp("1abc", "abc", &value));
  EXPECT_LT(value, 0);
  EXPECT_EQ(0, rcutils_strcasecmp("abc", "1abc", &value));
  EXPECT_GT(value, 0);
  EXPECT_EQ(0, rcutils_strcasecmp("abc1", "abc", &value));
  EXPECT_GT(value, 0);
  EXPECT_EQ(0, rcutils_strcasecmp("abc", "abc1", &value));
  EXPECT_LT(value, 0);
  EXPECT_EQ(0, rcutils_strcasecmp("ABCd", "abc", &value));
  EXPECT_GT(value, 0);
  EXPECT_EQ(0, rcutils_strcasecmp("abc", "ABCd", &value));
  EXPECT_LT(value, 0);
  EXPECT_EQ(0, rcutils_strcasecmp("1Abc", "abc", &value));
  EXPECT_LT(value, 0);
  EXPECT_EQ(0, rcutils_strcasecmp("abc", "1Abc", &value));
  EXPECT_GT(value, 0);
  EXPECT_EQ(0, rcutils_strcasecmp("a1Bc", "abc", &value));
  EXPECT_LT(value, 0);
  EXPECT_EQ(0, rcutils_strcasecmp("abc", "a1Bc", &value));
  EXPECT_GT(value, 0);
  EXPECT_EQ(0, rcutils_strcasecmp("ab1C", "abc", &value));
  EXPECT_LT(value, 0);
  EXPECT_EQ(0, rcutils_strcasecmp("abc", "ab1C", &value));
  EXPECT_GT(value, 0);
}

// Tests the rcutils_strncasecmp() function.
TEST(TestStrcasecmp, test_strncasecmp) {
  int value;
  EXPECT_EQ(-1, rcutils_strncasecmp(NULL, NULL, 0, NULL));
  EXPECT_EQ(-1, rcutils_strncasecmp(NULL, "", 1, &value));
  EXPECT_EQ(-1, rcutils_strncasecmp("", NULL, 1, &value));
  EXPECT_EQ(-1, rcutils_strncasecmp("", "", 1, NULL));
  EXPECT_EQ(-1, rcutils_strncasecmp(NULL, NULL, 0, &value));
  EXPECT_EQ(-1, rcutils_strncasecmp("", NULL, 0, NULL));
  EXPECT_EQ(-1, rcutils_strncasecmp(NULL, "", 0, NULL));

  EXPECT_EQ(0, rcutils_strncasecmp("", "", 0, &value));
  EXPECT_EQ(0, value);
  EXPECT_EQ(0, rcutils_strncasecmp("", "", 1, &value));
  EXPECT_EQ(0, value);
  EXPECT_EQ(0, rcutils_strncasecmp("abc", "", 0, &value));
  EXPECT_EQ(0, value);
  EXPECT_EQ(0, rcutils_strncasecmp("", "abc", 0, &value));
  EXPECT_EQ(0, value);
  EXPECT_EQ(0, rcutils_strncasecmp("abc", "abc", 3, &value));
  EXPECT_EQ(0, value);
  EXPECT_EQ(0, rcutils_strncasecmp("ABC", "ABC", 3, &value));
  EXPECT_EQ(0, value);
  EXPECT_EQ(0, rcutils_strncasecmp("1abc", "1abc", 4, &value));
  EXPECT_EQ(0, value);
  EXPECT_EQ(0, rcutils_strncasecmp("abc1", "abc1", 4, &value));
  EXPECT_EQ(0, value);
  EXPECT_EQ(0, rcutils_strncasecmp("1ABC", "1ABC", 4, &value));
  EXPECT_EQ(0, value);
  EXPECT_EQ(0, rcutils_strncasecmp("ABC1", "ABC1", 4, &value));
  EXPECT_EQ(0, value);
  EXPECT_EQ(0, rcutils_strncasecmp("ABC", "abc", 1, &value));
  EXPECT_EQ(0, value);
  EXPECT_EQ(0, rcutils_strncasecmp("abc", "ABC", 1, &value));
  EXPECT_EQ(0, value);
  EXPECT_EQ(0, rcutils_strncasecmp("Abc", "abc", 2, &value));
  EXPECT_EQ(0, value);
  EXPECT_EQ(0, rcutils_strncasecmp("abc", "Abc", 2, &value));
  EXPECT_EQ(0, value);
  EXPECT_EQ(0, rcutils_strncasecmp("Abc", "Abc", 2, &value));
  EXPECT_EQ(0, value);
  EXPECT_EQ(0, rcutils_strncasecmp("aBc", "abc", 3, &value));
  EXPECT_EQ(0, value);
  EXPECT_EQ(0, rcutils_strncasecmp("abc", "aBc", 3, &value));
  EXPECT_EQ(0, value);
  EXPECT_EQ(0, rcutils_strncasecmp("aBc", "aBc", 3, &value));
  EXPECT_EQ(0, value);
  EXPECT_EQ(0, rcutils_strncasecmp("abC", "abc", 4, &value));
  EXPECT_EQ(0, value);
  EXPECT_EQ(0, rcutils_strncasecmp("abc", "abC", 4, &value));
  EXPECT_EQ(0, value);
  EXPECT_EQ(0, rcutils_strncasecmp("abC", "abC", 4, &value));
  EXPECT_EQ(0, value);

  EXPECT_EQ(0, rcutils_strncasecmp("", "abc", 1, &value));
  EXPECT_LT(value, 0);
  EXPECT_EQ(0, rcutils_strncasecmp("abc", "", 1, &value));
  EXPECT_GT(value, 0);
  EXPECT_EQ(0, rcutils_strncasecmp("abcd", "abc", 4, &value));
  EXPECT_GT(value, 0);
  EXPECT_EQ(0, rcutils_strncasecmp("abc", "abcd", 4, &value));
  EXPECT_LT(value, 0);
  EXPECT_EQ(0, rcutils_strncasecmp("abcD", "abc", 4, &value));
  EXPECT_GT(value, 0);
  EXPECT_EQ(0, rcutils_strncasecmp("abc", "abcD", 4, &value));
  EXPECT_LT(value, 0);
  EXPECT_EQ(0, rcutils_strncasecmp("1abc", "abc", 4, &value));
  EXPECT_LT(value, 0);
  EXPECT_EQ(0, rcutils_strncasecmp("abc", "1abc", 4, &value));
  EXPECT_GT(value, 0);
  EXPECT_EQ(0, rcutils_strncasecmp("abc1", "abc", 4, &value));
  EXPECT_GT(value, 0);
  EXPECT_EQ(0, rcutils_strncasecmp("abc", "abc1", 4, &value));
  EXPECT_LT(value, 0);
  EXPECT_EQ(0, rcutils_strncasecmp("ABCd", "abc", 4, &value));
  EXPECT_GT(value, 0);
  EXPECT_EQ(0, rcutils_strncasecmp("abc", "ABCd", 4, &value));
  EXPECT_LT(value, 0);
  EXPECT_EQ(0, rcutils_strncasecmp("1Abc", "abc", 4, &value));
  EXPECT_LT(value, 0);
  EXPECT_EQ(0, rcutils_strncasecmp("abc", "1Abc", 4, &value));
  EXPECT_GT(value, 0);
  EXPECT_EQ(0, rcutils_strncasecmp("a1Bc", "abc", 4, &value));
  EXPECT_LT(value, 0);
  EXPECT_EQ(0, rcutils_strncasecmp("abc", "a1Bc", 4, &value));
  EXPECT_GT(value, 0);
  EXPECT_EQ(0, rcutils_strncasecmp("ab1C", "abc", 4, &value));
  EXPECT_LT(value, 0);
  EXPECT_EQ(0, rcutils_strncasecmp("abc", "ab1C", 4, &value));
  EXPECT_GT(value, 0);
}
