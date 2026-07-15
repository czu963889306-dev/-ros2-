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

#include <memory>

#include "rcutils/env.h"
#include "rcutils/testing/fault_injection.h"

#include "rmw/error_handling.h"

#include "../src/functions.hpp"

TEST(Functions, bad_load) {
  const char * rmw_implementation = nullptr;
  ASSERT_EQ(nullptr, rcutils_get_env("RMW_IMPLEMENTATION", &rmw_implementation));
  EXPECT_TRUE(rcutils_set_env("RMW_IMPLEMENTATION", "not_an_rmw_implementation"));

  EXPECT_EQ(nullptr, load_library());
  EXPECT_TRUE(rmw_error_is_set());
  rmw_reset_error();

  EXPECT_TRUE(rcutils_set_env("RMW_IMPLEMENTATION", rmw_implementation));
}

TEST(Functions, nominal_load_and_lookup) {
  std::shared_ptr<rcpputils::SharedLibrary> lib = load_library();
  ASSERT_NE(nullptr, lib) << rmw_get_error_string().str;
  EXPECT_NE(nullptr, lookup_symbol(lib, "rmw_init")) << rmw_get_error_string().str;
  EXPECT_EQ(nullptr, lookup_symbol(lib, "not_an_rmw_function"));
  EXPECT_TRUE(rmw_error_is_set());
  rmw_reset_error();
}

TEST(Functions, load_and_lookup_with_internal_errors) {
  RCUTILS_FAULT_INJECTION_TEST(
  {
    void * symbol = lookup_symbol(load_library(), "rmw_init");
    if (!symbol) {
      EXPECT_TRUE(rmw_error_is_set());
      rmw_reset_error();
    }
  });
}

TEST(Functions, nominal_prefetch_and_unload) {
  prefetch_symbols();
  unload_library();
}
