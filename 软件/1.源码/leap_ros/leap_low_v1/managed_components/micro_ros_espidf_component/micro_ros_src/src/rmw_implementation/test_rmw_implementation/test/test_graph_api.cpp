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

#include "osrf_testing_tools_cpp/scope_exit.hpp"

#include "rcutils/allocator.h"
#include "rcutils/strdup.h"

#include "rmw/error_handling.h"
#include "rmw/get_node_info_and_types.h"
#include "rmw/get_service_names_and_types.h"
#include "rmw/get_topic_endpoint_info.h"
#include "rmw/get_topic_names_and_types.h"
#include "rmw/rmw.h"
#include "rmw/sanity_checks.h"

#include "./config.hpp"
#include "./testing_macros.hpp"

#ifdef RMW_IMPLEMENTATION
# define CLASSNAME_(NAME, SUFFIX) NAME ## __ ## SUFFIX
# define CLASSNAME(NAME, SUFFIX) CLASSNAME_(NAME, SUFFIX)
#else
# define CLASSNAME(NAME, SUFFIX) NAME
#endif

class CLASSNAME (TestGraphAPI, RMW_IMPLEMENTATION) : public ::testing::Test
{
protected:
  void SetUp() override
  {
    rmw_init_options_t init_options = rmw_get_zero_initialized_init_options();
    rmw_ret_t ret = rmw_init_options_init(&init_options, rcutils_get_default_allocator());
    ASSERT_EQ(RMW_RET_OK, ret) << rcutils_get_error_string().str;
    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      rmw_ret_t ret = rmw_init_options_fini(&init_options);
      EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
    });
    init_options.enclave = rcutils_strdup("/", rcutils_get_default_allocator());
    ASSERT_STREQ("/", init_options.enclave);
    ret = rmw_init(&init_options, &context);
    ASSERT_EQ(RMW_RET_OK, ret) << rcutils_get_error_string().str;
    node = rmw_create_node(&context, node_name, node_namespace);
    ASSERT_NE(nullptr, node) << rcutils_get_error_string().str;
    other_node = rmw_create_node(&context, node_name, node_namespace);
    ASSERT_NE(nullptr, other_node) << rcutils_get_error_string().str;
  }

  void TearDown() override
  {
    rmw_ret_t ret = rmw_destroy_node(other_node);
    EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
    ret = rmw_destroy_node(node);
    EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
    ret = rmw_shutdown(&context);
    EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
    ret = rmw_context_fini(&context);
    EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  }

  rmw_context_t context{rmw_get_zero_initialized_context()};
  rmw_node_t * node{nullptr};
  const char * const node_name = "my_test_node";
  const char * const node_namespace = "/my_test_ns";
  rmw_node_t * other_node{nullptr};
  const char * const other_node_name = "my_other_test_node";
  const char * const other_node_namespace = "/my_other_test_ns";
};

TEST_F(CLASSNAME(TestGraphAPI, RMW_IMPLEMENTATION), get_node_names_with_bad_arguments) {
  rcutils_string_array_t node_names = rcutils_get_zero_initialized_string_array();
  rcutils_string_array_t node_namespaces = rcutils_get_zero_initialized_string_array();

  // A null node is an invalid argument.
  rmw_ret_t ret = rmw_get_node_names(nullptr, &node_names, &node_namespaces);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();
  EXPECT_EQ(RMW_RET_OK, rmw_check_zero_rmw_string_array(&node_names));
  EXPECT_EQ(RMW_RET_OK, rmw_check_zero_rmw_string_array(&node_namespaces));

  // A null array of node names is an invalid argument.
  ret = rmw_get_node_names(node, nullptr, &node_namespaces);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();
  EXPECT_EQ(RMW_RET_OK, rmw_check_zero_rmw_string_array(&node_names));
  EXPECT_EQ(RMW_RET_OK, rmw_check_zero_rmw_string_array(&node_namespaces));

  // A null array of node namespaces is an invalid argument.
  ret = rmw_get_node_names(node, &node_names, nullptr);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();
  EXPECT_EQ(RMW_RET_OK, rmw_check_zero_rmw_string_array(&node_names));
  EXPECT_EQ(RMW_RET_OK, rmw_check_zero_rmw_string_array(&node_namespaces));

  // A node from a different implementation cannot be used.
  const char * implementation_identifier = node->implementation_identifier;
  node->implementation_identifier = "not-an-rmw-implementation-identifier";
  ret = rmw_get_node_names(node, &node_names, &node_namespaces);
  EXPECT_EQ(RMW_RET_INCORRECT_RMW_IMPLEMENTATION, ret);
  node->implementation_identifier = implementation_identifier;
  rmw_reset_error();
  EXPECT_EQ(RMW_RET_OK, rmw_check_zero_rmw_string_array(&node_names));
  EXPECT_EQ(RMW_RET_OK, rmw_check_zero_rmw_string_array(&node_namespaces));

  // A non zero initialized array of node names is an invalid argument.
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  ret = rcutils_string_array_init(&node_names, 1u, &allocator);
  ASSERT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  ret = rmw_get_node_names(node, &node_names, &node_namespaces);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();
  EXPECT_EQ(RMW_RET_OK, rmw_check_zero_rmw_string_array(&node_namespaces));
  ret = rcutils_string_array_fini(&node_names);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;

  // A non zero initialized array of node namespaces is an invalid argument.
  ret = rcutils_string_array_init(&node_namespaces, 1u, &allocator);
  ASSERT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  ret = rmw_get_node_names(node, &node_names, &node_namespaces);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();
  EXPECT_EQ(RMW_RET_OK, rmw_check_zero_rmw_string_array(&node_names));
  ret = rcutils_string_array_fini(&node_namespaces);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
}

TEST_F(
  CLASSNAME(TestGraphAPI, RMW_IMPLEMENTATION),
  get_node_names_with_enclaves_with_bad_arguments) {
  rcutils_string_array_t node_names = rcutils_get_zero_initialized_string_array();
  rcutils_string_array_t node_namespaces = rcutils_get_zero_initialized_string_array();
  rcutils_string_array_t enclaves = rcutils_get_zero_initialized_string_array();

  // A null node is an invalid argument.
  rmw_ret_t ret =
    rmw_get_node_names_with_enclaves(nullptr, &node_names, &node_namespaces, &enclaves);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();
  EXPECT_EQ(RMW_RET_OK, rmw_check_zero_rmw_string_array(&node_names));
  EXPECT_EQ(RMW_RET_OK, rmw_check_zero_rmw_string_array(&node_namespaces));
  EXPECT_EQ(RMW_RET_OK, rmw_check_zero_rmw_string_array(&enclaves));

  // A null array of node names is an invalid argument.
  ret = rmw_get_node_names_with_enclaves(node, nullptr, &node_namespaces, &enclaves);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();
  EXPECT_EQ(RMW_RET_OK, rmw_check_zero_rmw_string_array(&node_namespaces));
  EXPECT_EQ(RMW_RET_OK, rmw_check_zero_rmw_string_array(&enclaves));

  // A null array of node namespaces is an invalid argument.
  ret = rmw_get_node_names_with_enclaves(node, &node_names, nullptr, &enclaves);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();
  EXPECT_EQ(RMW_RET_OK, rmw_check_zero_rmw_string_array(&node_names));
  EXPECT_EQ(RMW_RET_OK, rmw_check_zero_rmw_string_array(&enclaves));

  // A null array of enclaves is an invalid argument.
  ret = rmw_get_node_names_with_enclaves(node, &node_names, &node_namespaces, nullptr);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();
  EXPECT_EQ(RMW_RET_OK, rmw_check_zero_rmw_string_array(&node_names));
  EXPECT_EQ(RMW_RET_OK, rmw_check_zero_rmw_string_array(&node_namespaces));

  // A node from a different implementation cannot be used.
  const char * implementation_identifier = node->implementation_identifier;
  node->implementation_identifier = "not-an-rmw-implementation-identifier";
  ret = rmw_get_node_names_with_enclaves(node, &node_names, &node_namespaces, &enclaves);
  EXPECT_EQ(RMW_RET_INCORRECT_RMW_IMPLEMENTATION, ret);
  node->implementation_identifier = implementation_identifier;
  rmw_reset_error();
  EXPECT_EQ(RMW_RET_OK, rmw_check_zero_rmw_string_array(&node_names));
  EXPECT_EQ(RMW_RET_OK, rmw_check_zero_rmw_string_array(&node_namespaces));
  EXPECT_EQ(RMW_RET_OK, rmw_check_zero_rmw_string_array(&enclaves));

  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  // A non zero initialized array of node names is an invalid argument.
  ret = rcutils_string_array_init(&node_names, 1u, &allocator);
  ASSERT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  ret = rmw_get_node_names_with_enclaves(node, &node_names, &node_namespaces, &enclaves);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();
  EXPECT_EQ(RMW_RET_OK, rmw_check_zero_rmw_string_array(&node_namespaces));
  EXPECT_EQ(RMW_RET_OK, rmw_check_zero_rmw_string_array(&enclaves));
  ret = rcutils_string_array_fini(&node_names);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;

  // A non zero initialized array of node namespaces is an invalid argument.
  ret = rcutils_string_array_init(&node_namespaces, 1u, &allocator);
  ASSERT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  ret = rmw_get_node_names_with_enclaves(node, &node_names, &node_namespaces, &enclaves);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();
  EXPECT_EQ(RMW_RET_OK, rmw_check_zero_rmw_string_array(&node_names));
  EXPECT_EQ(RMW_RET_OK, rmw_check_zero_rmw_string_array(&enclaves));
  ret = rcutils_string_array_fini(&node_namespaces);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;

  // A non zero initialized array of enclaves is an invalid argument.
  ret = rcutils_string_array_init(&enclaves, 1u, &allocator);
  ASSERT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  ret = rmw_get_node_names_with_enclaves(node, &node_names, &node_namespaces, &enclaves);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();
  EXPECT_EQ(RMW_RET_OK, rmw_check_zero_rmw_string_array(&node_names));
  EXPECT_EQ(RMW_RET_OK, rmw_check_zero_rmw_string_array(&node_namespaces));
  ret = rcutils_string_array_fini(&enclaves);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
}

TEST_F(
  CLASSNAME(TestGraphAPI, RMW_IMPLEMENTATION),
  get_topic_names_and_types_with_bad_arguments) {
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  rmw_names_and_types_t topic_names_and_types = rmw_get_zero_initialized_names_and_types();
  bool no_demangle = false;

  // A null node is an invalid argument.
  rmw_ret_t ret =
    rmw_get_topic_names_and_types(nullptr, &allocator, no_demangle, &topic_names_and_types);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();
  EXPECT_EQ(RMW_RET_OK, rmw_names_and_types_check_zero(&topic_names_and_types));

  // A null allocator is an invalid argument.
  ret = rmw_get_topic_names_and_types(node, nullptr, no_demangle, &topic_names_and_types);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();
  EXPECT_EQ(RMW_RET_OK, rmw_names_and_types_check_zero(&topic_names_and_types));

  rcutils_allocator_t invalid_allocator = rcutils_get_zero_initialized_allocator();
  // An invalid (zero initialized) allocator is an invalid argument.
  ret = rmw_get_topic_names_and_types(
    node, &invalid_allocator, no_demangle, &topic_names_and_types);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();
  EXPECT_EQ(RMW_RET_OK, rmw_names_and_types_check_zero(&topic_names_and_types));

  // A non zero initialized array of topic names and types is an invalid argument.
  ret = rmw_names_and_types_init(&topic_names_and_types, 1u, &allocator);
  ASSERT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  ret = rmw_get_topic_names_and_types(node, &allocator, no_demangle, &topic_names_and_types);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();
  ret = rmw_names_and_types_fini(&topic_names_and_types);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;

  // A node from a different implementation cannot be used.
  const char * implementation_identifier = node->implementation_identifier;
  node->implementation_identifier = "not-an-rmw-implementation-identifier";
  ret = rmw_get_topic_names_and_types(node, &allocator, no_demangle, &topic_names_and_types);
  EXPECT_EQ(RMW_RET_INCORRECT_RMW_IMPLEMENTATION, ret);
  node->implementation_identifier = implementation_identifier;
  rmw_reset_error();
}

TEST_F(
  CLASSNAME(TestGraphAPI, RMW_IMPLEMENTATION),
  get_service_names_and_types_with_bad_arguments) {
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  rmw_names_and_types_t service_names_and_types = rmw_get_zero_initialized_names_and_types();

  // A null node is an invalid argument.
  rmw_ret_t ret =
    rmw_get_service_names_and_types(nullptr, &allocator, &service_names_and_types);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();
  EXPECT_EQ(RMW_RET_OK, rmw_names_and_types_check_zero(&service_names_and_types));

  // A null allocator is an invalid argument.
  ret = rmw_get_service_names_and_types(node, nullptr, &service_names_and_types);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();
  EXPECT_EQ(RMW_RET_OK, rmw_names_and_types_check_zero(&service_names_and_types));

  rcutils_allocator_t invalid_allocator = rcutils_get_zero_initialized_allocator();
  // An invalid (zero initialized) allocator is an invalid argument.
  ret = rmw_get_service_names_and_types(node, &invalid_allocator, &service_names_and_types);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();
  EXPECT_EQ(RMW_RET_OK, rmw_names_and_types_check_zero(&service_names_and_types));

  // A non zero initialized array of service names and types is an invalid argument.
  ASSERT_EQ(RMW_RET_OK, rmw_names_and_types_init(&service_names_and_types, 1u, &allocator)) <<
    rmw_get_error_string().str;
  ret = rmw_get_service_names_and_types(node, &allocator, &service_names_and_types);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();
  ret = rmw_names_and_types_fini(&service_names_and_types);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;

  // A node from a different implementation cannot be used.
  const char * implementation_identifier = node->implementation_identifier;
  node->implementation_identifier = "not-an-rmw-implementation-identifier";
  ret = rmw_get_service_names_and_types(node, &allocator, &service_names_and_types);
  EXPECT_EQ(RMW_RET_INCORRECT_RMW_IMPLEMENTATION, ret);
  node->implementation_identifier = implementation_identifier;
  rmw_reset_error();
}

TEST_F(
  CLASSNAME(TestGraphAPI, RMW_IMPLEMENTATION),
  get_subscriber_names_and_types_by_node_with_bad_arguments) {
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  rmw_names_and_types_t topic_names_and_types = rmw_get_zero_initialized_names_and_types();
  bool no_demangle = false;

  // A null node is an invalid argument.
  rmw_ret_t ret = rmw_get_subscriber_names_and_types_by_node(
    nullptr, &allocator, other_node_name, other_node_namespace,
    no_demangle, &topic_names_and_types);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();
  EXPECT_EQ(RMW_RET_OK, rmw_names_and_types_check_zero(&topic_names_and_types));

  // A node from a different implementation cannot be used to query.
  const char * implementation_identifier = node->implementation_identifier;
  node->implementation_identifier = "not-an-rmw-implementation-identifier";
  ret = rmw_get_subscriber_names_and_types_by_node(
    node, &allocator, other_node_name, other_node_namespace,
    no_demangle, &topic_names_and_types);
  EXPECT_EQ(RMW_RET_INCORRECT_RMW_IMPLEMENTATION, ret);
  node->implementation_identifier = implementation_identifier;
  rmw_reset_error();
  EXPECT_EQ(RMW_RET_OK, rmw_names_and_types_check_zero(&topic_names_and_types));

  // A null allocator is an invalid argument.
  ret = rmw_get_subscriber_names_and_types_by_node(
    node, nullptr, other_node_name, other_node_namespace,
    no_demangle, &topic_names_and_types);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();
  EXPECT_EQ(RMW_RET_OK, rmw_names_and_types_check_zero(&topic_names_and_types));

  // An invalid (zero initialized) allocator is an invalid argument.
  rcutils_allocator_t invalid_allocator = rcutils_get_zero_initialized_allocator();
  ret = rmw_get_subscriber_names_and_types_by_node(
    node, &invalid_allocator, other_node_name, other_node_namespace,
    no_demangle, &topic_names_and_types);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();
  EXPECT_EQ(RMW_RET_OK, rmw_names_and_types_check_zero(&topic_names_and_types));

  // A null node name is an invalid argument.
  ret = rmw_get_subscriber_names_and_types_by_node(
    node, &allocator, nullptr, other_node_namespace,
    no_demangle, &topic_names_and_types);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();
  EXPECT_EQ(RMW_RET_OK, rmw_names_and_types_check_zero(&topic_names_and_types));

  // An invalid node name is an invalid argument.
  constexpr char invalid_node_name[] = "not a not valid node name !";
  ret = rmw_get_subscriber_names_and_types_by_node(
    node, &allocator, invalid_node_name, other_node_namespace,
    no_demangle, &topic_names_and_types);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();
  EXPECT_EQ(RMW_RET_OK, rmw_names_and_types_check_zero(&topic_names_and_types));

  // A null node name is an invalid argument.
  ret = rmw_get_subscriber_names_and_types_by_node(
    node, &allocator, other_node_name, nullptr,
    no_demangle, &topic_names_and_types);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();
  EXPECT_EQ(RMW_RET_OK, rmw_names_and_types_check_zero(&topic_names_and_types));

  // An invalid node namespace is an invalid argument.
  constexpr char invalid_node_namespace[] = "not a not valid node namespace !";
  ret = rmw_get_subscriber_names_and_types_by_node(
    node, &allocator, other_node_name, invalid_node_namespace,
    no_demangle, &topic_names_and_types);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();
  EXPECT_EQ(RMW_RET_OK, rmw_names_and_types_check_zero(&topic_names_and_types));

  // A null array of topic names and types is an invalid argument.
  ret = rmw_get_subscriber_names_and_types_by_node(
    node, &allocator, other_node_name, other_node_namespace,
    no_demangle, nullptr);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();

  // A non zero initialized array of topic names and types is an invalid argument.
  ret = rmw_names_and_types_init(&topic_names_and_types, 1u, &allocator);
  ASSERT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  ret = rmw_get_subscriber_names_and_types_by_node(
    node, &allocator, other_node_name, other_node_namespace,
    no_demangle, &topic_names_and_types);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();
  ret = rmw_names_and_types_fini(&topic_names_and_types);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;

  // A non existent node cannot be queried.
  constexpr char nonexistent_node_name[] = "a_test_node_that_does_not_exist";
  ret = rmw_get_subscriber_names_and_types_by_node(
    node, &allocator, nonexistent_node_name, other_node_namespace,
    no_demangle, &topic_names_and_types);
  EXPECT_EQ(RMW_RET_NODE_NAME_NON_EXISTENT, ret);
  rmw_reset_error();
  EXPECT_EQ(RMW_RET_OK, rmw_names_and_types_check_zero(&topic_names_and_types));
}

TEST_F(
  CLASSNAME(TestGraphAPI, RMW_IMPLEMENTATION),
  get_publisher_names_and_types_by_node_with_bad_arguments) {
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  rmw_names_and_types_t topic_names_and_types = rmw_get_zero_initialized_names_and_types();
  bool no_demangle = false;

  // A null node is an invalid argument.
  rmw_ret_t ret = rmw_get_publisher_names_and_types_by_node(
    nullptr, &allocator, other_node_name, other_node_namespace,
    no_demangle, &topic_names_and_types);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();
  EXPECT_EQ(RMW_RET_OK, rmw_names_and_types_check_zero(&topic_names_and_types));

  // A node from a different implementation cannot be used to query.
  const char * implementation_identifier = node->implementation_identifier;
  node->implementation_identifier = "not-an-rmw-implementation-identifier";
  ret = rmw_get_publisher_names_and_types_by_node(
    node, &allocator, other_node_name, other_node_namespace,
    no_demangle, &topic_names_and_types);
  EXPECT_EQ(RMW_RET_INCORRECT_RMW_IMPLEMENTATION, ret);
  node->implementation_identifier = implementation_identifier;
  rmw_reset_error();
  EXPECT_EQ(RMW_RET_OK, rmw_names_and_types_check_zero(&topic_names_and_types));

  // A null allocator is an invalid argument.
  ret = rmw_get_publisher_names_and_types_by_node(
    node, nullptr, other_node_name, other_node_namespace,
    no_demangle, &topic_names_and_types);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();
  EXPECT_EQ(RMW_RET_OK, rmw_names_and_types_check_zero(&topic_names_and_types));

  // An invalid (zero initialized) allocator is an invalid argument.
  rcutils_allocator_t invalid_allocator = rcutils_get_zero_initialized_allocator();
  ret = rmw_get_publisher_names_and_types_by_node(
    node, &invalid_allocator, other_node_name, other_node_namespace,
    no_demangle, &topic_names_and_types);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();
  EXPECT_EQ(RMW_RET_OK, rmw_names_and_types_check_zero(&topic_names_and_types));

  // A null node name is an invalid argument.
  ret = rmw_get_publisher_names_and_types_by_node(
    node, &allocator, nullptr, other_node_namespace,
    no_demangle, &topic_names_and_types);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();
  EXPECT_EQ(RMW_RET_OK, rmw_names_and_types_check_zero(&topic_names_and_types));

  // An invalid node name is an invalid argument.
  constexpr char invalid_node_name[] = "not a not valid node name !";
  ret = rmw_get_publisher_names_and_types_by_node(
    node, &allocator, invalid_node_name, other_node_namespace,
    no_demangle, &topic_names_and_types);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();
  EXPECT_EQ(RMW_RET_OK, rmw_names_and_types_check_zero(&topic_names_and_types));

  // A null node namespace is an invalid argument.
  ret = rmw_get_publisher_names_and_types_by_node(
    node, &allocator, other_node_name, nullptr,
    no_demangle, &topic_names_and_types);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();
  EXPECT_EQ(RMW_RET_OK, rmw_names_and_types_check_zero(&topic_names_and_types));

  // An invalid node namespace is an invalid argument.
  constexpr char invalid_node_namespace[] = "not a not valid node namespace !";
  ret = rmw_get_publisher_names_and_types_by_node(
    node, &allocator, other_node_name, invalid_node_namespace,
    no_demangle, &topic_names_and_types);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();
  EXPECT_EQ(RMW_RET_OK, rmw_names_and_types_check_zero(&topic_names_and_types));

  // A null array of topic names and types is an invalid argument.
  ret = rmw_get_publisher_names_and_types_by_node(
    node, &allocator, other_node_name, other_node_namespace,
    no_demangle, nullptr);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();

  // A non zero initialized array of topic names and types is an invalid argument.
  ret = rmw_names_and_types_init(&topic_names_and_types, 1u, &allocator);
  ASSERT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  ret = rmw_get_publisher_names_and_types_by_node(
    node, &allocator, other_node_name, other_node_namespace,
    no_demangle, &topic_names_and_types);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();
  ret = rmw_names_and_types_fini(&topic_names_and_types);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;

  // A non existent node cannot be queried.
  constexpr char nonexistent_node_name[] = "a_test_node_that_does_not_exist";
  ret = rmw_get_publisher_names_and_types_by_node(
    node, &allocator, nonexistent_node_name, other_node_namespace,
    no_demangle, &topic_names_and_types);
  EXPECT_EQ(RMW_RET_NODE_NAME_NON_EXISTENT, ret);
  rmw_reset_error();
  EXPECT_EQ(RMW_RET_OK, rmw_names_and_types_check_zero(&topic_names_and_types));
}

TEST_F(
  CLASSNAME(TestGraphAPI, RMW_IMPLEMENTATION),
  get_service_names_and_types_by_node_with_bad_arguments) {
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  rmw_names_and_types_t service_names_and_types = rmw_get_zero_initialized_names_and_types();

  // A null node is an invalid argument.
  rmw_ret_t ret = rmw_get_service_names_and_types_by_node(
    nullptr, &allocator, other_node_name, other_node_namespace,
    &service_names_and_types);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();
  EXPECT_EQ(RMW_RET_OK, rmw_names_and_types_check_zero(&service_names_and_types));

  // A node from a different implementation cannot be used to query.
  const char * implementation_identifier = node->implementation_identifier;
  node->implementation_identifier = "not-an-rmw-implementation-identifier";
  ret = rmw_get_service_names_and_types_by_node(
    node, &allocator, other_node_name, other_node_namespace,
    &service_names_and_types);
  EXPECT_EQ(RMW_RET_INCORRECT_RMW_IMPLEMENTATION, ret);
  node->implementation_identifier = implementation_identifier;
  rmw_reset_error();
  EXPECT_EQ(RMW_RET_OK, rmw_names_and_types_check_zero(&service_names_and_types));

  // A null allocator is an invalid argument.
  ret = rmw_get_service_names_and_types_by_node(
    node, nullptr, other_node_name, other_node_namespace,
    &service_names_and_types);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();
  EXPECT_EQ(RMW_RET_OK, rmw_names_and_types_check_zero(&service_names_and_types));

  // An invalid (zero initialized) allocator is an invalid argument.
  rcutils_allocator_t invalid_allocator = rcutils_get_zero_initialized_allocator();
  ret = rmw_get_service_names_and_types_by_node(
    node, &invalid_allocator, other_node_name, other_node_namespace,
    &service_names_and_types);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();
  EXPECT_EQ(RMW_RET_OK, rmw_names_and_types_check_zero(&service_names_and_types));

  // A null node name is an invalid argument.
  ret = rmw_get_service_names_and_types_by_node(
    node, &allocator, nullptr, other_node_namespace,
    &service_names_and_types);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();
  EXPECT_EQ(RMW_RET_OK, rmw_names_and_types_check_zero(&service_names_and_types));

  // An invalid node name is an invalid argument.
  constexpr char invalid_node_name[] = "not a not valid node name !";
  ret = rmw_get_service_names_and_types_by_node(
    node, &allocator, invalid_node_name, other_node_namespace,
    &service_names_and_types);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();
  EXPECT_EQ(RMW_RET_OK, rmw_names_and_types_check_zero(&service_names_and_types));

  // A null node namespace is an invalid argument.
  ret = rmw_get_service_names_and_types_by_node(
    node, &allocator, other_node_name, nullptr,
    &service_names_and_types);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();
  EXPECT_EQ(RMW_RET_OK, rmw_names_and_types_check_zero(&service_names_and_types));

  // An invalid node namespace is an invalid argument.
  constexpr char invalid_node_namespace[] = "not a not valid node namespace !";
  ret = rmw_get_service_names_and_types_by_node(
    node, &allocator, other_node_name, invalid_node_namespace,
    &service_names_and_types);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();
  EXPECT_EQ(RMW_RET_OK, rmw_names_and_types_check_zero(&service_names_and_types));

  // A null array of service names and types is an invalid argument.
  ret = rmw_get_service_names_and_types_by_node(
    node, &allocator, other_node_name, other_node_namespace, nullptr);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();

  // A non zero initialized array of service names and types is an invalid argument.
  ret = rmw_names_and_types_init(&service_names_and_types, 1u, &allocator);
  ASSERT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  ret = rmw_get_service_names_and_types_by_node(
    node, &allocator, other_node_name, other_node_namespace,
    &service_names_and_types);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();
  ret = rmw_names_and_types_fini(&service_names_and_types);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;

  // A non existent node cannot be queried.
  constexpr char nonexistent_node_name[] = "a_test_node_that_does_not_exist";
  ret = rmw_get_service_names_and_types_by_node(
    node, &allocator, nonexistent_node_name, other_node_namespace,
    &service_names_and_types);
  EXPECT_EQ(RMW_RET_NODE_NAME_NON_EXISTENT, ret);
  rmw_reset_error();
  EXPECT_EQ(RMW_RET_OK, rmw_names_and_types_check_zero(&service_names_and_types));
}

TEST_F(
  CLASSNAME(TestGraphAPI, RMW_IMPLEMENTATION),
  get_client_names_and_types_by_node_with_bad_arguments) {
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  rmw_names_and_types_t service_names_and_types = rmw_get_zero_initialized_names_and_types();

  // A null node is an invalid argument.
  rmw_ret_t ret = rmw_get_client_names_and_types_by_node(
    nullptr, &allocator, other_node_name, other_node_namespace,
    &service_names_and_types);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();
  EXPECT_EQ(RMW_RET_OK, rmw_names_and_types_check_zero(&service_names_and_types));

  // A node from a different implementation cannot be used to query.
  const char * implementation_identifier = node->implementation_identifier;
  node->implementation_identifier = "not-an-rmw-implementation-identifier";
  ret = rmw_get_client_names_and_types_by_node(
    node, &allocator, other_node_name, other_node_namespace,
    &service_names_and_types);
  EXPECT_EQ(RMW_RET_INCORRECT_RMW_IMPLEMENTATION, ret);
  node->implementation_identifier = implementation_identifier;
  rmw_reset_error();
  EXPECT_EQ(RMW_RET_OK, rmw_names_and_types_check_zero(&service_names_and_types));

  // A null allocator is an invalid argument.
  ret = rmw_get_client_names_and_types_by_node(
    node, nullptr, other_node_name, other_node_namespace,
    &service_names_and_types);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();
  EXPECT_EQ(RMW_RET_OK, rmw_names_and_types_check_zero(&service_names_and_types));

  // An invalid (zero initialized) allocator is an invalid argument.
  rcutils_allocator_t invalid_allocator = rcutils_get_zero_initialized_allocator();
  ret = rmw_get_client_names_and_types_by_node(
    node, &invalid_allocator, other_node_name, other_node_namespace,
    &service_names_and_types);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();
  EXPECT_EQ(RMW_RET_OK, rmw_names_and_types_check_zero(&service_names_and_types));

  // A null node name is an invalid argument.
  ret = rmw_get_client_names_and_types_by_node(
    node, &allocator, nullptr, other_node_namespace,
    &service_names_and_types);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();
  EXPECT_EQ(RMW_RET_OK, rmw_names_and_types_check_zero(&service_names_and_types));

  // An invalid node name is an invalid argument.
  constexpr char invalid_node_name[] = "not a not valid node name !";
  ret = rmw_get_client_names_and_types_by_node(
    node, &allocator, invalid_node_name, other_node_namespace,
    &service_names_and_types);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();
  EXPECT_EQ(RMW_RET_OK, rmw_names_and_types_check_zero(&service_names_and_types));

  // A null node namespace is an invalid argument.
  ret = rmw_get_client_names_and_types_by_node(
    node, &allocator, other_node_name, nullptr,
    &service_names_and_types);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();
  EXPECT_EQ(RMW_RET_OK, rmw_names_and_types_check_zero(&service_names_and_types));

  // An invalid node namespace is an invalid argument.
  constexpr char invalid_node_namespace[] = "not a not valid node namespace !";
  ret = rmw_get_client_names_and_types_by_node(
    node, &allocator, other_node_name, invalid_node_namespace,
    &service_names_and_types);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();
  EXPECT_EQ(RMW_RET_OK, rmw_names_and_types_check_zero(&service_names_and_types));

  // A null array of service names and types is an invalid argument.
  ret = rmw_get_client_names_and_types_by_node(
    node, &allocator, other_node_name, other_node_namespace, nullptr);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();

  // A non zero initialized array of service names and types is an invalid argument.
  ret = rmw_names_and_types_init(&service_names_and_types, 1u, &allocator);
  ASSERT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  ret = rmw_get_client_names_and_types_by_node(
    node, &allocator, other_node_name, other_node_namespace,
    &service_names_and_types);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();
  ret = rmw_names_and_types_fini(&service_names_and_types);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;

  // A non existent node cannot be queried.
  constexpr char nonexistent_node_name[] = "a_test_node_that_does_not_exist";
  ret = rmw_get_client_names_and_types_by_node(
    node, &allocator, nonexistent_node_name, other_node_namespace,
    &service_names_and_types);
  EXPECT_EQ(RMW_RET_NODE_NAME_NON_EXISTENT, ret);
  rmw_reset_error();
  EXPECT_EQ(RMW_RET_OK, rmw_names_and_types_check_zero(&service_names_and_types));
}

TEST_F(
  CLASSNAME(TestGraphAPI, RMW_IMPLEMENTATION),
  get_publishers_info_by_topic_with_bad_arguments) {
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  constexpr char topic_name[] = "/test_topic";
  bool no_mangle = false;
  rmw_topic_endpoint_info_array_t publishers_info =
    rmw_get_zero_initialized_topic_endpoint_info_array();

  // A null node is an invalid argument.
  rmw_ret_t ret = rmw_get_publishers_info_by_topic(
    nullptr, &allocator, topic_name, no_mangle, &publishers_info);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();
  EXPECT_EQ(RMW_RET_OK, rmw_topic_endpoint_info_array_check_zero(&publishers_info));

  // A node from a different implementation is an invalid argument.
  const char * implementation_identifier = node->implementation_identifier;
  node->implementation_identifier = "not-an-rmw-implementation-identifier";
  ret = rmw_get_publishers_info_by_topic(
    node, &allocator, topic_name, no_mangle, &publishers_info);
  EXPECT_EQ(RMW_RET_INCORRECT_RMW_IMPLEMENTATION, ret);
  node->implementation_identifier = implementation_identifier;
  rmw_reset_error();
  EXPECT_EQ(RMW_RET_OK, rmw_topic_endpoint_info_array_check_zero(&publishers_info));

  // A null allocator is an invalid argument.
  ret = rmw_get_publishers_info_by_topic(node, nullptr, topic_name, no_mangle, &publishers_info);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();
  EXPECT_EQ(RMW_RET_OK, rmw_topic_endpoint_info_array_check_zero(&publishers_info));

  // An invalid (zero initialized) allocator is an invalid argument.
  rcutils_allocator_t invalid_allocator = rcutils_get_zero_initialized_allocator();
  ret = rmw_get_publishers_info_by_topic(
    node, &invalid_allocator, topic_name, no_mangle, &publishers_info);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();
  EXPECT_EQ(RMW_RET_OK, rmw_topic_endpoint_info_array_check_zero(&publishers_info));

  // A null topic name is an invalid argument.
  ret = rmw_get_publishers_info_by_topic(
    node, &allocator, nullptr, no_mangle, &publishers_info);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();
  EXPECT_EQ(RMW_RET_OK, rmw_topic_endpoint_info_array_check_zero(&publishers_info));

  // A null array of topic endpoint info is an invalid argument.
  ret = rmw_get_publishers_info_by_topic(node, &allocator, topic_name, no_mangle, nullptr);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();
  EXPECT_EQ(RMW_RET_OK, rmw_topic_endpoint_info_array_check_zero(&publishers_info));

  // A non zero initialized array of topic endpoint info is an invalid argument.
  ret = rmw_topic_endpoint_info_array_init_with_size(&publishers_info, 1u, &allocator);
  ASSERT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  ret = rmw_get_publishers_info_by_topic(
    node, &allocator, topic_name, no_mangle, &publishers_info);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();
  ret = rmw_topic_endpoint_info_array_fini(&publishers_info, &allocator);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
}

TEST_F(
  CLASSNAME(TestGraphAPI, RMW_IMPLEMENTATION),
  get_subscriptions_info_by_topic_with_bad_arguments) {
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  constexpr char topic_name[] = "/test_topic";
  bool no_mangle = false;
  rmw_topic_endpoint_info_array_t subscriptions_info =
    rmw_get_zero_initialized_topic_endpoint_info_array();

  // A null node is an invalid argument.
  rmw_ret_t ret = rmw_get_subscriptions_info_by_topic(
    nullptr, &allocator, topic_name, no_mangle, &subscriptions_info);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();
  EXPECT_EQ(RMW_RET_OK, rmw_topic_endpoint_info_array_check_zero(&subscriptions_info));

  // A node from a different implementation is an invalid argument.
  const char * implementation_identifier = node->implementation_identifier;
  node->implementation_identifier = "not-an-rmw-implementation-identifier";
  ret = rmw_get_subscriptions_info_by_topic(
    node, &allocator, topic_name, no_mangle, &subscriptions_info);
  EXPECT_EQ(RMW_RET_INCORRECT_RMW_IMPLEMENTATION, ret);
  node->implementation_identifier = implementation_identifier;
  rmw_reset_error();
  EXPECT_EQ(RMW_RET_OK, rmw_topic_endpoint_info_array_check_zero(&subscriptions_info));

  // A null allocator is an invalid argument.
  ret = rmw_get_subscriptions_info_by_topic(
    node, nullptr, topic_name, no_mangle, &subscriptions_info);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();
  EXPECT_EQ(RMW_RET_OK, rmw_topic_endpoint_info_array_check_zero(&subscriptions_info));

  // An invalid (zero initialized) allocator is an invalid argument.
  rcutils_allocator_t invalid_allocator = rcutils_get_zero_initialized_allocator();
  ret = rmw_get_subscriptions_info_by_topic(
    node, &invalid_allocator, topic_name, no_mangle, &subscriptions_info);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();
  EXPECT_EQ(RMW_RET_OK, rmw_topic_endpoint_info_array_check_zero(&subscriptions_info));

  // A null topic name is an invalid argument.
  ret = rmw_get_subscriptions_info_by_topic(
    node, &allocator, nullptr, no_mangle, &subscriptions_info);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();
  EXPECT_EQ(RMW_RET_OK, rmw_topic_endpoint_info_array_check_zero(&subscriptions_info));

  // A null array of topic endpoint info is an invalid argument.
  ret = rmw_get_subscriptions_info_by_topic(node, &allocator, topic_name, no_mangle, nullptr);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();
  EXPECT_EQ(RMW_RET_OK, rmw_topic_endpoint_info_array_check_zero(&subscriptions_info));

  // A non zero initialized array of topic endpoint info is an invalid argument.
  ret = rmw_topic_endpoint_info_array_init_with_size(&subscriptions_info, 1u, &allocator);
  ASSERT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  ret = rmw_get_subscriptions_info_by_topic(
    node, &allocator, topic_name, no_mangle, &subscriptions_info);
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();
  ret = rmw_topic_endpoint_info_array_fini(&subscriptions_info, &allocator);
  EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
}

TEST_F(CLASSNAME(TestGraphAPI, RMW_IMPLEMENTATION), count_publishers_with_bad_arguments) {
  size_t count = 0u;
  constexpr char topic_name[] = "/test_topic";

  // A null node is an invalid argument.
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, rmw_count_publishers(nullptr, topic_name, &count));
  rmw_reset_error();

  // A node from a different implementation cannot be used to query.
  const char * implementation_identifier = node->implementation_identifier;
  node->implementation_identifier = "not-an-rmw-implementation-identifier";
  EXPECT_EQ(RMW_RET_INCORRECT_RMW_IMPLEMENTATION, rmw_count_publishers(node, topic_name, &count));
  node->implementation_identifier = implementation_identifier;
  rmw_reset_error();

  // A null topic name is an invalid argument.
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, rmw_count_publishers(node, nullptr, &count));
  rmw_reset_error();

  // An invalid topic name is an invalid argument.
  constexpr char invalid_topic_name[] = "not a valid topic name !";
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, rmw_count_publishers(node, invalid_topic_name, &count));
  rmw_reset_error();

  // A null count is an invalid argument.
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, rmw_count_publishers(node, topic_name, nullptr));
  rmw_reset_error();
}

TEST_F(CLASSNAME(TestGraphAPI, RMW_IMPLEMENTATION), count_subscribers_with_bad_arguments) {
  size_t count = 0u;
  constexpr char topic_name[] = "/test_topic";

  // A null node is an invalid argument.
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, rmw_count_subscribers(nullptr, topic_name, &count));
  rmw_reset_error();

  // A node from a different implementation cannot be used to query.
  const char * implementation_identifier = node->implementation_identifier;
  node->implementation_identifier = "not-an-rmw-implementation-identifier";
  EXPECT_EQ(RMW_RET_INCORRECT_RMW_IMPLEMENTATION, rmw_count_subscribers(node, topic_name, &count));
  node->implementation_identifier = implementation_identifier;
  rmw_reset_error();

  // A null topic name is an invalid argument.
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, rmw_count_subscribers(node, nullptr, &count));
  rmw_reset_error();

  // An invalid topic name is an invalid argument.
  constexpr char invalid_topic_name[] = "not a valid topic name !";
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, rmw_count_subscribers(node, invalid_topic_name, &count));
  rmw_reset_error();

  // A null count is an invalid argument.
  EXPECT_EQ(RMW_RET_INVALID_ARGUMENT, rmw_count_subscribers(node, topic_name, nullptr));
  rmw_reset_error();
}
