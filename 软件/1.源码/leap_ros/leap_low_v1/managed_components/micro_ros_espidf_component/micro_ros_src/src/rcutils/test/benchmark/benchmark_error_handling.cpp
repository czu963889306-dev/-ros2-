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

#include <benchmark/benchmark.h>
#include <cassert>
#include <string>

#include "rcutils/error_handling.h"

static void benchmark_err_handling(benchmark::State & state)
{
  for (auto _ : state) {
    rcutils_ret_t ret =
      rcutils_initialize_error_handling_thread_local_storage(rcutils_get_default_allocator());
    assert(ret == RCUTILS_RET_OK);
    (void)ret;
    rcutils_reset_error();
    const char * test_message = "test message";
    RCUTILS_SET_ERROR_MSG(test_message);
  }
}

BENCHMARK(benchmark_err_handling);
