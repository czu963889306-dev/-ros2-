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

#include <memory>

#include "performance_test_fixture/performance_test_fixture.hpp"

#include "../../src/functions.hpp"

using performance_test_fixture::PerformanceTest;

BENCHMARK_F(PerformanceTest, prefetch_symbols)(benchmark::State & st)
{
  for (auto _ : st) {
    prefetch_symbols();
    unload_library();
  }
}

BENCHMARK_F(PerformanceTest, lookup_symbol)(benchmark::State & st)
{
  for (auto _ : st) {
    std::shared_ptr<rcpputils::SharedLibrary> lib = load_library();
    lookup_symbol(lib, "rmw_init");
  }
}
