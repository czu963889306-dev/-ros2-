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
#include "rcutils/repl_str.h"

TEST(test_repl_str, nominal) {
  auto allocator = rcutils_get_default_allocator();

  // replace with a string of the exact length
  {
    std::string typical = "foo/{bar}/baz";
    char * out = rcutils_repl_str(typical.c_str(), "{bar}", "bbarr", &allocator);
    EXPECT_STREQ("foo/bbarr/baz", out);
    allocator.deallocate(out, allocator.state);
  }

  // replace with a string of a smaller length
  {
    std::string typical = "foo/{bar}/baz";
    char * out = rcutils_repl_str(typical.c_str(), "{bar}", "bar", &allocator);
    EXPECT_STREQ("foo/bar/baz", out);
    allocator.deallocate(out, allocator.state);
  }

  // replace with a string of a longer length
  {
    std::string typical = "foo/{bar}/baz";
    char * out = rcutils_repl_str(typical.c_str(), "{bar}", "barbar", &allocator);
    EXPECT_STREQ("foo/barbar/baz", out);
    allocator.deallocate(out, allocator.state);
  }

  // replace with an empty string
  {
    std::string typical = "foo/{bar}/baz";
    char * out = rcutils_repl_str(typical.c_str(), "{bar}", "", &allocator);
    EXPECT_STREQ("foo//baz", out);
    allocator.deallocate(out, allocator.state);
  }

  // bad allocator
  {
    std::string typical = "foo/{bar}/baz";
    rcutils_allocator_t failing_allocator = get_failing_allocator();
    char * out = rcutils_repl_str(typical.c_str(), "{bar}", "", &failing_allocator);
    EXPECT_EQ(NULL, out);
  }

  // no matches
  {
    std::string typical = "foo/{bar}/baz";
    char * out = rcutils_repl_str(typical.c_str(), "no match", "n/a", &allocator);
    EXPECT_STREQ(typical.c_str(), out);
    allocator.deallocate(out, allocator.state);
  }

  // no matches and bad allocator
  {
    std::string typical = "foo/{bar}/baz";
    rcutils_allocator_t failing_allocator = get_failing_allocator();
    char * out = rcutils_repl_str(typical.c_str(), "no match", "n/a", &failing_allocator);
    EXPECT_EQ(NULL, out);
  }

  // Force cache_sz_inc to exceed cache_sz_inc_max
  {
    std::stringstream ss;
    for (size_t i = 0; i < 1048576; ++i) {
      ss << "f";
    }
    char * out = rcutils_repl_str(ss.str().c_str(), "f", "longer replacement", &allocator);
    EXPECT_NE(nullptr, out);
    allocator.deallocate(out, allocator.state);
  }
}
