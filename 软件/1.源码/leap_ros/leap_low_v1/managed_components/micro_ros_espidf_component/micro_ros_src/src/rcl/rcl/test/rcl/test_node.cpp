// Copyright 2015 Open Source Robotics Foundation, Inc.
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

#include <regex>
#include <string>
#include <cstdlib>

#include "rcl/rcl.h"
#include "rcl/node.h"
#include "rmw/rmw.h"  // For rmw_get_implementation_identifier.
#include "rmw/validate_namespace.h"
#include "rmw/validate_node_name.h"

#include "./failing_allocator_functions.hpp"
#include "osrf_testing_tools_cpp/memory_tools/memory_tools.hpp"
#include "osrf_testing_tools_cpp/scope_exit.hpp"
#include "rcutils/env.h"
#include "rcutils/testing/fault_injection.h"
#include "rcl/error_handling.h"
#include "rcl/logging.h"
#include "rcl/logging_rosout.h"

#include "../mocking_utils/patch.hpp"
#include "./arg_macros.hpp"

#ifdef RMW_IMPLEMENTATION
# define CLASSNAME_(NAME, SUFFIX) NAME ## __ ## SUFFIX
# define CLASSNAME(NAME, SUFFIX) CLASSNAME_(NAME, SUFFIX)
#else
# define CLASSNAME(NAME, SUFFIX) NAME
#endif

using osrf_testing_tools_cpp::memory_tools::on_unexpected_malloc;
using osrf_testing_tools_cpp::memory_tools::on_unexpected_realloc;
using osrf_testing_tools_cpp::memory_tools::on_unexpected_calloc;
using osrf_testing_tools_cpp::memory_tools::on_unexpected_free;

bool operator==(
  const rmw_time_t & lhs,
  const rmw_time_t & rhs)
{
  return lhs.sec == rhs.sec && lhs.nsec == rhs.nsec;
}

bool operator==(
  const rmw_qos_profile_t & lhs,
  const rmw_qos_profile_t & rhs)
{
  return lhs.history == rhs.history &&
         lhs.depth == rhs.depth &&
         lhs.reliability == rhs.reliability &&
         lhs.durability == rhs.durability &&
         lhs.deadline == rhs.deadline &&
         lhs.lifespan == rhs.lifespan &&
         lhs.liveliness == rhs.liveliness &&
         lhs.liveliness_lease_duration == rhs.liveliness_lease_duration &&
         lhs.avoid_ros_namespace_conventions == rhs.avoid_ros_namespace_conventions;
}

class CLASSNAME (TestNodeFixture, RMW_IMPLEMENTATION) : public ::testing::Test
{
public:
  void SetUp()
  {
    auto common =
      [](auto service, const char * name) {
        // only fail if call originated in our library, librcl.<something>
        std::regex pattern("/?librcl\\.");
        auto st = service.get_stack_trace();  // nullptr if stack trace not available
        if (st && st->matches_any_object_filename(pattern)) {
          // Implicitly this means if one of the rmw implementations uses threads
          // and does memory allocations in them, but the calls didn't originate
          // from an rcl call, we will ignore it.
          // The goal here is ensure that no rcl function or thread is using memory.
          // Separate tests will be needed to ensure the rmw implementation does
          // not allocate memory or cause it to be allocated.
          service.print_backtrace();
          ADD_FAILURE() << "Unexpected call to " << name << " originating from within librcl.";
        }
      };
    osrf_testing_tools_cpp::memory_tools::initialize();
    on_unexpected_malloc([common](auto service) {common(service, "malloc");});
    on_unexpected_realloc([common](auto service) {common(service, "realloc");});
    on_unexpected_calloc([common](auto service) {common(service, "calloc");});
    on_unexpected_free([common](auto service) {common(service, "free");});
  }

  void TearDown()
  {
    osrf_testing_tools_cpp::memory_tools::uninitialize();
  }
};

/* Tests the node accessors, i.e. rcl_node_get_* functions.
 */
TEST_F(CLASSNAME(TestNodeFixture, RMW_IMPLEMENTATION), test_rcl_node_accessors) {
  osrf_testing_tools_cpp::memory_tools::enable_monitoring_in_all_threads();
  rcl_ret_t ret;
  // Initialize rcl with rcl_init().
  rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();
  ret = rcl_init_options_init(&init_options, rcl_get_default_allocator());
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_init_options_fini(&init_options)) << rcl_get_error_string().str;
  });
  EXPECT_EQ(RCL_RET_OK, rcl_init_options_set_domain_id(&init_options, 42));
  rcl_context_t invalid_context = rcl_get_zero_initialized_context();
  ret = rcl_init(0, nullptr, &init_options, &invalid_context);
  ASSERT_EQ(RCL_RET_OK, ret);  // Shutdown later after invalid node.
  // Create an invalid node (rcl_shutdown).
  rcl_node_t invalid_node = rcl_get_zero_initialized_node();
  const char * name = "test_rcl_node_accessors_node";
  const char * namespace_ = "/ns";
  const char * fq_name = "/ns/test_rcl_node_accessors_node";
  rcl_node_options_t default_options = rcl_node_get_default_options();
  ret = rcl_node_init(&invalid_node, name, namespace_, &invalid_context, &default_options);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    osrf_testing_tools_cpp::memory_tools::disable_monitoring_in_all_threads();
    rcl_ret_t ret = rcl_node_fini(&invalid_node);
    EXPECT_EQ(RCL_RET_OK, rcl_context_fini(&invalid_context)) << rcl_get_error_string().str;
    EXPECT_EQ(RCL_RET_OK, ret);
  });
  ret = rcl_shutdown(&invalid_context);  // Shutdown to invalidate the node.
  ASSERT_EQ(RCL_RET_OK, ret);
  rcl_context_t context = rcl_get_zero_initialized_context();
  ret = rcl_init(0, nullptr, &init_options, &context);
  ASSERT_EQ(RCL_RET_OK, ret);
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    osrf_testing_tools_cpp::memory_tools::disable_monitoring_in_all_threads();
    ASSERT_EQ(RCL_RET_OK, rcl_shutdown(&context));
    ASSERT_EQ(RCL_RET_OK, rcl_context_fini(&context));
  });
  // Create a zero init node.
  rcl_node_t zero_node = rcl_get_zero_initialized_node();
  // Create a normal node.
  rcl_node_t node = rcl_get_zero_initialized_node();
  ret = rcl_node_init(&node, name, namespace_, &context, &default_options);
  ASSERT_EQ(RCL_RET_OK, ret);
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    osrf_testing_tools_cpp::memory_tools::disable_monitoring_in_all_threads();
    rcl_ret_t ret = rcl_node_fini(&node);
    EXPECT_EQ(RCL_RET_OK, ret);
  });
  // Test rcl_node_is_valid().
  bool is_valid;
  is_valid = rcl_node_is_valid(nullptr);
  EXPECT_FALSE(is_valid);
  rcl_reset_error();
  is_valid = rcl_node_is_valid(&zero_node);
  EXPECT_FALSE(is_valid);
  rcl_reset_error();

  // invalid node will be true for rcl_node_is_valid_except_context, but false for valid only
  is_valid = rcl_node_is_valid_except_context(&invalid_node);
  EXPECT_TRUE(is_valid);
  rcl_reset_error();
  is_valid = rcl_node_is_valid(&invalid_node);
  EXPECT_FALSE(is_valid);
  rcl_reset_error();

  is_valid = rcl_node_is_valid(&node);
  EXPECT_TRUE(is_valid);
  rcl_reset_error();
  // Test rcl_node_get_name().
  const char * actual_node_name;
  actual_node_name = rcl_node_get_name(nullptr);
  EXPECT_EQ(nullptr, actual_node_name);
  rcl_reset_error();
  actual_node_name = rcl_node_get_name(&zero_node);
  EXPECT_EQ(nullptr, actual_node_name);
  rcl_reset_error();
  actual_node_name = rcl_node_get_name(&invalid_node);
  EXPECT_STREQ(name, actual_node_name);
  rcl_reset_error();
  EXPECT_NO_MEMORY_OPERATIONS(
  {
    actual_node_name = rcl_node_get_name(&node);
  });
  EXPECT_TRUE(actual_node_name ? true : false);
  if (actual_node_name) {
    EXPECT_STREQ(name, actual_node_name);
  }
  // Test rcl_node_get_namespace().
  const char * actual_node_namespace;
  actual_node_namespace = rcl_node_get_namespace(nullptr);
  EXPECT_EQ(nullptr, actual_node_namespace);
  rcl_reset_error();
  actual_node_namespace = rcl_node_get_namespace(&zero_node);
  EXPECT_EQ(nullptr, actual_node_namespace);
  rcl_reset_error();
  actual_node_namespace = rcl_node_get_namespace(&invalid_node);
  EXPECT_STREQ(namespace_, actual_node_namespace);
  rcl_reset_error();
  EXPECT_NO_MEMORY_OPERATIONS(
  {
    actual_node_namespace = rcl_node_get_namespace(&node);
  });
  EXPECT_STREQ(namespace_, actual_node_namespace);
  // Test rcl_node_get_fully_qualified_name().
  const char * actual_fq_node_name;
  actual_fq_node_name = rcl_node_get_fully_qualified_name(nullptr);
  EXPECT_EQ(nullptr, actual_fq_node_name);
  rcl_reset_error();
  actual_fq_node_name = rcl_node_get_fully_qualified_name(&zero_node);
  EXPECT_EQ(nullptr, actual_fq_node_name);
  rcl_reset_error();
  actual_fq_node_name = rcl_node_get_fully_qualified_name(&invalid_node);
  EXPECT_STREQ(fq_name, actual_fq_node_name);
  rcl_reset_error();
  EXPECT_NO_MEMORY_OPERATIONS(
  {
    actual_fq_node_name = rcl_node_get_fully_qualified_name(&node);
  });
  EXPECT_STREQ(fq_name, actual_fq_node_name);
  // Test rcl_node_get_logger_name().
  const char * actual_node_logger_name;
  actual_node_logger_name = rcl_node_get_logger_name(nullptr);
  EXPECT_EQ(nullptr, actual_node_logger_name);
  rcl_reset_error();
  actual_node_logger_name = rcl_node_get_logger_name(&zero_node);
  EXPECT_EQ(nullptr, actual_node_logger_name);
  rcl_reset_error();
  actual_node_logger_name = rcl_node_get_logger_name(&invalid_node);
  EXPECT_NE(actual_node_logger_name, nullptr);
  if (actual_node_logger_name) {
    EXPECT_EQ("ns." + std::string(name), std::string(actual_node_logger_name));
  }
  rcl_reset_error();
  EXPECT_NO_MEMORY_OPERATIONS(
  {
    actual_node_logger_name = rcl_node_get_logger_name(&node);
  });
  EXPECT_NE(actual_node_logger_name, nullptr);
  if (actual_node_logger_name) {
    EXPECT_EQ("ns." + std::string(name), std::string(actual_node_logger_name));
  }
  // Test rcl_node_get_options().
  const rcl_node_options_t * actual_options;
  actual_options = rcl_node_get_options(nullptr);
  EXPECT_EQ(nullptr, actual_options);
  rcl_reset_error();
  actual_options = rcl_node_get_options(&zero_node);
  EXPECT_EQ(nullptr, actual_options);
  rcl_reset_error();
  actual_options = rcl_node_get_options(&invalid_node);
  EXPECT_NE(nullptr, actual_options);
  if (actual_options) {
    EXPECT_EQ(default_options.allocator.allocate, actual_options->allocator.allocate);
  }
  rcl_reset_error();
  EXPECT_NO_MEMORY_OPERATIONS(
  {
    actual_options = rcl_node_get_options(&node);
  });
  EXPECT_NE(nullptr, actual_options);
  if (actual_options) {
    EXPECT_EQ(default_options.allocator.allocate, actual_options->allocator.allocate);
  }
  // Test rcl_node_get_domain_id().
  size_t actual_domain_id;
  ret = rcl_node_get_domain_id(nullptr, &actual_domain_id);
  EXPECT_EQ(RCL_RET_NODE_INVALID, ret);
  ASSERT_TRUE(rcl_error_is_set());
  rcl_reset_error();
  ret = rcl_node_get_domain_id(&zero_node, &actual_domain_id);
  EXPECT_EQ(RCL_RET_NODE_INVALID, ret);
  ASSERT_TRUE(rcl_error_is_set());
  rcl_reset_error();
  ret = rcl_node_get_domain_id(&invalid_node, &actual_domain_id);
  EXPECT_EQ(RCL_RET_NODE_INVALID, ret);
  rcl_reset_error();
  EXPECT_NO_MEMORY_OPERATIONS(
  {
    ret = rcl_node_get_domain_id(&node, &actual_domain_id);
  });
  EXPECT_EQ(RCL_RET_OK, ret);
  EXPECT_EQ(42u, actual_domain_id);
  actual_domain_id = 0u;
  EXPECT_NO_MEMORY_OPERATIONS(
  {
    ret = rcl_context_get_domain_id(&context, &actual_domain_id);
  });
  EXPECT_EQ(RCL_RET_OK, ret);
  EXPECT_EQ(42u, actual_domain_id);

  // Test rcl_node_get_rmw_handle().
  rmw_node_t * node_handle;
  node_handle = rcl_node_get_rmw_handle(nullptr);
  EXPECT_EQ(nullptr, node_handle);
  rcl_reset_error();
  node_handle = rcl_node_get_rmw_handle(&zero_node);
  EXPECT_EQ(nullptr, node_handle);
  rcl_reset_error();
  node_handle = rcl_node_get_rmw_handle(&invalid_node);
  EXPECT_NE(nullptr, node_handle);
  rcl_reset_error();
  EXPECT_NO_MEMORY_OPERATIONS(
  {
    node_handle = rcl_node_get_rmw_handle(&node);
  });
  EXPECT_NE(nullptr, node_handle);
  // Test rcl_node_get_rcl_instance_id().
  uint64_t instance_id;
  instance_id = rcl_node_get_rcl_instance_id(nullptr);
  EXPECT_EQ(0u, instance_id);
  rcl_reset_error();
  instance_id = rcl_node_get_rcl_instance_id(&zero_node);
  EXPECT_EQ(0u, instance_id);
  rcl_reset_error();
  instance_id = rcl_node_get_rcl_instance_id(&invalid_node);
  EXPECT_EQ(0u, instance_id);
  rcl_reset_error();
  EXPECT_NO_MEMORY_OPERATIONS(
  {
    instance_id = rcl_node_get_rcl_instance_id(&node);
  });
  EXPECT_NE(0u, instance_id);
  // Test rcl_node_get_graph_guard_condition
  const rcl_guard_condition_t * graph_guard_condition;
  graph_guard_condition = rcl_node_get_graph_guard_condition(nullptr);
  EXPECT_EQ(nullptr, graph_guard_condition);
  rcl_reset_error();
  graph_guard_condition = rcl_node_get_graph_guard_condition(&zero_node);
  EXPECT_EQ(nullptr, graph_guard_condition);
  rcl_reset_error();
  graph_guard_condition = rcl_node_get_graph_guard_condition(&invalid_node);
  EXPECT_NE(nullptr, graph_guard_condition);
  rcl_reset_error();
  EXPECT_NO_MEMORY_OPERATIONS(
  {
    graph_guard_condition = rcl_node_get_graph_guard_condition(&node);
  });
  EXPECT_NE(nullptr, graph_guard_condition);
}

/* Tests the node life cycle, including rcl_node_init() and rcl_node_fini().
 */
TEST_F(CLASSNAME(TestNodeFixture, RMW_IMPLEMENTATION), test_rcl_node_life_cycle) {
  rcl_ret_t ret;
  rcl_context_t context = rcl_get_zero_initialized_context();
  rcl_node_t node = rcl_get_zero_initialized_node();
  const char * name = "test_rcl_node_life_cycle_node";
  const char * namespace_ = "/ns";
  rcl_node_options_t default_options = rcl_node_get_default_options();
  // Trying to init before rcl_init() should fail.
  ret = rcl_node_init(&node, name, "", &context, &default_options);
  ASSERT_EQ(RCL_RET_NOT_INIT, ret) << "Expected RCL_RET_NOT_INIT";
  ASSERT_TRUE(rcl_error_is_set());
  rcl_reset_error();
  // Initialize rcl with rcl_init().
  rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();
  ret = rcl_init_options_init(&init_options, rcl_get_default_allocator());
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_init_options_fini(&init_options)) << rcl_get_error_string().str;
  });
  ret = rcl_init(0, nullptr, &init_options, &context);
  ASSERT_EQ(RCL_RET_OK, ret);
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    ASSERT_EQ(RCL_RET_OK, rcl_shutdown(&context));
    ASSERT_EQ(RCL_RET_OK, rcl_context_fini(&context));
  });
  // Try invalid arguments.
  ret = rcl_node_init(nullptr, name, namespace_, &context, &default_options);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret);
  ASSERT_TRUE(rcl_error_is_set());
  rcl_reset_error();
  ret = rcl_node_init(&node, nullptr, namespace_, &context, &default_options);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret);
  ASSERT_TRUE(rcl_error_is_set());
  rcl_reset_error();
  ret = rcl_node_init(&node, name, nullptr, &context, &default_options);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret);
  ASSERT_TRUE(rcl_error_is_set());
  rcl_reset_error();
  ret = rcl_node_init(&node, name, namespace_, nullptr, &default_options);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret);
  ASSERT_TRUE(rcl_error_is_set());
  rcl_reset_error();
  ret = rcl_node_init(&node, name, namespace_, &context, nullptr);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret);
  ASSERT_TRUE(rcl_error_is_set());
  rcl_reset_error();
  // Try fini with invalid arguments.
  ret = rcl_node_fini(nullptr);
  EXPECT_EQ(RCL_RET_NODE_INVALID, ret) << "Expected RCL_RET_NODE_INVALID";
  ASSERT_TRUE(rcl_error_is_set());
  rcl_reset_error();
  // Try fini with an uninitialized node.
  ret = rcl_node_fini(&node);
  EXPECT_EQ(RCL_RET_OK, ret);
  // Try a normal init and fini.
  ret = rcl_node_init(&node, name, namespace_, &context, &default_options);
  EXPECT_EQ(RCL_RET_OK, ret);
  ret = rcl_node_fini(&node);
  EXPECT_EQ(RCL_RET_OK, ret);
  // Try repeated init and fini calls.
  ret = rcl_node_init(&node, name, namespace_, &context, &default_options);
  EXPECT_EQ(RCL_RET_OK, ret);
  ret = rcl_node_init(&node, name, namespace_, &context, &default_options);
  EXPECT_EQ(RCL_RET_ALREADY_INIT, ret) << "Expected RCL_RET_ALREADY_INIT";
  ASSERT_TRUE(rcl_error_is_set());
  rcl_reset_error();
  ret = rcl_node_fini(&node);
  EXPECT_EQ(RCL_RET_OK, ret);
  ret = rcl_node_fini(&node);
  EXPECT_EQ(RCL_RET_OK, ret);
}

TEST_F(CLASSNAME(TestNodeFixture, RMW_IMPLEMENTATION), test_rcl_node_init_with_internal_errors) {
  rcl_ret_t ret;
  rcl_context_t context = rcl_get_zero_initialized_context();
  rcl_node_t node = rcl_get_zero_initialized_node();
  const char * name = "test_rcl_node_init_with_internal_errors";
  const char * namespace_ = "ns";  // force non-absolute namespace handling
  rcl_node_options_t options = rcl_node_get_default_options();
  options.enable_rosout = true;  // enable logging to cover more ground
  // Initialize rcl with rcl_init().
  rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();
  rcl_allocator_t allocator = rcl_get_default_allocator();
  ret = rcl_init_options_init(&init_options, allocator);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_init_options_fini(&init_options)) << rcl_get_error_string().str;
  });
  ret = rcl_init(0, nullptr, &init_options, &context);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_shutdown(&context)) << rcl_get_error_string().str;
    EXPECT_EQ(RCL_RET_OK, rcl_context_fini(&context)) << rcl_get_error_string().str;
  });
  // Initialize logging and rosout.
  ret = rcl_logging_configure(&context.global_arguments, &allocator);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_logging_fini()) << rcl_get_error_string().str;
  });
  ret = rcl_logging_rosout_init(&allocator);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_logging_rosout_fini()) << rcl_get_error_string().str;
  });
  // Try with invalid allocator.
  rcl_node_options_t options_with_invalid_allocator = rcl_node_get_default_options();
  options_with_invalid_allocator.allocator.allocate = nullptr;
  options_with_invalid_allocator.allocator.deallocate = nullptr;
  options_with_invalid_allocator.allocator.reallocate = nullptr;
  ret = rcl_node_init(&node, name, namespace_, &context, &options_with_invalid_allocator);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret) << "Expected RCL_RET_INVALID_ARGUMENT";
  ASSERT_TRUE(rcl_error_is_set());
  rcl_reset_error();
  // Try with failing allocator.
  rcl_node_options_t options_with_failing_allocator = rcl_node_get_default_options();
  options_with_failing_allocator.allocator.allocate = failing_malloc;
  options_with_failing_allocator.allocator.reallocate = failing_realloc;
  ret = rcl_node_init(&node, name, namespace_, &context, &options_with_failing_allocator);
  EXPECT_EQ(RCL_RET_BAD_ALLOC, ret) << "Expected RCL_RET_BAD_ALLOC";
  ASSERT_TRUE(rcl_error_is_set());
  rcl_reset_error();
  // Try init but force internal errors.
  {
    auto mock = mocking_utils::patch_and_return("lib:rcl", rmw_create_node, nullptr);
    ret = rcl_node_init(&node, name, namespace_, &context, &options);
    EXPECT_EQ(RCL_RET_ERROR, ret);
    rcl_reset_error();
  }

  {
    auto mock = mocking_utils::patch_and_return(
      "lib:rcl", rmw_node_get_graph_guard_condition, nullptr);
    ret = rcl_node_init(&node, name, namespace_, &context, &options);
    EXPECT_EQ(RCL_RET_ERROR, ret);
    rcl_reset_error();
  }

  {
    auto mock = mocking_utils::patch_and_return(
      "lib:rcl", rmw_validate_node_name, RMW_RET_ERROR);
    ret = rcl_node_init(&node, name, namespace_, &context, &options);
    EXPECT_EQ(RCL_RET_ERROR, ret);
    rcl_reset_error();
  }

  {
    auto mock = mocking_utils::patch_and_return(
      "lib:rcl", rmw_validate_namespace, RMW_RET_ERROR);
    ret = rcl_node_init(&node, name, namespace_, &context, &options);
    EXPECT_EQ(RCL_RET_ERROR, ret);
    rcl_reset_error();
  }
  // Try normal init but force an internal error on fini.
  {
    ret = rcl_node_init(&node, name, namespace_, &context, &options);
    EXPECT_EQ(RCL_RET_OK, ret);
    auto mock = mocking_utils::inject_on_return("lib:rcl", rmw_destroy_node, RMW_RET_ERROR);
    ret = rcl_node_fini(&node);
    EXPECT_EQ(RCL_RET_ERROR, ret);
    rcl_reset_error();
  }

  // Battle test node init.
  RCUTILS_FAULT_INJECTION_TEST(
  {
    ret = rcl_node_init(&node, name, namespace_, &context, &options);

    int64_t count = rcutils_fault_injection_get_count();
    rcutils_fault_injection_set_count(RCUTILS_FAULT_INJECTION_NEVER_FAIL);

    if (RCL_RET_OK == ret) {
      ASSERT_TRUE(rcl_node_is_valid(&node));
      EXPECT_EQ(RCL_RET_OK, rcl_node_fini(&node)) << rcl_get_error_string().str;
    } else {
      ASSERT_FALSE(rcl_node_is_valid(&node));
      rcl_reset_error();
    }

    rcutils_fault_injection_set_count(count);
  });
}

/* Tests the node name restrictions enforcement.
 */
TEST_F(CLASSNAME(TestNodeFixture, RMW_IMPLEMENTATION), test_rcl_node_name_restrictions) {
  rcl_ret_t ret;

  // Initialize rcl with rcl_init().
  rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();
  ret = rcl_init_options_init(&init_options, rcl_get_default_allocator());
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_init_options_fini(&init_options)) << rcl_get_error_string().str;
  });
  rcl_context_t context = rcl_get_zero_initialized_context();
  ret = rcl_init(0, nullptr, &init_options, &context);
  ASSERT_EQ(RCL_RET_OK, ret);
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    ASSERT_EQ(RCL_RET_OK, rcl_shutdown(&context));
    ASSERT_EQ(RCL_RET_OK, rcl_context_fini(&context));
  });

  const char * namespace_ = "/ns";
  rcl_node_options_t default_options = rcl_node_get_default_options();

  // First do a normal node name.
  {
    rcl_node_t node = rcl_get_zero_initialized_node();
    ret = rcl_node_init(&node, "my_node_42", namespace_, &context, &default_options);
    ASSERT_EQ(RCL_RET_OK, ret);
    rcl_ret_t ret = rcl_node_fini(&node);
    EXPECT_EQ(RCL_RET_OK, ret);
  }

  // Node name with invalid characters.
  {
    rcl_node_t node = rcl_get_zero_initialized_node();
    ret = rcl_node_init(&node, "my_node_42$", namespace_, &context, &default_options);
    ASSERT_EQ(RCL_RET_NODE_INVALID_NAME, ret);
    ASSERT_TRUE(rcl_error_is_set());
    rcl_reset_error();
    rcl_ret_t ret = rcl_node_fini(&node);
    EXPECT_EQ(RCL_RET_OK, ret);
  }

  // Node name with /, which is valid in a topic, but not a node name.
  {
    rcl_node_t node = rcl_get_zero_initialized_node();
    ret = rcl_node_init(&node, "my/node_42", namespace_, &context, &default_options);
    ASSERT_EQ(RCL_RET_NODE_INVALID_NAME, ret);
    ASSERT_TRUE(rcl_error_is_set());
    rcl_reset_error();
    rcl_ret_t ret = rcl_node_fini(&node);
    EXPECT_EQ(RCL_RET_OK, ret);
  }

  // Node name with {}, which is valid in a topic, but not a node name.
  {
    rcl_node_t node = rcl_get_zero_initialized_node();
    ret = rcl_node_init(&node, "my_{node}_42", namespace_, &context, &default_options);
    ASSERT_EQ(RCL_RET_NODE_INVALID_NAME, ret);
    ASSERT_TRUE(rcl_error_is_set());
    rcl_reset_error();
    rcl_ret_t ret = rcl_node_fini(&node);
    EXPECT_EQ(RCL_RET_OK, ret);
  }
}

/* Tests the node namespace restrictions enforcement.
 */
TEST_F(CLASSNAME(TestNodeFixture, RMW_IMPLEMENTATION), test_rcl_node_namespace_restrictions) {
  rcl_ret_t ret;

  // Initialize rcl with rcl_init().
  rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();
  ret = rcl_init_options_init(&init_options, rcl_get_default_allocator());
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_init_options_fini(&init_options)) << rcl_get_error_string().str;
  });
  rcl_context_t context = rcl_get_zero_initialized_context();
  ret = rcl_init(0, nullptr, &init_options, &context);
  ASSERT_EQ(RCL_RET_OK, ret);
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    ASSERT_EQ(RCL_RET_OK, rcl_shutdown(&context));
    ASSERT_EQ(RCL_RET_OK, rcl_context_fini(&context));
  });

  const char * name = "node";
  rcl_node_options_t default_options = rcl_node_get_default_options();

  // First do a normal node namespace.
  {
    rcl_node_t node = rcl_get_zero_initialized_node();
    ret = rcl_node_init(&node, name, "/ns", &context, &default_options);
    ASSERT_EQ(RCL_RET_OK, ret);
    rcl_ret_t ret = rcl_node_fini(&node);
    EXPECT_EQ(RCL_RET_OK, ret);
  }

  // Node namespace which is an empty string, which is also valid.
  {
    rcl_node_t node = rcl_get_zero_initialized_node();
    ret = rcl_node_init(&node, name, "", &context, &default_options);
    ASSERT_EQ(RCL_RET_OK, ret);
    ASSERT_STREQ("/", rcl_node_get_namespace(&node));
    rcl_ret_t ret = rcl_node_fini(&node);
    EXPECT_EQ(RCL_RET_OK, ret);
  }

  // Node namespace which is just a forward slash, which is valid.
  {
    rcl_node_t node = rcl_get_zero_initialized_node();
    ret = rcl_node_init(&node, name, "/", &context, &default_options);
    ASSERT_EQ(RCL_RET_OK, ret);
    rcl_ret_t ret = rcl_node_fini(&node);
    EXPECT_EQ(RCL_RET_OK, ret);
  }

  // Node namespaces with invalid characters.
  {
    rcl_node_t node = rcl_get_zero_initialized_node();
    ret = rcl_node_init(&node, name, "/ns/{name}", &context, &default_options);
    ASSERT_EQ(RCL_RET_NODE_INVALID_NAMESPACE, ret);
    ASSERT_TRUE(rcl_error_is_set());
    rcl_reset_error();
    rcl_ret_t ret = rcl_node_fini(&node);
    EXPECT_EQ(RCL_RET_OK, ret);
  }
  {
    rcl_node_t node = rcl_get_zero_initialized_node();
    ret = rcl_node_init(&node, name, "/~/", &context, &default_options);
    ASSERT_EQ(RCL_RET_NODE_INVALID_NAMESPACE, ret);
    ASSERT_TRUE(rcl_error_is_set());
    rcl_reset_error();
    rcl_ret_t ret = rcl_node_fini(&node);
    EXPECT_EQ(RCL_RET_OK, ret);
  }

  // Node namespace with a trailing / which is not allowed.
  {
    rcl_node_t node = rcl_get_zero_initialized_node();
    ret = rcl_node_init(&node, name, "/ns/foo/", &context, &default_options);
    ASSERT_EQ(RCL_RET_NODE_INVALID_NAMESPACE, ret);
    ASSERT_TRUE(rcl_error_is_set());
    rcl_reset_error();
    rcl_ret_t ret = rcl_node_fini(&node);
    EXPECT_EQ(RCL_RET_OK, ret);
  }

  // Node namespace which is not absolute, it should get / added automatically.
  {
    rcl_node_t node = rcl_get_zero_initialized_node();
    ret = rcl_node_init(&node, name, "ns", &context, &default_options);
    ASSERT_EQ(RCL_RET_OK, ret);
    ASSERT_STREQ("/ns", rcl_node_get_namespace(&node));
    rcl_ret_t ret = rcl_node_fini(&node);
    EXPECT_EQ(RCL_RET_OK, ret);
  }

  // Other reasons for being invalid, which are related to being part of a topic.
  {
    rcl_node_t node = rcl_get_zero_initialized_node();
    ret = rcl_node_init(&node, name, "/starts/with/42number", &context, &default_options);
    ASSERT_EQ(RCL_RET_NODE_INVALID_NAMESPACE, ret);
    ASSERT_TRUE(rcl_error_is_set());
    rcl_reset_error();
    rcl_ret_t ret = rcl_node_fini(&node);
    EXPECT_EQ(RCL_RET_OK, ret);
  }
}

/* Tests the logger name as well as fully qualified name associated with the node.
 */
TEST_F(CLASSNAME(TestNodeFixture, RMW_IMPLEMENTATION), test_rcl_node_names) {
  rcl_ret_t ret;

  // Initialize rcl with rcl_init().
  rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();
  ret = rcl_init_options_init(&init_options, rcl_get_default_allocator());
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_init_options_fini(&init_options)) << rcl_get_error_string().str;
  });
  rcl_context_t context = rcl_get_zero_initialized_context();
  ret = rcl_init(0, nullptr, &init_options, &context);
  ASSERT_EQ(RCL_RET_OK, ret);
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    ASSERT_EQ(RCL_RET_OK, rcl_shutdown(&context));
    ASSERT_EQ(RCL_RET_OK, rcl_context_fini(&context));
  });

  const char * actual_node_logger_name;
  const char * actual_node_name;
  const char * actual_node_namespace;
  const char * actual_node_fq_name;

  rcl_node_options_t default_options = rcl_node_get_default_options();

  // First do a normal node namespace.
  {
    rcl_node_t node = rcl_get_zero_initialized_node();
    ret = rcl_node_init(&node, "node", "/ns", &context, &default_options);
    ASSERT_EQ(RCL_RET_OK, ret);

    actual_node_logger_name = rcl_node_get_logger_name(&node);
    actual_node_name = rcl_node_get_name(&node);
    actual_node_namespace = rcl_node_get_namespace(&node);
    actual_node_fq_name = rcl_node_get_fully_qualified_name(&node);

    EXPECT_STREQ("ns.node", actual_node_logger_name);
    EXPECT_STREQ("node", actual_node_name);
    EXPECT_STREQ("/ns", actual_node_namespace);
    EXPECT_STREQ("/ns/node", actual_node_fq_name);

    rcl_ret_t ret = rcl_node_fini(&node);
    EXPECT_EQ(RCL_RET_OK, ret);
  }

  // Node namespace that is an empty string.
  {
    rcl_node_t node = rcl_get_zero_initialized_node();
    ret = rcl_node_init(&node, "node", "", &context, &default_options);
    ASSERT_EQ(RCL_RET_OK, ret);

    actual_node_logger_name = rcl_node_get_logger_name(&node);
    actual_node_name = rcl_node_get_name(&node);
    actual_node_namespace = rcl_node_get_namespace(&node);
    actual_node_fq_name = rcl_node_get_fully_qualified_name(&node);

    EXPECT_STREQ("node", actual_node_logger_name);
    EXPECT_STREQ("node", actual_node_name);
    EXPECT_STREQ("/", actual_node_namespace);
    EXPECT_STREQ("/node", actual_node_fq_name);

    rcl_ret_t ret = rcl_node_fini(&node);
    EXPECT_EQ(RCL_RET_OK, ret);
  }

  // Node namespace that is just a forward slash.
  {
    rcl_node_t node = rcl_get_zero_initialized_node();
    ret = rcl_node_init(&node, "node", "/", &context, &default_options);
    ASSERT_EQ(RCL_RET_OK, ret);

    actual_node_logger_name = rcl_node_get_logger_name(&node);
    actual_node_name = rcl_node_get_name(&node);
    actual_node_namespace = rcl_node_get_namespace(&node);
    actual_node_fq_name = rcl_node_get_fully_qualified_name(&node);

    EXPECT_STREQ("node", actual_node_logger_name);
    EXPECT_STREQ("node", actual_node_name);
    EXPECT_STREQ("/", actual_node_namespace);
    EXPECT_STREQ("/node", actual_node_fq_name);

    rcl_ret_t ret = rcl_node_fini(&node);
    EXPECT_EQ(RCL_RET_OK, ret);
  }

  // Node namespace that is not absolute.
  {
    rcl_node_t node = rcl_get_zero_initialized_node();
    ret = rcl_node_init(&node, "node", "ns", &context, &default_options);
    ASSERT_EQ(RCL_RET_OK, ret);

    actual_node_logger_name = rcl_node_get_logger_name(&node);
    actual_node_name = rcl_node_get_name(&node);
    actual_node_namespace = rcl_node_get_namespace(&node);
    actual_node_fq_name = rcl_node_get_fully_qualified_name(&node);

    EXPECT_STREQ("ns.node", actual_node_logger_name);
    EXPECT_STREQ("node", actual_node_name);
    EXPECT_STREQ("/ns", actual_node_namespace);
    EXPECT_STREQ("/ns/node", actual_node_fq_name);

    rcl_ret_t ret = rcl_node_fini(&node);
    EXPECT_EQ(RCL_RET_OK, ret);
  }

  // Nested namespace.
  {
    rcl_node_t node = rcl_get_zero_initialized_node();
    ret = rcl_node_init(&node, "node", "/ns/sub_1/sub_2", &context, &default_options);
    ASSERT_EQ(RCL_RET_OK, ret);

    actual_node_logger_name = rcl_node_get_logger_name(&node);
    actual_node_name = rcl_node_get_name(&node);
    actual_node_namespace = rcl_node_get_namespace(&node);
    actual_node_fq_name = rcl_node_get_fully_qualified_name(&node);

    EXPECT_STREQ("ns.sub_1.sub_2.node", actual_node_logger_name);
    EXPECT_STREQ("node", actual_node_name);
    EXPECT_STREQ("/ns/sub_1/sub_2", actual_node_namespace);
    EXPECT_STREQ("/ns/sub_1/sub_2/node", actual_node_fq_name);

    rcl_ret_t ret = rcl_node_fini(&node);
    EXPECT_EQ(RCL_RET_OK, ret);
  }
}

/* Tests the node_options functionality
 */
TEST_F(CLASSNAME(TestNodeFixture, RMW_IMPLEMENTATION), test_rcl_node_options) {
  rcl_node_options_t default_options = rcl_node_get_default_options();
  rcl_node_options_t not_ini_options = rcl_node_get_default_options();
  memset(&not_ini_options.rosout_qos, 0, sizeof(rmw_qos_profile_t));

  EXPECT_TRUE(default_options.use_global_arguments);
  EXPECT_TRUE(default_options.enable_rosout);
  EXPECT_EQ(rcl_qos_profile_rosout_default, default_options.rosout_qos);
  EXPECT_TRUE(rcutils_allocator_is_valid(&(default_options.allocator)));

  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rcl_node_options_copy(nullptr, &default_options));
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rcl_node_options_copy(&default_options, nullptr));
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rcl_node_options_copy(&default_options, &default_options));

  const char * argv[] = {
    "process_name", "--ros-args", "/foo/bar:=", "-r", "bar:=/fiz/buz", "}bar:=fiz", "--", "arg"};
  int argc = sizeof(argv) / sizeof(const char *);
  EXPECT_EQ(
    RCL_RET_OK,
    rcl_parse_arguments(argc, argv, default_options.allocator, &(default_options.arguments)));
  default_options.use_global_arguments = false;
  default_options.enable_rosout = false;
  default_options.rosout_qos = rmw_qos_profile_default;
  EXPECT_EQ(RCL_RET_OK, rcl_node_options_copy(&default_options, &not_ini_options));
  EXPECT_FALSE(not_ini_options.use_global_arguments);
  EXPECT_FALSE(not_ini_options.enable_rosout);
  EXPECT_EQ(default_options.rosout_qos, not_ini_options.rosout_qos);
  EXPECT_EQ(
    rcl_arguments_get_count_unparsed(&(default_options.arguments)),
    rcl_arguments_get_count_unparsed(&(not_ini_options.arguments)));
  EXPECT_EQ(
    rcl_arguments_get_count_unparsed_ros(&(default_options.arguments)),
    rcl_arguments_get_count_unparsed_ros(&(not_ini_options.arguments)));

  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rcl_node_options_fini(nullptr));
  EXPECT_EQ(RCL_RET_OK, rcl_node_options_fini(&default_options));
  EXPECT_EQ(RCL_RET_OK, rcl_node_options_fini(&not_ini_options));
}

/* Tests special case node_options
 */
TEST_F(CLASSNAME(TestNodeFixture, RMW_IMPLEMENTATION), test_rcl_node_options_fail) {
  rcl_node_options_t prev_ini_options = rcl_node_get_default_options();
  const char * argv[] = {"--ros-args"};
  int argc = sizeof(argv) / sizeof(const char *);
  EXPECT_EQ(
    RCL_RET_OK,
    rcl_parse_arguments(argc, argv, rcl_get_default_allocator(), &prev_ini_options.arguments));

  rcl_node_options_t default_options = rcl_node_get_default_options();
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rcl_node_options_copy(&default_options, &prev_ini_options));

  EXPECT_EQ(RCL_RET_OK, rcl_arguments_fini(&prev_ini_options.arguments));
}

/* Tests special case node_options
 */
TEST_F(CLASSNAME(TestNodeFixture, RMW_IMPLEMENTATION), test_rcl_node_resolve_name) {
  rcl_allocator_t default_allocator = rcl_get_default_allocator();
  char * final_name = NULL;
  rcl_node_t node = rcl_get_zero_initialized_node();
  // Invalid node
  EXPECT_EQ(
    RCL_RET_INVALID_ARGUMENT,
    rcl_node_resolve_name(NULL, "my_topic", default_allocator, false, false, &final_name));
  EXPECT_EQ(
    RCL_RET_ERROR,
    rcl_node_resolve_name(&node, "my_topic", default_allocator, false, false, &final_name));

  // Initialize rcl with rcl_init().
  rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();
  rcl_ret_t ret = rcl_init_options_init(&init_options, rcl_get_default_allocator());
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    EXPECT_EQ(RCL_RET_OK, rcl_init_options_fini(&init_options)) << rcl_get_error_string().str;
  });
  rcl_context_t context = rcl_get_zero_initialized_context();
  ret = rcl_init(0, nullptr, &init_options, &context);
  ASSERT_EQ(RCL_RET_OK, ret);
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    ASSERT_EQ(RCL_RET_OK, rcl_shutdown(&context));
    ASSERT_EQ(RCL_RET_OK, rcl_context_fini(&context));
  });

  // Initialize node with default options
  rcl_node_options_t options = rcl_node_get_default_options();
  rcl_arguments_t local_arguments = rcl_get_zero_initialized_arguments();
  const char * argv[] = {"process_name", "--ros-args", "-r", "/bar/foo:=/foo/local_args"};
  unsigned int argc = (sizeof(argv) / sizeof(const char *));
  ret = rcl_parse_arguments(
    argc, argv, default_allocator, &local_arguments);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  options.arguments = local_arguments;  // transfer ownership
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rcl_node_options_fini(&options);
  });
  ret = rcl_node_init(&node, "node", "/ns", &context, &options);
  ASSERT_EQ(RCL_RET_OK, ret);
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    ASSERT_EQ(RCL_RET_OK, rcl_node_fini(&node));
  });

  // Invalid arguments
  EXPECT_EQ(
    RCL_RET_INVALID_ARGUMENT,
    rcl_node_resolve_name(&node, NULL, default_allocator, false, false, &final_name));
  EXPECT_EQ(
    RCL_RET_INVALID_ARGUMENT,
    rcl_node_resolve_name(&node, "my_topic", default_allocator, false, false, NULL));

  // Some valid options, test_remap and test_expand_topic_name already have good coverage
  EXPECT_EQ(
    RCL_RET_OK,
    rcl_node_resolve_name(&node, "my_topic", default_allocator, false, false, &final_name));
  ASSERT_TRUE(final_name);
  EXPECT_STREQ("/ns/my_topic", final_name);
  default_allocator.deallocate(final_name, default_allocator.state);

  EXPECT_EQ(
    RCL_RET_OK,
    rcl_node_resolve_name(&node, "my_service", default_allocator, true, false, &final_name));
  ASSERT_TRUE(final_name);
  EXPECT_STREQ("/ns/my_service", final_name);
  default_allocator.deallocate(final_name, default_allocator.state);

  EXPECT_EQ(
    RCL_RET_OK,
    rcl_node_resolve_name(&node, "/bar/foo", default_allocator, false, false, &final_name));
  ASSERT_TRUE(final_name);
  EXPECT_STREQ("/foo/local_args", final_name);
  default_allocator.deallocate(final_name, default_allocator.state);

  EXPECT_EQ(
    RCL_RET_OK,
    rcl_node_resolve_name(&node, "/bar/foo", default_allocator, false, true, &final_name));
  ASSERT_TRUE(final_name);
  EXPECT_STREQ("/bar/foo", final_name);
  default_allocator.deallocate(final_name, default_allocator.state);

  EXPECT_EQ(
    RCL_RET_OK,
    rcl_node_resolve_name(&node, "relative_ns/foo", default_allocator, true, false, &final_name));
  ASSERT_TRUE(final_name);
  EXPECT_STREQ("/ns/relative_ns/foo", final_name);
  default_allocator.deallocate(final_name, default_allocator.state);
}

/* Tests special case node_options
 */
TEST_F(CLASSNAME(TestNodeFixture, RMW_IMPLEMENTATION), test_rcl_get_disable_loaned_message) {
  {
    EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rcl_get_disable_loaned_message(nullptr));
    rcl_reset_error();
  }

  {
    bool disable_loaned_message = false;
    auto mock = mocking_utils::patch_and_return(
      "lib:rcl", rcutils_get_env, "internal error");
    EXPECT_EQ(RCL_RET_ERROR, rcl_get_disable_loaned_message(&disable_loaned_message));
    rcl_reset_error();
  }

  {
    ASSERT_TRUE(rcutils_set_env("ROS_DISABLE_LOANED_MESSAGES", "0"));
    bool disable_loaned_message = true;
    EXPECT_EQ(RCL_RET_OK, rcl_get_disable_loaned_message(&disable_loaned_message));
    EXPECT_FALSE(disable_loaned_message);
  }

  {
    ASSERT_TRUE(rcutils_set_env("ROS_DISABLE_LOANED_MESSAGES", "1"));
    bool disable_loaned_message = false;
    EXPECT_EQ(RCL_RET_OK, rcl_get_disable_loaned_message(&disable_loaned_message));
    EXPECT_TRUE(disable_loaned_message);
  }

  {
    ASSERT_TRUE(rcutils_set_env("ROS_DISABLE_LOANED_MESSAGES", "2"));
    bool disable_loaned_message = true;
    EXPECT_EQ(RCL_RET_OK, rcl_get_disable_loaned_message(&disable_loaned_message));
    EXPECT_FALSE(disable_loaned_message);
  }

  {
    ASSERT_TRUE(rcutils_set_env("ROS_DISABLE_LOANED_MESSAGES", "11"));
    bool disable_loaned_message = true;
    EXPECT_EQ(RCL_RET_OK, rcl_get_disable_loaned_message(&disable_loaned_message));
    EXPECT_FALSE(disable_loaned_message);
  }
}
