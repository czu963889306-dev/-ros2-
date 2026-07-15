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


#include <rcutils/allocator.h>
#include <rcutils/logging.h>

#include <rcl_logging_interface/rcl_logging_interface.h>

#include <string>

#include "performance_test_fixture/performance_test_fixture.hpp"

using performance_test_fixture::PerformanceTest;

namespace
{
constexpr const uint64_t kSize = 4096;
}

class LoggingBenchmarkPerformance : public PerformanceTest
{
public:
  void SetUp(benchmark::State & st)
  {
    rcl_logging_ret_t ret = rcl_logging_external_initialize(
      nullptr,
      rcutils_get_default_allocator());
    if (ret != RCL_LOGGING_RET_OK) {
      st.SkipWithError(rcutils_get_error_string().str);
    }

    data = std::string(kSize, '0');
    PerformanceTest::SetUp(st);
  }
  void TearDown(benchmark::State & st)
  {
    PerformanceTest::TearDown(st);
    rcl_logging_ret_t ret = rcl_logging_external_shutdown();
    if (ret != RCL_LOGGING_RET_OK) {
      st.SkipWithError(rcutils_get_error_string().str);
    }
  }

  static void setLogLevel(int logger_level, benchmark::State & st)
  {
    rcl_logging_ret_t ret = rcl_logging_external_set_logger_level(nullptr, logger_level);
    if (ret != RCL_LOGGING_RET_OK) {
      st.SkipWithError(rcutils_get_error_string().str);
    }
  }

  std::string data;
};

BENCHMARK_F(LoggingBenchmarkPerformance, log_level_hit)(benchmark::State & st)
{
  setLogLevel(RCUTILS_LOG_SEVERITY_INFO, st);

  rcl_logging_external_log(RCUTILS_LOG_SEVERITY_INFO, nullptr, data.c_str());
  reset_heap_counters();

  for (auto _ : st) {
    rcl_logging_external_log(RCUTILS_LOG_SEVERITY_INFO, nullptr, data.c_str());
  }
}

BENCHMARK_F(LoggingBenchmarkPerformance, log_level_miss)(benchmark::State & st)
{
  setLogLevel(RCUTILS_LOG_SEVERITY_INFO, st);
  for (auto _ : st) {
    rcl_logging_external_log(RCUTILS_LOG_SEVERITY_DEBUG, nullptr, data.c_str());
  }
}

BENCHMARK_F(PerformanceTest, logging_reinitialize)(benchmark::State & st)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  rcl_logging_ret_t ret = rcl_logging_external_initialize(nullptr, allocator);
  if (ret != RCL_LOGGING_RET_OK) {
    st.SkipWithError(rcutils_get_error_string().str);
  }

  reset_heap_counters();

  for (auto _ : st) {
    ret = rcl_logging_external_initialize(nullptr, allocator);
    if (ret != RCL_LOGGING_RET_OK) {
      st.SkipWithError(rcutils_get_error_string().str);
    }
  }

  ret = rcl_logging_external_shutdown();
  if (ret != RCL_LOGGING_RET_OK) {
    st.SkipWithError(rcutils_get_error_string().str);
  }
}

BENCHMARK_F(PerformanceTest, logging_initialize_shutdown)(benchmark::State & st)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  for (auto _ : st) {
    rcl_logging_ret_t ret = rcl_logging_external_initialize(nullptr, allocator);
    if (ret != RCL_LOGGING_RET_OK) {
      st.SkipWithError(rcutils_get_error_string().str);
    }
    ret = rcl_logging_external_shutdown();
    if (ret != RCL_LOGGING_RET_OK) {
      st.SkipWithError(rcutils_get_error_string().str);
    }
  }
}
