// Copyright 2018 Open Source Robotics Foundation, Inc.
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

#include "rcutils/snprintf.h"

// Tests the rcutils_snprintf() function.
TEST(TestSnprintf, test_snprintf) {
  const char * test_str = "0123456789";
  char buffer[256];
  int ret;

  ret = rcutils_snprintf(buffer, sizeof(buffer), "%s", test_str);
  EXPECT_EQ(static_cast<int>(strlen(test_str)), ret);
  EXPECT_STREQ(test_str, buffer);

  ret = rcutils_snprintf(nullptr, 0, "%s", test_str);
  EXPECT_EQ(static_cast<int>(strlen(test_str)), ret);

  ret = rcutils_snprintf(buffer, 4, "%s", test_str);  // NOLINT(runtime/printf)
  EXPECT_EQ(static_cast<int>(strlen(test_str)), ret);
  EXPECT_STREQ("012", buffer);

  ret = rcutils_snprintf(nullptr, sizeof(buffer), "%s", test_str);
  EXPECT_EQ(-1, ret);

  ret = rcutils_snprintf(buffer, 0, "%s", test_str);
  EXPECT_EQ(-1, ret);

  ret = rcutils_snprintf(buffer, 1, "%s", test_str);  // NOLINT(runtime/printf)
  EXPECT_EQ(static_cast<int>(strlen(test_str)), ret);
  EXPECT_STREQ("", buffer);

  ret = rcutils_snprintf(buffer, 2, "%s", test_str);  // NOLINT(runtime/printf)
  EXPECT_EQ(static_cast<int>(strlen(test_str)), ret);
  EXPECT_STREQ("0", buffer);

  ret = rcutils_snprintf(buffer, strlen(test_str), "%s", test_str);
  EXPECT_EQ(static_cast<int>(strlen(test_str)), ret);
  EXPECT_STREQ("012345678", buffer);

  ret = rcutils_snprintf(buffer, strlen(test_str) + 1, "%s", test_str);
  EXPECT_EQ(static_cast<int>(strlen(test_str)), ret);
  EXPECT_STREQ(test_str, buffer);

  EXPECT_EQ(-1, rcutils_snprintf(buffer, 2, NULL));  // NOLINT(runtime/printf)
}
