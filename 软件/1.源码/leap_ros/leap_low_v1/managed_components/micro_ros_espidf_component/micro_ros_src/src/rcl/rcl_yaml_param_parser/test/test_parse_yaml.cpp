// Copyright 2018 Apex.AI, Inc.
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

#include <cmath>
#include <string>
#include <vector>

#include "osrf_testing_tools_cpp/scope_exit.hpp"

#include "rcl_yaml_param_parser/parser.h"

#include "rcutils/allocator.h"
#include "rcutils/error_handling.h"
#include "rcutils/filesystem.h"

static char cur_dir[1024];

TEST(test_parser, correct_syntax) {
  rcutils_reset_error();
  EXPECT_TRUE(rcutils_get_cwd(cur_dir, 1024)) << rcutils_get_error_string().str;
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  char * test_path = rcutils_join_path(cur_dir, "test", allocator);
  ASSERT_TRUE(NULL != test_path) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    allocator.deallocate(test_path, allocator.state);
  });
  char * path = rcutils_join_path(test_path, "correct_config.yaml", allocator);
  ASSERT_TRUE(NULL != path) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    allocator.deallocate(path, allocator.state);
  });
  ASSERT_TRUE(rcutils_exists(path)) << "No test YAML file found at " << path;
  rcl_params_t * params_hdl = rcl_yaml_node_struct_init(allocator);
  ASSERT_TRUE(NULL != params_hdl) << rcutils_get_error_string().str;

  // Parse correct_config.yaml as expected
  bool res = rcl_parse_yaml_file(path, params_hdl);
  ASSERT_TRUE(res) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rcl_yaml_node_struct_fini(params_hdl);
  });

  char * another_path = rcutils_join_path(test_path, "overlay.yaml", allocator);
  ASSERT_TRUE(NULL != another_path) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    allocator.deallocate(another_path, allocator.state);
  });
  ASSERT_TRUE(rcutils_exists(another_path)) << "No test YAML file found at " << another_path;

  // Parse overlay.yaml using the same params_hdl, expect them to merge nicely
  res = rcl_parse_yaml_file(another_path, params_hdl);
  ASSERT_TRUE(res) << rcutils_get_error_string().str;

  rcl_params_t * copy_of_params_hdl = rcl_yaml_node_struct_copy(params_hdl);
  ASSERT_TRUE(NULL != copy_of_params_hdl) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rcl_yaml_node_struct_fini(copy_of_params_hdl);
  });

  rcl_params_t * params_hdl_set[] = {params_hdl, copy_of_params_hdl};
  for (rcl_params_t * params : params_hdl_set) {
    rcl_variant_t * param_value = rcl_yaml_node_struct_get("lidar_ns/lidar_2", "is_back", params);
    ASSERT_TRUE(NULL != param_value) << rcutils_get_error_string().str;
    ASSERT_TRUE(NULL != param_value->bool_value);
    // Assigning to local to avoid clang analysis warning "Forming reference to null pointer"
    bool bool_value = *param_value->bool_value;
    EXPECT_TRUE(bool_value);
    res = rcl_parse_yaml_value("lidar_ns/lidar_2", "is_back", "false", params);
    EXPECT_TRUE(res) << rcutils_get_error_string().str;
    ASSERT_TRUE(NULL != param_value->bool_value);
    bool_value = *param_value->bool_value;
    EXPECT_FALSE(bool_value);

    param_value = rcl_yaml_node_struct_get("lidar_ns/lidar_2", "id", params);
    ASSERT_TRUE(NULL != param_value) << rcutils_get_error_string().str;
    ASSERT_TRUE(NULL != param_value->integer_value);
    // Make sure that we can correctly parse bigger than LONG_MAX = 2147483647 values
    EXPECT_EQ(992147483647, *param_value->integer_value);
    res = rcl_parse_yaml_value("lidar_ns/lidar_2", "id", "12", params);
    EXPECT_TRUE(res) << rcutils_get_error_string().str;
    ASSERT_TRUE(NULL != param_value->integer_value);
    EXPECT_EQ(12, *param_value->integer_value);

    param_value = rcl_yaml_node_struct_get("camera", "loc", params);
    ASSERT_TRUE(NULL != param_value) << rcutils_get_error_string().str;
    ASSERT_TRUE(NULL != param_value->string_value);
    EXPECT_STREQ("back", param_value->string_value);
    res = rcl_parse_yaml_value("camera", "loc", "front", params);
    EXPECT_TRUE(res) << rcutils_get_error_string().str;
    ASSERT_TRUE(NULL != param_value->string_value);
    EXPECT_STREQ("front", param_value->string_value);

    param_value = rcl_yaml_node_struct_get("camera", "cam_spec.angle", params);
    ASSERT_TRUE(NULL != param_value) << rcutils_get_error_string().str;
    ASSERT_TRUE(NULL != param_value->double_value);
    EXPECT_DOUBLE_EQ(2.34, *param_value->double_value);
    res = rcl_parse_yaml_value("camera", "cam_spec.angle", "2.2", params);
    EXPECT_TRUE(res) << rcutils_get_error_string().str;
    ASSERT_TRUE(NULL != param_value->double_value);
    EXPECT_DOUBLE_EQ(2.2, *param_value->double_value);

    param_value = rcl_yaml_node_struct_get("intel", "num_cores", params);
    ASSERT_TRUE(NULL != param_value) << rcutils_get_error_string().str;
    ASSERT_TRUE(NULL != param_value->integer_value);
    EXPECT_EQ(12, *param_value->integer_value);
    res = rcl_parse_yaml_value("intel", "num_cores", "8", params);
    EXPECT_TRUE(res) << rcutils_get_error_string().str;
    ASSERT_TRUE(NULL != param_value->integer_value);
    EXPECT_EQ(8, *param_value->integer_value);

    param_value = rcl_yaml_node_struct_get("intel", "arch", params);
    ASSERT_TRUE(NULL != param_value) << rcutils_get_error_string().str;
    ASSERT_TRUE(NULL != param_value->string_value);
    EXPECT_STREQ("x86_64", param_value->string_value);
    res = rcl_parse_yaml_value("intel", "arch", "x86", params);
    EXPECT_TRUE(res) << rcutils_get_error_string().str;
    ASSERT_TRUE(NULL != param_value->string_value);
    EXPECT_STREQ("x86", param_value->string_value);

    param_value = rcl_yaml_node_struct_get("new_camera_ns/new_camera1", "is_cam_on", params);
    ASSERT_TRUE(NULL != param_value) << rcutils_get_error_string().str;
    ASSERT_TRUE(NULL != param_value->bool_array_value);
    ASSERT_EQ(6U, param_value->bool_array_value->size);
    EXPECT_TRUE(param_value->bool_array_value->values[0]);
    EXPECT_TRUE(param_value->bool_array_value->values[1]);
    EXPECT_FALSE(param_value->bool_array_value->values[2]);
    EXPECT_TRUE(param_value->bool_array_value->values[3]);
    EXPECT_FALSE(param_value->bool_array_value->values[4]);
    EXPECT_FALSE(param_value->bool_array_value->values[5]);
    res = rcl_parse_yaml_value("new_camera_ns/new_camera1", "is_cam_on", "[false, true]", params);
    EXPECT_TRUE(res) << rcutils_get_error_string().str;
    ASSERT_TRUE(NULL != param_value->bool_array_value);
    ASSERT_EQ(2U, param_value->bool_array_value->size);
    EXPECT_FALSE(param_value->bool_array_value->values[0]);
    EXPECT_TRUE(param_value->bool_array_value->values[1]);

    param_value = rcl_yaml_node_struct_get("lidar_ns/lidar_1", "ports", params);
    ASSERT_TRUE(NULL != param_value) << rcutils_get_error_string().str;
    ASSERT_TRUE(NULL != param_value->integer_array_value);
    ASSERT_EQ(3U, param_value->integer_array_value->size);
    EXPECT_EQ(2438, param_value->integer_array_value->values[0]);
    EXPECT_EQ(2439, param_value->integer_array_value->values[1]);
    EXPECT_EQ(2440, param_value->integer_array_value->values[2]);
    res = rcl_parse_yaml_value("lidar_ns/lidar_1", "ports", "[8080]", params);
    EXPECT_TRUE(res) << rcutils_get_error_string().str;
    ASSERT_TRUE(NULL != param_value->integer_array_value);
    ASSERT_EQ(1U, param_value->integer_array_value->size);
    EXPECT_EQ(8080, param_value->integer_array_value->values[0]);

    param_value = rcl_yaml_node_struct_get(
      "lidar_ns/lidar_1", "driver1.bk_sensor_specs", params);
    ASSERT_TRUE(NULL != param_value) << rcutils_get_error_string().str;
    ASSERT_TRUE(NULL != param_value->double_array_value);
    ASSERT_EQ(4U, param_value->double_array_value->size);
    EXPECT_DOUBLE_EQ(12.1, param_value->double_array_value->values[0]);
    EXPECT_DOUBLE_EQ(-2.3, param_value->double_array_value->values[1]);
    EXPECT_DOUBLE_EQ(5.2, param_value->double_array_value->values[2]);
    EXPECT_DOUBLE_EQ(9.0, param_value->double_array_value->values[3]);
    res = rcl_parse_yaml_value("lidar_ns/lidar_1", "driver1.bk_sensor_specs", "[1.0]", params);
    EXPECT_TRUE(res) << rcutils_get_error_string().str;
    ASSERT_TRUE(NULL != param_value->double_array_value);
    ASSERT_EQ(1U, param_value->double_array_value->size);
    EXPECT_DOUBLE_EQ(1.0, param_value->double_array_value->values[0]);

    param_value = rcl_yaml_node_struct_get("camera", "cam_spec.supported_brands", params);
    ASSERT_TRUE(NULL != param_value) << rcutils_get_error_string().str;
    ASSERT_TRUE(NULL != param_value->string_array_value);
    ASSERT_EQ(3U, param_value->string_array_value->size);
    EXPECT_STREQ("Bosch", param_value->string_array_value->data[0]);
    EXPECT_STREQ("Novatek", param_value->string_array_value->data[1]);
    EXPECT_STREQ("Mobius", param_value->string_array_value->data[2]);
    res = rcl_parse_yaml_value(
      "camera", "cam_spec.supported_brands", "[Mobius]", params);
    EXPECT_TRUE(res) << rcutils_get_error_string().str;
    ASSERT_TRUE(NULL != param_value);
    ASSERT_TRUE(NULL != param_value->string_array_value);
    ASSERT_EQ(1U, param_value->string_array_value->size);
    EXPECT_STREQ("Mobius", param_value->string_array_value->data[0]);

    rcl_yaml_node_struct_print(params);
  }
}

TEST(test_file_parser, string_array_with_quoted_number) {
  rcutils_reset_error();
  EXPECT_TRUE(rcutils_get_cwd(cur_dir, 1024)) << rcutils_get_error_string().str;
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  char * test_path = rcutils_join_path(cur_dir, "test", allocator);
  ASSERT_TRUE(NULL != test_path) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    allocator.deallocate(test_path, allocator.state);
  });
  char * path = rcutils_join_path(test_path, "string_array_with_quoted_number.yaml", allocator);
  ASSERT_TRUE(NULL != path) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    allocator.deallocate(path, allocator.state);
  });
  ASSERT_TRUE(rcutils_exists(path)) << "No test YAML file found at " << path;
  rcl_params_t * params_hdl = rcl_yaml_node_struct_init(allocator);
  ASSERT_TRUE(NULL != params_hdl) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rcl_yaml_node_struct_fini(params_hdl);
  });
  bool res = rcl_parse_yaml_file(path, params_hdl);
  ASSERT_TRUE(res) << rcutils_get_error_string().str;
  rcl_variant_t * param_value = rcl_yaml_node_struct_get(
    "initial_params_node", "sa2", params_hdl);
  ASSERT_TRUE(NULL != param_value) << rcutils_get_error_string().str;
  ASSERT_TRUE(NULL != param_value->string_array_value);
  ASSERT_EQ(2U, param_value->string_array_value->size);
  EXPECT_STREQ("and", param_value->string_array_value->data[0]);
  EXPECT_STREQ("7", param_value->string_array_value->data[1]);
  res = rcl_parse_yaml_value("initial_params_node", "category", "'0'", params_hdl);
  EXPECT_TRUE(res) << rcutils_get_error_string().str;
  param_value = rcl_yaml_node_struct_get("initial_params_node", "category", params_hdl);
  ASSERT_TRUE(NULL != param_value) << rcutils_get_error_string().str;
  ASSERT_TRUE(NULL != param_value->string_value);
  EXPECT_STREQ("0", param_value->string_value);
  rcl_yaml_node_struct_print(params_hdl);
}

TEST(test_file_parser, multi_ns_correct_syntax) {
  rcutils_reset_error();
  EXPECT_TRUE(rcutils_get_cwd(cur_dir, 1024)) << rcutils_get_error_string().str;
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  char * test_path = rcutils_join_path(cur_dir, "test", allocator);
  ASSERT_TRUE(NULL != test_path) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    allocator.deallocate(test_path, allocator.state);
  });
  char * path = rcutils_join_path(test_path, "multi_ns_correct.yaml", allocator);
  ASSERT_TRUE(NULL != path) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    allocator.deallocate(path, allocator.state);
  });
  ASSERT_TRUE(rcutils_exists(path)) << "No test YAML file found at " << path;
  rcl_params_t * params_hdl = rcl_yaml_node_struct_init(allocator);
  ASSERT_TRUE(NULL != params_hdl) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rcl_yaml_node_struct_fini(params_hdl);
  });
  bool res = rcl_parse_yaml_file(path, params_hdl);
  EXPECT_TRUE(res) << rcutils_get_error_string().str;
  rcl_yaml_node_struct_print(params_hdl);
}

TEST(test_file_parser, root_ns) {
  rcutils_reset_error();
  EXPECT_TRUE(rcutils_get_cwd(cur_dir, 1024)) << rcutils_get_error_string().str;
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  char * test_path = rcutils_join_path(cur_dir, "test", allocator);
  ASSERT_TRUE(NULL != test_path) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    allocator.deallocate(test_path, allocator.state);
  });
  char * path = rcutils_join_path(test_path, "root_ns.yaml", allocator);
  ASSERT_TRUE(NULL != path) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    allocator.deallocate(path, allocator.state);
  });
  ASSERT_TRUE(rcutils_exists(path)) << "No test YAML file found at " << path;
  rcl_params_t * params_hdl = rcl_yaml_node_struct_init(allocator);
  ASSERT_TRUE(NULL != params_hdl) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rcl_yaml_node_struct_fini(params_hdl);
  });
  bool res = rcl_parse_yaml_file(path, params_hdl);
  EXPECT_TRUE(res) << rcutils_get_error_string().str;
  rcl_yaml_node_struct_print(params_hdl);
  // Check that there is only one forward slash in the node's FQN.
  // (Regression test for https://github.com/ros2/rcl/pull/299).
  EXPECT_EQ(1u, params_hdl->num_nodes);
  EXPECT_STREQ("/my_node", params_hdl->node_names[0]);
}

TEST(test_file_parser, seq_map1) {
  rcutils_reset_error();
  EXPECT_TRUE(rcutils_get_cwd(cur_dir, 1024)) << rcutils_get_error_string().str;
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  char * test_path = rcutils_join_path(cur_dir, "test", allocator);
  ASSERT_TRUE(NULL != test_path) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    allocator.deallocate(test_path, allocator.state);
  });
  char * path = rcutils_join_path(test_path, "seq_map1.yaml", allocator);
  ASSERT_TRUE(NULL != path) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    allocator.deallocate(path, allocator.state);
  });
  ASSERT_TRUE(rcutils_exists(path)) << "No test YAML file found at " << path;
  rcl_params_t * params_hdl = rcl_yaml_node_struct_init(allocator);
  ASSERT_TRUE(NULL != params_hdl) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rcl_yaml_node_struct_fini(params_hdl);
  });
  bool res = rcl_parse_yaml_file(path, params_hdl);
  EXPECT_FALSE(res);
}

TEST(test_file_parser, seq_map2) {
  rcutils_reset_error();
  EXPECT_TRUE(rcutils_get_cwd(cur_dir, 1024)) << rcutils_get_error_string().str;
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  char * test_path = rcutils_join_path(cur_dir, "test", allocator);
  ASSERT_TRUE(NULL != test_path) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    allocator.deallocate(test_path, allocator.state);
  });
  char * path = rcutils_join_path(test_path, "seq_map2.yaml", allocator);
  ASSERT_TRUE(NULL != path) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    allocator.deallocate(path, allocator.state);
  });
  ASSERT_TRUE(rcutils_exists(path)) << "No test YAML file found at " << path;
  rcl_params_t * params_hdl = rcl_yaml_node_struct_init(allocator);
  ASSERT_TRUE(NULL != params_hdl) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rcl_yaml_node_struct_fini(params_hdl);
  });
  bool res = rcl_parse_yaml_file(path, params_hdl);
  EXPECT_FALSE(res);
}

TEST(test_file_parser, params_with_no_node) {
  rcutils_reset_error();
  EXPECT_TRUE(rcutils_get_cwd(cur_dir, 1024)) << rcutils_get_error_string().str;
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  char * test_path = rcutils_join_path(cur_dir, "test", allocator);
  ASSERT_TRUE(NULL != test_path) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    allocator.deallocate(test_path, allocator.state);
  });
  char * path = rcutils_join_path(test_path, "params_with_no_node.yaml", allocator);
  ASSERT_TRUE(NULL != path) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    allocator.deallocate(path, allocator.state);
  });
  ASSERT_TRUE(rcutils_exists(path)) << "No test YAML file found at " << path;
  rcl_params_t * params_hdl = rcl_yaml_node_struct_init(allocator);
  ASSERT_TRUE(NULL != params_hdl) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rcl_yaml_node_struct_fini(params_hdl);
  });
  bool res = rcl_parse_yaml_file(path, params_hdl);
  EXPECT_FALSE(res);
}

TEST(test_file_parser, no_alias_support) {
  rcutils_reset_error();
  EXPECT_TRUE(rcutils_get_cwd(cur_dir, 1024)) << rcutils_get_error_string().str;
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  char * test_path = rcutils_join_path(cur_dir, "test", allocator);
  ASSERT_TRUE(NULL != test_path) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    allocator.deallocate(test_path, allocator.state);
  });
  char * path = rcutils_join_path(test_path, "no_alias_support.yaml", allocator);
  ASSERT_TRUE(NULL != path) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    allocator.deallocate(path, allocator.state);
  });
  ASSERT_TRUE(rcutils_exists(path)) << "No test YAML file found at " << path;
  rcl_params_t * params_hdl = rcl_yaml_node_struct_init(allocator);
  ASSERT_TRUE(NULL != params_hdl) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rcl_yaml_node_struct_fini(params_hdl);
  });
  bool res = rcl_parse_yaml_file(path, params_hdl);
  EXPECT_FALSE(res);
}

TEST(test_file_parser, empty_string) {
  rcutils_reset_error();
  EXPECT_TRUE(rcutils_get_cwd(cur_dir, 1024)) << rcutils_get_error_string().str;
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  char * test_path = rcutils_join_path(cur_dir, "test", allocator);
  ASSERT_TRUE(NULL != test_path) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    allocator.deallocate(test_path, allocator.state);
  });
  char * path = rcutils_join_path(test_path, "empty_string.yaml", allocator);
  ASSERT_TRUE(NULL != path) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    allocator.deallocate(path, allocator.state);
  });
  ASSERT_TRUE(rcutils_exists(path)) << "No test YAML file found at " << path;
  rcl_params_t * params_hdl = rcl_yaml_node_struct_init(allocator);
  ASSERT_TRUE(NULL != params_hdl) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rcl_yaml_node_struct_fini(params_hdl);
  });
  bool res = rcl_parse_yaml_file(path, params_hdl);
  EXPECT_TRUE(res) << rcutils_get_error_string().str;
  rcl_yaml_node_struct_print(params_hdl);
}

TEST(test_file_parser, no_value1) {
  rcutils_reset_error();
  EXPECT_TRUE(rcutils_get_cwd(cur_dir, 1024)) << rcutils_get_error_string().str;
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  char * test_path = rcutils_join_path(cur_dir, "test", allocator);
  ASSERT_TRUE(NULL != test_path) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    allocator.deallocate(test_path, allocator.state);
  });
  char * path = rcutils_join_path(test_path, "no_value1.yaml", allocator);
  ASSERT_TRUE(NULL != path) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    allocator.deallocate(path, allocator.state);
  });
  ASSERT_TRUE(rcutils_exists(path)) << "No test YAML file found at " << path;
  rcl_params_t * params_hdl = rcl_yaml_node_struct_init(allocator);
  ASSERT_TRUE(NULL != params_hdl) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rcl_yaml_node_struct_fini(params_hdl);
  });
  bool res = rcl_parse_yaml_file(path, params_hdl);
  EXPECT_FALSE(res);
}

TEST(test_file_parser, indented_ns) {
  rcutils_reset_error();
  EXPECT_TRUE(rcutils_get_cwd(cur_dir, 1024)) << rcutils_get_error_string().str;
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  char * test_path = rcutils_join_path(cur_dir, "test", allocator);
  ASSERT_TRUE(NULL != test_path) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    allocator.deallocate(test_path, allocator.state);
  });
  char * path = rcutils_join_path(test_path, "indented_name_space.yaml", allocator);
  ASSERT_TRUE(NULL != path) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    allocator.deallocate(path, allocator.state);
  });
  ASSERT_TRUE(rcutils_exists(path)) << "No test YAML file found at " << path;
  rcl_params_t * params_hdl = rcl_yaml_node_struct_init(allocator);
  ASSERT_TRUE(NULL != params_hdl) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rcl_yaml_node_struct_fini(params_hdl);
  });
  bool res = rcl_parse_yaml_file(path, params_hdl);
  EXPECT_FALSE(res);
}

// Test special float point(https://github.com/ros2/rcl/issues/555).
TEST(test_file_parser, special_float_point) {
  rcutils_reset_error();
  EXPECT_TRUE(rcutils_get_cwd(cur_dir, 1024)) << rcutils_get_error_string().str;
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  char * test_path = rcutils_join_path(cur_dir, "test", allocator);
  ASSERT_TRUE(NULL != test_path) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    allocator.deallocate(test_path, allocator.state);
  });
  char * path = rcutils_join_path(test_path, "special_float.yaml", allocator);
  ASSERT_TRUE(NULL != path) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    allocator.deallocate(path, allocator.state);
  });
  ASSERT_TRUE(rcutils_exists(path)) << "No test YAML file found at " << path;
  rcl_params_t * params_hdl = rcl_yaml_node_struct_init(allocator);
  ASSERT_TRUE(NULL != params_hdl) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rcl_yaml_node_struct_fini(params_hdl);
  });

  bool res = rcl_parse_yaml_file(path, params_hdl);
  EXPECT_TRUE(res) << rcutils_get_error_string().str;
  rcl_variant_t * param_value = rcl_yaml_node_struct_get("test_node", "isstring", params_hdl);
  ASSERT_TRUE(NULL != param_value) << rcutils_get_error_string().str;
  ASSERT_TRUE(NULL != param_value->string_array_value);
  EXPECT_STREQ(".nananan", param_value->string_array_value->data[1]);
  EXPECT_STREQ(".nAN", param_value->string_array_value->data[2]);
  EXPECT_STREQ(".infinf", param_value->string_array_value->data[4]);
  EXPECT_STREQ(".INf", param_value->string_array_value->data[5]);
  param_value = rcl_yaml_node_struct_get(
    "test_node", "nan_inf", params_hdl);
  ASSERT_TRUE(NULL != param_value) << rcutils_get_error_string().str;
  ASSERT_TRUE(NULL != param_value->double_array_value);
  ASSERT_EQ(7U, param_value->double_array_value->size);
  EXPECT_FALSE(std::isnan(param_value->double_array_value->values[1]));
  EXPECT_TRUE(std::isnan(param_value->double_array_value->values[2]));
  EXPECT_TRUE(std::isnan(param_value->double_array_value->values[3]));
  EXPECT_TRUE(std::isinf(param_value->double_array_value->values[4]));
  EXPECT_TRUE(std::isinf(param_value->double_array_value->values[5]));
  EXPECT_TRUE(std::isinf(param_value->double_array_value->values[6]));
}

TEST(test_file_parser, empty_name_in_ns) {
  rcutils_reset_error();
  EXPECT_TRUE(rcutils_get_cwd(cur_dir, 1024));
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  char * test_path = rcutils_join_path(cur_dir, "test", allocator);
  ASSERT_TRUE(NULL != test_path) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    allocator.deallocate(test_path, allocator.state);
  });
  char * path = rcutils_join_path(test_path, "empty_name_in_ns.yaml", allocator);
  ASSERT_TRUE(NULL != path) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    allocator.deallocate(path, allocator.state);
  });
  EXPECT_TRUE(rcutils_exists(path));
  rcl_params_t * params_hdl = rcl_yaml_node_struct_init(allocator);
  ASSERT_TRUE(NULL != params_hdl) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rcl_yaml_node_struct_fini(params_hdl);
  });
  bool res = rcl_parse_yaml_file(path, params_hdl);
  EXPECT_FALSE(res);
}

TEST(test_file_parser, wildcards) {
  rcutils_reset_error();
  EXPECT_TRUE(rcutils_get_cwd(cur_dir, 1024));
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  char * test_path = rcutils_join_path(cur_dir, "test", allocator);
  ASSERT_TRUE(NULL != test_path) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    allocator.deallocate(test_path, allocator.state);
  });
  char * path = rcutils_join_path(test_path, "wildcards.yaml", allocator);
  ASSERT_TRUE(NULL != path) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    allocator.deallocate(path, allocator.state);
  });
  EXPECT_TRUE(rcutils_exists(path));
  rcl_params_t * params_hdl = rcl_yaml_node_struct_init(allocator);
  ASSERT_TRUE(NULL != params_hdl) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rcl_yaml_node_struct_fini(params_hdl);
  });
  bool res = rcl_parse_yaml_file(path, params_hdl);
  EXPECT_TRUE(res) << rcutils_get_error_string().str;
}

TEST(test_file_parser, wildcards_node_slash) {
  rcutils_reset_error();
  EXPECT_TRUE(rcutils_get_cwd(cur_dir, 1024));
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  char * test_path = rcutils_join_path(cur_dir, "test", allocator);
  ASSERT_TRUE(NULL != test_path) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    allocator.deallocate(test_path, allocator.state);
  });
  char * path = rcutils_join_path(test_path, "wildcards_node_slash.yaml", allocator);
  ASSERT_TRUE(NULL != path) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    allocator.deallocate(path, allocator.state);
  });
  EXPECT_TRUE(rcutils_exists(path));
  rcl_params_t * params_hdl = rcl_yaml_node_struct_init(allocator);
  ASSERT_TRUE(NULL != params_hdl) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rcl_yaml_node_struct_fini(params_hdl);
  });
  bool res = rcl_parse_yaml_file(path, params_hdl);
  EXPECT_FALSE(res);
}

TEST(test_file_parser, wildcards_partial) {
  rcutils_reset_error();
  EXPECT_TRUE(rcutils_get_cwd(cur_dir, 1024));
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  char * test_path = rcutils_join_path(cur_dir, "test", allocator);
  ASSERT_TRUE(NULL != test_path) << rcutils_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    allocator.deallocate(test_path, allocator.state);
  });
  const std::vector<std::string> filenames = {
    "wildcards_partial_01.yaml",
    "wildcards_partial_02.yaml",
    "wildcards_partial_03.yaml",
    "wildcards_partial_04.yaml",
    "wildcards_partial_05.yaml",
    "wildcards_partial_06.yaml",
    "wildcards_partial_07.yaml",
    "wildcards_partial_08.yaml",
    "wildcards_partial_09.yaml",
    "wildcards_partial_10.yaml",
    "wildcards_partial_11.yaml",
    "wildcards_partial_12.yaml"
  };

  for (auto & filename : filenames) {
    char * path = rcutils_join_path(test_path, filename.c_str(), allocator);
    ASSERT_TRUE(NULL != path) << rcutils_get_error_string().str;
    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      allocator.deallocate(path, allocator.state);
    });
    EXPECT_TRUE(rcutils_exists(path));
    rcl_params_t * params_hdl = rcl_yaml_node_struct_init(allocator);
    ASSERT_TRUE(NULL != params_hdl) << rcutils_get_error_string().str;
    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      rcl_yaml_node_struct_fini(params_hdl);
    });
    bool res = rcl_parse_yaml_file(path, params_hdl);
    EXPECT_FALSE(res);
  }
}

int32_t main(int32_t argc, char ** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
