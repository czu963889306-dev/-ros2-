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

#include <chrono>
#include <cinttypes>
#include <thread>

#include "osrf_testing_tools_cpp/memory_tools/memory_tools.hpp"

#include "rcutils/error_handling.h"
#include "rcutils/time.h"

#include "./mocking_utils/patch.hpp"

using osrf_testing_tools_cpp::memory_tools::disable_monitoring_in_all_threads;
using osrf_testing_tools_cpp::memory_tools::enable_monitoring_in_all_threads;
using osrf_testing_tools_cpp::memory_tools::on_unexpected_calloc;
using osrf_testing_tools_cpp::memory_tools::on_unexpected_free;
using osrf_testing_tools_cpp::memory_tools::on_unexpected_malloc;
using osrf_testing_tools_cpp::memory_tools::on_unexpected_realloc;

class TestTimeFixture : public ::testing::Test
{
public:
  void SetUp()
  {
    osrf_testing_tools_cpp::memory_tools::initialize();
    on_unexpected_malloc([]() {FAIL() << "UNEXPECTED MALLOC";});
    on_unexpected_realloc([]() {FAIL() << "UNEXPECTED REALLOC";});
    on_unexpected_calloc([]() {FAIL() << "UNEXPECTED CALLOC";});
    on_unexpected_free([]() {FAIL() << "UNEXPECTED FREE";});
    enable_monitoring_in_all_threads();
  }

  void TearDown()
  {
    disable_monitoring_in_all_threads();
    osrf_testing_tools_cpp::memory_tools::uninitialize();
  }
};

// Tests the rcutils time unit conversion macros.
TEST_F(TestTimeFixture, test_rcutils_time_conversion_macros) {
  // Note: 9007199254740993 or higher cannot be represented anymore by intermediate type double
  // without a loss of precision.

  // seconds to nanoseconds
  EXPECT_EQ(RCUTILS_S_TO_NS(1), 1000000000ll);  // int
  EXPECT_EQ(RCUTILS_S_TO_NS(0.2), 200000000.);  // double
  EXPECT_EQ(RCUTILS_S_TO_NS(1 + 1), 2000000000ll);  // sum of ints
  EXPECT_EQ(
    RCUTILS_S_TO_NS(9007199.254740992),
    9007199254740992.);  // maximum precision double (53 bits)
  EXPECT_EQ(
    RCUTILS_S_TO_NS(9007199.254740993),
    9007199254740992.);  // value is truncated!

  // milliseconds to nanoseconds
  EXPECT_EQ(RCUTILS_MS_TO_NS(1), 1000000ll);  // int
  EXPECT_EQ(RCUTILS_MS_TO_NS(0.2), 200000.);  // double
  EXPECT_EQ(RCUTILS_MS_TO_NS(1 + 1), 2000000ll);  // sum of ints
  EXPECT_EQ(
    RCUTILS_MS_TO_NS(9007199254.740992),
    9007199254740992.);  // maximum precision double (53 bits)
  EXPECT_EQ(
    RCUTILS_MS_TO_NS(9007199254.740993),
    9007199254740994.);  // value is truncated!

  // microseconds to nanoseconds
  EXPECT_EQ(RCUTILS_US_TO_NS(1), 1000ll);  // int
  EXPECT_EQ(RCUTILS_US_TO_NS(0.2), 200.);  // double
  EXPECT_EQ(RCUTILS_US_TO_NS(1 + 1), 2000ll);  // sum of ints
  EXPECT_EQ(
    RCUTILS_US_TO_NS(9007199254740.992),
    9007199254740992.);  // maximum precision double (53 bits)
  EXPECT_EQ(
    RCUTILS_US_TO_NS(9007199254740.993),
    9007199254740992.);  // value is truncated!

  // nanoseconds to seconds
  EXPECT_EQ(RCUTILS_NS_TO_S(1000000000ll), 1ll);  // int64_t
  EXPECT_EQ(RCUTILS_NS_TO_S(1000000042ll), 1ll);  // int64_t (truncated)
  EXPECT_EQ(RCUTILS_NS_TO_S(-1999999999ll), -1ll);  // int64_t (truncated)
  EXPECT_EQ(RCUTILS_NS_TO_S(200000000.), 0.2);  // double
  EXPECT_EQ(RCUTILS_NS_TO_S(1.0 + 1.0), 0.000000002);  // sum of doubles
  EXPECT_EQ(
    RCUTILS_NS_TO_S(9007199254740992.),
    9007199.254740992);  // maximum precision double (53 bits)

  // nanoseconds to milliseconds
  EXPECT_EQ(RCUTILS_NS_TO_MS(1000000ll), 1ll);  // int64_t
  EXPECT_EQ(RCUTILS_NS_TO_MS(1000042ll), 1ll);  // int64_t (truncated)
  EXPECT_EQ(RCUTILS_NS_TO_MS(-1999999ll), -1ll);  // int64_t (truncated)
  EXPECT_EQ(RCUTILS_NS_TO_MS(200000.), 0.2);  // double
  EXPECT_EQ(RCUTILS_NS_TO_MS(1.0 + 1.0), 0.000002);  // sum of doubles
  EXPECT_EQ(
    RCUTILS_NS_TO_MS(9007199254740992.),
    9007199254.740992);  // maximum precision double (53 bits)

  // nanoseconds to microseconds
  EXPECT_EQ(RCUTILS_NS_TO_US(1000ll), 1ll);  // int64_t
  EXPECT_EQ(RCUTILS_NS_TO_US(1042ll), 1ll);  // int64_t (truncated)
  EXPECT_EQ(RCUTILS_NS_TO_US(-1999ll), -1ll);  // int64_t (truncated)
  EXPECT_EQ(RCUTILS_NS_TO_US(200.), 0.2);  // double
  EXPECT_EQ(RCUTILS_NS_TO_US(1.0 + 1.0), 0.002);  // sum of doubles
  EXPECT_EQ(
    RCUTILS_NS_TO_US(9007199254740992.),
    9007199254740.992);  // maximum precision double (53 bits)
}

// Tests the rcutils_system_time_now() function.
TEST_F(TestTimeFixture, test_rcutils_system_time_now) {
  rcutils_ret_t ret;
  // Check for invalid argument error condition (allowed to alloc).
  ret = rcutils_system_time_now(nullptr);
  EXPECT_EQ(ret, RCUTILS_RET_INVALID_ARGUMENT) << rcutils_get_error_string().str;
  rcutils_reset_error();
  // Check for normal operation (not allowed to alloc).
  rcutils_time_point_value_t now = 0;
  EXPECT_NO_MEMORY_OPERATIONS(
  {
    ret = rcutils_system_time_now(&now);
  });
  EXPECT_EQ(ret, RCUTILS_RET_OK) << rcutils_get_error_string().str;
  EXPECT_NE(0u, now);
  // Compare to std::chrono::system_clock time (within a second).
  now = 0;
  ret = rcutils_system_time_now(&now);
  ASSERT_EQ(ret, RCUTILS_RET_OK) << rcutils_get_error_string().str;
  {
    std::chrono::system_clock::time_point now_sc = std::chrono::system_clock::now();
    auto now_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(now_sc.time_since_epoch());
    int64_t now_ns_int = now_ns.count();
    int64_t now_diff = now - now_ns_int;
    const int k_tolerance_ms = 1000;
    EXPECT_LE(llabs(now_diff), RCUTILS_MS_TO_NS(k_tolerance_ms)) << "system_clock differs";
  }
}

// Tests the rcutils_steady_time_now() function.
TEST_F(TestTimeFixture, test_rcutils_steady_time_now) {
  rcutils_ret_t ret;
  // Check for invalid argument error condition (allowed to alloc).
  ret = rcutils_steady_time_now(nullptr);
  EXPECT_EQ(ret, RCUTILS_RET_INVALID_ARGUMENT) << rcutils_get_error_string().str;
  rcutils_reset_error();
  // Check for normal operation (not allowed to alloc).
  rcutils_time_point_value_t now = 0;
  EXPECT_NO_MEMORY_OPERATIONS(
  {
    ret = rcutils_steady_time_now(&now);
  });
  EXPECT_EQ(ret, RCUTILS_RET_OK) << rcutils_get_error_string().str;
  EXPECT_NE(0u, now);
  // Compare to std::chrono::steady_clock difference of two times (within a second).
  now = 0;
  EXPECT_NO_MEMORY_OPERATIONS(
  {
    ret = rcutils_steady_time_now(&now);
  });
  std::chrono::steady_clock::time_point now_sc = std::chrono::steady_clock::now();
  EXPECT_EQ(ret, RCUTILS_RET_OK) << rcutils_get_error_string().str;
  // Wait for a little while.
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  // Then take a new timestamp with each and compare.
  rcutils_time_point_value_t later;
  EXPECT_NO_MEMORY_OPERATIONS(
  {
    ret = rcutils_steady_time_now(&later);
  });
  std::chrono::steady_clock::time_point later_sc = std::chrono::steady_clock::now();
  EXPECT_EQ(ret, RCUTILS_RET_OK) << rcutils_get_error_string().str;
  int64_t steady_diff = later - now;
  int64_t sc_diff =
    std::chrono::duration_cast<std::chrono::nanoseconds>(later_sc - now_sc).count();
  const int k_tolerance_ms = 1;
  EXPECT_LE(
    llabs(steady_diff - sc_diff), RCUTILS_MS_TO_NS(k_tolerance_ms)) << "steady_clock differs";
}

// Tests the rcutils_raw_steady_time_now() function.
TEST_F(TestTimeFixture, test_rcutils_raw_steady_time_now) {
  rcutils_ret_t ret;
  // Check for invalid argument error condition (allowed to alloc).
  ret = rcutils_raw_steady_time_now(nullptr);
  EXPECT_EQ(ret, RCUTILS_RET_INVALID_ARGUMENT) << rcutils_get_error_string().str;
  rcutils_reset_error();
  // Check for normal operation (not allowed to alloc).
  rcutils_time_point_value_t now = 0;
  EXPECT_NO_MEMORY_OPERATIONS(
  {
    ret = rcutils_raw_steady_time_now(&now);
  });
  EXPECT_EQ(ret, RCUTILS_RET_OK) << rcutils_get_error_string().str;
  EXPECT_NE(0u, now);
  // Compare to std::chrono::steady_clock difference of two times (within a second).
  now = 0;
  EXPECT_NO_MEMORY_OPERATIONS(
  {
    ret = rcutils_raw_steady_time_now(&now);
  });
  std::chrono::steady_clock::time_point now_sc = std::chrono::steady_clock::now();
  EXPECT_EQ(ret, RCUTILS_RET_OK) << rcutils_get_error_string().str;
  // Wait for a little while.
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  // Then take a new timestamp with each and compare.
  rcutils_time_point_value_t later;
  EXPECT_NO_MEMORY_OPERATIONS(
  {
    ret = rcutils_raw_steady_time_now(&later);
  });
  std::chrono::steady_clock::time_point later_sc = std::chrono::steady_clock::now();
  EXPECT_EQ(ret, RCUTILS_RET_OK) << rcutils_get_error_string().str;
  int64_t steady_diff = later - now;
  int64_t sc_diff =
    std::chrono::duration_cast<std::chrono::nanoseconds>(later_sc - now_sc).count();
  const int k_tolerance_ms = 1;
  EXPECT_LE(
    llabs(steady_diff - sc_diff), RCUTILS_MS_TO_NS(k_tolerance_ms)) << "steady_clock differs";
}

#if !defined(_WIN32)

// For mocking purposes
#if defined(__MACH__)
#include <mach/clock.h>
#include <mach/mach.h>
#define clock_gettime clock_get_time
#endif

// Tests rcutils_system_time_now() and rcutils_steady_time_now() functions
// when system clocks misbehave.
TEST_F(TestTimeFixture, test_rcutils_with_bad_system_clocks) {
#if !defined (__MACH__)  // as tv_sec is an unsigned integer there
  {
    auto mock = mocking_utils::patch(
      "lib:rcutils", clock_gettime,
      [](auto, auto * ts) {
        ts->tv_sec = -1;
        ts->tv_nsec = 0;
        return 0;
      });

    rcutils_time_point_value_t now = 0;
    rcutils_ret_t ret = rcutils_system_time_now(&now);
    EXPECT_EQ(RCUTILS_RET_ERROR, ret);
    rcutils_reset_error();

    ret = rcutils_steady_time_now(&now);
    EXPECT_EQ(RCUTILS_RET_ERROR, ret);
    rcutils_reset_error();

    ret = rcutils_raw_steady_time_now(&now);
    EXPECT_EQ(RCUTILS_RET_ERROR, ret);
    rcutils_reset_error();
  }
#endif
  {
    auto mock = mocking_utils::patch(
      "lib:rcutils", clock_gettime,
      [](auto, auto * ts) {
        ts->tv_sec = 0;
        ts->tv_nsec = -1;
        return 0;
      });

    rcutils_time_point_value_t now = 0;
    rcutils_ret_t ret = rcutils_system_time_now(&now);
    EXPECT_EQ(RCUTILS_RET_ERROR, ret);
    rcutils_reset_error();

    ret = rcutils_steady_time_now(&now);
    EXPECT_EQ(RCUTILS_RET_ERROR, ret);
    rcutils_reset_error();

    ret = rcutils_raw_steady_time_now(&now);
    EXPECT_EQ(RCUTILS_RET_ERROR, ret);
    rcutils_reset_error();
  }
}

#if defined(__MACH__)
#undef clock_gettime
#endif
#endif  // !defined(_WIN32)

// Tests the rcutils_time_point_value_as_nanoseconds_string() function.
TEST_F(TestTimeFixture, test_rcutils_time_point_value_as_nanoseconds_string) {
  rcutils_ret_t ret;
  rcutils_time_point_value_t timepoint;
  char buffer[256] = "";

  // Typical use case.
  timepoint = 100;
  ret = rcutils_time_point_value_as_nanoseconds_string(&timepoint, buffer, sizeof(buffer));
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
  EXPECT_STREQ("0000000000000000100", buffer);

  // nullptr for timepoint
  ret = rcutils_time_point_value_as_nanoseconds_string(nullptr, buffer, sizeof(buffer));
  EXPECT_EQ(RCUTILS_RET_INVALID_ARGUMENT, ret);
  rcutils_reset_error();

  // nullptr for string
  timepoint = 100;
  ret = rcutils_time_point_value_as_nanoseconds_string(&timepoint, nullptr, 0);
  EXPECT_EQ(RCUTILS_RET_INVALID_ARGUMENT, ret);
  rcutils_reset_error();

  // test truncations
  timepoint = 100;
  ret = rcutils_time_point_value_as_nanoseconds_string(&timepoint, buffer, 18);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
  EXPECT_STREQ("00000000000000001", buffer);

  const char * test_str = "should not be touched";
  timepoint = 100;
  (void)memmove(buffer, test_str, strlen(test_str) + 1);  // buffer is of size 256, so it will fit
  ret = rcutils_time_point_value_as_nanoseconds_string(&timepoint, buffer, 0);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
  EXPECT_STREQ(test_str, buffer);

  timepoint = 100;
  ret = rcutils_time_point_value_as_nanoseconds_string(&timepoint, buffer, 1);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
  EXPECT_STREQ("", buffer);

  timepoint = 100;
  ret = rcutils_time_point_value_as_nanoseconds_string(&timepoint, buffer, 3);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
  EXPECT_STREQ("00", buffer);

  // test negative
  timepoint = -100;
  ret = rcutils_time_point_value_as_nanoseconds_string(&timepoint, buffer, sizeof(buffer));
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
  EXPECT_STREQ("-0000000000000000100", buffer);
}

// Tests the rcutils_time_point_value_as_date_string() function.
TEST_F(TestTimeFixture, test_rcutils_time_point_value_as_date_string) {
  rcutils_ret_t ret;
  rcutils_time_point_value_t timepoint;
  char buffer[256] = "";

  // Typical use case.
  timepoint = 100;
  ret = rcutils_time_point_value_as_date_string(&timepoint, buffer, sizeof(buffer));
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
  std::tm t = {};
  std::istringstream ss(buffer);
  // To test that it works we call it once with the correct format string
  ss >> std::get_time(&t, "%Y-%m-%d %H:%M:%S");
  ASSERT_FALSE(ss.fail());
  std::istringstream ss2(buffer);
  // and once with the false one
  ss2 >> std::get_time(&t, "%Y-%b-%d %H:%M:%S");
  ASSERT_TRUE(ss2.fail());

  // nullptr for timepoint
  ret = rcutils_time_point_value_as_date_string(nullptr, buffer, sizeof(buffer));
  EXPECT_EQ(RCUTILS_RET_INVALID_ARGUMENT, ret);
  rcutils_reset_error();

  // nullptr for string
  timepoint = 100;
  ret = rcutils_time_point_value_as_date_string(&timepoint, nullptr, 0);
  EXPECT_EQ(RCUTILS_RET_INVALID_ARGUMENT, ret);
  rcutils_reset_error();

  const char * test_str = "should not be touched";
  timepoint = 100;
  // buffer is of size 256, so it will fit
  (void)memmove(buffer, test_str, strlen(test_str) + 1);
  ret = rcutils_time_point_value_as_date_string(&timepoint, buffer, 0);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
  EXPECT_STREQ(test_str, buffer);
}

// Tests the rcutils_time_point_value_as_seconds_string() function.
TEST_F(TestTimeFixture, test_rcutils_time_point_value_as_seconds_string) {
  rcutils_ret_t ret;
  rcutils_time_point_value_t timepoint;
  char buffer[256] = "";

  // Typical use case.
  timepoint = 100;
  ret = rcutils_time_point_value_as_seconds_string(&timepoint, buffer, sizeof(buffer));
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
  EXPECT_STREQ("0000000000.000000100", buffer);

  // nullptr for timepoint
  ret = rcutils_time_point_value_as_seconds_string(nullptr, buffer, sizeof(buffer));
  EXPECT_EQ(RCUTILS_RET_INVALID_ARGUMENT, ret);
  rcutils_reset_error();

  // nullptr for string
  timepoint = 100;
  ret = rcutils_time_point_value_as_seconds_string(&timepoint, nullptr, 0);
  EXPECT_EQ(RCUTILS_RET_INVALID_ARGUMENT, ret);
  rcutils_reset_error();

  // test truncations
  timepoint = 100;
  ret = rcutils_time_point_value_as_seconds_string(&timepoint, buffer, 19);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
  EXPECT_STREQ("0000000000.0000001", buffer);

  const char * test_str = "should not be touched";
  timepoint = 100;
  (void)memmove(buffer, test_str, strlen(test_str) + 1);  // buffer is of size 256, so it will fit
  ret = rcutils_time_point_value_as_seconds_string(&timepoint, buffer, 0);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
  EXPECT_STREQ(test_str, buffer);

  timepoint = 100;
  ret = rcutils_time_point_value_as_seconds_string(&timepoint, buffer, 1);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
  EXPECT_STREQ("", buffer);

  timepoint = 100;
  ret = rcutils_time_point_value_as_seconds_string(&timepoint, buffer, 3);
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
  EXPECT_STREQ("00", buffer);

  // test negative
  timepoint = -100;
  ret = rcutils_time_point_value_as_seconds_string(&timepoint, buffer, sizeof(buffer));
  EXPECT_EQ(RCUTILS_RET_OK, ret) << rcutils_get_error_string().str;
  EXPECT_STREQ("-0000000000.000000100", buffer);
}
