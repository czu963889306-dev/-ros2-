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

#include <memory>
#include <string>
#include <chrono>
#include <thread>
#include <vector>

#include "rmw/error_handling.h"
#include "rmw/rmw.h"
#include "rmw/validate_namespace.h"
#include "rmw/validate_node_name.h"

#include "./rmw_base_test.hpp"
#include "./test_utils.hpp"

#include "rosidl_runtime_c/string.h"

#define MICROXRCEDDS_PADDING sizeof(uint32_t)

class TestGuardCondition : public ::testing::Test
{
public:
  void SetUp() override
  {
    ASSERT_EQ(rmw_init_options_init(&options, rcutils_get_default_allocator()), RMW_RET_OK);
    ASSERT_EQ(rmw_init(&options, &context), RMW_RET_OK);
  }

  void TearDown() override
  {
    ASSERT_EQ(rmw_init_options_fini(&options), RMW_RET_OK);
    ASSERT_EQ(rmw_shutdown(&context), RMW_RET_OK);
  }

protected:
  rmw_context_t context = rmw_get_zero_initialized_context();
  rmw_init_options_t options = rmw_get_zero_initialized_init_options();
};

TEST_F(TestGuardCondition, guard_condition)
{
  rmw_guard_condition_t * gc = rmw_create_guard_condition(&context);
  ASSERT_NE(gc, nullptr);

  rmw_guard_conditions_t guard_conditions;
  void * aux[1] = {gc->data};
  guard_conditions.guard_conditions = aux;
  guard_conditions.guard_condition_count = 1;

  rmw_time_t wait_timeout = (rmw_time_t) {1LL, 1LL};

  rmw_ret_t rc = rmw_wait(NULL, &guard_conditions, NULL, NULL, NULL, NULL, &wait_timeout);
  ASSERT_EQ(rc, RMW_RET_TIMEOUT);

  rc = rmw_trigger_guard_condition(gc);
  ASSERT_EQ(rc, RMW_RET_OK);

  aux[0] = gc->data;
  guard_conditions.guard_conditions = aux;
  guard_conditions.guard_condition_count = 1;
  rc = rmw_wait(NULL, &guard_conditions, NULL, NULL, NULL, NULL, &wait_timeout);
  ASSERT_EQ(rc, RMW_RET_OK);

  rc = rmw_destroy_guard_condition(gc);
  ASSERT_EQ(rc, RMW_RET_OK);
}
