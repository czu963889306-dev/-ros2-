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

#include "rcutils/cmdline_parser.h"


TEST(CmdLineParser, cli_option_exist) {
  char const * args[] = {"option1", "option2", "option3"};
  const int args_count = sizeof(args) / sizeof(char *);
  char ** arr = const_cast<char **>(args);

  EXPECT_TRUE(rcutils_cli_option_exist(arr, arr + args_count, "option1"));
  EXPECT_TRUE(rcutils_cli_option_exist(arr, arr + args_count, "option2"));
  EXPECT_TRUE(rcutils_cli_option_exist(arr, arr + args_count, "option3"));
  EXPECT_FALSE(rcutils_cli_option_exist(arr, arr + args_count, "opt"));
  EXPECT_FALSE(rcutils_cli_option_exist(arr, arr + args_count, "NotRelated"));
}

TEST(CmdLineParser, cli_get_option) {
  char const * args[] = {"option1", "sub1", "option2"};
  const int args_count = sizeof(args) / sizeof(char *);
  char ** arr = const_cast<char **>(args);

  EXPECT_STREQ(rcutils_cli_get_option(arr, arr + args_count, "option1"), "sub1");
  EXPECT_STREQ(rcutils_cli_get_option(arr, arr + args_count, "NotRelated"), NULL);
  EXPECT_STREQ(rcutils_cli_get_option(arr, arr + args_count, "option2"), NULL);
}
