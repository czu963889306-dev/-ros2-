// Copyright 2016 Open Source Robotics Foundation, Inc.
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

#include <algorithm>  // for std::max
#include <atomic>
#include <chrono>
#include <future>
#include <sstream>
#include <thread>
#include <vector>

#include "gtest/gtest.h"

#include "osrf_testing_tools_cpp/scope_exit.hpp"
#include "rcl/error_handling.h"
#include "rcl/rcl.h"
#include "rcl/wait.h"

#include "rcutils/logging_macros.h"

#include "./allocator_testing_utils.h"
#include "../mocking_utils/patch.hpp"

#ifdef RMW_IMPLEMENTATION
# define CLASSNAME_(NAME, SUFFIX) NAME ## __ ## SUFFIX
# define CLASSNAME(NAME, SUFFIX) CLASSNAME_(NAME, SUFFIX)
#else
# define CLASSNAME(NAME, SUFFIX) NAME
#endif

#ifndef _WIN32
#define TOLERANCE RCL_MS_TO_NS(6)
#else
#define TOLERANCE RCL_MS_TO_NS(25)
#endif

class CLASSNAME (WaitSetTestFixture, RMW_IMPLEMENTATION) : public ::testing::Test
{
public:
  rcl_context_t * context_ptr;
  void SetUp()
  {
    rcl_ret_t ret;
    rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();
    ret = rcl_init_options_init(&init_options, rcl_get_default_allocator());
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      EXPECT_EQ(RCL_RET_OK, rcl_init_options_fini(&init_options)) << rcl_get_error_string().str;
    });
    this->context_ptr = new rcl_context_t;
    *this->context_ptr = rcl_get_zero_initialized_context();
    ret = rcl_init(0, nullptr, &init_options, this->context_ptr);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  }

  void TearDown()
  {
    EXPECT_EQ(RCL_RET_OK, rcl_shutdown(this->context_ptr)) << rcl_get_error_string().str;
    EXPECT_EQ(RCL_RET_OK, rcl_context_fini(this->context_ptr)) << rcl_get_error_string().str;
    delete this->context_ptr;
  }
};

TEST_F(CLASSNAME(WaitSetTestFixture, RMW_IMPLEMENTATION), wait_set_is_valid) {
  // null pointers are invalid
  EXPECT_FALSE(rcl_wait_set_is_valid(nullptr));

  // uninitialized wait set is invalid
  rcl_wait_set_t wait_set = rcl_get_zero_initialized_wait_set();
  EXPECT_FALSE(rcl_wait_set_is_valid(&wait_set));

  // initialized wait set is valid
  rcl_ret_t ret =
    rcl_wait_set_init(&wait_set, 1, 1, 1, 1, 1, 0, context_ptr, rcl_get_default_allocator());
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  EXPECT_TRUE(rcl_wait_set_is_valid(&wait_set));

  // finalized wait set is invalid
  ret = rcl_wait_set_fini(&wait_set);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  EXPECT_FALSE(rcl_wait_set_is_valid(&wait_set));
}

TEST_F(CLASSNAME(WaitSetTestFixture, RMW_IMPLEMENTATION), test_failed_resize) {
  // Initialize a wait set with a subscription and then resize it to zero.
  rcl_allocator_t allocator = get_failing_allocator();
  rcl_wait_set_t wait_set = rcl_get_zero_initialized_wait_set();
  set_failing_allocator_is_failing(allocator, false);
  rcl_ret_t ret =
    rcl_wait_set_init(&wait_set, 1, 1, 1, 1, 1, 0, context_ptr, allocator);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  set_failing_allocator_is_failing(allocator, true);
  ret = rcl_wait_set_resize(&wait_set, 0, 1, 0, 0, 0, 0);
  EXPECT_EQ(RCL_RET_BAD_ALLOC, ret);
  rcl_reset_error();

  set_failing_allocator_is_failing(allocator, false);
  ret = rcl_wait_set_fini(&wait_set);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
}

TEST_F(CLASSNAME(WaitSetTestFixture, RMW_IMPLEMENTATION), test_resize_to_zero) {
  // Initialize a wait set with a subscription and then resize it to zero.
  rcl_wait_set_t wait_set = rcl_get_zero_initialized_wait_set();
  rcl_ret_t ret =
    rcl_wait_set_init(&wait_set, 1, 1, 1, 1, 1, 0, context_ptr, rcl_get_default_allocator());
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  ret = rcl_wait_set_resize(&wait_set, 0u, 0u, 0u, 0u, 0u, 0u);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  EXPECT_EQ(wait_set.size_of_subscriptions, 0ull);
  EXPECT_EQ(wait_set.size_of_guard_conditions, 0ull);
  EXPECT_EQ(wait_set.size_of_clients, 0ull);
  EXPECT_EQ(wait_set.size_of_services, 0ull);
  EXPECT_EQ(wait_set.size_of_timers, 0ull);

  ret = rcl_wait_set_fini(&wait_set);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
}

// Test rcl_wait with a positive finite timeout value (1ms)
TEST_F(CLASSNAME(WaitSetTestFixture, RMW_IMPLEMENTATION), finite_timeout) {
  rcl_wait_set_t wait_set = rcl_get_zero_initialized_wait_set();
  rcl_ret_t ret =
    rcl_wait_set_init(&wait_set, 0, 0, 1, 0, 0, 0, context_ptr, rcl_get_default_allocator());
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  int64_t timeout = RCL_MS_TO_NS(10);  // nanoseconds
  std::chrono::steady_clock::time_point before_sc = std::chrono::steady_clock::now();
  ret = rcl_wait(&wait_set, timeout);
  std::chrono::steady_clock::time_point after_sc = std::chrono::steady_clock::now();
  ASSERT_EQ(RCL_RET_TIMEOUT, ret) << rcl_get_error_string().str;
  // Check time
  int64_t diff = std::chrono::duration_cast<std::chrono::nanoseconds>(after_sc - before_sc).count();
  EXPECT_LE(diff, timeout + TOLERANCE);

  ret = rcl_wait_set_fini(&wait_set);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
}

// Check that a timer overrides a negative timeout value (blocking forever)
TEST_F(CLASSNAME(WaitSetTestFixture, RMW_IMPLEMENTATION), negative_timeout) {
  rcl_wait_set_t wait_set = rcl_get_zero_initialized_wait_set();
  rcl_ret_t ret =
    rcl_wait_set_init(&wait_set, 0, 1, 1, 0, 0, 0, context_ptr, rcl_get_default_allocator());
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    ret = rcl_wait_set_fini(&wait_set);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  });

  // Add a dummy guard condition to avoid an error
  rcl_guard_condition_t guard_cond = rcl_get_zero_initialized_guard_condition();
  ret = rcl_guard_condition_init(
    &guard_cond, this->context_ptr, rcl_guard_condition_get_default_options());
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    ret = rcl_guard_condition_fini(&guard_cond);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  });
  ret = rcl_wait_set_add_guard_condition(&wait_set, &guard_cond, NULL);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  rcl_clock_t clock;
  rcl_allocator_t allocator = rcl_get_default_allocator();
  ret = rcl_clock_init(RCL_STEADY_TIME, &clock, &allocator);
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    ret = rcl_clock_fini(&clock);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  });
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  rcl_timer_t timer = rcl_get_zero_initialized_timer();
  ret = rcl_timer_init(
    &timer, &clock, this->context_ptr, RCL_MS_TO_NS(10), nullptr, rcl_get_default_allocator());
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    ret = rcl_timer_fini(&timer);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  });
  ret = rcl_wait_set_add_timer(&wait_set, &timer, NULL);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  int64_t timeout = -1;
  std::chrono::steady_clock::time_point before_sc = std::chrono::steady_clock::now();
  ret = rcl_wait(&wait_set, timeout);
  std::chrono::steady_clock::time_point after_sc = std::chrono::steady_clock::now();
  // We expect a timeout here (timer value reached)
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  // Check time
  int64_t diff = std::chrono::duration_cast<std::chrono::nanoseconds>(after_sc - before_sc).count();
  EXPECT_LE(diff, RCL_MS_TO_NS(10) + TOLERANCE);
}

// Test rcl_wait with a timeout value of 0 (non-blocking)
TEST_F(CLASSNAME(WaitSetTestFixture, RMW_IMPLEMENTATION), zero_timeout) {
  rcl_wait_set_t wait_set = rcl_get_zero_initialized_wait_set();
  rcl_ret_t ret =
    rcl_wait_set_init(&wait_set, 0, 1, 1, 0, 0, 0, context_ptr, rcl_get_default_allocator());
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    ret = rcl_wait_set_fini(&wait_set);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  });

  // Add a dummy guard condition to avoid an error
  rcl_guard_condition_t guard_cond = rcl_get_zero_initialized_guard_condition();
  ret = rcl_guard_condition_init(
    &guard_cond, this->context_ptr, rcl_guard_condition_get_default_options());
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    ret = rcl_guard_condition_fini(&guard_cond);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  });
  ret = rcl_wait_set_add_guard_condition(&wait_set, &guard_cond, NULL);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  // Time spent during wait should be negligible.
  int64_t timeout = 0;
  std::chrono::steady_clock::time_point before_sc = std::chrono::steady_clock::now();
  ret = rcl_wait(&wait_set, timeout);
  std::chrono::steady_clock::time_point after_sc = std::chrono::steady_clock::now();
  // We expect a timeout here (timer value reached)
  ASSERT_EQ(RCL_RET_TIMEOUT, ret) << rcl_get_error_string().str;
  int64_t diff = std::chrono::duration_cast<std::chrono::nanoseconds>(after_sc - before_sc).count();
  EXPECT_LE(diff, TOLERANCE);
}

// Test rcl_wait with a timeout value of 0 (non-blocking) and an already triggered guard condition
TEST_F(CLASSNAME(WaitSetTestFixture, RMW_IMPLEMENTATION), zero_timeout_triggered_guard_condition) {
  rcl_wait_set_t wait_set = rcl_get_zero_initialized_wait_set();
  rcl_ret_t ret =
    rcl_wait_set_init(&wait_set, 0, 1, 0, 0, 0, 0, context_ptr, rcl_get_default_allocator());
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    ret = rcl_wait_set_fini(&wait_set);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  });

  rcl_guard_condition_t guard_cond = rcl_get_zero_initialized_guard_condition();
  ret = rcl_guard_condition_init(
    &guard_cond, this->context_ptr, rcl_guard_condition_get_default_options());
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    ret = rcl_guard_condition_fini(&guard_cond);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  });
  ret = rcl_wait_set_add_guard_condition(&wait_set, &guard_cond, NULL);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  ret = rcl_trigger_guard_condition(&guard_cond);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

// Time spent during wait should be negligible.
  int64_t timeout = 0;
  std::chrono::steady_clock::time_point before_sc = std::chrono::steady_clock::now();
  ret = rcl_wait(&wait_set, timeout);
  std::chrono::steady_clock::time_point after_sc = std::chrono::steady_clock::now();
  // We don't expect a timeout here (since the guard condition had already been triggered)
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  int64_t diff = std::chrono::duration_cast<std::chrono::nanoseconds>(after_sc - before_sc).count();
  EXPECT_LE(diff, TOLERANCE);
}

// Test rcl_wait with a timeout value and an overrun timer
TEST_F(CLASSNAME(WaitSetTestFixture, RMW_IMPLEMENTATION), zero_timeout_overrun_timer) {
  rcl_wait_set_t wait_set = rcl_get_zero_initialized_wait_set();
  rcl_ret_t ret =
    rcl_wait_set_init(&wait_set, 0, 0, 1, 0, 0, 0, context_ptr, rcl_get_default_allocator());
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    ret = rcl_wait_set_fini(&wait_set);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  });
  rcl_clock_t clock;
  rcl_allocator_t allocator = rcl_get_default_allocator();
  ret = rcl_clock_init(RCL_STEADY_TIME, &clock, &allocator);
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    ret = rcl_clock_fini(&clock);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  });
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  rcl_timer_t timer = rcl_get_zero_initialized_timer();
  ret = rcl_timer_init(
    &timer, &clock, this->context_ptr, 0, nullptr, rcl_get_default_allocator());
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    ret = rcl_timer_fini(&timer);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  });
  ret = rcl_wait_set_add_timer(&wait_set, &timer, NULL);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  // Time spent during wait should be negligible, definitely less than the given timeout
  std::chrono::steady_clock::time_point before_sc = std::chrono::steady_clock::now();
  ret = rcl_wait(&wait_set, RCL_MS_TO_NS(100));
  std::chrono::steady_clock::time_point after_sc = std::chrono::steady_clock::now();
  // We don't expect a timeout here (since the guard condition had already been triggered)
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  int64_t diff = std::chrono::duration_cast<std::chrono::nanoseconds>(after_sc - before_sc).count();
  EXPECT_LE(diff, RCL_MS_TO_NS(50));
}

// Check that a canceled timer doesn't wake up rcl_wait
TEST_F(CLASSNAME(WaitSetTestFixture, RMW_IMPLEMENTATION), canceled_timer) {
  rcl_wait_set_t wait_set = rcl_get_zero_initialized_wait_set();
  rcl_ret_t ret =
    rcl_wait_set_init(&wait_set, 0, 1, 1, 0, 0, 0, context_ptr, rcl_get_default_allocator());
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    ret = rcl_wait_set_fini(&wait_set);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  });

  // Add a dummy guard condition to avoid an error
  rcl_guard_condition_t guard_cond = rcl_get_zero_initialized_guard_condition();
  ret = rcl_guard_condition_init(
    &guard_cond, this->context_ptr, rcl_guard_condition_get_default_options());
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    ret = rcl_guard_condition_fini(&guard_cond);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  });
  ret = rcl_wait_set_add_guard_condition(&wait_set, &guard_cond, NULL);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  rcl_clock_t clock;
  rcl_allocator_t allocator = rcl_get_default_allocator();
  ret = rcl_clock_init(RCL_STEADY_TIME, &clock, &allocator);
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    ret = rcl_clock_fini(&clock);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  });
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  rcl_timer_t canceled_timer = rcl_get_zero_initialized_timer();
  ret = rcl_timer_init(
    &canceled_timer, &clock, this->context_ptr,
    RCL_MS_TO_NS(1), nullptr, rcl_get_default_allocator());
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    ret = rcl_timer_fini(&canceled_timer);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  });
  ret = rcl_timer_cancel(&canceled_timer);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  ret = rcl_wait_set_add_timer(&wait_set, &canceled_timer, NULL);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  int64_t timeout = RCL_MS_TO_NS(10);
  std::chrono::steady_clock::time_point before_sc = std::chrono::steady_clock::now();
  ret = rcl_wait(&wait_set, timeout);
  std::chrono::steady_clock::time_point after_sc = std::chrono::steady_clock::now();
  // We expect a timeout here (timer value reached)
  ASSERT_EQ(RCL_RET_TIMEOUT, ret) << rcl_get_error_string().str;
  // Check time
  int64_t diff = std::chrono::duration_cast<std::chrono::nanoseconds>(after_sc - before_sc).count();
  EXPECT_LE(diff, RCL_MS_TO_NS(10) + TOLERANCE);
}

// Test rcl_wait_set_t with excess capacity works.
TEST_F(CLASSNAME(WaitSetTestFixture, RMW_IMPLEMENTATION), excess_capacity) {
  rcl_wait_set_t wait_set = rcl_get_zero_initialized_wait_set();
  rcl_ret_t ret =
    rcl_wait_set_init(&wait_set, 42, 42, 42, 42, 42, 0, context_ptr, rcl_get_default_allocator());
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    ret = rcl_wait_set_fini(&wait_set);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  });

  int64_t timeout = 1;
  ret = rcl_wait(&wait_set, timeout);
  ASSERT_EQ(RCL_RET_TIMEOUT, ret) << rcl_get_error_string().str;
}

// Check rcl_wait can be called in many threads, each with unique wait sets and resources.
TEST_F(CLASSNAME(WaitSetTestFixture, RMW_IMPLEMENTATION), multi_wait_set_threaded) {
  rcl_ret_t ret;
  const size_t number_of_threads = 20;  // concurrent waits
  const size_t count_target = 10;  // number of times each wait should wake up before being "done"
  const size_t retry_limit = 100;  // number of times to retry when a timeout occurs waiting
  const uint64_t wait_period = RCL_MS_TO_NS(100);  // timeout passed to rcl_wait each try
  const std::chrono::milliseconds trigger_period(2);  // period between each round of triggers
  struct TestSet
  {
    std::atomic<size_t> wake_count;
    rcl_wait_set_t wait_set;
    rcl_guard_condition_t guard_condition;
    std::thread thread;
    size_t thread_id;
  };
  std::vector<TestSet> test_sets(number_of_threads);
  // *INDENT-OFF* (prevent uncrustify from making unnecessary whitespace around [=])
  // Setup common function for waiting on the triggered guard conditions.
  auto wait_func_factory =
    [=](TestSet & test_set) {
      return
        [=, &test_set]() {
          while (test_set.wake_count < count_target) {
            bool change_detected = false;
            size_t wake_try_count = 0;
            while (wake_try_count < retry_limit) {
              wake_try_count++;
              rcl_ret_t ret;
              ret = rcl_wait_set_clear(&test_set.wait_set);
              EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
              ret = rcl_wait_set_add_guard_condition(
                &test_set.wait_set, &test_set.guard_condition, NULL);
              EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
              ret = rcl_wait(&test_set.wait_set, wait_period);
              if (ret != RCL_RET_TIMEOUT) {
                ASSERT_EQ(ret, RCL_RET_OK);
                change_detected = true;
                // if not timeout, then the single guard condition should be set
                if (!test_set.wait_set.guard_conditions[0]) {
                  test_set.wake_count.store(count_target + 1);  // indicates an error
                  ASSERT_NE(nullptr, test_set.wait_set.guard_conditions[0]) <<
                    "[thread " << test_set.thread_id <<
                    "] expected guard condition to be marked ready after non-timeout wake up";
                }
                // no need to wait again
                break;
              } else {
                std::stringstream ss;
                ss << "[thread " << test_set.thread_id << "] Timeout (try #" << wake_try_count <<
                  ")";
                // TODO(mikaelarguedas) replace this with stream logging once they exist
                RCUTILS_LOG_INFO_NAMED(ROS_PACKAGE_NAME, "%s", ss.str().c_str());
              }
            }
            if (!change_detected) {
              test_set.wake_count.store(count_target + 1);  // indicates an error
              ASSERT_TRUE(change_detected);
            }
            test_set.wake_count++;
          }
        };
    };
  // *INDENT-ON*
  // Setup each test set.
  for (auto & test_set : test_sets) {
    rcl_ret_t ret;
    // init the wake count
    test_set.wake_count.store(0);
    // setup the guard condition
    test_set.guard_condition = rcl_get_zero_initialized_guard_condition();
    ret = rcl_guard_condition_init(
      &test_set.guard_condition, this->context_ptr, rcl_guard_condition_get_default_options());
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    // guard_condition must live longer than this loop. Call fini from function-level exit-scope.
    // setup the wait set
    test_set.wait_set = rcl_get_zero_initialized_wait_set();
    ret = rcl_wait_set_init(
      &test_set.wait_set, 0, 1, 0, 0, 0, 0, context_ptr, rcl_get_default_allocator());
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    // wait_set must live longer than this loop. Call fini from function-level exit-scope.
    ret = rcl_wait_set_add_guard_condition(&test_set.wait_set, &test_set.guard_condition, NULL);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    test_set.thread_id = 0;
  }
  // Setup safe tear-down.
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    for (auto & test_set : test_sets) {
      rcl_ret_t ret = rcl_guard_condition_fini(&test_set.guard_condition);
      EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
      ret = rcl_wait_set_fini(&test_set.wait_set);
      EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    }
  });
  // Now kick off all the threads.
  size_t thread_enumeration = 0;
  for (auto & test_set : test_sets) {
    thread_enumeration++;
    test_set.thread_id = thread_enumeration;
    test_set.thread = std::thread(wait_func_factory(test_set));
  }
  // Loop, triggering every trigger_period until the threads are done.
  // *INDENT-OFF* (prevent uncrustify from making unnecessary whitespace around [=])
  auto loop_test =
    [=, &test_sets]() -> bool {
      for (const auto & test_set : test_sets) {
        if (test_set.wake_count.load() < count_target) {
          return true;
        }
      }
      return false;
    };
  // *INDENT-ON*
  size_t loop_count = 0;
  while (loop_test()) {
    loop_count++;
    for (auto & test_set : test_sets) {
      ret = rcl_trigger_guard_condition(&test_set.guard_condition);
      ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    }
    std::this_thread::sleep_for(trigger_period);
  }
  // Join the threads.
  for (auto & test_set : test_sets) {
    test_set.thread.join();
  }
  // Ensure the individual wake counts match the target (otherwise there was a problem).
  for (auto & test_set : test_sets) {
    ASSERT_EQ(count_target, test_set.wake_count.load());
  }
}

// Check the interaction of a guard condition and a negative timeout by
// triggering a guard condition in a separate thread
TEST_F(CLASSNAME(WaitSetTestFixture, RMW_IMPLEMENTATION), guard_condition) {
  rcl_wait_set_t wait_set = rcl_get_zero_initialized_wait_set();
  rcl_ret_t ret =
    rcl_wait_set_init(&wait_set, 0, 1, 0, 0, 0, 0, context_ptr, rcl_get_default_allocator());
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    ret = rcl_wait_set_fini(&wait_set);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  });
  rcl_guard_condition_t guard_cond = rcl_get_zero_initialized_guard_condition();
  ret = rcl_guard_condition_init(
    &guard_cond, this->context_ptr, rcl_guard_condition_get_default_options());
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    ret = rcl_guard_condition_fini(&guard_cond);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  });
  ret = rcl_wait_set_add_guard_condition(&wait_set, &guard_cond, NULL);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  std::promise<rcl_ret_t> p;

  int64_t timeout = -1;

  std::chrono::nanoseconds trigger_diff;
  std::thread trigger_thread(
    [&p, &guard_cond, &trigger_diff]() {
      std::chrono::steady_clock::time_point before_trigger = std::chrono::steady_clock::now();
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      rcl_ret_t ret = rcl_trigger_guard_condition(&guard_cond);
      std::chrono::steady_clock::time_point after_trigger = std::chrono::steady_clock::now();
      trigger_diff = std::chrono::duration_cast<std::chrono::nanoseconds>(
        after_trigger - before_trigger);
      p.set_value(ret);
    });
  auto f = p.get_future();

  std::chrono::steady_clock::time_point before_sc = std::chrono::steady_clock::now();
  ret = rcl_wait(&wait_set, timeout);
  std::chrono::steady_clock::time_point after_sc = std::chrono::steady_clock::now();
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  int64_t diff = std::chrono::duration_cast<std::chrono::nanoseconds>(after_sc - before_sc).count();
  trigger_thread.join();
  EXPECT_EQ(RCL_RET_OK, f.get());
  EXPECT_LE(std::abs(diff - trigger_diff.count()), TOLERANCE);
}

// Check that index arguments are properly set when adding entities
TEST_F(CLASSNAME(WaitSetTestFixture, RMW_IMPLEMENTATION), add_with_index) {
  const size_t kNumEntities = 3u;
  rcl_wait_set_t wait_set = rcl_get_zero_initialized_wait_set();
  rcl_ret_t ret = rcl_wait_set_init(
    &wait_set, 0, kNumEntities, 0, 0, 0, 0, context_ptr, rcl_get_default_allocator());
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    ret = rcl_wait_set_fini(&wait_set);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  });

  // Initialize to invalid value
  size_t guard_condition_index = 42u;

  rcl_guard_condition_t guard_conditions[kNumEntities];
  for (size_t i = 0u; i < kNumEntities; ++i) {
    guard_conditions[i] = rcl_get_zero_initialized_guard_condition();
    ret = rcl_guard_condition_init(
      &guard_conditions[i], this->context_ptr, rcl_guard_condition_get_default_options());
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      ret = rcl_guard_condition_fini(&guard_conditions[i]);
      EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    });
    ret = rcl_wait_set_add_guard_condition(
      &wait_set, &guard_conditions[i], &guard_condition_index);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    EXPECT_EQ(guard_condition_index, i);
    EXPECT_EQ(&guard_conditions[i], wait_set.guard_conditions[i]);
  }
}

// Extra invalid arguments not tested
TEST_F(CLASSNAME(WaitSetTestFixture, RMW_IMPLEMENTATION), wait_set_valid_arguments) {
  rcl_wait_set_t wait_set = rcl_get_zero_initialized_wait_set();
  rcl_ret_t ret =
    rcl_wait_set_init(&wait_set, 0, 0, 0, 0, 0, 0, context_ptr, rcl_get_default_allocator());
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  EXPECT_EQ(
    RCL_RET_WAIT_SET_EMPTY, rcl_wait(&wait_set, RCL_MS_TO_NS(1000))) << rcl_get_error_string().str;
  rcl_reset_error();
  EXPECT_EQ(RCL_RET_OK, rcl_wait_set_fini(&wait_set)) << rcl_get_error_string().str;

  EXPECT_EQ(
    RCL_RET_INVALID_ARGUMENT, rcl_wait(nullptr, RCL_MS_TO_NS(1000))) << rcl_get_error_string().str;
  rcl_reset_error();
  EXPECT_EQ(
    RCL_RET_WAIT_SET_INVALID,
    rcl_wait(&wait_set, RCL_MS_TO_NS(1000))) << rcl_get_error_string().str;
  rcl_reset_error();

  rcl_context_t not_init_context = rcl_get_zero_initialized_context();
  ret =
    rcl_wait_set_init(&wait_set, 1, 1, 1, 1, 1, 0, &not_init_context, rcl_get_default_allocator());
  EXPECT_EQ(RCL_RET_NOT_INIT, ret) << rcl_get_error_string().str;
  rcl_reset_error();

  // nullptr failures
  ret =
    rcl_wait_set_init(nullptr, 1, 1, 1, 1, 1, 0, context_ptr, rcl_get_default_allocator());
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret);
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  ret =
    rcl_wait_set_init(&wait_set, 1, 1, 1, 1, 1, 0, nullptr, rcl_get_default_allocator());
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret);
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  rcl_allocator_t zero_init_allocator =
    static_cast<rcl_allocator_t>(rcutils_get_zero_initialized_allocator());
  ret =
    rcl_wait_set_init(&wait_set, 1, 1, 1, 1, 1, 0, context_ptr, zero_init_allocator);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret);
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  ret =
    rcl_wait_set_init(&wait_set, 1, 1, 1, 1, 1, 0, context_ptr, rcl_get_default_allocator());
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  ret =
    rcl_wait_set_init(&wait_set, 1, 1, 1, 1, 1, 0, context_ptr, rcl_get_default_allocator());
  EXPECT_EQ(RCL_RET_ALREADY_INIT, ret) << rcl_get_error_string().str;
  rcl_reset_error();
  ret = rcl_wait_set_fini(&wait_set);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
}

// Test get allocator function
TEST_F(CLASSNAME(WaitSetTestFixture, RMW_IMPLEMENTATION), wait_set_get_allocator) {
  rcl_allocator_t allocator_returned;
  rcl_wait_set_t wait_set = rcl_get_zero_initialized_wait_set();

  EXPECT_EQ(
    RCL_RET_INVALID_ARGUMENT,
    rcl_wait_set_get_allocator(nullptr, &allocator_returned)) << rcl_get_error_string().str;
  rcl_reset_error();
  EXPECT_EQ(
    RCL_RET_WAIT_SET_INVALID,
    rcl_wait_set_get_allocator(&wait_set, &allocator_returned)) << rcl_get_error_string().str;
  rcl_reset_error();

  rcl_ret_t ret =
    rcl_wait_set_init(&wait_set, 1, 1, 1, 1, 1, 0, context_ptr, rcl_get_default_allocator());
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  EXPECT_EQ(
    RCL_RET_INVALID_ARGUMENT,
    rcl_wait_set_get_allocator(&wait_set, nullptr)) << rcl_get_error_string().str;
  rcl_reset_error();
  EXPECT_EQ(RCL_RET_OK, rcl_wait_set_get_allocator(&wait_set, &allocator_returned));
  EXPECT_TRUE(rcutils_allocator_is_valid(&allocator_returned));

  ret = rcl_wait_set_fini(&wait_set);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
}

// Test wait set init failure cases using mocks
TEST_F(CLASSNAME(WaitSetTestFixture, RMW_IMPLEMENTATION), wait_set_failed_init) {
  rcl_wait_set_t wait_set = rcl_get_zero_initialized_wait_set();
  // Mock rmw implementation to fail init
  auto mock = mocking_utils::patch_and_return(
    "lib:rcl", rmw_create_wait_set, nullptr);
  rcl_ret_t ret =
    rcl_wait_set_init(&wait_set, 1, 1, 1, 1, 1, 0, context_ptr, rcl_get_default_allocator());
  EXPECT_EQ(RCL_RET_WAIT_SET_INVALID, ret);
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();
}

// Test wait set fini failure cases using mocks
TEST_F(CLASSNAME(WaitSetTestFixture, RMW_IMPLEMENTATION), wait_set_failed_fini) {
  rcl_wait_set_t wait_set = rcl_get_zero_initialized_wait_set();
  rcl_ret_t ret =
    rcl_wait_set_init(&wait_set, 1, 1, 1, 1, 1, 0, context_ptr, rcl_get_default_allocator());
  ASSERT_EQ(RCL_RET_OK, ret);
  {
    // Mock rmw implementation to fail fini
    auto mock = mocking_utils::inject_on_return(
      "lib:rcl", rmw_destroy_wait_set, RMW_RET_ERROR);
    EXPECT_EQ(RCL_RET_WAIT_SET_INVALID, rcl_wait_set_fini(&wait_set));
    EXPECT_TRUE(rcl_error_is_set());
    rcl_reset_error();
  }
}

// Test proper error handling with a fault injection test
TEST_F(CLASSNAME(WaitSetTestFixture, RMW_IMPLEMENTATION), wait_set_test_maybe_init_fail) {
  rcl_wait_set_t wait_set = rcl_get_zero_initialized_wait_set();
  rcl_ret_t ret = RCL_RET_OK;
  rcl_allocator_t alloc = rcl_get_default_allocator();

  RCUTILS_FAULT_INJECTION_TEST(
  {
    // Test init in the case where guard_conditions_size + timers_size = 0
    // (used for guard condition size)
    ret = rcl_wait_set_init(&wait_set, 1, 0, 0, 1, 1, 0, context_ptr, alloc);
    if (RCL_RET_OK == ret) {
      ret = rcl_wait_set_fini(&wait_set);
      if (ret != RCL_RET_OK) {
        rcl_reset_error();
      }
    } else {
      EXPECT_TRUE(RCL_RET_WAIT_SET_INVALID == ret || RCL_RET_BAD_ALLOC == ret);
      rcl_reset_error();
    }
  });

  RCUTILS_FAULT_INJECTION_TEST(
  {
    // Test init wait_set using at least one of each of the possible elements that receives
    // (subs, guard conditions, timers, clients, services, events)
    ret = rcl_wait_set_init(&wait_set, 1, 1, 1, 1, 1, 1, context_ptr, alloc);
    if (RCL_RET_OK == ret) {
      ret = rcl_wait_set_fini(&wait_set);
      if (ret != RCL_RET_OK) {
        rcl_reset_error();
      }
      EXPECT_EQ(RCL_RET_OK, rcl_wait_set_fini(&wait_set));
    } else {
      EXPECT_TRUE(RCL_RET_WAIT_SET_INVALID == ret || RCL_RET_BAD_ALLOC == ret);
      rcl_reset_error();
    }
  });
}
