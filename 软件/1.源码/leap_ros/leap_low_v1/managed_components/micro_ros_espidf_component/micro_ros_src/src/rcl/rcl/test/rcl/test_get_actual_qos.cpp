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

#include "rcl/error_handling.h"
#include "rcl/rcl.h"
#include "rcl/publisher.h"

#include "osrf_testing_tools_cpp/scope_exit.hpp"
#include "rcutils/logging_macros.h"
#include "rcutils/macros.h"

#include "test_msgs/msg/basic_types.h"
#include "test_msgs/srv/basic_types.h"

#ifdef RMW_IMPLEMENTATION
# define CLASSNAME_(NAME, SUFFIX) NAME ## __ ## SUFFIX
# define CLASSNAME(NAME, SUFFIX) CLASSNAME_(NAME, SUFFIX)
# define RMW_IMPLEMENTATION_STR RCUTILS_STRINGIFY(RMW_IMPLEMENTATION)
#else
# define CLASSNAME(NAME, SUFFIX) NAME
#endif

#define EXPAND(x) x
#define TEST_FIXTURE_P_RMW(test_fixture_name) CLASSNAME( \
    test_fixture_name, RMW_IMPLEMENTATION)
#define APPLY(macro, ...) EXPAND(macro(__VA_ARGS__))
#define TEST_P_RMW(test_case_name, test_name) \
  APPLY( \
    TEST_P, \
    CLASSNAME(test_case_name, RMW_IMPLEMENTATION), test_name)
#define INSTANTIATE_TEST_SUITE_P_RMW(instance_name, test_case_name, ...) \
  EXPAND( \
    APPLY( \
      INSTANTIATE_TEST_SUITE_P, instance_name, \
      CLASSNAME(test_case_name, RMW_IMPLEMENTATION), __VA_ARGS__))

/**
 * Parameterized test.
 * The first param are the NodeOptions used to create the nodes.
 * The second param are the expect intraprocess count results.
 */
struct TestParameters
{
  rmw_qos_profile_t qos_to_set;
  rmw_qos_profile_t qos_expected;
  std::string description;
};

std::ostream & operator<<(
  std::ostream & out,
  const TestParameters & params)
{
  out << params.description;
  return out;
}

bool operator==(
  const rmw_time_t & lhs,
  const rmw_time_t & rhs)
{
  return lhs.sec == rhs.sec && lhs.nsec == rhs.nsec;
}

bool operator>=(
  const rmw_time_t & lhs,
  const rmw_time_t & rhs)
{
  if (lhs.sec > rhs.sec) {
    return true;
  } else if (lhs.sec == rhs.sec) {
    return lhs.nsec >= rhs.nsec;
  } else {
    return false;
  }
}

std::ostream & operator<<(
  std::ostream & out,
  const rmw_time_t & param)
{
  out << "sec: " << param.sec << " nsec: " << param.nsec;
  return out;
}

class TEST_FIXTURE_P_RMW (TestGetActualQoS)
  : public ::testing::TestWithParam<TestParameters>
{
public:
  void SetUp() override
  {
    rcl_ret_t ret;
    rcl_node_options_t node_options = rcl_node_get_default_options();

    rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();
    ret = rcl_init_options_init(&init_options, rcl_get_default_allocator());
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    this->context_ptr = new rcl_context_t;
    *this->context_ptr = rcl_get_zero_initialized_context();
    ret = rcl_init(0, nullptr, &init_options, this->context_ptr);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      EXPECT_EQ(RCL_RET_OK, rcl_init_options_fini(&init_options)) << rcl_get_error_string().str;
    });
    this->node_ptr = new rcl_node_t;
    *this->node_ptr = rcl_get_zero_initialized_node();
    const char * name = "test_get_actual_qos_node";
    ret = rcl_node_init(this->node_ptr, name, "", this->context_ptr, &node_options);
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  }

  void TearDown() override
  {
    rcl_ret_t ret;

    ret = rcl_node_fini(this->node_ptr);
    delete this->node_ptr;
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

    ret = rcl_shutdown(this->context_ptr);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

    ret = rcl_context_fini(this->context_ptr);
    delete this->context_ptr;
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  }

protected:
  rcl_node_t * node_ptr;
  rcl_context_t * context_ptr;
};


class TEST_FIXTURE_P_RMW (TestPublisherGetActualQoS)
  : public TEST_FIXTURE_P_RMW(TestGetActualQoS) {};

TEST_P_RMW(TestPublisherGetActualQoS, test_publisher_get_qos_settings)
{
  TestParameters parameters = GetParam();
  std::string topic_name("/test_publisher_get_actual_qos__");
  rcl_ret_t ret;

  rcl_publisher_t pub = rcl_get_zero_initialized_publisher();
  rcl_publisher_options_t pub_ops = rcl_publisher_get_default_options();
  pub_ops.qos = parameters.qos_to_set;
  auto ts = ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, BasicTypes);
  ret = rcl_publisher_init(&pub, this->node_ptr, ts, topic_name.c_str(), &pub_ops);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  rcl_reset_error();

  const rmw_qos_profile_t * qos;
  qos = rcl_publisher_get_actual_qos(&pub);
  EXPECT_NE(nullptr, qos) << rcl_get_error_string().str;

  EXPECT_EQ(
    qos->history,
    parameters.qos_expected.history);
  if (parameters.qos_expected.history == RMW_QOS_POLICY_HISTORY_KEEP_LAST) {
    EXPECT_EQ(qos->depth, parameters.qos_expected.depth);
  }
  EXPECT_EQ(
    qos->reliability,
    parameters.qos_expected.reliability);
  EXPECT_EQ(
    qos->durability,
    parameters.qos_expected.durability);
  EXPECT_GE(
    qos->deadline,
    parameters.qos_expected.deadline);
  EXPECT_GE(
    qos->lifespan,
    parameters.qos_expected.lifespan);
  EXPECT_EQ(
    qos->liveliness,
    parameters.qos_expected.liveliness);
  EXPECT_GE(
    qos->liveliness_lease_duration,
    parameters.qos_expected.liveliness_lease_duration);
  EXPECT_EQ(
    qos->avoid_ros_namespace_conventions,
    parameters.qos_expected.avoid_ros_namespace_conventions);

  ret = rcl_publisher_fini(&pub, this->node_ptr);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  rcl_reset_error();
}


class TEST_FIXTURE_P_RMW (TestSubscriptionGetActualQoS)
  : public TEST_FIXTURE_P_RMW(TestGetActualQoS) {};

TEST_P_RMW(TestSubscriptionGetActualQoS, test_subscription_get_qos_settings)
{
  TestParameters parameters = GetParam();
  std::string topic_name("/test_subscription_get_qos_settings");
  rcl_ret_t ret;

  rcl_subscription_t pub = rcl_get_zero_initialized_subscription();
  rcl_subscription_options_t pub_ops = rcl_subscription_get_default_options();
  pub_ops.qos = parameters.qos_to_set;
  auto ts = ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, BasicTypes);
  ret = rcl_subscription_init(&pub, this->node_ptr, ts, topic_name.c_str(), &pub_ops);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  rcl_reset_error();

  const rmw_qos_profile_t * qos;
  qos = rcl_subscription_get_actual_qos(&pub);
  EXPECT_NE(nullptr, qos) << rcl_get_error_string().str;

  EXPECT_EQ(
    qos->history,
    parameters.qos_expected.history);
  if (parameters.qos_expected.history == RMW_QOS_POLICY_HISTORY_KEEP_LAST) {
    EXPECT_EQ(qos->depth, parameters.qos_expected.depth);
  }
  EXPECT_EQ(
    qos->reliability,
    parameters.qos_expected.reliability);
  EXPECT_EQ(
    qos->durability,
    parameters.qos_expected.durability);
  EXPECT_GE(
    qos->deadline,
    parameters.qos_expected.deadline);
  // note: lifespan is not a concept in subscriptions
  EXPECT_EQ(
    qos->liveliness,
    parameters.qos_expected.liveliness);
  EXPECT_GE(
    qos->liveliness_lease_duration,
    parameters.qos_expected.liveliness_lease_duration);
  EXPECT_EQ(
    qos->avoid_ros_namespace_conventions,
    parameters.qos_expected.avoid_ros_namespace_conventions);

  ret = rcl_subscription_fini(&pub, this->node_ptr);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  rcl_reset_error();
}


//
// other input profile settings
//

static constexpr rmw_qos_profile_t
nondefault_qos_profile()
{
  rmw_qos_profile_t profile = rmw_qos_profile_default;
  profile.history = RMW_QOS_POLICY_HISTORY_KEEP_ALL;
  profile.depth = 1000;
  profile.reliability = RMW_QOS_POLICY_RELIABILITY_BEST_EFFORT;
  profile.durability = RMW_QOS_POLICY_DURABILITY_TRANSIENT_LOCAL;
  profile.deadline.sec = 1;
  profile.lifespan.nsec = 500000;
  profile.liveliness = RMW_QOS_POLICY_LIVELINESS_AUTOMATIC;
  profile.liveliness_lease_duration.sec = 1;
  profile.avoid_ros_namespace_conventions = true;
  return profile;
}

static constexpr rmw_qos_profile_t
nondefault_qos_profile_for_fastrtps()
{
  rmw_qos_profile_t profile = rmw_qos_profile_default;
  profile.history = RMW_QOS_POLICY_HISTORY_KEEP_ALL;
  profile.depth = 1000;
  profile.reliability = RMW_QOS_POLICY_RELIABILITY_BEST_EFFORT;
  profile.durability = RMW_QOS_POLICY_DURABILITY_TRANSIENT_LOCAL;
  profile.deadline.sec = 1;
  profile.lifespan.nsec = 500000;
  profile.liveliness = RMW_QOS_POLICY_LIVELINESS_AUTOMATIC;
  // profile.liveliness_lease_duration.sec = 1; // fastrtps does not fully support liveliness
  profile.avoid_ros_namespace_conventions = true;
  return profile;
}


//
// expected output profile settings
//

static constexpr rmw_qos_profile_t
expected_default_qos_profile()
{
  rmw_qos_profile_t profile = rmw_qos_profile_default;
  profile.deadline.sec = 2147483647;
  profile.lifespan.sec = 2147483647;
  profile.liveliness = RMW_QOS_POLICY_LIVELINESS_AUTOMATIC;
  profile.liveliness_lease_duration.sec = 2147483647;
  return profile;
}

static constexpr rmw_qos_profile_t
expected_nondefault_qos_profile()
{
  return nondefault_qos_profile();
}

static constexpr rmw_qos_profile_t
expected_nondefault_qos_profile_for_fastrtps()
{
  rmw_qos_profile_t profile = rmw_qos_profile_default;
  profile.history = RMW_QOS_POLICY_HISTORY_KEEP_ALL;
  profile.depth = 1000;
  profile.reliability = RMW_QOS_POLICY_RELIABILITY_BEST_EFFORT;
  profile.durability = RMW_QOS_POLICY_DURABILITY_TRANSIENT_LOCAL;
  profile.deadline.sec = 1;
  profile.lifespan.nsec = 500000;
  profile.liveliness = RMW_QOS_POLICY_LIVELINESS_AUTOMATIC;
  profile.liveliness_lease_duration.sec = 2147483647;
  profile.avoid_ros_namespace_conventions = true;
  return profile;
}

static constexpr rmw_qos_profile_t
expected_system_default_publisher_qos_profile()
{
  rmw_qos_profile_t profile = rmw_qos_profile_default;
  profile.depth = 1;
  profile.deadline.sec = 2147483647;
  profile.lifespan.sec = 2147483647;
  profile.liveliness = RMW_QOS_POLICY_LIVELINESS_AUTOMATIC;
  profile.liveliness_lease_duration.sec = 2147483647;
  return profile;
}

static constexpr rmw_qos_profile_t
expected_system_default_publisher_qos_profile_for_fastrtps()
{
  rmw_qos_profile_t profile = rmw_qos_profile_default;
  profile.depth = 1;
  profile.durability = RMW_QOS_POLICY_DURABILITY_TRANSIENT_LOCAL;
  profile.liveliness = RMW_QOS_POLICY_LIVELINESS_AUTOMATIC;
  profile.liveliness_lease_duration.sec = 2147483647;
  return profile;
}

static constexpr rmw_qos_profile_t
expected_system_default_subscription_qos_profile()
{
  rmw_qos_profile_t profile = rmw_qos_profile_default;
  profile.depth = 1;
  profile.reliability = RMW_QOS_POLICY_RELIABILITY_BEST_EFFORT;
  profile.deadline.sec = 2147483647;
  profile.liveliness = RMW_QOS_POLICY_LIVELINESS_AUTOMATIC;
  profile.liveliness_lease_duration.sec = 2147483647;
  return profile;
}

static constexpr rmw_qos_profile_t
expected_system_default_subscription_qos_profile_for_fastrtps()
{
  rmw_qos_profile_t profile = rmw_qos_profile_default;
  profile.depth = 1;
  profile.reliability = RMW_QOS_POLICY_RELIABILITY_BEST_EFFORT;
  profile.durability = RMW_QOS_POLICY_DURABILITY_VOLATILE;
  profile.deadline.sec = 2147483647;
  profile.liveliness = RMW_QOS_POLICY_LIVELINESS_AUTOMATIC;
  profile.liveliness_lease_duration.sec = 2147483647;
  return profile;
}


//
// create set of input and expected output profile setting pairs
//
std::vector<TestParameters>
get_parameters(bool for_publisher)
{
  std::vector<TestParameters> parameters;

  /*
   * Testing with default qos settings.
   */
  parameters.push_back(
  {
    rmw_qos_profile_default,
    expected_default_qos_profile(),
    "default_qos"
  });

#ifdef RMW_IMPLEMENTATION_STR
  std::string rmw_implementation_str = RMW_IMPLEMENTATION_STR;
  if (rmw_implementation_str == "rmw_fastrtps_cpp" ||
    rmw_implementation_str == "rmw_fastrtps_dynamic_cpp")
  {
    /*
     * Test with non-default settings.
     */
    parameters.push_back(
    {
      nondefault_qos_profile_for_fastrtps(),
      expected_nondefault_qos_profile_for_fastrtps(),
      "nondefault_qos"
    });

    /*
     * Test with system default settings.
     */
    if (for_publisher) {
      parameters.push_back(
      {
        rmw_qos_profile_system_default,
        expected_system_default_publisher_qos_profile_for_fastrtps(),
        "system_default_publisher_qos"
      });
    } else {
      parameters.push_back(
      {
        rmw_qos_profile_system_default,
        expected_system_default_subscription_qos_profile_for_fastrtps(),
        "system_default_publisher_qos"
      });
    }
  } else {
    // TODO(asorbini): Remove this block once ros2/rmw_connext is deprecated.
    if (rmw_implementation_str == "rmw_connext_cpp" ||
      rmw_implementation_str == "rmw_connext_dynamic_cpp")
    {
      /*
       * Test with non-default settings.
       */
      parameters.push_back(
      {
        nondefault_qos_profile(),
        expected_nondefault_qos_profile(),
        "nondefault_qos"
      });

      /*
       * Test with system default settings.
       */
      if (for_publisher) {
        parameters.push_back(
        {
          rmw_qos_profile_system_default,
          expected_system_default_publisher_qos_profile(),
          "system_default_publisher_qos"
        });
      } else {
        parameters.push_back(
        {
          rmw_qos_profile_system_default,
          expected_system_default_subscription_qos_profile(),
          "system_default_publisher_qos"
        });
      }
    }
  }
#endif

  return parameters;
}

INSTANTIATE_TEST_SUITE_P_RMW(
  TestPublisherWithDifferentQoSSettings,
  TestPublisherGetActualQoS,
  ::testing::ValuesIn(get_parameters(true)),
  ::testing::PrintToStringParamName());

INSTANTIATE_TEST_SUITE_P_RMW(
  TestSubscriptionWithDifferentQoSSettings,
  TestSubscriptionGetActualQoS,
  ::testing::ValuesIn(get_parameters(false)),
  ::testing::PrintToStringParamName());
