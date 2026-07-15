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

#include <yaml.h>

#include <string>
#include <vector>

#include "gtest/gtest.h"

#include "osrf_testing_tools_cpp/scope_exit.hpp"
#include "rcl_yaml_param_parser/parser.h"
#include "rcutils/error_handling.h"
#include "rcutils/filesystem.h"
#include "rcutils/testing/fault_injection.h"
#include "./mocking_utils/patch.hpp"
#include "./time_bomb_allocator_testing_utils.h"

TEST(RclYamlParamParser, node_init_fini) {
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  rcl_params_t * params_st = rcl_yaml_node_struct_init(allocator);
  EXPECT_NE(params_st, nullptr);
  rcl_yaml_node_struct_fini(params_st);

  allocator = get_time_bomb_allocator();
  // Bad alloc of params_st
  set_time_bomb_allocator_calloc_count(allocator, 0);
  // This cleans up after itself if it fails so no need to call fini()
  EXPECT_EQ(rcl_yaml_node_struct_init(allocator), nullptr);

  // Bad alloc of params_st->node_names
  set_time_bomb_allocator_calloc_count(allocator, 1);
  EXPECT_EQ(rcl_yaml_node_struct_init(allocator), nullptr);

  // Bad alloc of params_st->params
  set_time_bomb_allocator_calloc_count(allocator, 2);
  EXPECT_EQ(rcl_yaml_node_struct_init(allocator), nullptr);

  // Check this doesn't die.
  rcl_yaml_node_struct_fini(nullptr);
}

TEST(RclYamlParamParser, node_init_with_capacity_fini) {
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  rcl_params_t * params_st = rcl_yaml_node_struct_init_with_capacity(1024, allocator);
  ASSERT_NE(params_st, nullptr);
  EXPECT_EQ(0U, params_st->num_nodes);
  EXPECT_EQ(1024U, params_st->capacity_nodes);
  rcl_yaml_node_struct_fini(params_st);

  allocator = get_time_bomb_allocator();
  // Bad alloc of params_st
  set_time_bomb_allocator_calloc_count(allocator, 0);
  // This cleans up after itself if it fails so no need to call fini()
  EXPECT_EQ(rcl_yaml_node_struct_init_with_capacity(1024, allocator), nullptr);

  // Bad alloc of params_st->node_names
  set_time_bomb_allocator_calloc_count(allocator, 1);
  EXPECT_EQ(rcl_yaml_node_struct_init_with_capacity(1024, allocator), nullptr);

  // Bad alloc of params_st->params
  set_time_bomb_allocator_calloc_count(allocator, 2);
  EXPECT_EQ(rcl_yaml_node_struct_init_with_capacity(1024, allocator), nullptr);

  // Check this doesn't die.
  rcl_yaml_node_struct_fini(nullptr);
}

TEST(RclYamlParamParser, reallocate_node_init_with_capacity_fini) {
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  rcl_params_t * params_st = rcl_yaml_node_struct_init_with_capacity(1024, allocator);
  ASSERT_NE(params_st, nullptr);
  EXPECT_EQ(0U, params_st->num_nodes);
  EXPECT_EQ(1024U, params_st->capacity_nodes);
  EXPECT_EQ(RCUTILS_RET_OK, rcl_yaml_node_struct_reallocate(params_st, 2048, allocator));
  EXPECT_EQ(0U, params_st->num_nodes);
  EXPECT_EQ(2048U, params_st->capacity_nodes);
  rcl_yaml_node_struct_fini(params_st);
}

TEST(RclYamlParamParser, node_copy) {
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  rcl_params_t * params_st = rcl_yaml_node_struct_init(allocator);
  EXPECT_NE(params_st, nullptr);

  EXPECT_EQ(nullptr, rcl_yaml_node_struct_copy(nullptr));

  const char node_name[] = "node name";
  const char param_name[] = "param name";
  const char yaml_value[] = "true";
  EXPECT_TRUE(rcl_parse_yaml_value(node_name, param_name, yaml_value, params_st));

  rcl_params_t * copy = rcl_yaml_node_struct_copy(params_st);
  EXPECT_NE(copy, nullptr);
  rcl_yaml_node_struct_fini(copy);

  params_st->allocator = get_time_bomb_allocator();

  // init of out_params_st fails
  set_time_bomb_allocator_calloc_count(params_st->allocator, 0);
  EXPECT_EQ(nullptr, rcl_yaml_node_struct_copy(params_st));

  set_time_bomb_allocator_calloc_count(params_st->allocator, 1);
  EXPECT_EQ(nullptr, rcl_yaml_node_struct_copy(params_st));

  constexpr int expected_num_calloc_calls = 5;
  for (int i = 0; i < expected_num_calloc_calls; ++i) {
    // Check various locations for allocation failures
    set_time_bomb_allocator_calloc_count(params_st->allocator, i);
    EXPECT_EQ(nullptr, rcl_yaml_node_struct_copy(params_st));
  }
  // Check that the expected number of calloc calls occur
  set_time_bomb_allocator_calloc_count(params_st->allocator, expected_num_calloc_calls);
  copy = rcl_yaml_node_struct_copy(params_st);
  EXPECT_NE(nullptr, copy);
  rcl_yaml_node_struct_fini(copy);

  // Reset calloc countdown
  set_time_bomb_allocator_calloc_count(params_st->allocator, -1);

  constexpr int expected_num_malloc_calls = 3;
  for (int i = 0; i < expected_num_malloc_calls; ++i) {
    set_time_bomb_allocator_malloc_count(params_st->allocator, i);
    EXPECT_EQ(nullptr, rcl_yaml_node_struct_copy(params_st));
  }

  // Check that the expected number of malloc calls occur
  set_time_bomb_allocator_malloc_count(params_st->allocator, expected_num_malloc_calls);
  copy = rcl_yaml_node_struct_copy(params_st);
  EXPECT_NE(nullptr, copy);
  rcl_yaml_node_struct_fini(copy);

  constexpr int num_malloc_calls_until_copy_param = 2;

  // Check integer value
  int64_t temp_int = 42;
  if (params_st->params->parameter_values[0].bool_value != nullptr) {
    // Since this bool was allocated above in rcl_parse_yaml_value, it needs to be freed.
    params_st->allocator.deallocate(
      params_st->params->parameter_values[0].bool_value, params_st->allocator.state);
    params_st->params->parameter_values[0].bool_value = nullptr;
  }
  set_time_bomb_allocator_malloc_count(params_st->allocator, num_malloc_calls_until_copy_param);
  params_st->params->parameter_values[0].integer_value = &temp_int;
  EXPECT_EQ(nullptr, rcl_yaml_node_struct_copy(params_st));
  params_st->params->parameter_values[0].integer_value = nullptr;

  // Check double value
  double temp_double = 42.0;
  set_time_bomb_allocator_malloc_count(params_st->allocator, num_malloc_calls_until_copy_param);
  params_st->params->parameter_values[0].double_value = &temp_double;
  EXPECT_EQ(nullptr, rcl_yaml_node_struct_copy(params_st));
  params_st->params->parameter_values[0].double_value = nullptr;

  // Check string value
  char temp_string[] = "stringy string";
  set_time_bomb_allocator_malloc_count(params_st->allocator, num_malloc_calls_until_copy_param);
  params_st->params->parameter_values[0].string_value = temp_string;
  EXPECT_EQ(nullptr, rcl_yaml_node_struct_copy(params_st));
  params_st->params->parameter_values[0].string_value = nullptr;

  // Check bool_array_value array pointer is allocated
  bool bool_array[] = {true};
  rcl_bool_array_s temp_bool_array = {bool_array, 1};
  set_time_bomb_allocator_malloc_count(params_st->allocator, num_malloc_calls_until_copy_param);
  params_st->params->parameter_values[0].bool_array_value = &temp_bool_array;
  EXPECT_EQ(nullptr, rcl_yaml_node_struct_copy(params_st));

  // Check bool_array_value->values is allocated
  set_time_bomb_allocator_malloc_count(
    params_st->allocator, num_malloc_calls_until_copy_param + 1);
  EXPECT_EQ(nullptr, rcl_yaml_node_struct_copy(params_st));

  // Check bool_array_value->values is set to null if size is 0
  set_time_bomb_allocator_malloc_count(params_st->allocator, -1);
  params_st->params->parameter_values[0].bool_array_value->size = 0u;
  copy = rcl_yaml_node_struct_copy(params_st);
  EXPECT_NE(nullptr, copy);
  EXPECT_EQ(nullptr, copy->params->parameter_values[0].bool_array_value->values);
  rcl_yaml_node_struct_fini(copy);
  params_st->params->parameter_values[0].bool_array_value = nullptr;

  // Check integer_array array pointer is allocated
  int64_t integer_array[] = {42};
  rcl_int64_array_s temp_integer_array = {integer_array, 1};
  set_time_bomb_allocator_malloc_count(params_st->allocator, num_malloc_calls_until_copy_param);
  params_st->params->parameter_values[0].integer_array_value = &temp_integer_array;
  EXPECT_EQ(nullptr, rcl_yaml_node_struct_copy(params_st));

  // Check integer_array->values is allocated
  set_time_bomb_allocator_malloc_count(
    params_st->allocator, num_malloc_calls_until_copy_param + 1);
  EXPECT_EQ(nullptr, rcl_yaml_node_struct_copy(params_st));

  // Check integer_array->values is set to null if size is 0
  params_st->params->parameter_values[0].integer_array_value->size = 0u;
  copy = rcl_yaml_node_struct_copy(params_st);
  EXPECT_NE(nullptr, copy);
  EXPECT_EQ(nullptr, copy->params->parameter_values[0].integer_array_value->values);
  rcl_yaml_node_struct_fini(copy);
  params_st->params->parameter_values[0].integer_array_value = nullptr;


  double double_array[] = {42.0};
  rcl_double_array_s temp_double_array = {double_array, 1};
  set_time_bomb_allocator_malloc_count(params_st->allocator, num_malloc_calls_until_copy_param);
  params_st->params->parameter_values[0].double_array_value = &temp_double_array;
  EXPECT_EQ(nullptr, rcl_yaml_node_struct_copy(params_st));

  set_time_bomb_allocator_malloc_count(
    params_st->allocator, num_malloc_calls_until_copy_param + 1);
  EXPECT_EQ(nullptr, rcl_yaml_node_struct_copy(params_st));

  params_st->params->parameter_values[0].double_array_value->size = 0u;
  copy = rcl_yaml_node_struct_copy(params_st);
  EXPECT_NE(nullptr, copy);
  EXPECT_EQ(nullptr, copy->params->parameter_values[0].double_array_value->values);
  rcl_yaml_node_struct_fini(copy);
  params_st->params->parameter_values[0].double_array_value = nullptr;

  char s[] = "stringy string";
  char * string_array[] = {s};
  rcutils_string_array_t temp_string_array = {1, string_array, allocator};
  set_time_bomb_allocator_malloc_count(params_st->allocator, num_malloc_calls_until_copy_param);
  params_st->params->parameter_values[0].string_array_value = &temp_string_array;
  EXPECT_EQ(nullptr, rcl_yaml_node_struct_copy(params_st));
  params_st->params->parameter_values[0].string_array_value = nullptr;

  for (int i = 0; i < 5; ++i) {
    set_time_bomb_allocator_calloc_count(params_st->allocator, i);
    EXPECT_EQ(nullptr, rcl_yaml_node_struct_copy(params_st));
  }

  rcl_yaml_node_struct_fini(params_st);
}
// // This just tests a couple of basic failures that test_parse_yaml.cpp misses.
// // See that file for more thorough testing of bad yaml files
TEST(RclYamlParamParser, test_file) {
  // file path is null
  EXPECT_FALSE(rcl_parse_yaml_file(nullptr, nullptr));
  const char bad_file_path[] = "not_a_file.yaml";

  // params_st is null
  EXPECT_FALSE(rcl_parse_yaml_file(bad_file_path, nullptr));

  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  rcl_params_t * params_st = rcl_yaml_node_struct_init(allocator);
  EXPECT_FALSE(rcl_parse_yaml_file(bad_file_path, params_st));

  rcl_yaml_node_struct_fini(params_st);
}

TEST(RclYamlParamParser, test_parse_yaml_value) {
  const char node_name[] = "node name";
  const char param_name[] = "param name";
  const char yaml_value[] = "true";
  const char empty_string[] = "\0";
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  rcl_params_t * params_st = rcl_yaml_node_struct_init(allocator);
  EXPECT_NE(params_st, nullptr);

  // Check null arguments
  EXPECT_FALSE(rcl_parse_yaml_value(nullptr, param_name, yaml_value, params_st));
  EXPECT_FALSE(rcl_parse_yaml_value(node_name, nullptr, yaml_value, params_st));
  EXPECT_FALSE(rcl_parse_yaml_value(node_name, param_name, nullptr, params_st));
  EXPECT_FALSE(rcl_parse_yaml_value(node_name, param_name, yaml_value, nullptr));

  // Check strings are empty
  EXPECT_FALSE(rcl_parse_yaml_value(empty_string, param_name, yaml_value, params_st));
  EXPECT_FALSE(rcl_parse_yaml_value(node_name, empty_string, yaml_value, params_st));
  EXPECT_FALSE(rcl_parse_yaml_value(node_name, param_name, empty_string, params_st));

  // Check allocating params_st->node_names[*node_idx] fails
  params_st->allocator = get_time_bomb_allocator();
  set_time_bomb_allocator_malloc_count(params_st->allocator, 0);
  EXPECT_FALSE(rcl_parse_yaml_value(node_name, param_name, yaml_value, params_st));

  // Check allocating node_params->parameter_names fails
  allocator = get_time_bomb_allocator();
  set_time_bomb_allocator_calloc_count(params_st->allocator, 0);
  EXPECT_FALSE(rcl_parse_yaml_value(node_name, param_name, yaml_value, params_st));

  // Check allocating node_params->parameter_values fails
  allocator = get_time_bomb_allocator();
  set_time_bomb_allocator_calloc_count(params_st->allocator, 1);
  EXPECT_FALSE(rcl_parse_yaml_value(node_name, param_name, yaml_value, params_st));

  allocator = rcutils_get_default_allocator();
  EXPECT_TRUE(rcl_parse_yaml_value(node_name, param_name, yaml_value, params_st));

  rcl_yaml_node_struct_fini(params_st);
}

TEST(RclYamlParamParser, test_yaml_node_struct_get) {
  const char node_name[] = "node name";
  const char param_name[] = "param name";
  const char yaml_value[] = "true";
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  rcl_params_t * params_st = rcl_yaml_node_struct_init(allocator);
  ASSERT_NE(params_st, nullptr);
  EXPECT_TRUE(rcl_parse_yaml_value(node_name, param_name, yaml_value, params_st));

  // Check null arguments
  EXPECT_EQ(nullptr, rcl_yaml_node_struct_get(nullptr, param_name, params_st));
  EXPECT_EQ(nullptr, rcl_yaml_node_struct_get(node_name, nullptr, params_st));
  EXPECT_EQ(nullptr, rcl_yaml_node_struct_get(node_name, param_name, nullptr));

  rcl_variant_t * result = rcl_yaml_node_struct_get(node_name, param_name, params_st);
  ASSERT_NE(nullptr, result->bool_value);
  EXPECT_TRUE(*result->bool_value);

  EXPECT_EQ(nullptr, result->integer_value);
  EXPECT_EQ(nullptr, result->double_value);
  EXPECT_EQ(nullptr, result->string_value);
  EXPECT_EQ(nullptr, result->byte_array_value);
  EXPECT_EQ(nullptr, result->bool_array_value);
  EXPECT_EQ(nullptr, result->integer_array_value);
  EXPECT_EQ(nullptr, result->double_array_value);
  EXPECT_EQ(nullptr, result->string_array_value);
  rcl_yaml_node_struct_fini(params_st);
}

// Just testing basic parameters, this is exercised more in test_parse_yaml.cpp
TEST(RclYamlParamParser, test_yaml_node_struct_print) {
  rcl_yaml_node_struct_print(nullptr);
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  rcl_params_t * params_st = rcl_yaml_node_struct_init(allocator);
  rcl_yaml_node_struct_print(params_st);
  rcl_yaml_node_struct_fini(params_st);
}

TEST(RclYamlParamParser, test_parse_file_with_bad_allocator) {
  char cur_dir[1024];
  rcutils_reset_error();
  EXPECT_TRUE(rcutils_get_cwd(cur_dir, 1024)) << rcutils_get_error_string().str;
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  char * test_path = rcutils_join_path(cur_dir, "test", allocator);
  ASSERT_TRUE(NULL != test_path) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    allocator.deallocate(test_path, allocator.state);
  });

  const std::vector<std::string> filenames = {
    "correct_config.yaml",
    "empty_string.yaml",
    "indented_name_space.yaml",
    "multi_ns_correct.yaml",
    "no_alias_support.yaml",
    "no_value1.yaml",
    "overlay.yaml",
    "params_with_no_node.yaml",
    "root_ns.yaml",
    "seq_map1.yaml",
    "seq_map2.yaml",
    "string_array_with_quoted_number.yaml"
  };

  for (auto & filename : filenames) {
    SCOPED_TRACE(filename);
    char * path = rcutils_join_path(test_path, filename.c_str(), allocator);
    ASSERT_TRUE(NULL != path) << rcutils_get_error_string().str;
    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      allocator.deallocate(path, allocator.state);
    });
    ASSERT_TRUE(rcutils_exists(path)) << "No test YAML file found at " << path;

    RCUTILS_FAULT_INJECTION_TEST(
    {
      rcutils_allocator_t allocator = rcutils_get_default_allocator();
      rcl_params_t * params_hdl = rcl_yaml_node_struct_init(allocator);
      if (NULL == params_hdl) {
        continue;
      }

      bool res = rcl_parse_yaml_file(path, params_hdl);
      // Not verifying res is true or false here, because eventually it will come back with an ok
      // result. We're just trying to make sure that bad allocations are properly handled
      (void)res;

      // If `rcutils_string_array_fini` fails, there will be a small memory leak here.
      // However, it's necessary for coverage
      rcl_yaml_node_struct_fini(params_hdl);
      params_hdl = NULL;
    });
  }
}

TEST(RclYamlParamParser, test_parse_yaml_initialize_mock) {
  char cur_dir[1024];
  rcutils_reset_error();
  EXPECT_TRUE(rcutils_get_cwd(cur_dir, 1024)) << rcutils_get_error_string().str;

  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  char * test_path = rcutils_join_path(cur_dir, "test", allocator);

  char * path = rcutils_join_path(test_path, "correct_config.yaml", allocator);
  ASSERT_TRUE(NULL != path) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    allocator.deallocate(test_path, allocator.state);
    allocator.deallocate(path, allocator.state);
  });

  rcl_params_t * params_hdl = rcl_yaml_node_struct_init(allocator);
  ASSERT_NE(nullptr, params_hdl);
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rcl_yaml_node_struct_fini(params_hdl);
  });
  auto mock = mocking_utils::patch_and_return(
    "lib:rcl_yaml_param_parser", yaml_parser_initialize, false);

  EXPECT_FALSE(rcl_parse_yaml_file(path, params_hdl));

  constexpr char node_name[] = "node name";
  constexpr char param_name[] = "param name";
  constexpr char yaml_value[] = "true";

  rcl_params_t * params_st = rcl_yaml_node_struct_init(allocator);
  ASSERT_NE(params_st, nullptr);
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rcl_yaml_node_struct_fini(params_st);
  });
  EXPECT_FALSE(rcl_parse_yaml_value(node_name, param_name, yaml_value, params_st));
}


int32_t main(int32_t argc, char ** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
