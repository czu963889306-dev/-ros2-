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
#include "rcutils/testing/fault_injection.h"

#include "rmw/rmw.h"
#include "rmw/error_handling.h"
#include "rmw/event.h"

#include "test_msgs/msg/basic_types.h"
#include "test_msgs/srv/basic_types.h"

#ifdef RMW_IMPLEMENTATION
# define CLASSNAME_(NAME, SUFFIX) NAME ## __ ## SUFFIX
# define CLASSNAME(NAME, SUFFIX) CLASSNAME_(NAME, SUFFIX)
#else
# define CLASSNAME(NAME, SUFFIX) NAME
#endif

class CLASSNAME (TestWaitSet, RMW_IMPLEMENTATION) : public ::testing::Test
{
protected:
  void SetUp() override
  {
    rmw_init_options_t options = rmw_get_zero_initialized_init_options();
    rmw_ret_t ret = rmw_init_options_init(&options, rcutils_get_default_allocator());
    ASSERT_EQ(RMW_RET_OK, ret) << rcutils_get_error_string().str;
    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
    {
      rmw_ret_t ret = rmw_init_options_fini(&options);
      EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
    });
    options.enclave = rcutils_strdup("/", rcutils_get_default_allocator());
    ASSERT_STREQ("/", options.enclave);
    context = rmw_get_zero_initialized_context();
    ret = rmw_init(&options, &context);
    ASSERT_EQ(RMW_RET_OK, ret) << rcutils_get_error_string().str;
  }

  void TearDown() override
  {
    rmw_ret_t ret = rmw_shutdown(&context);
    EXPECT_EQ(RMW_RET_OK, ret) << rcutils_get_error_string().str;
    ret = rmw_context_fini(&context);
    EXPECT_EQ(RMW_RET_OK, ret) << rcutils_get_error_string().str;
  }

  rmw_context_t context;
};

TEST_F(CLASSNAME(TestWaitSet, RMW_IMPLEMENTATION), rmw_create_wait_set)
{
  // Created a valid wait_set
  rmw_wait_set_t * wait_set = rmw_create_wait_set(&context, 0);
  ASSERT_NE(nullptr, wait_set) << rcutils_get_error_string().str;
  rmw_reset_error();

  // Destroyed a valid wait_set
  rmw_ret_t ret = rmw_destroy_wait_set(wait_set);
  EXPECT_EQ(ret, RMW_RET_OK) << rcutils_get_error_string().str;

  // Try to create a wait_set using a invalid argument
  wait_set = rmw_create_wait_set(nullptr, 0);
  EXPECT_EQ(wait_set, nullptr) << rcutils_get_error_string().str;
  rmw_reset_error();

  // Battle test rmw_create_wait_set.
  RCUTILS_FAULT_INJECTION_TEST(
  {
    wait_set = rmw_create_wait_set(&context, 0);

    if (wait_set) {
      RCUTILS_NO_FAULT_INJECTION(
      {
        ret = rmw_destroy_wait_set(wait_set);
        EXPECT_EQ(ret, RMW_RET_OK) << rcutils_get_error_string().str;
      });
    } else {
      rmw_reset_error();
    }
  });
}

class CLASSNAME (TestWaitSetUse, RMW_IMPLEMENTATION)
  : public CLASSNAME(TestWaitSet, RMW_IMPLEMENTATION)
{
protected:
  using Base = CLASSNAME(TestWaitSet, RMW_IMPLEMENTATION);

  void SetUp() override
  {
    Base::SetUp();
    constexpr char node_name[] = "my_node";
    constexpr char node_namespace[] = "/my_ns";
    node = rmw_create_node(&context, node_name, node_namespace);
    ASSERT_NE(nullptr, node) << rmw_get_error_string().str;
    constexpr char topic_name[] = "/test";
    const rosidl_message_type_support_t * message_ts =
      ROSIDL_GET_MSG_TYPE_SUPPORT(test_msgs, msg, BasicTypes);
    rmw_subscription_options_t sub_options = rmw_get_default_subscription_options();
    sub = rmw_create_subscription(
      node, message_ts, topic_name, &rmw_qos_profile_default, &sub_options);
    ASSERT_NE(nullptr, sub) << rmw_get_error_string().str;
    gc = rmw_create_guard_condition(&context);
    ASSERT_NE(nullptr, gc) << rmw_get_error_string().str;
    constexpr char service_name[] = "/test";
    const rosidl_service_type_support_t * service_ts =
      ROSIDL_GET_SRV_TYPE_SUPPORT(test_msgs, srv, BasicTypes);
    srv = rmw_create_service(node, service_ts, service_name, &rmw_qos_profile_default);
    ASSERT_NE(nullptr, srv) << rmw_get_error_string().str;
    client = rmw_create_client(node, service_ts, service_name, &rmw_qos_profile_default);
    ASSERT_NE(nullptr, client) << rmw_get_error_string().str;
    rmw_ret_t ret = rmw_subscription_event_init(&event, sub, RMW_EVENT_LIVELINESS_CHANGED);
    ASSERT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
  }

  void TearDown() override
  {
    rmw_ret_t ret = rmw_event_fini(&event);
    EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
    ret = rmw_destroy_client(node, client);
    EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
    ret = rmw_destroy_service(node, srv);
    EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
    ret = rmw_destroy_subscription(node, sub);
    EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
    ret = rmw_destroy_guard_condition(gc);
    EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
    ret = rmw_destroy_node(node);
    EXPECT_EQ(RMW_RET_OK, ret) << rmw_get_error_string().str;
    Base::TearDown();
  }

  rmw_node_t * node{nullptr};
  rmw_subscription_t * sub{nullptr};
  rmw_guard_condition_t * gc{nullptr};
  rmw_service_t * srv{nullptr};
  rmw_client_t * client{nullptr};
  rmw_event_t event{rmw_get_zero_initialized_event()};
};

// Macro to initialize and manage rmw_wait input arrays.
// Requires `size` to a constant expression.
#define INITIALIZE_ARRAY(var_name, internal_var_name, size) \
  void * var_name ## _storage[size]; \
  var_name.internal_var_name ## s = var_name ## _storage; \
  var_name.internal_var_name ## _count = size;

TEST_F(CLASSNAME(TestWaitSetUse, RMW_IMPLEMENTATION), rmw_wait)
{
  constexpr size_t number_of_subscriptions = 1u;
  constexpr size_t number_of_guard_conditions = 1u;
  constexpr size_t number_of_clients = 1u;
  constexpr size_t number_of_services = 1u;
  constexpr size_t number_of_events = 1u;
  constexpr size_t num_conditions =
    number_of_subscriptions +
    number_of_guard_conditions +
    number_of_clients +
    number_of_services +
    number_of_events;

  // Created a valid wait_set
  rmw_wait_set_t * wait_set = rmw_create_wait_set(&context, num_conditions);
  ASSERT_NE(nullptr, wait_set) << rmw_get_error_string().str;
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rmw_ret_t ret = rmw_destroy_wait_set(wait_set);
    EXPECT_EQ(ret, RMW_RET_OK) << rcutils_get_error_string().str;
  });

  // Call rmw_wait with invalid arguments
  rmw_ret_t ret = rmw_wait(nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
  EXPECT_EQ(ret, RMW_RET_INVALID_ARGUMENT) << rcutils_get_error_string().str;
  rmw_reset_error();

  // Created two timeout,
  // - Equal to zero: do not block -- check only for immediately available entities.
  // - 100ms: This is represents the maximum amount of time to wait for an entity to become ready.
  rmw_time_t timeout_argument = {0, 100000000};  // 100ms
  rmw_time_t timeout_argument_zero = {0, 0};

  rmw_subscriptions_t subscriptions;
  rmw_guard_conditions_t guard_conditions;
  rmw_services_t services;
  rmw_clients_t clients;
  rmw_events_t events;
  INITIALIZE_ARRAY(subscriptions, subscriber, number_of_subscriptions);
  INITIALIZE_ARRAY(guard_conditions, guard_condition, number_of_guard_conditions);
  INITIALIZE_ARRAY(services, service, number_of_services);
  INITIALIZE_ARRAY(clients, client, number_of_clients);
  INITIALIZE_ARRAY(events, event, number_of_events);

  const char * implementation_identifier = wait_set->implementation_identifier;
  wait_set->implementation_identifier = "not-an-rmw-implementation-identifier";
  subscriptions.subscribers[0] = sub->data;
  guard_conditions.guard_conditions[0] = gc->data;
  services.services[0] = srv->data;
  clients.clients[0] = client->data;
  events.events[0] = &event;
  ret = rmw_wait(
    &subscriptions, &guard_conditions, &services, &clients, &events, wait_set,
    &timeout_argument);
  wait_set->implementation_identifier = implementation_identifier;
  EXPECT_EQ(ret, RMW_RET_INCORRECT_RMW_IMPLEMENTATION) << rcutils_get_error_string().str;
  EXPECT_NE(nullptr, subscriptions.subscribers[0]);  // left unchanged
  EXPECT_NE(nullptr, guard_conditions.guard_conditions[0]);
  EXPECT_NE(nullptr, services.services[0]);
  EXPECT_NE(nullptr, clients.clients[0]);
  EXPECT_NE(nullptr, events.events[0]);
  rmw_reset_error();

  // Used a valid wait_set
  ret = rmw_wait(nullptr, nullptr, nullptr, nullptr, nullptr, wait_set, &timeout_argument);
  EXPECT_EQ(ret, RMW_RET_TIMEOUT) << rcutils_get_error_string().str;
  rmw_reset_error();

  ret = rmw_wait(nullptr, nullptr, nullptr, nullptr, nullptr, wait_set, &timeout_argument_zero);
  EXPECT_EQ(ret, RMW_RET_TIMEOUT) << rcutils_get_error_string().str;
  rmw_reset_error();

  // Used a valid wait_set and subscription with 100ms timeout
  subscriptions.subscribers[0] = sub->data;
  ret = rmw_wait(&subscriptions, nullptr, nullptr, nullptr, nullptr, wait_set, &timeout_argument);
  EXPECT_EQ(ret, RMW_RET_TIMEOUT) << rcutils_get_error_string().str;
  EXPECT_EQ(nullptr, subscriptions.subscribers[0]);
  rmw_reset_error();

  // Used a valid wait_set and subscription with no timeout
  subscriptions.subscribers[0] = sub->data;
  ret = rmw_wait(
    &subscriptions, nullptr, nullptr, nullptr, nullptr, wait_set,
    &timeout_argument_zero);
  EXPECT_EQ(ret, RMW_RET_TIMEOUT) << rcutils_get_error_string().str;
  EXPECT_EQ(nullptr, subscriptions.subscribers[0]);
  rmw_reset_error();

  // Used a valid wait_set, subscription and guard_conditions with 100ms timeout
  subscriptions.subscribers[0] = sub->data;
  guard_conditions.guard_conditions[0] = gc->data;
  ret = rmw_wait(
    &subscriptions, &guard_conditions, nullptr, nullptr, nullptr, wait_set,
    &timeout_argument);
  EXPECT_EQ(ret, RMW_RET_TIMEOUT) << rcutils_get_error_string().str;
  EXPECT_EQ(nullptr, subscriptions.subscribers[0]);
  EXPECT_EQ(nullptr, guard_conditions.guard_conditions[0]);
  rmw_reset_error();

  // Used a valid wait_set, subscription and guard_conditions with no timeout
  subscriptions.subscribers[0] = sub->data;
  guard_conditions.guard_conditions[0] = gc->data;
  ret = rmw_wait(
    &subscriptions, &guard_conditions, nullptr, nullptr, nullptr, wait_set,
    &timeout_argument_zero);
  EXPECT_EQ(ret, RMW_RET_TIMEOUT) << rcutils_get_error_string().str;
  EXPECT_EQ(nullptr, subscriptions.subscribers[0]);
  EXPECT_EQ(nullptr, guard_conditions.guard_conditions[0]);
  rmw_reset_error();

  // Used a valid wait_set, subscription, guard_conditions and services with 100ms timeout
  subscriptions.subscribers[0] = sub->data;
  guard_conditions.guard_conditions[0] = gc->data;
  services.services[0] = srv->data;
  ret = rmw_wait(
    &subscriptions, &guard_conditions, &services, nullptr, nullptr, wait_set,
    &timeout_argument);
  EXPECT_EQ(ret, RMW_RET_TIMEOUT) << rcutils_get_error_string().str;
  EXPECT_EQ(nullptr, subscriptions.subscribers[0]);
  EXPECT_EQ(nullptr, guard_conditions.guard_conditions[0]);
  EXPECT_EQ(nullptr, services.services[0]);
  rmw_reset_error();

  // Used a valid wait_set, subscription, guard_conditions and services with no timeout
  subscriptions.subscribers[0] = sub->data;
  guard_conditions.guard_conditions[0] = gc->data;
  services.services[0] = srv->data;
  ret = rmw_wait(
    &subscriptions, &guard_conditions, &services, nullptr, nullptr, wait_set,
    &timeout_argument_zero);
  EXPECT_EQ(ret, RMW_RET_TIMEOUT) << rcutils_get_error_string().str;
  EXPECT_EQ(nullptr, subscriptions.subscribers[0]);
  EXPECT_EQ(nullptr, guard_conditions.guard_conditions[0]);
  EXPECT_EQ(nullptr, services.services[0]);
  rmw_reset_error();

  // Used a valid wait_set, subscription, guard_conditions, services and clients with 100ms timeout
  subscriptions.subscribers[0] = sub->data;
  guard_conditions.guard_conditions[0] = gc->data;
  services.services[0] = srv->data;
  clients.clients[0] = client->data;
  ret = rmw_wait(
    &subscriptions, &guard_conditions, &services, &clients, nullptr, wait_set,
    &timeout_argument);
  EXPECT_EQ(ret, RMW_RET_TIMEOUT) << rcutils_get_error_string().str;
  EXPECT_EQ(nullptr, subscriptions.subscribers[0]);
  EXPECT_EQ(nullptr, guard_conditions.guard_conditions[0]);
  EXPECT_EQ(nullptr, services.services[0]);
  EXPECT_EQ(nullptr, clients.clients[0]);
  rmw_reset_error();

  // Used a valid wait_set, subscription, guard_conditions, services and clients with no timeout
  subscriptions.subscribers[0] = sub->data;
  guard_conditions.guard_conditions[0] = gc->data;
  services.services[0] = srv->data;
  clients.clients[0] = client->data;
  ret = rmw_wait(
    &subscriptions, &guard_conditions, &services, &clients, nullptr, wait_set,
    &timeout_argument_zero);
  EXPECT_EQ(ret, RMW_RET_TIMEOUT) << rcutils_get_error_string().str;
  EXPECT_EQ(nullptr, subscriptions.subscribers[0]);
  EXPECT_EQ(nullptr, guard_conditions.guard_conditions[0]);
  EXPECT_EQ(nullptr, services.services[0]);
  EXPECT_EQ(nullptr, clients.clients[0]);
  rmw_reset_error();

  // Used a valid wait_set, subscription, guard_conditions, services, clients and events with 100ms
  // timeout
  subscriptions.subscribers[0] = sub->data;
  guard_conditions.guard_conditions[0] = gc->data;
  services.services[0] = srv->data;
  clients.clients[0] = client->data;
  events.events[0] = &event;
  ret = rmw_wait(
    &subscriptions, &guard_conditions, &services, &clients, &events, wait_set,
    &timeout_argument);
  EXPECT_EQ(ret, RMW_RET_TIMEOUT) << rcutils_get_error_string().str;
  EXPECT_EQ(nullptr, subscriptions.subscribers[0]);
  EXPECT_EQ(nullptr, guard_conditions.guard_conditions[0]);
  EXPECT_EQ(nullptr, services.services[0]);
  EXPECT_EQ(nullptr, clients.clients[0]);
  EXPECT_EQ(nullptr, events.events[0]);
  rmw_reset_error();

  // Used a valid wait_set, subscription, guard_conditions, services, clients and events with no
  // timeout
  subscriptions.subscribers[0] = sub->data;
  guard_conditions.guard_conditions[0] = gc->data;
  services.services[0] = srv->data;
  clients.clients[0] = client->data;
  events.events[0] = &event;
  ret = rmw_wait(
    &subscriptions, &guard_conditions, &services, &clients, &events, wait_set,
    &timeout_argument_zero);
  EXPECT_EQ(ret, RMW_RET_TIMEOUT) << rcutils_get_error_string().str;
  EXPECT_EQ(nullptr, subscriptions.subscribers[0]);
  EXPECT_EQ(nullptr, guard_conditions.guard_conditions[0]);
  EXPECT_EQ(nullptr, services.services[0]);
  EXPECT_EQ(nullptr, clients.clients[0]);
  EXPECT_EQ(nullptr, events.events[0]);
  rmw_reset_error();

  // Battle test rmw_wait.
  RCUTILS_FAULT_INJECTION_TEST(
  {
    subscriptions.subscribers[0] = sub->data;
    guard_conditions.guard_conditions[0] = gc->data;
    services.services[0] = srv->data;
    clients.clients[0] = client->data;
    events.events[0] = &event;
    ret = rmw_wait(
      &subscriptions, &guard_conditions, &services, &clients, &events, wait_set,
      &timeout_argument);
    EXPECT_NE(RMW_RET_OK, ret);
    if (RMW_RET_TIMEOUT == ret) {
      EXPECT_EQ(nullptr, subscriptions.subscribers[0]);
      EXPECT_EQ(nullptr, guard_conditions.guard_conditions[0]);
      EXPECT_EQ(nullptr, services.services[0]);
      EXPECT_EQ(nullptr, clients.clients[0]);
      EXPECT_EQ(nullptr, events.events[0]);
    } else {
      rmw_reset_error();
    }
  });
}

TEST_F(CLASSNAME(TestWaitSet, RMW_IMPLEMENTATION), rmw_destroy_wait_set)
{
  // Try to destroy a nullptr
  rmw_ret_t ret = rmw_destroy_wait_set(nullptr);
  EXPECT_EQ(ret, RMW_RET_ERROR) << rcutils_get_error_string().str;
  rmw_reset_error();

  // Created a valid wait set
  rmw_wait_set_t * wait_set = rmw_create_wait_set(&context, 1);
  ASSERT_NE(nullptr, wait_set) << rmw_get_error_string().str;
  rmw_reset_error();

  // Keep the implementation_identifier
  const char * implementation_identifier = wait_set->implementation_identifier;

  // Use a invalid implementation_identifier
  wait_set->implementation_identifier = "not-an-rmw-implementation-identifier";
  ret = rmw_destroy_wait_set(wait_set);
  wait_set->implementation_identifier = implementation_identifier;
  EXPECT_EQ(ret, RMW_RET_INCORRECT_RMW_IMPLEMENTATION) << rcutils_get_error_string().str;
  rmw_reset_error();

  // Restored the identifier and destroy the wait_set
  ret = rmw_destroy_wait_set(wait_set);
  EXPECT_EQ(ret, RMW_RET_OK) << rcutils_get_error_string().str;
}
