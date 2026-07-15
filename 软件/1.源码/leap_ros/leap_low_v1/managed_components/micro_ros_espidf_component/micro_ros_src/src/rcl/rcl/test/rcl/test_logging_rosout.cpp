// Copyright 2019 Open Source Robotics Foundation, Inc.
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

#include <string>
#include <vector>

#include "osrf_testing_tools_cpp/scope_exit.hpp"
#include "rcl/error_handling.h"
#include "rcl/logging.h"
#include "rcl/rcl.h"
#include "rcl/subscription.h"
#include "rcl_interfaces/msg/log.h"
#include "rcutils/logging_macros.h"

#include "rcl/logging_rosout.h"

#ifdef RMW_IMPLEMENTATION
# define CLASSNAME_(NAME, SUFFIX) NAME ## __ ## SUFFIX
# define CLASSNAME(NAME, SUFFIX) CLASSNAME_(NAME, SUFFIX)
#else
# define CLASSNAME(NAME, SUFFIX) NAME
#endif

#define EXPAND(x) x
#define TEST_FIXTURE_P_RMW(test_fixture_name) CLASSNAME( \
    test_fixture_name, RMW_IMPLEMENTATION)
#define APPLY(macro, ...) EXPAND(macro(__VA_ARGS__))
#define TEST_P_RMW(test_case_name, test_name) \
  APPLY( \
    TEST_P, CLASSNAME(test_case_name, RMW_IMPLEMENTATION), test_name)
#define INSTANTIATE_TEST_SUITE_P_RMW(instance_name, test_case_name, ...) \
  EXPAND( \
    APPLY( \
      INSTANTIATE_TEST_SUITE_P, instance_name, \
      CLASSNAME(test_case_name, RMW_IMPLEMENTATION), __VA_ARGS__))

struct TestParameters
{
  int argc;
  const char ** argv;
  bool enable_node_option_rosout;
  bool expected_success;
  std::string description;
};

// for ::testing::PrintToStringParamName() to show the exact test case name
std::ostream & operator<<(
  std::ostream & out,
  const TestParameters & params)
{
  out << params.description;
  return out;
}

class CLASSNAME (TestLogRosoutFixtureNotParam, RMW_IMPLEMENTATION) : public ::testing::Test {};

class TEST_FIXTURE_P_RMW (TestLoggingRosoutFixture)
  : public ::testing::TestWithParam<TestParameters>
{
public:
  void SetUp()
  {
    auto param = GetParam();
    rcl_ret_t ret;
    rcl_allocator_t allocator = rcl_get_default_allocator();
    rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();
    ret = rcl_init_options_init(&init_options, allocator);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      EXPECT_EQ(RCL_RET_OK, rcl_init_options_fini(&init_options)) << rcl_get_error_string().str;
    });
    this->context_ptr = new rcl_context_t;
    *this->context_ptr = rcl_get_zero_initialized_context();

    ret = rcl_init(param.argc, param.argv, &init_options, this->context_ptr);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

    EXPECT_EQ(
      RCL_RET_OK,
      rcl_logging_configure(&this->context_ptr->global_arguments, &allocator)
    ) << rcl_get_error_string().str;

    // create node
    rcl_node_options_t node_options = rcl_node_get_default_options();
    if (!param.enable_node_option_rosout) {
      node_options.enable_rosout = param.enable_node_option_rosout;
    }
    const char * name = "test_rcl_node_logging_rosout";
    const char * namespace_ = "/ns";
    this->node_ptr = new rcl_node_t;
    *this->node_ptr = rcl_get_zero_initialized_node();
    ret = rcl_node_init(this->node_ptr, name, namespace_, this->context_ptr, &node_options);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

    // create rosout subscription
    const rosidl_message_type_support_t * ts =
      ROSIDL_GET_MSG_TYPE_SUPPORT(rcl_interfaces, msg, Log);
    const char * topic = "/rosout";
    this->subscription_ptr = new rcl_subscription_t;
    *this->subscription_ptr = rcl_get_zero_initialized_subscription();
    rcl_subscription_options_t subscription_options = rcl_subscription_get_default_options();
    ret = rcl_subscription_init(
      this->subscription_ptr, this->node_ptr, ts, topic, &subscription_options);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  }

  void TearDown()
  {
    rcl_ret_t ret = rcl_subscription_fini(this->subscription_ptr, this->node_ptr);
    delete this->subscription_ptr;
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    ret = rcl_node_fini(this->node_ptr);
    delete this->node_ptr;
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    ret = rcl_shutdown(this->context_ptr);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    ret = rcl_context_fini(this->context_ptr);
    delete this->context_ptr;
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    EXPECT_EQ(RCL_RET_OK, rcl_logging_fini()) << rcl_get_error_string().str;
  }

protected:
  rcl_context_t * context_ptr;
  rcl_node_t * node_ptr;
  rcl_subscription_t * subscription_ptr;
};

void
check_if_rosout_subscription_gets_a_message(
  const char * logger_name,
  rcl_subscription_t * subscription,
  rcl_context_t * context,
  size_t max_tries,
  int64_t period_ms,
  bool & success)
{
  rcl_wait_set_t wait_set = rcl_get_zero_initialized_wait_set();
  rcl_ret_t ret =
    rcl_wait_set_init(&wait_set, 1, 0, 0, 0, 0, 0, context, rcl_get_default_allocator());
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rcl_ret_t ret = rcl_wait_set_fini(&wait_set);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  });
  size_t iteration = 0;
  do {
    RCUTILS_LOG_INFO_NAMED(logger_name, "SOMETHING");
    ++iteration;
    ret = rcl_wait_set_clear(&wait_set);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    ret = rcl_wait_set_add_subscription(&wait_set, subscription, NULL);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    ret = rcl_wait(&wait_set, RCL_MS_TO_NS(period_ms));
    if (ret == RCL_RET_TIMEOUT) {
      continue;
    }
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    for (size_t i = 0; i < wait_set.size_of_subscriptions; ++i) {
      if (wait_set.subscriptions[i] && wait_set.subscriptions[i] == subscription) {
        success = true;
        return;
      }
    }
  } while (iteration < max_tries);
  success = false;
}

/* Testing the subscriber of topic 'rosout' whether to get event from logging or not.
 */
TEST_P_RMW(TestLoggingRosoutFixture, test_logging_rosout) {
  bool success = false;
  check_if_rosout_subscription_gets_a_message(
    rcl_node_get_logger_name(this->node_ptr), this->subscription_ptr,
    this->context_ptr, 30, 100, success);
  ASSERT_EQ(success, GetParam().expected_success);
}

//
// create set of input and expected values
//
std::vector<TestParameters>
get_parameters()
{
  std::vector<TestParameters> parameters;
  static int s_argc = 2;
  static const char * s_argv_enable_rosout[] = {"--ros-args", "--enable-rosout-logs"};
  static const char * s_argv_disable_rosout[] = {"--ros-args", "--disable-rosout-logs"};

  /*
   * Test with enable(implicit) global rosout logs and enable node option of rosout.
   */
  parameters.push_back(
  {
    0,
    nullptr,
    true,
    true,
    "test_enable_implicit_global_rosout_enable_node_option"
  });
  /*
   * Test with enable(implicit) global rosout logs and disable node option of rosout.
   */
  parameters.push_back(
  {
    0,
    nullptr,
    false,
    false,
    "test_enable_implicit_global_rosout_disable_node_option"
  });
  /*
   * Test with enable(explicit) global rosout logs and enable node option of rosout.
   */
  parameters.push_back(
  {
    s_argc,
    s_argv_enable_rosout,
    true,
    true,
    "test_enable_explicit_global_rosout_enable_node_option"
  });
  /*
   * Test with enable(explicit) global rosout logs and disable node option of rosout.
   */
  parameters.push_back(
  {
    s_argc,
    s_argv_enable_rosout,
    false,
    false,
    "test_enable_explicit_global_rosout_disable_node_option"
  });
  /*
   * Test with disable global rosout logs and enable node option of rosout.
   */
  parameters.push_back(
  {
    s_argc,
    s_argv_disable_rosout,
    true,
    false,
    "test_disable_global_rosout_enable_node_option"
  });
  /*
   * Test with disable global rosout logs and disable node option of rosout.
   */
  parameters.push_back(
  {
    s_argc,
    s_argv_disable_rosout,
    false,
    false,
    "test_disable_global_rosout_disable_node_option"
  });

  return parameters;
}

INSTANTIATE_TEST_SUITE_P_RMW(
  TestLoggingRosoutWithDifferentSettings,
  TestLoggingRosoutFixture,
  ::testing::ValuesIn(get_parameters()),
  ::testing::PrintToStringParamName());

/* Testing twice init logging_rosout
 */
TEST_F(
  CLASSNAME(TestLogRosoutFixtureNotParam, RMW_IMPLEMENTATION), test_twice_init_logging_rosout)
{
  rcl_allocator_t allocator = rcl_get_default_allocator();
  EXPECT_EQ(RCL_RET_OK, rcl_logging_rosout_init(&allocator));

  // Init twice returns RCL_RET_OK
  EXPECT_EQ(RCL_RET_OK, rcl_logging_rosout_init(&allocator));

  EXPECT_EQ(RCL_RET_OK, rcl_logging_rosout_fini());
}

/* Bad params
 */
TEST_F(
  CLASSNAME(
    TestLogRosoutFixtureNotParam, RMW_IMPLEMENTATION),
  test_bad_params_init_fini_node_publisher)
{
  rcl_allocator_t allocator = rcl_get_default_allocator();
  rcl_node_t not_init_node = rcl_get_zero_initialized_node();
  EXPECT_EQ(RCL_RET_OK, rcl_logging_rosout_init(&allocator));

  EXPECT_EQ(RCL_RET_NODE_INVALID, rcl_logging_rosout_init_publisher_for_node(nullptr));
  rcl_reset_error();
  EXPECT_EQ(RCL_RET_ERROR, rcl_logging_rosout_init_publisher_for_node(&not_init_node));
  rcl_reset_error();

  EXPECT_EQ(RCL_RET_NODE_INVALID, rcl_logging_rosout_fini_publisher_for_node(nullptr));
  rcl_reset_error();
  EXPECT_EQ(RCL_RET_ERROR, rcl_logging_rosout_fini_publisher_for_node(&not_init_node));
  rcl_reset_error();

  EXPECT_EQ(RCL_RET_OK, rcl_logging_rosout_fini());
}
