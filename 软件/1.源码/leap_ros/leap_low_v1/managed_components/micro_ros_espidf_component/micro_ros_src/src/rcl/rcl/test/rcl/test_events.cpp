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

#include <algorithm>
#include <array>
#include <chrono>
#include <string>
#include <thread>
#include <tuple>

#include "rcl/rcl.h"
#include "rcl/subscription.h"
#include "rcl/error_handling.h"
#include "rmw/incompatible_qos_events_statuses.h"
#include "rmw/event.h"

#include "test_msgs/msg/strings.h"
#include "rosidl_runtime_c/string_functions.h"

#include "osrf_testing_tools_cpp/scope_exit.hpp"

#include "./event_impl.h"

using namespace std::chrono_literals;
using std::chrono::seconds;
using std::chrono::duration_cast;

constexpr seconds LIVELINESS_LEASE_DURATION_IN_S = 1s;
constexpr seconds DEADLINE_PERIOD_IN_S = 2s;
constexpr seconds MAX_WAIT_PER_TESTCASE = 10s;

#ifdef RMW_IMPLEMENTATION
# define CLASSNAME_(NAME, SUFFIX) NAME ## __ ## SUFFIX
# define CLASSNAME(NAME, SUFFIX) CLASSNAME_(NAME, SUFFIX)
#else
# define CLASSNAME(NAME, SUFFIX) NAME
#endif

#define EXPECT_OK(varname) EXPECT_EQ(varname, RCL_RET_OK) << rcl_get_error_string().str

struct TestIncompatibleQosEventParams
{
  std::string testcase_name;
  rmw_qos_policy_kind_t qos_policy_kind;
  rmw_qos_profile_t publisher_qos_profile;
  rmw_qos_profile_t subscription_qos_profile;
  std::string error_msg;
};

class CLASSNAME (TestEventFixture, RMW_IMPLEMENTATION)
  : public ::testing::TestWithParam<TestIncompatibleQosEventParams>
{
public:
  void SetUp()
  {
    rcl_ret_t ret;
    {
      rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();
      ret = rcl_init_options_init(&init_options, rcl_get_default_allocator());
      ASSERT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
      OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
      {
        EXPECT_EQ(RCL_RET_OK, rcl_init_options_fini(&init_options)) << rcl_get_error_string().str;
      });
      this->context_ptr = new rcl_context_t;
      *this->context_ptr = rcl_get_zero_initialized_context();
      ret = rcl_init(0, nullptr, &init_options, this->context_ptr);
      ASSERT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
    }
    this->node_ptr = new rcl_node_t;
    *this->node_ptr = rcl_get_zero_initialized_node();
    const char * name = "test_event_node";
    rcl_node_options_t node_options = rcl_node_get_default_options();
    ret = rcl_node_init(this->node_ptr, name, "", this->context_ptr, &node_options);
    ASSERT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
    ts = ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, Strings);
  }

  rcl_ret_t setup_publisher(const rmw_qos_profile_t qos_profile)
  {
    // init publisher
    publisher = rcl_get_zero_initialized_publisher();
    rcl_publisher_options_t publisher_options = rcl_publisher_get_default_options();
    publisher_options.qos = qos_profile;
    return rcl_publisher_init(
      &publisher,
      this->node_ptr,
      this->ts,
      this->topic,
      &publisher_options);
  }

  rcl_ret_t setup_subscriber(const rmw_qos_profile_t qos_profile)
  {
    // init publisher
    subscription = rcl_get_zero_initialized_subscription();
    rcl_subscription_options_t subscription_options = rcl_subscription_get_default_options();
    subscription_options.qos = qos_profile;
    return rcl_subscription_init(
      &subscription,
      this->node_ptr,
      this->ts,
      this->topic,
      &subscription_options);
  }

  void setup_publisher_subscriber(
    const rmw_qos_profile_t pub_qos_profile,
    const rmw_qos_profile_t sub_qos_profile)
  {
    rcl_ret_t ret;

    // init publisher
    ret = setup_publisher(pub_qos_profile);
    ASSERT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;

    // init subscription
    ret = setup_subscriber(sub_qos_profile);
    ASSERT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  }

  void setup_publisher_subscriber_events(
    const rcl_publisher_event_type_t & pub_event_type,
    const rcl_subscription_event_type_t & sub_event_type)
  {
    rcl_ret_t ret;

    // init publisher events
    publisher_event = rcl_get_zero_initialized_event();
    ret = rcl_publisher_event_init(&publisher_event, &publisher, pub_event_type);
    ASSERT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;

    // init subscription event
    subscription_event = rcl_get_zero_initialized_event();
    ret = rcl_subscription_event_init(&subscription_event, &subscription, sub_event_type);
    ASSERT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  }

  void setup_publisher_subscriber_and_events_and_assert_discovery(
    const rcl_publisher_event_type_t & pub_event_type,
    const rcl_subscription_event_type_t & sub_event_type)
  {
    setup_publisher_subscriber(default_qos_profile, default_qos_profile);
    setup_publisher_subscriber_events(pub_event_type, sub_event_type);

    rcl_ret_t ret;
    // wait for discovery, time out after 10s
    static const size_t max_iterations = 1000;
    static const auto wait_period = 10ms;
    bool subscribe_success = false;
    for (size_t i = 0; i < max_iterations; ++i) {
      size_t subscription_count = 0;
      size_t publisher_count = 0;
      ret = rcl_subscription_get_publisher_count(&subscription, &publisher_count);
      EXPECT_OK(ret);
      ret = rcl_publisher_get_subscription_count(&publisher, &subscription_count);
      EXPECT_OK(ret);
      if (subscription_count && publisher_count) {
        subscribe_success = true;
        break;
      }
      std::this_thread::sleep_for(wait_period);
    }
    ASSERT_TRUE(subscribe_success) << "Publisher/Subscription discovery timed out";
  }

  void tear_down_publisher_subscriber()
  {
    rcl_ret_t ret;

    ret = rcl_subscription_fini(&subscription, this->node_ptr);
    EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;

    ret = rcl_publisher_fini(&publisher, this->node_ptr);
    EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  }

  void tear_down_publisher_subscriber_events()
  {
    rcl_ret_t ret;

    ret = rcl_event_fini(&subscription_event);
    EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;

    ret = rcl_event_fini(&publisher_event);
    EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  }

  void TearDown()
  {
    rcl_ret_t ret;

    ret = rcl_node_fini(this->node_ptr);
    delete this->node_ptr;
    EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;

    ret = rcl_shutdown(this->context_ptr);
    EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;

    ret = rcl_context_fini(this->context_ptr);
    delete this->context_ptr;
    EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  }

  struct PrintToStringParamName
  {
    template<class ParamType>
    std::string operator()(const ::testing::TestParamInfo<ParamType> & info) const
    {
      const auto & test_params = static_cast<const TestIncompatibleQosEventParams &>(info.param);
      return test_params.testcase_name;
    }
  };

  static const rmw_qos_profile_t default_qos_profile;

protected:
  rcl_context_t * context_ptr;
  rcl_node_t * node_ptr;

  rcl_publisher_t publisher;
  rcl_event_t publisher_event;
  rcl_subscription_t subscription;
  rcl_event_t subscription_event;
  const char * topic = "rcl_test_publisher_subscription_events";
  const rosidl_message_type_support_t * ts;
};

using TestEventFixture = CLASSNAME(TestEventFixture, RMW_IMPLEMENTATION);

const rmw_qos_profile_t TestEventFixture::default_qos_profile = {
  RMW_QOS_POLICY_HISTORY_KEEP_LAST,             // history
  0,                                            // depth
  RMW_QOS_POLICY_RELIABILITY_BEST_EFFORT,       // reliability
  RMW_QOS_POLICY_DURABILITY_SYSTEM_DEFAULT,     // durability
  {DEADLINE_PERIOD_IN_S.count(), 0},            // deadline
  {0, 0},                                       // lifespan
  RMW_QOS_POLICY_LIVELINESS_MANUAL_BY_TOPIC,    // liveliness
  {LIVELINESS_LEASE_DURATION_IN_S.count(), 0},  // liveliness_lease_duration
  false                                         // avoid_ros_namespace_conventions
};

rcl_ret_t
wait_for_msgs_and_events(
  rcl_context_t * context,
  rcl_subscription_t * subscription,
  rcl_event_t * subscription_event,
  rcl_event_t * publisher_event,
  bool * msg_ready,
  bool * subscription_event_ready,
  bool * publisher_event_ready,
  seconds period = 1s)
{
  *msg_ready = false;
  *subscription_event_ready = false;
  *publisher_event_ready = false;

  int num_subscriptions = (nullptr == subscription ? 0 : 1);
  int num_events = (nullptr == subscription_event ? 0 : 1) + (nullptr == publisher_event ? 0 : 1);

  rcl_wait_set_t wait_set = rcl_get_zero_initialized_wait_set();
  rcl_ret_t ret = rcl_wait_set_init(
    &wait_set,
    num_subscriptions, 0, 0, 0, 0, num_events,
    context,
    rcl_get_default_allocator());
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rcl_ret_t ret = rcl_wait_set_fini(&wait_set);
    EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  });

  ret = rcl_wait_set_clear(&wait_set);
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;

  if (nullptr != subscription) {
    ret = rcl_wait_set_add_subscription(&wait_set, subscription, NULL);
    EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  }
  if (nullptr != subscription_event) {
    ret = rcl_wait_set_add_event(&wait_set, subscription_event, NULL);
    EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  }
  if (nullptr != publisher_event) {
    ret = rcl_wait_set_add_event(&wait_set, publisher_event, NULL);
    EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  }

  ret = rcl_wait(&wait_set, RCL_S_TO_NS(period.count()));
  if (ret == RCL_RET_TIMEOUT) {
    return ret;
  }
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;

  for (size_t i = 0; i < wait_set.size_of_subscriptions; ++i) {
    if (wait_set.subscriptions[i] && wait_set.subscriptions[i] == subscription) {
      *msg_ready = true;
    }
  }
  for (size_t i = 0; i < wait_set.size_of_events; ++i) {
    if (nullptr != wait_set.events[i]) {
      if (wait_set.events[i] == subscription_event) {
        *subscription_event_ready = true;
      } else if (wait_set.events[i] == publisher_event) {
        *publisher_event_ready = true;
      }
    }
  }
  ret = rcl_wait_set_fini(&wait_set);
  EXPECT_OK(ret);
  return ret;
}

/// Conditional function for determining when the wait_for_msgs_and_events loop is complete.
/**
 * Conditional function for determining when the wait_for_msgs_and_events loop is complete.
 *
 * \param msg_persist_ready true if a msg has ever been received
 * \param subscription_persist_readytrue if a subscription has been received
 * \param publisher_persist_ready true if a pulbisher event has been received
 * \return true when the desired conditions are met
 */
using WaitConditionPredicate = std::function<
  bool (
    const bool & msg_persist_ready,
    const bool & subscription_persist_ready,
    const bool & publisher_persist_ready
  )>;

// Wait for msgs and events until all conditions are met or a timeout has occured.
rcl_ret_t
conditional_wait_for_msgs_and_events(
  rcl_context_t * context,
  seconds timeout,
  const WaitConditionPredicate & events_ready,
  rcl_subscription_t * subscription,
  rcl_event_t * subscription_event,
  rcl_event_t * publisher_event,
  bool * msg_persist_ready,
  bool * subscription_persist_ready,
  bool * publisher_persist_ready)
{
  *msg_persist_ready = false;
  *subscription_persist_ready = false;
  *publisher_persist_ready = false;

  auto start_time = std::chrono::system_clock::now();
  while (std::chrono::system_clock::now() - start_time < timeout) {
    bool msg_ready, subscription_event_ready, publisher_event_ready;

    // wait for msg and events
    rcl_ret_t ret = wait_for_msgs_and_events(
      context, subscription, subscription_event, publisher_event,
      &msg_ready, &subscription_event_ready, &publisher_event_ready);
    if (RCL_RET_OK != ret) {
      continue;
    }

    *msg_persist_ready |= msg_ready;
    *subscription_persist_ready |= subscription_event_ready;
    *publisher_persist_ready |= publisher_event_ready;
    if (events_ready(*msg_persist_ready, *subscription_persist_ready, *publisher_persist_ready)) {
      return RCL_RET_OK;
    }
  }
  return RCL_RET_TIMEOUT;
}

/*
 * Basic test of publisher and subscriber deadline events, with first message sent before deadline
 */
TEST_F(TestEventFixture, test_pubsub_no_deadline_missed)
{
  setup_publisher_subscriber_and_events_and_assert_discovery(
    RCL_PUBLISHER_OFFERED_DEADLINE_MISSED,
    RCL_SUBSCRIPTION_REQUESTED_DEADLINE_MISSED);
  rcl_ret_t ret;

  // publish message to topic
  const char * test_string = "testing";
  {
    test_msgs__msg__Strings msg;
    test_msgs__msg__Strings__init(&msg);
    ASSERT_TRUE(rosidl_runtime_c__String__assign(&msg.string_value, test_string));
    ret = rcl_publish(&publisher, &msg, nullptr);
    test_msgs__msg__Strings__fini(&msg);
    EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  }

  // wait for msg and events
  bool msg_ready, subscription_event_ready, publisher_event_ready;
  rcl_ret_t wait_res = wait_for_msgs_and_events(
    context_ptr, &subscription, &subscription_event, &publisher_event,
    &msg_ready, &subscription_event_ready, &publisher_event_ready, DEADLINE_PERIOD_IN_S);
  EXPECT_EQ(wait_res, RCL_RET_OK);

  // test that the message published to topic is as expected
  EXPECT_TRUE(msg_ready);
  if (msg_ready) {
    test_msgs__msg__Strings msg;
    test_msgs__msg__Strings__init(&msg);
    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      test_msgs__msg__Strings__fini(&msg);
    });
    ret = rcl_take(&subscription, &msg, nullptr, nullptr);
    EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
    EXPECT_EQ(std::string(msg.string_value.data, msg.string_value.size), std::string(test_string));
  }

  // test subscriber/datareader deadline missed status
  EXPECT_FALSE(subscription_event_ready);
  {
    rmw_requested_deadline_missed_status_t deadline_status;
    ret = rcl_take_event(&subscription_event, &deadline_status);
    EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
    EXPECT_EQ(deadline_status.total_count, 0);
    EXPECT_EQ(deadline_status.total_count_change, 0);
  }

  // test publisher/datawriter deadline missed status
  EXPECT_FALSE(publisher_event_ready);
  {
    rmw_offered_deadline_missed_status_t deadline_status;
    ret = rcl_take_event(&publisher_event, &deadline_status);
    EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
    EXPECT_EQ(deadline_status.total_count, 0);
    EXPECT_EQ(deadline_status.total_count_change, 0);
  }

  // clean up
  tear_down_publisher_subscriber_events();
  tear_down_publisher_subscriber();
}

/*
 * Basic test of publisher and subscriber deadline events, with first message sent after deadline
 */
TEST_F(TestEventFixture, test_pubsub_deadline_missed)
{
  setup_publisher_subscriber_and_events_and_assert_discovery(
    RCL_PUBLISHER_OFFERED_DEADLINE_MISSED,
    RCL_SUBSCRIPTION_REQUESTED_DEADLINE_MISSED);
  rcl_ret_t ret;

  // publish message to topic
  const char * test_string = "testing";
  {
    test_msgs__msg__Strings msg;
    test_msgs__msg__Strings__init(&msg);
    ASSERT_TRUE(rosidl_runtime_c__String__assign(&msg.string_value, test_string));
    ret = rcl_publish(&publisher, &msg, nullptr);
    test_msgs__msg__Strings__fini(&msg);
    EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  }

  WaitConditionPredicate all_ready = [](
    const bool & msg_persist_ready,
    const bool & subscription_persist_ready,
    const bool & publisher_persist_ready) {
      return msg_persist_ready && subscription_persist_ready && publisher_persist_ready;
    };
  bool msg_persist_ready, subscription_persist_ready, publisher_persist_ready;
  rcl_ret_t wait_res = conditional_wait_for_msgs_and_events(
    context_ptr, MAX_WAIT_PER_TESTCASE, all_ready,
    &subscription, &subscription_event, &publisher_event,
    &msg_persist_ready, &subscription_persist_ready, &publisher_persist_ready);
  EXPECT_EQ(wait_res, RCL_RET_OK);

  // test that the message published to topic is as expected
  EXPECT_TRUE(msg_persist_ready);
  if (msg_persist_ready) {
    test_msgs__msg__Strings msg;
    test_msgs__msg__Strings__init(&msg);
    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      test_msgs__msg__Strings__fini(&msg);
    });
    ret = rcl_take(&subscription, &msg, nullptr, nullptr);
    EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
    EXPECT_EQ(
      std::string(msg.string_value.data, msg.string_value.size),
      std::string(test_string));
  }

  // test subscriber/datareader deadline missed status
  EXPECT_TRUE(subscription_persist_ready);
  if (subscription_persist_ready) {
    rmw_requested_deadline_missed_status_t requested_deadline_status;
    ret = rcl_take_event(&subscription_event, &requested_deadline_status);
    EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
    EXPECT_EQ(requested_deadline_status.total_count, 1);
    EXPECT_EQ(requested_deadline_status.total_count_change, 1);
  }

  // test publisher/datawriter deadline missed status
  EXPECT_TRUE(publisher_persist_ready);
  if (publisher_persist_ready) {
    rmw_offered_deadline_missed_status_t offered_deadline_status;
    ret = rcl_take_event(&publisher_event, &offered_deadline_status);
    EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
    EXPECT_EQ(offered_deadline_status.total_count, 1);
    EXPECT_EQ(offered_deadline_status.total_count_change, 1);
  }

  // clean up
  tear_down_publisher_subscriber_events();
  tear_down_publisher_subscriber();
}

/*
 * Basic test of publisher and subscriber liveliness events, with publisher killed
 */
TEST_F(TestEventFixture, test_pubsub_liveliness_kill_pub)
{
  setup_publisher_subscriber_and_events_and_assert_discovery(
    RCL_PUBLISHER_LIVELINESS_LOST,
    RCL_SUBSCRIPTION_LIVELINESS_CHANGED);
  rcl_ret_t ret;

  // publish message to topic
  const char * test_string = "testing";
  {
    test_msgs__msg__Strings msg;
    test_msgs__msg__Strings__init(&msg);
    ASSERT_TRUE(rosidl_runtime_c__String__assign(&msg.string_value, test_string));
    ret = rcl_publish(&publisher, &msg, nullptr);
    test_msgs__msg__Strings__fini(&msg);
    EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  }

  std::this_thread::sleep_for(2 * LIVELINESS_LEASE_DURATION_IN_S);

  WaitConditionPredicate all_ready = [](
    const bool & msg_persist_ready,
    const bool & subscription_persist_ready,
    const bool & publisher_persist_ready) {
      return msg_persist_ready && subscription_persist_ready && publisher_persist_ready;
    };
  bool msg_persist_ready, subscription_persist_ready, publisher_persist_ready;
  rcl_ret_t wait_res = conditional_wait_for_msgs_and_events(
    context_ptr, MAX_WAIT_PER_TESTCASE, all_ready,
    &subscription, &subscription_event, &publisher_event,
    &msg_persist_ready, &subscription_persist_ready, &publisher_persist_ready);
  EXPECT_EQ(wait_res, RCL_RET_OK);

  // test that the message published to topic is as expected
  EXPECT_TRUE(msg_persist_ready);
  if (msg_persist_ready) {
    test_msgs__msg__Strings msg;
    test_msgs__msg__Strings__init(&msg);
    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      test_msgs__msg__Strings__fini(&msg);
    });
    ret = rcl_take(&subscription, &msg, nullptr, nullptr);
    EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
    EXPECT_EQ(
      std::string(msg.string_value.data, msg.string_value.size),
      std::string(test_string));
  }

  // test subscriber/datareader liveliness changed status
  EXPECT_TRUE(subscription_persist_ready);
  if (subscription_persist_ready) {
    rmw_liveliness_changed_status_t liveliness_status;
    ret = rcl_take_event(&subscription_event, &liveliness_status);
    EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
    EXPECT_EQ(liveliness_status.alive_count, 0);
    EXPECT_EQ(liveliness_status.alive_count_change, 0);
    EXPECT_EQ(liveliness_status.not_alive_count, 1);
    EXPECT_EQ(liveliness_status.not_alive_count_change, 1);
  }

  // test that the killed publisher/datawriter has no active events
  EXPECT_TRUE(publisher_persist_ready);
  if (publisher_persist_ready) {
    rmw_liveliness_lost_status_t liveliness_status;
    ret = rcl_take_event(&publisher_event, &liveliness_status);
    EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
    EXPECT_EQ(liveliness_status.total_count, 1);
    EXPECT_EQ(liveliness_status.total_count_change, 1);
  }

  // clean up
  tear_down_publisher_subscriber_events();
  tear_down_publisher_subscriber();
}

/*
 * Basic test of publisher and subscriber incompatible qos callback events.
 */
TEST_P(TestEventFixture, test_pubsub_incompatible_qos)
{
  const auto & input = GetParam();
  const auto & qos_policy_kind = input.qos_policy_kind;
  const auto & publisher_qos_profile = input.publisher_qos_profile;
  const auto & subscription_qos_profile = input.subscription_qos_profile;
  const auto & error_msg = input.error_msg;

  setup_publisher_subscriber(publisher_qos_profile, subscription_qos_profile);
  setup_publisher_subscriber_events(
    RCL_PUBLISHER_OFFERED_INCOMPATIBLE_QOS,
    RCL_SUBSCRIPTION_REQUESTED_INCOMPATIBLE_QOS);

  WaitConditionPredicate events_ready = [](
    const bool & /*msg_persist_ready*/,
    const bool & subscription_persist_ready,
    const bool & publisher_persist_ready) {
      return subscription_persist_ready && publisher_persist_ready;
    };
  bool msg_persist_ready, subscription_persist_ready, publisher_persist_ready;
  rcl_ret_t wait_res = conditional_wait_for_msgs_and_events(
    context_ptr, MAX_WAIT_PER_TESTCASE, events_ready,
    &subscription, &subscription_event, &publisher_event,
    &msg_persist_ready, &subscription_persist_ready, &publisher_persist_ready);
  EXPECT_EQ(wait_res, RCL_RET_OK);

  // test that the subscriber/datareader discovered an incompatible publisher/datawriter
  EXPECT_TRUE(subscription_persist_ready);
  if (subscription_persist_ready) {
    rmw_requested_qos_incompatible_event_status_t requested_incompatible_qos_status;
    rcl_ret_t ret = rcl_take_event(&subscription_event, &requested_incompatible_qos_status);
    EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
    const auto & pub_total_count = requested_incompatible_qos_status.total_count;
    const auto & pub_total_count_change = requested_incompatible_qos_status.total_count_change;
    const auto & pub_last_policy_kind = requested_incompatible_qos_status.last_policy_kind;
    if (pub_total_count != 0 && pub_total_count_change != 0 && pub_last_policy_kind != 0) {
      EXPECT_EQ(pub_total_count, 1) << error_msg;
      EXPECT_EQ(pub_total_count_change, 1) << error_msg;
      EXPECT_EQ(pub_last_policy_kind, qos_policy_kind) << error_msg;
    } else {
      ADD_FAILURE() << "Subscription incompatible qos event timed out for: " << error_msg;
    }
  }

  // test that the publisher/datawriter discovered an incompatible subscription/datareader
  EXPECT_TRUE(publisher_persist_ready);
  if (publisher_persist_ready) {
    rmw_offered_qos_incompatible_event_status_t offered_incompatible_qos_status;
    rcl_ret_t ret = rcl_take_event(&publisher_event, &offered_incompatible_qos_status);
    EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
    const auto & sub_total_count = offered_incompatible_qos_status.total_count;
    const auto & sub_total_count_change = offered_incompatible_qos_status.total_count_change;
    const auto & sub_last_policy_kind = offered_incompatible_qos_status.last_policy_kind;
    if (sub_total_count != 0 && sub_total_count_change != 0 && sub_last_policy_kind != 0) {
      EXPECT_EQ(sub_total_count, 1) << error_msg;
      EXPECT_EQ(sub_total_count_change, 1) << error_msg;
      EXPECT_EQ(sub_last_policy_kind, qos_policy_kind) << error_msg;
    } else {
      ADD_FAILURE() << "Publisher incompatible qos event timed out for: " << error_msg;
    }
  }

  // clean up
  tear_down_publisher_subscriber_events();
  tear_down_publisher_subscriber();
}

/*
 * Passing bad param subscriber/publisher event ini
 */
TEST_F(TestEventFixture, test_bad_event_ini)
{
  setup_publisher_subscriber(default_qos_profile, default_qos_profile);
  const rcl_subscription_event_type_t unknown_sub_type = (rcl_subscription_event_type_t) 5432;
  const rcl_publisher_event_type_t unknown_pub_type = (rcl_publisher_event_type_t) 5432;

  publisher_event = rcl_get_zero_initialized_event();
  rcl_ret_t ret = rcl_publisher_event_init(
    &publisher_event,
    &publisher,
    unknown_pub_type);
  EXPECT_EQ(ret, RCL_RET_INVALID_ARGUMENT);

  subscription_event = rcl_get_zero_initialized_event();
  ret = rcl_subscription_event_init(
    &subscription_event,
    &subscription,
    unknown_sub_type);
  EXPECT_EQ(ret, RCL_RET_INVALID_ARGUMENT);

  tear_down_publisher_subscriber();
}

/*
 * Test cases for the event_is_valid function
 */
TEST_F(TestEventFixture, test_event_is_valid)
{
  EXPECT_FALSE(rcl_event_is_valid(nullptr));
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  setup_publisher_subscriber(default_qos_profile, default_qos_profile);
  rcl_event_t publisher_event_test = rcl_get_zero_initialized_event();
  EXPECT_FALSE(rcl_event_is_valid(&publisher_event_test));
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  rcl_ret_t ret = rcl_publisher_event_init(
    &publisher_event_test, &publisher, RCL_PUBLISHER_OFFERED_DEADLINE_MISSED);
  ASSERT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  EXPECT_TRUE(rcl_event_is_valid(&publisher_event_test));

  rmw_event_type_t saved_event_type = publisher_event_test.impl->rmw_handle.event_type;
  publisher_event_test.impl->rmw_handle.event_type = RMW_EVENT_INVALID;
  EXPECT_FALSE(rcl_event_is_valid(&publisher_event_test));
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();
  publisher_event_test.impl->rmw_handle.event_type = saved_event_type;

  rcl_allocator_t saved_alloc = publisher_event_test.impl->allocator;
  rcl_allocator_t bad_alloc = rcutils_get_zero_initialized_allocator();
  publisher_event_test.impl->allocator = bad_alloc;
  EXPECT_FALSE(rcl_event_is_valid(&publisher_event_test));
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();
  publisher_event_test.impl->allocator = saved_alloc;

  ret = rcl_event_fini(&publisher_event_test);
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  tear_down_publisher_subscriber();
}

/*
 * Test passing not init to take_event/get_handle
 */
TEST_F(TestEventFixture, test_event_is_invalid) {
  // nullptr
  rmw_offered_deadline_missed_status_t deadline_status;
  EXPECT_EQ(RCL_RET_EVENT_INVALID, rcl_take_event(NULL, &deadline_status));
  EXPECT_EQ(NULL, rcl_event_get_rmw_handle(NULL));

  // Zero Init, invalid
  rcl_event_t publisher_event_test = rcl_get_zero_initialized_event();
  EXPECT_EQ(RCL_RET_EVENT_INVALID, rcl_take_event(&publisher_event_test, &deadline_status));
  EXPECT_EQ(NULL, rcl_event_get_rmw_handle(&publisher_event_test));
}

/*
 * Basic test subscriber event message lost
 */
TEST_F(TestEventFixture, test_sub_message_lost_event)
{
  const rmw_qos_profile_t subscription_qos_profile = default_qos_profile;

  rcl_ret_t ret = setup_subscriber(subscription_qos_profile);
  ASSERT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;

  subscription_event = rcl_get_zero_initialized_event();
  ret = rcl_subscription_event_init(
    &subscription_event,
    &subscription,
    RCL_SUBSCRIPTION_MESSAGE_LOST);
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    ret = rcl_event_fini(&subscription_event);
    EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
    ret = rcl_subscription_fini(&subscription, this->node_ptr);
    EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  });

  EXPECT_EQ(ret, RCL_RET_OK);

  // Can't reproduce reliably this event
  // Test that take_event is able to read the configured event
  rmw_message_lost_status_t message_lost_status;
  ret = rcl_take_event(&subscription_event, &message_lost_status);
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  EXPECT_EQ(message_lost_status.total_count, 0u);
  EXPECT_EQ(message_lost_status.total_count_change, 0u);
}

static
std::array<TestIncompatibleQosEventParams, 5>
get_test_pubsub_incompatible_qos_inputs()
{
  // an array of tuples that holds the expected qos_policy_kind, publisher qos profile,
  // subscription qos profile and the error message, in that order.
  std::array<TestIncompatibleQosEventParams, 5> inputs;

  // durability
  inputs[0].testcase_name = "IncompatibleQoS_Durability";
  inputs[0].qos_policy_kind = RMW_QOS_POLICY_DURABILITY;
  inputs[0].publisher_qos_profile = TestEventFixture::default_qos_profile;
  inputs[0].publisher_qos_profile.durability = RMW_QOS_POLICY_DURABILITY_VOLATILE;
  inputs[0].subscription_qos_profile = TestEventFixture::default_qos_profile;
  inputs[0].subscription_qos_profile.durability = RMW_QOS_POLICY_DURABILITY_TRANSIENT_LOCAL;
  inputs[0].error_msg = "Incompatible qos durability";

  // deadline
  inputs[1].testcase_name = "IncompatibleQoS_Deadline";
  inputs[1].qos_policy_kind = RMW_QOS_POLICY_DEADLINE;
  inputs[1].publisher_qos_profile = TestEventFixture::default_qos_profile;
  inputs[1].publisher_qos_profile.deadline = {DEADLINE_PERIOD_IN_S.count() + 5, 0};
  inputs[1].subscription_qos_profile = TestEventFixture::default_qos_profile;
  inputs[1].subscription_qos_profile.deadline = {DEADLINE_PERIOD_IN_S.count(), 0};
  inputs[1].error_msg = "Incompatible qos deadline";

  // liveliness
  inputs[2].testcase_name = "IncompatibleQoS_LivelinessPolicy";
  inputs[2].qos_policy_kind = RMW_QOS_POLICY_LIVELINESS;
  inputs[2].publisher_qos_profile = TestEventFixture::default_qos_profile;
  inputs[2].publisher_qos_profile.liveliness = RMW_QOS_POLICY_LIVELINESS_AUTOMATIC;
  inputs[2].subscription_qos_profile = TestEventFixture::default_qos_profile;
  inputs[2].subscription_qos_profile.liveliness = RMW_QOS_POLICY_LIVELINESS_MANUAL_BY_TOPIC;
  inputs[2].error_msg = "Incompatible qos liveliness policy";

  // liveliness lease duration
  inputs[3].testcase_name = "IncompatibleQoS_LivelinessLeaseDuration";
  inputs[3].qos_policy_kind = RMW_QOS_POLICY_LIVELINESS;
  inputs[3].publisher_qos_profile = TestEventFixture::default_qos_profile;
  inputs[3].publisher_qos_profile.liveliness_lease_duration = {DEADLINE_PERIOD_IN_S.count() + 5, 0};
  inputs[3].subscription_qos_profile = TestEventFixture::default_qos_profile;
  inputs[3].subscription_qos_profile.liveliness_lease_duration = {DEADLINE_PERIOD_IN_S.count(), 0};
  inputs[3].error_msg = "Incompatible qos liveliness lease duration";

  // reliability
  inputs[4].testcase_name = "IncompatibleQoS_Reliability";
  inputs[4].qos_policy_kind = RMW_QOS_POLICY_RELIABILITY;
  inputs[4].publisher_qos_profile = TestEventFixture::default_qos_profile;
  inputs[4].publisher_qos_profile.reliability = RMW_QOS_POLICY_RELIABILITY_BEST_EFFORT;
  inputs[4].subscription_qos_profile = TestEventFixture::default_qos_profile;
  inputs[4].subscription_qos_profile.reliability = RMW_QOS_POLICY_RELIABILITY_RELIABLE;
  inputs[4].error_msg = "Incompatible qos reliability";

  return inputs;
}

INSTANTIATE_TEST_SUITE_P(
  TestPubSubIncompatibilityWithDifferentQosSettings,
  TestEventFixture,
  ::testing::ValuesIn(get_test_pubsub_incompatible_qos_inputs()),
  TestEventFixture::PrintToStringParamName());
