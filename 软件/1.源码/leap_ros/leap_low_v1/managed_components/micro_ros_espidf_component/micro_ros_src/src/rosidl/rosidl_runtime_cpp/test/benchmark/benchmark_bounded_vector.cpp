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

#include <forward_list>
#include <iterator>
#include <utility>
#include <sstream>
#include <string>

#include "rosidl_runtime_cpp/bounded_vector.hpp"

#include "performance_test_fixture/performance_test_fixture.hpp"

using performance_test_fixture::PerformanceTest;

BENCHMARK_F(PerformanceTest, bounded_vector)(benchmark::State & st)
{
  rosidl_runtime_cpp::BoundedVector<int, 1> v;

  v.reserve(1);

  reset_heap_counters();

  for (auto _ : st) {
    v.push_back(0);
    v.erase(v.begin());
  }
}

BENCHMARK_F(PerformanceTest, bounded_vector_rvalue)(benchmark::State & st)
{
  rosidl_runtime_cpp::BoundedVector<int, 1> v;

  v.reserve(1);

  reset_heap_counters();

  for (auto _ : st) {
    v.emplace_back(0);
    v.erase(v.begin());
  }
}

BENCHMARK_F(PerformanceTest, bounded_vector_insert)(benchmark::State & st)
{
  rosidl_runtime_cpp::BoundedVector<int, 1> v;

  rosidl_runtime_cpp::BoundedVector<int, 1> v2;
  v2.push_back(0);
  v.reserve(1);

  reset_heap_counters();

  for (auto _ : st) {
    v.insert(v.begin(), v2.begin(), v2.end());
    v.erase(v.begin());
  }
}

BENCHMARK_F(PerformanceTest, bounded_vector_input_iterators)(benchmark::State & st)
{
  rosidl_runtime_cpp::BoundedVector<int, 1> v;

  std::string vector_string;
  vector_string += std::to_string(0);
  std::istringstream ss;
  ss.str(vector_string);
  std::istream_iterator<int> ii(ss), end;
  v.reserve(1);

  reset_heap_counters();

  for (auto _ : st) {
    v.assign(ii, end);
    v.erase(v.begin());
  }
}

BENCHMARK_F(PerformanceTest, bounded_vector_forward_iterators)(benchmark::State & st)
{
  rosidl_runtime_cpp::BoundedVector<int, 1> v;

  std::forward_list<int> l;
  l.push_front(0);
  v.reserve(1);

  reset_heap_counters();

  for (auto _ : st) {
    v.assign(l.begin(), l.end());
    v.erase(v.begin());
  }
}
