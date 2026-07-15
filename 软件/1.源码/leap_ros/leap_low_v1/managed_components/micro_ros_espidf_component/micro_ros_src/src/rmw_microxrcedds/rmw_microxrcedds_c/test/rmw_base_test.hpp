// Copyright 2018 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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


#ifndef RMW_BASE_TEST_HPP_
#define RMW_BASE_TEST_HPP_

#include <gtest/gtest.h>

class RMWBaseTest : public ::testing::Test
{
protected:
  static void SetUpTestSuite()
  {
  }

  static void TearDownTestSuite()
  {
  }

  void SetUp() override
  {
    ASSERT_EQ(rmw_init_options_init(&test_options, rcutils_get_default_allocator()), RMW_RET_OK);
    ASSERT_EQ(rmw_init(&test_options, &test_context), RMW_RET_OK);
  }

  void TearDown() override
  {
    ASSERT_EQ(rmw_init_options_fini(&test_options), RMW_RET_OK);
    ASSERT_EQ(rmw_shutdown(&test_context), RMW_RET_OK);
  }

  rmw_context_t test_context = rmw_get_zero_initialized_context();
  rmw_init_options_t test_options = rmw_get_zero_initialized_init_options();
};

#endif  // RMW_BASE_TEST_HPP_
