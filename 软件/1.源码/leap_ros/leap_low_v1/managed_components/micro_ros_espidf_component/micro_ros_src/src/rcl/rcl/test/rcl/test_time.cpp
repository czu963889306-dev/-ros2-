// Copyright 2015 Open Source Robotics Foundation, Inc.
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

#include <inttypes.h>

#include <chrono>
#include <thread>

#include "osrf_testing_tools_cpp/memory_tools/memory_tools.hpp"
#include "osrf_testing_tools_cpp/scope_exit.hpp"
#include "rcl/error_handling.h"
#include "rcl/time.h"

#include "./allocator_testing_utils.h"

#ifdef RMW_IMPLEMENTATION
# define CLASSNAME_(NAME, SUFFIX) NAME ## __ ## SUFFIX
# define CLASSNAME(NAME, SUFFIX) CLASSNAME_(NAME, SUFFIX)
#else
# define CLASSNAME(NAME, SUFFIX) NAME
#endif

using osrf_testing_tools_cpp::memory_tools::on_unexpected_malloc;
using osrf_testing_tools_cpp::memory_tools::on_unexpected_realloc;
using osrf_testing_tools_cpp::memory_tools::on_unexpected_calloc;
using osrf_testing_tools_cpp::memory_tools::on_unexpected_free;

class CLASSNAME (TestTimeFixture, RMW_IMPLEMENTATION) : public ::testing::Test
{
public:
  void SetUp()
  {
    osrf_testing_tools_cpp::memory_tools::initialize();
    on_unexpected_malloc([]() {ADD_FAILURE() << "UNEXPECTED MALLOC";});
    on_unexpected_realloc([]() {ADD_FAILURE() << "UNEXPECTED REALLOC";});
    on_unexpected_calloc([]() {ADD_FAILURE() << "UNEXPECTED CALLOC";});
    on_unexpected_free([]() {ADD_FAILURE() << "UNEXPECTED FREE";});
  }

  void TearDown()
  {
    osrf_testing_tools_cpp::memory_tools::uninitialize();
  }
};

// Tests the rcl_set_ros_time_override() function.
TEST_F(CLASSNAME(TestTimeFixture, RMW_IMPLEMENTATION), test_rcl_ros_time_set_override) {
  osrf_testing_tools_cpp::memory_tools::enable_monitoring_in_all_threads();
  rcl_clock_t ros_clock;
  rcl_allocator_t allocator = rcl_get_default_allocator();
  rcl_ret_t retval = rcl_ros_clock_init(&ros_clock, &allocator);
  ASSERT_EQ(RCL_RET_OK, retval) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_ros_clock_fini(&ros_clock)) << rcl_get_error_string().str;
  });

  rcl_ret_t ret;
  // Check for invalid argument error condition (allowed to alloc).
  ret = rcl_set_ros_time_override(nullptr, 0);
  EXPECT_EQ(ret, RCL_RET_INVALID_ARGUMENT) << rcl_get_error_string().str;
  rcl_reset_error();
  bool result;
  ret = rcl_is_enabled_ros_time_override(nullptr, &result);
  EXPECT_EQ(ret, RCL_RET_INVALID_ARGUMENT) << rcl_get_error_string().str;
  rcl_reset_error();
  ret = rcl_is_enabled_ros_time_override(&ros_clock, nullptr);
  EXPECT_EQ(ret, RCL_RET_INVALID_ARGUMENT) << rcl_get_error_string().str;
  rcl_reset_error();
  ret = rcl_is_enabled_ros_time_override(nullptr, nullptr);
  EXPECT_EQ(ret, RCL_RET_INVALID_ARGUMENT) << rcl_get_error_string().str;
  rcl_reset_error();
  rcl_time_point_value_t query_now;
  bool is_enabled;
  ret = rcl_is_enabled_ros_time_override(&ros_clock, &is_enabled);
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  EXPECT_EQ(is_enabled, false);
  EXPECT_NO_MEMORY_OPERATIONS(
  {
    // Check for normal operation (not allowed to alloc).
    ret = rcl_clock_get_now(&ros_clock, &query_now);
  });
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  EXPECT_NE(query_now, 0u);
  // Compare to std::chrono::system_clock time (within a second).
  ret = rcl_clock_get_now(&ros_clock, &query_now);
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  {
    std::chrono::system_clock::time_point now_sc = std::chrono::system_clock::now();
    auto now_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(now_sc.time_since_epoch());
    int64_t now_ns_int = now_ns.count();
    int64_t now_diff = query_now - now_ns_int;
    const int k_tolerance_ms = 1000;
    EXPECT_LE(llabs(now_diff), RCL_MS_TO_NS(k_tolerance_ms)) << "ros_clock differs";
  }
  // Test ros time specific APIs
  rcl_time_point_value_t set_point = 1000000000ull;
  // Check initialized state
  ret = rcl_is_enabled_ros_time_override(&ros_clock, &is_enabled);
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  EXPECT_EQ(is_enabled, false);
  // set the time point
  ret = rcl_set_ros_time_override(&ros_clock, set_point);
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  // check still disabled
  ret = rcl_is_enabled_ros_time_override(&ros_clock, &is_enabled);
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  EXPECT_EQ(is_enabled, false);
  // get real
  ret = rcl_clock_get_now(&ros_clock, &query_now);
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  {
    std::chrono::system_clock::time_point now_sc = std::chrono::system_clock::now();
    auto now_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(now_sc.time_since_epoch());
    int64_t now_ns_int = now_ns.count();
    int64_t now_diff = query_now - now_ns_int;
    const int k_tolerance_ms = 1000;
    EXPECT_LE(llabs(now_diff), RCL_MS_TO_NS(k_tolerance_ms)) << "ros_clock differs";
  }
  // enable
  ret = rcl_enable_ros_time_override(&ros_clock);
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  // check enabled
  ret = rcl_is_enabled_ros_time_override(&ros_clock, &is_enabled);
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  EXPECT_EQ(is_enabled, true);
  // get sim
  ret = rcl_clock_get_now(&ros_clock, &query_now);
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  EXPECT_EQ(query_now, set_point);
  // disable
  ret = rcl_disable_ros_time_override(&ros_clock);
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  // check disabled
  ret = rcl_is_enabled_ros_time_override(&ros_clock, &is_enabled);
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  EXPECT_EQ(is_enabled, false);
  // get real
  ret = rcl_clock_get_now(&ros_clock, &query_now);
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  {
    std::chrono::system_clock::time_point now_sc = std::chrono::system_clock::now();
    auto now_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(now_sc.time_since_epoch());
    int64_t now_ns_int = now_ns.count();
    int64_t now_diff = query_now - now_ns_int;
    const int k_tolerance_ms = 1000;
    EXPECT_LE(llabs(now_diff), RCL_MS_TO_NS(k_tolerance_ms)) << "ros_clock differs";
  }
}

TEST_F(CLASSNAME(TestTimeFixture, RMW_IMPLEMENTATION), test_rcl_init_for_clock_and_point) {
  rcl_ret_t ret;
  rcl_allocator_t allocator = rcl_get_default_allocator();
  // Check for invalid argument error condition (allowed to alloc).
  ret = rcl_ros_clock_init(nullptr, &allocator);
  EXPECT_EQ(ret, RCL_RET_INVALID_ARGUMENT) << rcl_get_error_string().str;
  rcl_reset_error();
  // Check for invalid argument error condition (allowed to alloc).
  rcl_clock_t uninitialized_clock;
  ret = rcl_ros_clock_init(&uninitialized_clock, nullptr);
  EXPECT_EQ(ret, RCL_RET_INVALID_ARGUMENT) << rcl_get_error_string().str;
  rcl_reset_error();
  // Check for normal operation (not allowed to alloc).
  rcl_clock_t source;
  ret = rcl_ros_clock_init(&source, &allocator);
  ASSERT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_ros_clock_fini(&source)) << rcl_get_error_string().str;
  });

  rcl_clock_t ros_clock;
  rcl_ret_t retval = rcl_ros_clock_init(&ros_clock, &allocator);
  ASSERT_EQ(retval, RCL_RET_OK) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_ros_clock_fini(&ros_clock)) << rcl_get_error_string().str;
  });
}

TEST_F(CLASSNAME(TestTimeFixture, RMW_IMPLEMENTATION), test_ros_clock_initially_zero) {
  rcl_allocator_t allocator = rcl_get_default_allocator();
  rcl_clock_t ros_clock;
  ASSERT_EQ(RCL_RET_OK, rcl_ros_clock_init(&ros_clock, &allocator)) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_clock_fini(&ros_clock)) << rcl_get_error_string().str;
  });
  ASSERT_EQ(RCL_RET_OK, rcl_enable_ros_time_override(&ros_clock)) << rcl_get_error_string().str;
  rcl_time_point_value_t query_now = 5;
  ASSERT_EQ(RCL_RET_OK, rcl_clock_get_now(&ros_clock, &query_now)) << rcl_get_error_string().str;
  EXPECT_EQ(0, query_now);
}

TEST(CLASSNAME(rcl_time, RMW_IMPLEMENTATION), clock_validation) {
  ASSERT_FALSE(rcl_clock_valid(NULL));
  rcl_clock_t uninitialized;
  // Not reliably detectable due to random values.
  // ASSERT_FALSE(rcl_clock_valid(&uninitialized));
  rcl_allocator_t allocator = rcl_get_default_allocator();
  rcl_ret_t ret;
  ret = rcl_ros_clock_init(&uninitialized, &allocator);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_ros_clock_fini(&uninitialized)) << rcl_get_error_string().str;
  });
}

TEST(CLASSNAME(rcl_time, RMW_IMPLEMENTATION), default_clock_instanciation) {
  rcl_clock_t ros_clock;
  rcl_allocator_t allocator = rcl_get_default_allocator();
  rcl_ret_t retval = rcl_ros_clock_init(&ros_clock, &allocator);
  ASSERT_EQ(retval, RCL_RET_OK) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_ros_clock_fini(&ros_clock)) << rcl_get_error_string().str;
  });
  ASSERT_TRUE(rcl_clock_valid(&ros_clock));

  auto * steady_clock = static_cast<rcl_clock_t *>(
    allocator.zero_allocate(1, sizeof(rcl_clock_t), allocator.state));
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    allocator.deallocate(steady_clock, allocator.state);
  });

  retval = rcl_steady_clock_init(steady_clock, &allocator);
  EXPECT_EQ(retval, RCL_RET_OK) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_steady_clock_fini(steady_clock)) << rcl_get_error_string().str;
  });
  ASSERT_TRUE(rcl_clock_valid(steady_clock));

  auto * system_clock = static_cast<rcl_clock_t *>(
    allocator.zero_allocate(1, sizeof(rcl_clock_t), allocator.state));
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    allocator.deallocate(system_clock, allocator.state);
  });
  retval = rcl_system_clock_init(system_clock, &allocator);
  EXPECT_EQ(retval, RCL_RET_OK) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_system_clock_fini(system_clock)) << rcl_get_error_string().str;
  });
  ASSERT_TRUE(rcl_clock_valid(system_clock));
}

TEST(CLASSNAME(rcl_time, RMW_IMPLEMENTATION), specific_clock_instantiation) {
  rcl_allocator_t allocator = rcl_get_default_allocator();
  {
    rcl_clock_t uninitialized_clock;
    rcl_ret_t ret = rcl_clock_init(RCL_CLOCK_UNINITIALIZED, &uninitialized_clock, &allocator);
    EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
    EXPECT_EQ(uninitialized_clock.type, RCL_CLOCK_UNINITIALIZED) <<
      "Expected time source of type RCL_CLOCK_UNINITIALIZED";
    EXPECT_TRUE(rcutils_allocator_is_valid(&(uninitialized_clock.allocator)));
    ret = rcl_clock_fini(&uninitialized_clock);
    EXPECT_EQ(ret, RCL_RET_INVALID_ARGUMENT) << rcl_get_error_string().str;
    rcl_reset_error();
    EXPECT_EQ(
      rcl_ros_clock_fini(&uninitialized_clock), RCL_RET_ERROR) << rcl_get_error_string().str;
    rcl_reset_error();
    EXPECT_EQ(
      rcl_steady_clock_fini(&uninitialized_clock), RCL_RET_ERROR) << rcl_get_error_string().str;
    rcl_reset_error();
    EXPECT_EQ(
      rcl_system_clock_fini(&uninitialized_clock), RCL_RET_ERROR) << rcl_get_error_string().str;
    rcl_reset_error();
  }
  {
    rcl_clock_t ros_clock;
    rcl_ret_t ret = rcl_clock_init(RCL_ROS_TIME, &ros_clock, &allocator);
    EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
    EXPECT_EQ(ros_clock.type, RCL_ROS_TIME) <<
      "Expected time source of type RCL_ROS_TIME";
    ret = rcl_clock_fini(&ros_clock);
    EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  }
  {
    rcl_clock_t system_clock;
    rcl_ret_t ret = rcl_clock_init(RCL_SYSTEM_TIME, &system_clock, &allocator);
    EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
    EXPECT_EQ(system_clock.type, RCL_SYSTEM_TIME) <<
      "Expected time source of type RCL_SYSTEM_TIME";
    ret = rcl_clock_fini(&system_clock);
    EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  }
  {
    rcl_clock_t steady_clock;
    rcl_ret_t ret = rcl_clock_init(RCL_STEADY_TIME, &steady_clock, &allocator);
    EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
    EXPECT_EQ(steady_clock.type, RCL_STEADY_TIME) <<
      "Expected time source of type RCL_STEADY_TIME";
    ret = rcl_clock_fini(&steady_clock);
    EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  }
  {
    rcl_clock_t fail_clock;
    rcl_clock_type_t undefined_type = (rcl_clock_type_t) 130;
    rcl_ret_t ret = rcl_clock_init(undefined_type, &fail_clock, &allocator);
    EXPECT_EQ(ret, RCL_RET_INVALID_ARGUMENT) << rcl_get_error_string().str;
  }
}

TEST(CLASSNAME(rcl_time, RMW_IMPLEMENTATION), rcl_clock_time_started) {
  rcl_allocator_t allocator = rcl_get_default_allocator();
  {
    rcl_clock_t ros_clock;
    rcl_ret_t ret = rcl_clock_init(RCL_ROS_TIME, &ros_clock, &allocator);
    // At this point, the ROS clock is reading system time since the ROS time override isn't on
    // So we expect it to be started (it's extremely unlikely that system time is at epoch start)
    ASSERT_TRUE(rcl_clock_time_started(&ros_clock));
    ASSERT_EQ(RCL_RET_OK, rcl_enable_ros_time_override(&ros_clock)) << rcl_get_error_string().str;
    ASSERT_FALSE(rcl_clock_time_started(&ros_clock));
    ret = rcl_set_ros_time_override(&ros_clock, 1);
    EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
    ASSERT_TRUE(rcl_clock_time_started(&ros_clock));
    ret = rcl_clock_fini(&ros_clock);
    EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  }
  {
    rcl_clock_t system_clock;
    rcl_ret_t ret = rcl_clock_init(RCL_SYSTEM_TIME, &system_clock, &allocator);
    ASSERT_TRUE(rcl_clock_time_started(&system_clock));  // As long as system time isn't 0
    ret = rcl_clock_fini(&system_clock);
    EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  }
  {
    rcl_clock_t steady_clock;
    rcl_ret_t ret = rcl_clock_init(RCL_STEADY_TIME, &steady_clock, &allocator);
    ASSERT_TRUE(rcl_clock_time_started(&steady_clock));
    ret = rcl_clock_fini(&steady_clock);
    EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  }
}

TEST(CLASSNAME(rcl_time, RMW_IMPLEMENTATION), rcl_time_difference) {
  rcl_ret_t ret;
  rcl_time_point_t a, b;

  a.nanoseconds = 1000;
  b.nanoseconds = 2000;
  a.clock_type = RCL_ROS_TIME;
  b.clock_type = RCL_ROS_TIME;

  rcl_duration_t d;
  ret = rcl_difference_times(&a, &b, &d);
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;

  EXPECT_EQ(d.nanoseconds, 1000);

  ret = rcl_difference_times(&b, &a, &d);
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  EXPECT_EQ(d.nanoseconds, -1000);

  b.clock_type = RCL_SYSTEM_TIME;
  EXPECT_EQ(rcl_difference_times(&a, &b, &d), RCL_RET_ERROR) << rcl_get_error_string().str;
  rcl_reset_error();
}

TEST(CLASSNAME(rcl_time, RMW_IMPLEMENTATION), rcl_time_difference_signed) {
  rcl_time_point_t a, b;
  a.nanoseconds = RCL_S_TO_NS(0LL) + 0LL;
  b.nanoseconds = RCL_S_TO_NS(10LL) + 0LL;
  a.clock_type = RCL_ROS_TIME;
  b.clock_type = RCL_ROS_TIME;

  {
    rcl_duration_t d;
    rcl_ret_t ret;
    ret = rcl_difference_times(&a, &b, &d);
    EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
    EXPECT_EQ(d.nanoseconds, RCL_S_TO_NS(10LL));
  }

  {
    rcl_duration_t d;
    rcl_ret_t ret;
    ret = rcl_difference_times(&b, &a, &d);
    EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
    EXPECT_EQ(d.nanoseconds, RCL_S_TO_NS(-10LL));
  }

  // Construct example from issue.
  a.nanoseconds = RCL_S_TO_NS(1514423496LL) + 0LL;
  b.nanoseconds = RCL_S_TO_NS(1514423498LL) + 147483647LL;

  {
    rcl_duration_t d;
    rcl_ret_t ret;
    ret = rcl_difference_times(&a, &b, &d);
    EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
    EXPECT_EQ(d.nanoseconds, 2147483647LL);
  }

  {
    rcl_duration_t d;
    rcl_ret_t ret;
    ret = rcl_difference_times(&b, &a, &d);
    EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
    // The erroneous value was -2147483648 (https://github.com/ros2/rcl/issues/204)
    EXPECT_EQ(d.nanoseconds, -2147483647LL);
  }
}

static bool pre_callback_called = false;
static bool post_callback_called = false;

void clock_callback(
  const rcl_time_jump_t * time_jump,
  bool before_jump,
  void * user_data)
{
  if (before_jump) {
    pre_callback_called = true;
    EXPECT_FALSE(post_callback_called);
  } else {
    EXPECT_TRUE(pre_callback_called);
    post_callback_called = true;
  }
  *(static_cast<rcl_time_jump_t *>(user_data)) = *time_jump;
}

void reset_callback_triggers(void)
{
  pre_callback_called = false;
  post_callback_called = false;
}

TEST(CLASSNAME(rcl_time, RMW_IMPLEMENTATION), rcl_time_clock_change_callbacks) {
  rcl_allocator_t allocator = rcl_get_default_allocator();
  rcl_clock_t ros_clock;
  rcl_ret_t ret = rcl_ros_clock_init(&ros_clock, &allocator);
  ASSERT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_ros_clock_fini(&ros_clock));
  });
  rcl_time_point_value_t query_now;

  // set callbacks
  rcl_time_jump_t time_jump;
  rcl_jump_threshold_t threshold;
  threshold.on_clock_change = true;
  threshold.min_forward.nanoseconds = 0;
  threshold.min_backward.nanoseconds = 0;
  ASSERT_EQ(
    RCL_RET_OK,
    rcl_clock_add_jump_callback(&ros_clock, threshold, clock_callback, &time_jump)) <<
    rcl_get_error_string().str;
  reset_callback_triggers();

  // Query time, no changes expected.
  ret = rcl_clock_get_now(&ros_clock, &query_now);
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  EXPECT_FALSE(pre_callback_called);
  EXPECT_FALSE(post_callback_called);

  // Clock change callback called when ROS time is enabled
  ret = rcl_enable_ros_time_override(&ros_clock);
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  EXPECT_TRUE(pre_callback_called);
  EXPECT_TRUE(post_callback_called);
  EXPECT_EQ(RCL_ROS_TIME_ACTIVATED, time_jump.clock_change);
  reset_callback_triggers();

  // Clock change callback not called because ROS time is already enabled.
  ret = rcl_enable_ros_time_override(&ros_clock);
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  EXPECT_FALSE(pre_callback_called);
  EXPECT_FALSE(post_callback_called);
  reset_callback_triggers();

  // Clock change callback called when ROS time is disabled
  ret = rcl_disable_ros_time_override(&ros_clock);
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  EXPECT_TRUE(pre_callback_called);
  EXPECT_TRUE(post_callback_called);
  EXPECT_EQ(RCL_ROS_TIME_DEACTIVATED, time_jump.clock_change);
  reset_callback_triggers();

  // Clock change callback not called because ROS time is already disabled.
  ret = rcl_disable_ros_time_override(&ros_clock);
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  EXPECT_FALSE(pre_callback_called);
  EXPECT_FALSE(post_callback_called);
  reset_callback_triggers();
}

TEST(CLASSNAME(rcl_time, RMW_IMPLEMENTATION), rcl_time_fail_set_jump_callbacks) {
  rcl_allocator_t allocator = rcl_get_default_allocator();
  rcl_clock_t fail_clock;
  rcl_time_jump_t time_jump;
  rcl_jump_threshold_t threshold;
  rcl_ret_t ret = rcl_clock_init(RCL_CLOCK_UNINITIALIZED, &fail_clock, &allocator);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  threshold.on_clock_change = false;
  threshold.min_forward.nanoseconds = -1;
  threshold.min_backward.nanoseconds = 0;

  EXPECT_EQ(
    RCL_RET_INVALID_ARGUMENT,
    rcl_clock_add_jump_callback(&fail_clock, threshold, clock_callback, &time_jump)) <<
    rcl_get_error_string().str;
  rcl_reset_error();

  threshold.min_forward.nanoseconds = 0;
  threshold.min_backward.nanoseconds = 1;
  EXPECT_EQ(
    RCL_RET_INVALID_ARGUMENT,
    rcl_clock_add_jump_callback(&fail_clock, threshold, clock_callback, &time_jump)) <<
    rcl_get_error_string().str;
  rcl_reset_error();
}

TEST(CLASSNAME(rcl_time, RMW_IMPLEMENTATION), rcl_time_forward_jump_callbacks) {
  rcl_clock_t ros_clock;
  rcl_allocator_t allocator = rcl_get_default_allocator();
  rcl_ret_t ret = rcl_ros_clock_init(&ros_clock, &allocator);
  ASSERT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_ros_clock_fini(&ros_clock));
  });

  rcl_time_point_value_t set_point1 = 1000L * 1000L * 1000L;
  rcl_time_point_value_t set_point2 = 2L * 1000L * 1000L * 1000L;

  rcl_time_jump_t time_jump;
  rcl_jump_threshold_t threshold;
  threshold.on_clock_change = false;
  threshold.min_forward.nanoseconds = 1;
  threshold.min_backward.nanoseconds = 0;
  ASSERT_EQ(
    RCL_RET_OK,
    rcl_clock_add_jump_callback(&ros_clock, threshold, clock_callback, &time_jump)) <<
    rcl_get_error_string().str;
  reset_callback_triggers();

  // Set the time before it's enabled. Should be no callbacks
  ret = rcl_set_ros_time_override(&ros_clock, set_point1);
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  EXPECT_FALSE(pre_callback_called);
  EXPECT_FALSE(post_callback_called);

  // enable no callbacks
  ret = rcl_enable_ros_time_override(&ros_clock);
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  EXPECT_FALSE(pre_callback_called);
  EXPECT_FALSE(post_callback_called);

  // Set the time now that it's enabled, now get callbacks
  ret = rcl_set_ros_time_override(&ros_clock, set_point2);
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  EXPECT_TRUE(pre_callback_called);
  EXPECT_TRUE(post_callback_called);
  EXPECT_EQ(set_point2 - set_point1, time_jump.delta.nanoseconds);
  EXPECT_EQ(RCL_ROS_TIME_NO_CHANGE, time_jump.clock_change);
  reset_callback_triggers();

  // Setting same value as previous time, not a jump
  ret = rcl_set_ros_time_override(&ros_clock, set_point2);
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  EXPECT_FALSE(pre_callback_called);
  EXPECT_FALSE(post_callback_called);

  // Jump backwards, no jump callbacks
  ret = rcl_set_ros_time_override(&ros_clock, set_point1);
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  EXPECT_FALSE(pre_callback_called);
  EXPECT_FALSE(post_callback_called);

  // disable no callbacks
  ret = rcl_disable_ros_time_override(&ros_clock);
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  EXPECT_FALSE(pre_callback_called);
  EXPECT_FALSE(post_callback_called);
}

TEST(CLASSNAME(rcl_time, RMW_IMPLEMENTATION), rcl_time_backward_jump_callbacks) {
  rcl_clock_t ros_clock;
  rcl_allocator_t allocator = rcl_get_default_allocator();
  rcl_ret_t ret = rcl_ros_clock_init(&ros_clock, &allocator);
  ASSERT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_ros_clock_fini(&ros_clock));
  });
  rcl_time_point_value_t set_point1 = 1000000000;
  rcl_time_point_value_t set_point2 = 2000000000;

  rcl_time_jump_t time_jump;
  rcl_jump_threshold_t threshold;
  threshold.on_clock_change = false;
  threshold.min_forward.nanoseconds = 0;
  threshold.min_backward.nanoseconds = -1;
  ASSERT_EQ(
    RCL_RET_OK,
    rcl_clock_add_jump_callback(&ros_clock, threshold, clock_callback, &time_jump)) <<
    rcl_get_error_string().str;
  reset_callback_triggers();

  // Set the time before it's enabled. Should be no callbacks
  ret = rcl_set_ros_time_override(&ros_clock, set_point2);
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  EXPECT_FALSE(pre_callback_called);
  EXPECT_FALSE(post_callback_called);

  // enable no callbacks
  ret = rcl_enable_ros_time_override(&ros_clock);
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  EXPECT_FALSE(pre_callback_called);
  EXPECT_FALSE(post_callback_called);

  // Set the time now that it's enabled, now get callbacks
  ret = rcl_set_ros_time_override(&ros_clock, set_point1);
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  EXPECT_TRUE(pre_callback_called);
  EXPECT_TRUE(post_callback_called);
  EXPECT_EQ(set_point1 - set_point2, time_jump.delta.nanoseconds);
  EXPECT_EQ(RCL_ROS_TIME_NO_CHANGE, time_jump.clock_change);
  reset_callback_triggers();

  // Setting same value as previous time, not a jump
  ret = rcl_set_ros_time_override(&ros_clock, set_point1);
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  EXPECT_FALSE(pre_callback_called);
  EXPECT_FALSE(post_callback_called);

  // Jump forwards, no jump callbacks
  ret = rcl_set_ros_time_override(&ros_clock, set_point2);
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  EXPECT_FALSE(pre_callback_called);
  EXPECT_FALSE(post_callback_called);

  // disable no callbacks
  ret = rcl_disable_ros_time_override(&ros_clock);
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  EXPECT_FALSE(pre_callback_called);
  EXPECT_FALSE(post_callback_called);
}

TEST(CLASSNAME(rcl_time, RMW_IMPLEMENTATION), rcl_clock_add_jump_callback) {
  rcl_clock_t clock;
  rcl_allocator_t allocator = rcl_get_default_allocator();
  rcl_ret_t ret = rcl_ros_clock_init(&clock, &allocator);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_ros_clock_fini(&clock));
  });

  rcl_jump_threshold_t threshold;
  threshold.on_clock_change = false;
  threshold.min_forward.nanoseconds = 0;
  threshold.min_backward.nanoseconds = 0;
  rcl_jump_callback_t cb = reinterpret_cast<rcl_jump_callback_t>(0xBEEF);
  void * user_data = reinterpret_cast<void *>(0xCAFE);

  EXPECT_EQ(
    RCL_RET_INVALID_ARGUMENT, rcl_clock_add_jump_callback(&clock, threshold, NULL, user_data));
  rcl_reset_error();

  EXPECT_EQ(RCL_RET_OK, rcl_clock_add_jump_callback(&clock, threshold, cb, NULL)) <<
    rcl_get_error_string().str;
  EXPECT_EQ(RCL_RET_ERROR, rcl_clock_add_jump_callback(&clock, threshold, cb, NULL));
  rcl_reset_error();

  EXPECT_EQ(RCL_RET_OK, rcl_clock_add_jump_callback(&clock, threshold, cb, user_data)) <<
    rcl_get_error_string().str;
  EXPECT_EQ(RCL_RET_ERROR, rcl_clock_add_jump_callback(&clock, threshold, cb, user_data));
  rcl_reset_error();

  EXPECT_EQ(2u, clock.num_jump_callbacks);
}

TEST(CLASSNAME(rcl_time, RMW_IMPLEMENTATION), rcl_clock_remove_jump_callback) {
  rcl_clock_t clock;
  rcl_allocator_t allocator = rcl_get_default_allocator();
  rcl_ret_t ret = rcl_ros_clock_init(&clock, &allocator);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_ros_clock_fini(&clock));
  });

  rcl_jump_threshold_t threshold;
  threshold.on_clock_change = false;
  threshold.min_forward.nanoseconds = 0;
  threshold.min_backward.nanoseconds = 0;
  rcl_jump_callback_t cb = reinterpret_cast<rcl_jump_callback_t>(0xBEEF);
  void * user_data1 = reinterpret_cast<void *>(0xCAFE);
  void * user_data2 = reinterpret_cast<void *>(0xFACE);
  void * user_data3 = reinterpret_cast<void *>(0xBEAD);
  void * user_data4 = reinterpret_cast<void *>(0xDEED);

  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rcl_clock_remove_jump_callback(&clock, NULL, user_data1));
  rcl_reset_error();
  EXPECT_EQ(RCL_RET_ERROR, rcl_clock_remove_jump_callback(&clock, cb, NULL));
  rcl_reset_error();

  ASSERT_EQ(RCL_RET_OK, rcl_clock_add_jump_callback(&clock, threshold, cb, user_data1)) <<
    rcl_get_error_string().str;
  ASSERT_EQ(RCL_RET_OK, rcl_clock_add_jump_callback(&clock, threshold, cb, user_data2)) <<
    rcl_get_error_string().str;
  ASSERT_EQ(RCL_RET_OK, rcl_clock_add_jump_callback(&clock, threshold, cb, user_data3)) <<
    rcl_get_error_string().str;
  ASSERT_EQ(RCL_RET_OK, rcl_clock_add_jump_callback(&clock, threshold, cb, user_data4)) <<
    rcl_get_error_string().str;
  EXPECT_EQ(4u, clock.num_jump_callbacks);

  EXPECT_EQ(RCL_RET_OK, rcl_clock_remove_jump_callback(&clock, cb, user_data3));
  EXPECT_EQ(3u, clock.num_jump_callbacks);
  EXPECT_EQ(RCL_RET_OK, rcl_clock_remove_jump_callback(&clock, cb, user_data4));
  EXPECT_EQ(2u, clock.num_jump_callbacks);
  EXPECT_EQ(RCL_RET_OK, rcl_clock_remove_jump_callback(&clock, cb, user_data1));
  EXPECT_EQ(1u, clock.num_jump_callbacks);
  EXPECT_EQ(RCL_RET_OK, rcl_clock_remove_jump_callback(&clock, cb, user_data2));
  EXPECT_EQ(0u, clock.num_jump_callbacks);
}

TEST(CLASSNAME(rcl_time, RMW_IMPLEMENTATION), add_remove_add_jump_callback) {
  rcl_allocator_t failing_allocator = get_failing_allocator();
  set_failing_allocator_is_failing(failing_allocator, false);

  rcl_clock_t clock;
  rcl_ret_t ret = rcl_clock_init(RCL_SYSTEM_TIME, &clock, &failing_allocator);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_clock_fini(&clock));
  });

  rcl_jump_threshold_t threshold;
  threshold.on_clock_change = false;
  threshold.min_forward.nanoseconds = 0;
  threshold.min_backward.nanoseconds = 0;
  rcl_jump_callback_t cb = reinterpret_cast<rcl_jump_callback_t>(0xBEEF);
  void * user_data1 = reinterpret_cast<void *>(0xCAFE);
  void * user_data2 = reinterpret_cast<void *>(0xFACE);
  void * user_data3 = reinterpret_cast<void *>(0xDEED);

  ASSERT_EQ(RCL_RET_OK, rcl_clock_add_jump_callback(&clock, threshold, cb, user_data1)) <<
    rcl_get_error_string().str;
  ASSERT_EQ(RCL_RET_OK, rcl_clock_add_jump_callback(&clock, threshold, cb, user_data2)) <<
    rcl_get_error_string().str;
  EXPECT_EQ(2u, clock.num_jump_callbacks);

  set_failing_allocator_is_failing(failing_allocator, true);

  // Fail to add callback
  EXPECT_EQ(RCL_RET_BAD_ALLOC, rcl_clock_add_jump_callback(&clock, threshold, cb, user_data3));
  rcl_reset_error();

  // Remove callback but fail to shrink storage
  EXPECT_EQ(RCL_RET_BAD_ALLOC, rcl_clock_remove_jump_callback(&clock, cb, user_data1));
  rcl_reset_error();

  set_failing_allocator_is_failing(failing_allocator, false);

  // Callback has already been removed
  EXPECT_EQ(RCL_RET_ERROR, rcl_clock_remove_jump_callback(&clock, cb, user_data1));
  rcl_reset_error();

  EXPECT_EQ(RCL_RET_OK, rcl_clock_remove_jump_callback(&clock, cb, user_data2));
  EXPECT_EQ(0u, clock.num_jump_callbacks);

  EXPECT_EQ(RCL_RET_OK, rcl_clock_add_jump_callback(&clock, threshold, cb, user_data1)) <<
    rcl_get_error_string().str;
  EXPECT_EQ(1u, clock.num_jump_callbacks);
}

TEST(CLASSNAME(rcl_time, RMW_IMPLEMENTATION), failed_get_now) {
  rcl_allocator_t allocator = rcl_get_default_allocator();
  rcl_clock_t uninitialized_clock;
  rcl_time_point_value_t query_now;
  rcl_ret_t ret = rcl_clock_init(RCL_CLOCK_UNINITIALIZED, &uninitialized_clock, &allocator);
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  EXPECT_EQ(uninitialized_clock.type, RCL_CLOCK_UNINITIALIZED);
  uninitialized_clock.get_now = NULL;
  EXPECT_EQ(RCL_RET_ERROR, rcl_clock_get_now(&uninitialized_clock, &query_now));
  rcl_reset_error();
}

TEST(CLASSNAME(rcl_time, RMW_IMPLEMENTATION), fail_ros_time_override) {
  bool result;
  rcl_time_point_value_t set_point = 1000000000ull;

  rcl_clock_t ros_clock;
  rcl_allocator_t allocator = rcl_get_default_allocator();
  rcl_ret_t ret = rcl_clock_init(RCL_CLOCK_UNINITIALIZED, &ros_clock, &allocator);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  EXPECT_EQ(RCL_RET_ERROR, rcl_enable_ros_time_override(&ros_clock));
  rcl_reset_error();
  EXPECT_EQ(RCL_RET_ERROR, rcl_disable_ros_time_override(&ros_clock));
  rcl_reset_error();
  EXPECT_EQ(RCL_RET_ERROR, rcl_is_enabled_ros_time_override(&ros_clock, &result));
  rcl_reset_error();
  EXPECT_EQ(RCL_RET_ERROR, rcl_set_ros_time_override(&ros_clock, set_point));
  rcl_reset_error();

  ret = rcl_clock_init(RCL_ROS_TIME, &ros_clock, &allocator);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  ret = rcl_clock_fini(&ros_clock);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  EXPECT_EQ(RCL_RET_ERROR, rcl_enable_ros_time_override(&ros_clock));
  rcl_reset_error();
  EXPECT_EQ(RCL_RET_ERROR, rcl_disable_ros_time_override(&ros_clock));
  rcl_reset_error();
  EXPECT_EQ(RCL_RET_ERROR, rcl_is_enabled_ros_time_override(&ros_clock, &result));
  rcl_reset_error();
  EXPECT_EQ(RCL_RET_ERROR, rcl_set_ros_time_override(&ros_clock, set_point));
  rcl_reset_error();
}
