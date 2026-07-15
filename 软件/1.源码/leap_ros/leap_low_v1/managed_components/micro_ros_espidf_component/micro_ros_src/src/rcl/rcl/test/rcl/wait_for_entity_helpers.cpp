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

#include "wait_for_entity_helpers.hpp"

#include <chrono>
#include <stdexcept>
#include <thread>

#include "rcutils/logging_macros.h"

#include "osrf_testing_tools_cpp/scope_exit.hpp"
#include "rcl/error_handling.h"
#include "rcl/graph.h"

bool
wait_for_server_to_be_available(
  rcl_node_t * node,
  rcl_client_t * client,
  size_t max_tries,
  int64_t period_ms)
{
  size_t iteration = 0;
  while (iteration < max_tries) {
    ++iteration;
    bool is_ready;
    rcl_ret_t ret = rcl_service_server_is_available(node, client, &is_ready);
    if (ret != RCL_RET_OK) {
      RCUTILS_LOG_ERROR_NAMED(
        ROS_PACKAGE_NAME,
        "Error in rcl_service_server_is_available: %s",
        rcl_get_error_string().str);
      return false;
    }
    if (is_ready) {
      return true;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(period_ms));
  }
  return false;
}

bool
wait_for_client_to_be_ready(
  rcl_client_t * client,
  rcl_context_t * context,
  size_t max_tries,
  int64_t period_ms)
{
  rcl_wait_set_t wait_set = rcl_get_zero_initialized_wait_set();
  rcl_ret_t ret =
    rcl_wait_set_init(&wait_set, 0, 0, 0, 1, 0, 0, context, rcl_get_default_allocator());
  if (ret != RCL_RET_OK) {
    RCUTILS_LOG_ERROR_NAMED(
      ROS_PACKAGE_NAME, "Error in wait set init: %s", rcl_get_error_string().str);
    return false;
  }
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    if (rcl_wait_set_fini(&wait_set) != RCL_RET_OK) {
      RCUTILS_LOG_ERROR_NAMED(
        ROS_PACKAGE_NAME, "Error in wait set fini: %s", rcl_get_error_string().str);
      throw std::runtime_error("error while waiting for client");
    }
  });
  size_t iteration = 0;
  while (iteration < max_tries) {
    ++iteration;
    if (rcl_wait_set_clear(&wait_set) != RCL_RET_OK) {
      RCUTILS_LOG_ERROR_NAMED(
        ROS_PACKAGE_NAME, "Error in wait_set_clear: %s", rcl_get_error_string().str);
      return false;
    }
    if (rcl_wait_set_add_client(&wait_set, client, NULL) != RCL_RET_OK) {
      RCUTILS_LOG_ERROR_NAMED(
        ROS_PACKAGE_NAME, "Error in wait_set_add_client: %s", rcl_get_error_string().str);
      return false;
    }
    ret = rcl_wait(&wait_set, RCL_MS_TO_NS(period_ms));
    if (ret == RCL_RET_TIMEOUT) {
      continue;
    }
    if (ret != RCL_RET_OK) {
      RCUTILS_LOG_ERROR_NAMED(ROS_PACKAGE_NAME, "Error in wait: %s", rcl_get_error_string().str);
      return false;
    }
    for (size_t i = 0; i < wait_set.size_of_clients; ++i) {
      if (wait_set.clients[i] && wait_set.clients[i] == client) {
        return true;
      }
    }
  }
  return false;
}

bool
wait_for_service_to_be_ready(
  rcl_service_t * service,
  rcl_context_t * context,
  size_t max_tries,
  int64_t period_ms)
{
  rcl_wait_set_t wait_set = rcl_get_zero_initialized_wait_set();
  rcl_ret_t ret =
    rcl_wait_set_init(&wait_set, 0, 0, 0, 0, 1, 0, context, rcl_get_default_allocator());
  if (ret != RCL_RET_OK) {
    RCUTILS_LOG_ERROR_NAMED(
      ROS_PACKAGE_NAME, "Error in wait set init: %s", rcl_get_error_string().str);
    return false;
  }
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    if (rcl_wait_set_fini(&wait_set) != RCL_RET_OK) {
      RCUTILS_LOG_ERROR_NAMED(
        ROS_PACKAGE_NAME, "Error in wait set fini: %s", rcl_get_error_string().str);
      throw std::runtime_error("error waiting for service to be ready");
    }
  });
  size_t iteration = 0;
  while (iteration < max_tries) {
    ++iteration;
    if (rcl_wait_set_clear(&wait_set) != RCL_RET_OK) {
      RCUTILS_LOG_ERROR_NAMED(
        ROS_PACKAGE_NAME, "Error in wait_set_clear: %s", rcl_get_error_string().str);
      return false;
    }
    if (rcl_wait_set_add_service(&wait_set, service, NULL) != RCL_RET_OK) {
      RCUTILS_LOG_ERROR_NAMED(
        ROS_PACKAGE_NAME, "Error in wait_set_add_service: %s", rcl_get_error_string().str);
      return false;
    }
    ret = rcl_wait(&wait_set, RCL_MS_TO_NS(period_ms));
    if (ret == RCL_RET_TIMEOUT) {
      continue;
    }
    if (ret != RCL_RET_OK) {
      RCUTILS_LOG_ERROR_NAMED(ROS_PACKAGE_NAME, "Error in wait: %s", rcl_get_error_string().str);
      return false;
    }
    for (size_t i = 0; i < wait_set.size_of_services; ++i) {
      if (wait_set.services[i] && wait_set.services[i] == service) {
        return true;
      }
    }
  }
  return false;
}

bool
wait_for_established_subscription(
  const rcl_publisher_t * publisher,
  size_t max_tries,
  int64_t period_ms)
{
  size_t iteration = 0;
  rcl_ret_t ret = RCL_RET_OK;
  size_t subscription_count = 0;
  while (iteration < max_tries) {
    ++iteration;
    ret = rcl_publisher_get_subscription_count(publisher, &subscription_count);
    if (ret != RCL_RET_OK) {
      RCUTILS_LOG_ERROR_NAMED(
        ROS_PACKAGE_NAME,
        "Error in rcl_publisher_get_subscription_count: %s",
        rcl_get_error_string().str);
      return false;
    }
    if (subscription_count > 0) {
      return true;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(period_ms));
  }
  return false;
}

bool
wait_for_subscription_to_be_ready(
  rcl_subscription_t * subscription,
  rcl_context_t * context,
  size_t max_tries,
  int64_t period_ms)
{
  rcl_wait_set_t wait_set = rcl_get_zero_initialized_wait_set();
  rcl_ret_t ret =
    rcl_wait_set_init(&wait_set, 1, 0, 0, 0, 0, 0, context, rcl_get_default_allocator());
  if (ret != RCL_RET_OK) {
    RCUTILS_LOG_ERROR_NAMED(
      ROS_PACKAGE_NAME, "Error in wait set init: %s", rcl_get_error_string().str);
    return false;
  }
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    if (rcl_wait_set_fini(&wait_set) != RCL_RET_OK) {
      RCUTILS_LOG_ERROR_NAMED(
        ROS_PACKAGE_NAME, "Error in wait set fini: %s", rcl_get_error_string().str);
      throw std::runtime_error("error waiting for service to be ready");
    }
  });
  size_t iteration = 0;
  while (iteration < max_tries) {
    ++iteration;
    if (rcl_wait_set_clear(&wait_set) != RCL_RET_OK) {
      RCUTILS_LOG_ERROR_NAMED(
        ROS_PACKAGE_NAME, "Error in wait_set_clear: %s", rcl_get_error_string().str);
      return false;
    }
    if (rcl_wait_set_add_subscription(&wait_set, subscription, NULL) != RCL_RET_OK) {
      RCUTILS_LOG_ERROR_NAMED(
        ROS_PACKAGE_NAME, "Error in rcl_wait_set_add_subscription: %s", rcl_get_error_string().str);
      return false;
    }
    ret = rcl_wait(&wait_set, RCL_MS_TO_NS(period_ms));
    if (ret == RCL_RET_TIMEOUT) {
      continue;
    }
    if (ret != RCL_RET_OK) {
      RCUTILS_LOG_ERROR_NAMED(ROS_PACKAGE_NAME, "Error in wait: %s", rcl_get_error_string().str);
      return false;
    }
    for (size_t i = 0; i < wait_set.size_of_subscriptions; ++i) {
      if (wait_set.subscriptions[i] && wait_set.subscriptions[i] == subscription) {
        return true;
      }
    }
  }
  return false;
}
