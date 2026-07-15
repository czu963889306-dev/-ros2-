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

#include <string>
#include <type_traits>

#include "osrf_testing_tools_cpp/scope_exit.hpp"

#include "performance_test_fixture/performance_test_fixture.hpp"

#include "rcl_yaml_param_parser/parser.h"

#include "rcutils/allocator.h"
#include "rcutils/error_handling.h"
#include "rcutils/filesystem.h"
#include "rcutils/strdup.h"

#include "../src/impl/yaml_variant.h"

using performance_test_fixture::PerformanceTest;

namespace
{
constexpr const uint64_t kSize = 1024;
}

BENCHMARK_F(PerformanceTest, bool_copy_variant)(benchmark::State & st)
{
  bool bool_value = true;
  rcl_variant_t src_variant{};
  rcl_variant_t dest_variant{};
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  src_variant.bool_value = &bool_value;

  reset_heap_counters();

  for (auto _ : st) {
    if (!rcl_yaml_variant_copy(&dest_variant, &src_variant, allocator)) {
      st.SkipWithError(rcutils_get_error_string().str);
    }
    rcl_yaml_variant_fini(&dest_variant, allocator);
  }
  src_variant.bool_value = nullptr;
}

BENCHMARK_F(PerformanceTest, int_copy_variant)(benchmark::State & st)
{
  int64_t int_value = 42;
  rcl_variant_t src_variant{};
  rcl_variant_t dest_variant{};
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  src_variant.integer_value = &int_value;

  reset_heap_counters();

  for (auto _ : st) {
    if (!rcl_yaml_variant_copy(&dest_variant, &src_variant, allocator)) {
      st.SkipWithError(rcutils_get_error_string().str);
    }
    rcl_yaml_variant_fini(&dest_variant, allocator);
  }
  src_variant.integer_value = nullptr;
}

BENCHMARK_F(PerformanceTest, double_copy_variant)(benchmark::State & st)
{
  double double_value = 3.14157;
  rcl_variant_t src_variant{};
  rcl_variant_t dest_variant{};
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  src_variant.double_value = &double_value;

  reset_heap_counters();

  for (auto _ : st) {
    if (!rcl_yaml_variant_copy(&dest_variant, &src_variant, allocator)) {
      st.SkipWithError(rcutils_get_error_string().str);
    }
    rcl_yaml_variant_fini(&dest_variant, allocator);
  }
  src_variant.double_value = nullptr;
}

BENCHMARK_F(PerformanceTest, string_copy_variant)(benchmark::State & st)
{
  std::string data(kSize, '*');
  rcl_variant_t src_variant{};
  rcl_variant_t dest_variant{};
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  char * tmp_string = rcutils_strdup(data.c_str(), allocator);
  src_variant.string_value = tmp_string;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rcl_yaml_variant_fini(&src_variant, allocator);
    rcl_yaml_variant_fini(&dest_variant, allocator);
  });

  reset_heap_counters();

  for (auto _ : st) {
    if (!rcl_yaml_variant_copy(&dest_variant, &src_variant, allocator)) {
      st.SkipWithError(rcutils_get_error_string().str);
    }
    rcl_yaml_variant_fini(&dest_variant, allocator);
  }
}

BENCHMARK_F(PerformanceTest, array_bool_copy_variant)(benchmark::State & st)
{
  rcl_variant_t src_variant{};
  rcl_variant_t dest_variant{};
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  using ArrayT = std::remove_pointer<decltype(src_variant.bool_array_value)>::type;
  src_variant.bool_array_value =
    static_cast<ArrayT *>(allocator.allocate(sizeof(ArrayT), allocator.state));
  using ValueT = std::remove_pointer<decltype(src_variant.bool_array_value->values)>::type;
  src_variant.bool_array_value->values =
    static_cast<ValueT *>(
    allocator.zero_allocate(kSize, sizeof(ValueT), allocator.state));
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rcl_yaml_variant_fini(&src_variant, allocator);
    rcl_yaml_variant_fini(&dest_variant, allocator);
  });
  src_variant.bool_array_value->size = kSize;

  reset_heap_counters();

  for (auto _ : st) {
    if (!rcl_yaml_variant_copy(&dest_variant, &src_variant, allocator)) {
      st.SkipWithError(rcutils_get_error_string().str);
    }
    rcl_yaml_variant_fini(&dest_variant, allocator);
  }
}

BENCHMARK_F(PerformanceTest, array_int_copy_variant)(benchmark::State & st)
{
  rcl_variant_t src_variant{};
  rcl_variant_t dest_variant{};
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  using ArrayT = std::remove_pointer<decltype(src_variant.integer_array_value)>::type;
  src_variant.integer_array_value =
    static_cast<ArrayT *>(allocator.allocate(sizeof(ArrayT), allocator.state));
  using ValueT = std::remove_pointer<decltype(src_variant.integer_array_value->values)>::type;
  src_variant.integer_array_value->values =
    static_cast<ValueT *>(
    allocator.zero_allocate(kSize, sizeof(ValueT), allocator.state));
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rcl_yaml_variant_fini(&src_variant, allocator);
    rcl_yaml_variant_fini(&dest_variant, allocator);
  });
  src_variant.integer_array_value->size = kSize;

  reset_heap_counters();

  for (auto _ : st) {
    if (!rcl_yaml_variant_copy(&dest_variant, &src_variant, allocator)) {
      st.SkipWithError(rcutils_get_error_string().str);
    }
    rcl_yaml_variant_fini(&dest_variant, allocator);
  }
}

BENCHMARK_F(PerformanceTest, array_double_copy_variant)(benchmark::State & st)
{
  rcl_variant_t src_variant{};
  rcl_variant_t dest_variant{};
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  using ArrayT = std::remove_pointer<decltype(src_variant.double_array_value)>::type;
  src_variant.double_array_value =
    static_cast<ArrayT *>(allocator.allocate(sizeof(ArrayT), allocator.state));
  using ValueT = std::remove_pointer<decltype(src_variant.double_array_value->values)>::type;
  src_variant.double_array_value->values =
    static_cast<ValueT *>(
    allocator.zero_allocate(kSize, sizeof(ValueT), allocator.state));
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rcl_yaml_variant_fini(&src_variant, allocator);
    rcl_yaml_variant_fini(&dest_variant, allocator);
  });
  src_variant.double_array_value->size = kSize;

  reset_heap_counters();

  for (auto _ : st) {
    if (!rcl_yaml_variant_copy(&dest_variant, &src_variant, allocator)) {
      st.SkipWithError(rcutils_get_error_string().str);
    }
    rcl_yaml_variant_fini(&dest_variant, allocator);
  }
}

BENCHMARK_F(PerformanceTest, array_string_copy_variant)(benchmark::State & st)
{
  rcl_variant_t src_variant{};
  rcl_variant_t dest_variant{};
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  src_variant.string_array_value =
    static_cast<rcutils_string_array_t *>(
    allocator.allocate(sizeof(rcutils_string_array_t), allocator.state));
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rcl_yaml_variant_fini(&src_variant, allocator);
    rcl_yaml_variant_fini(&dest_variant, allocator);
  });
  *src_variant.string_array_value = rcutils_get_zero_initialized_string_array();
  if (rcutils_string_array_init(src_variant.string_array_value, kSize, &allocator) !=
    RCUTILS_RET_OK)
  {
    st.SkipWithError(rcutils_get_error_string().str);
  }

  for (size_t i = 0; i < kSize; i++) {
    src_variant.string_array_value->data[i] = rcutils_strdup("string", allocator);
    if (src_variant.string_array_value->data[i] == NULL) {
      st.SkipWithError(rcutils_get_error_string().str);
    }
  }

  reset_heap_counters();

  for (auto _ : st) {
    if (!rcl_yaml_variant_copy(&dest_variant, &src_variant, allocator)) {
      st.SkipWithError(rcutils_get_error_string().str);
    }
    rcl_yaml_variant_fini(&dest_variant, allocator);
  }
}
