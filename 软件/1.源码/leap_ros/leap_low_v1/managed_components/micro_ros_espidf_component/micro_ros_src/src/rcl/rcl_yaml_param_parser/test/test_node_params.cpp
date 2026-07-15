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

#include <gtest/gtest.h>

#include <yaml.h>

#include <string>
#include <vector>

#include "osrf_testing_tools_cpp/scope_exit.hpp"
#include "rcl_yaml_param_parser/parser.h"
#include "../src/impl/node_params.h"
#include "rcutils/allocator.h"

TEST(TestNodeParams, init_fini) {
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  rcl_node_params_t node_params = {NULL, NULL, 0u, 128u};
  EXPECT_EQ(RCUTILS_RET_OK, node_params_init(&node_params, allocator));
  EXPECT_NE(nullptr, node_params.parameter_names);
  EXPECT_NE(nullptr, node_params.parameter_values);
  EXPECT_EQ(0u, node_params.num_params);
  EXPECT_EQ(128u, node_params.capacity_params);
  rcl_yaml_node_params_fini(&node_params, allocator);
  EXPECT_EQ(nullptr, node_params.parameter_names);
  EXPECT_EQ(nullptr, node_params.parameter_values);
  EXPECT_EQ(0u, node_params.num_params);
  EXPECT_EQ(0u, node_params.capacity_params);

  // This function doesn't return anything, so just check it doesn't segfault on the second try
  rcl_yaml_node_params_fini(&node_params, allocator);
  rcl_yaml_node_params_fini(nullptr, allocator);
}

TEST(TestNodeParams, init_with_capacity_fini) {
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  rcl_node_params_t node_params = {NULL, NULL, 0u, 1024u};
  EXPECT_EQ(RCUTILS_RET_OK, node_params_init_with_capacity(&node_params, 1024, allocator));
  EXPECT_NE(nullptr, node_params.parameter_names);
  EXPECT_NE(nullptr, node_params.parameter_values);
  EXPECT_EQ(0u, node_params.num_params);
  EXPECT_EQ(1024u, node_params.capacity_params);
  rcl_yaml_node_params_fini(&node_params, allocator);
  EXPECT_EQ(nullptr, node_params.parameter_names);
  EXPECT_EQ(nullptr, node_params.parameter_values);
  EXPECT_EQ(0u, node_params.num_params);
  EXPECT_EQ(0u, node_params.capacity_params);

  // This function doesn't return anything, so just check it doesn't segfault on the second try
  rcl_yaml_node_params_fini(&node_params, allocator);
  rcl_yaml_node_params_fini(nullptr, allocator);
}

TEST(TestNodeParams, reallocate_with_capacity_fini) {
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  rcl_node_params_t node_params = {NULL, NULL, 0u, 1024u};
  EXPECT_EQ(RCUTILS_RET_OK, node_params_init_with_capacity(&node_params, 1024, allocator));
  EXPECT_NE(nullptr, node_params.parameter_names);
  EXPECT_NE(nullptr, node_params.parameter_values);
  EXPECT_EQ(0u, node_params.num_params);
  EXPECT_EQ(1024u, node_params.capacity_params);
  EXPECT_EQ(RCUTILS_RET_OK, node_params_reallocate(&node_params, 2048, allocator));
  EXPECT_NE(nullptr, node_params.parameter_names);
  EXPECT_NE(nullptr, node_params.parameter_values);
  EXPECT_EQ(0u, node_params.num_params);
  EXPECT_EQ(2048u, node_params.capacity_params);
  rcl_yaml_node_params_fini(&node_params, allocator);
  EXPECT_EQ(nullptr, node_params.parameter_names);
  EXPECT_EQ(nullptr, node_params.parameter_values);
  EXPECT_EQ(0u, node_params.num_params);
  EXPECT_EQ(0u, node_params.capacity_params);

  // This function doesn't return anything, so just check it doesn't segfault on the second try
  rcl_yaml_node_params_fini(&node_params, allocator);
  rcl_yaml_node_params_fini(nullptr, allocator);
}
