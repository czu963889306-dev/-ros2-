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

#include "rosidl_runtime_c/string_functions.h"
#include "rosidl_runtime_c/u16string_functions.h"

#include "performance_test_fixture/performance_test_fixture.hpp"

using performance_test_fixture::PerformanceTest;

BENCHMARK_DEFINE_F(PerformanceTest, string_assign)(benchmark::State & st)
{
  size_t len = st.range(0);
  std::string data(len, '*');

  rosidl_runtime_c__String s;
  if (!rosidl_runtime_c__String__init(&s)) {
    st.SkipWithError("String initialization failed");
    return;
  }

  // No explicit resize function - just do a copy
  rosidl_runtime_c__String__assignn(&s, data.c_str(), len);

  reset_heap_counters();

  for (auto _ : st) {
    rosidl_runtime_c__String__assignn(&s, data.c_str(), len);
  }

  rosidl_runtime_c__String__fini(&s);
}

BENCHMARK_REGISTER_F(PerformanceTest, string_assign)
->Arg(4096);

BENCHMARK_DEFINE_F(PerformanceTest, string_resize_assign)(benchmark::State & st)
{
  size_t len = st.range(0);
  std::string data(len, '*');

  rosidl_runtime_c__String s;
  if (!rosidl_runtime_c__String__init(&s)) {
    st.SkipWithError("String initialization failed");
    return;
  }

  // No explicit resize function - just do a copy
  rosidl_runtime_c__String__assignn(&s, data.c_str(), 0);

  reset_heap_counters();

  for (auto _ : st) {
    rosidl_runtime_c__String__assignn(&s, data.c_str(), len);
    rosidl_runtime_c__String__assignn(&s, data.c_str(), 0);
  }

  rosidl_runtime_c__String__fini(&s);
}

BENCHMARK_REGISTER_F(PerformanceTest, string_resize_assign)
->Arg(4096);

BENCHMARK_DEFINE_F(PerformanceTest, u16string_assign)(benchmark::State & st)
{
  size_t len = st.range(0);
  std::string data(len, '*');

  rosidl_runtime_c__U16String s;
  if (!rosidl_runtime_c__U16String__init(&s)) {
    st.SkipWithError("U16String initialization failed");
    return;
  }

  rosidl_runtime_c__U16String__resize(&s, len);

  reset_heap_counters();

  for (auto _ : st) {
    rosidl_runtime_c__U16String__assignn_from_char(&s, data.c_str(), len);
  }

  rosidl_runtime_c__U16String__fini(&s);
}

BENCHMARK_REGISTER_F(PerformanceTest, u16string_assign)
->Arg(4096);

BENCHMARK_DEFINE_F(PerformanceTest, u16string_resize_assign)(benchmark::State & st)
{
  size_t len = st.range(0);
  std::string data(len, '*');

  rosidl_runtime_c__U16String s;
  if (!rosidl_runtime_c__U16String__init(&s)) {
    st.SkipWithError("U16String initialization failed");
    return;
  }

  rosidl_runtime_c__U16String__resize(&s, 0);

  reset_heap_counters();

  for (auto _ : st) {
    rosidl_runtime_c__U16String__assignn_from_char(&s, data.c_str(), len);
    rosidl_runtime_c__U16String__resize(&s, 0);
  }

  rosidl_runtime_c__U16String__fini(&s);
}

BENCHMARK_REGISTER_F(PerformanceTest, u16string_resize_assign)
->Arg(4096);
