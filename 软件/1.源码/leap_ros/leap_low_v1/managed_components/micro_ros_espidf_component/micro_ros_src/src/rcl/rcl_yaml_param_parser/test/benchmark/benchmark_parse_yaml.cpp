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

#include "performance_test_fixture/performance_test_fixture.hpp"

#include "rcl_yaml_param_parser/parser.h"

#include "rcpputils/filesystem_helper.hpp"

#include "rcutils/allocator.h"
#include "rcutils/error_handling.h"
#include "rcutils/filesystem.h"

using performance_test_fixture::PerformanceTest;

BENCHMARK_F(PerformanceTest, parser_yaml_param)(benchmark::State & st)
{
  std::string path =
    (rcpputils::fs::current_path() / "test" / "benchmark" / "benchmark_params.yaml").string();
  reset_heap_counters();
  for (auto _ : st) {
    rcl_params_t * params_hdl = rcl_yaml_node_struct_init(rcutils_get_default_allocator());
    if (NULL == params_hdl) {
      st.SkipWithError(rcutils_get_error_string().str);
    }
    bool res = rcl_parse_yaml_file(path.c_str(), params_hdl);
    if (!res) {
      st.SkipWithError(rcutils_get_error_string().str);
    }
    rcl_yaml_node_struct_fini(params_hdl);
  }
}
