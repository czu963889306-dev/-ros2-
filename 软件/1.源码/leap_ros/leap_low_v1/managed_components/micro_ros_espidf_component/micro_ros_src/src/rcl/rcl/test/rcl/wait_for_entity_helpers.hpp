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

#ifndef RCL__WAIT_FOR_ENTITY_HELPERS_HPP_
#define RCL__WAIT_FOR_ENTITY_HELPERS_HPP_

#include "rcl/client.h"
#include "rcl/service.h"
#include "rcl/rcl.h"

/// Wait for a server to be available for `client`, by trying at most `max_tries` times
/// with a `period_ms` period.
bool
wait_for_server_to_be_available(
  rcl_node_t * node,
  rcl_client_t * client,
  size_t max_tries,
  int64_t period_ms);

/// Wait for `client` to be ready, i.e. a response is available to be handled.
/// It's tried at most `max_tries` times with a period of `period_ms`.
bool
wait_for_client_to_be_ready(
  rcl_client_t * client,
  rcl_context_t * context,
  size_t max_tries,
  int64_t period_ms);

/// Wait for service to be ready, i.e. a request is available to be handled.
/// It's tried at most `max_tries` times with a period of `period_ms`.
bool
wait_for_service_to_be_ready(
  rcl_service_t * service,
  rcl_context_t * context,
  size_t max_tries,
  int64_t period_ms);

/// Wait for a publisher to get one or more established subscriptions
/// by trying at most `max_tries` times with a `period_ms` period.
bool
wait_for_established_subscription(
  const rcl_publisher_t * publisher,
  size_t max_tries,
  int64_t period_ms);

/// Wait a subscription to be ready, i.e. a message is ready to be handled,
/// by trying at least `max_tries` times with a `period_ms` period.
bool
wait_for_subscription_to_be_ready(
  rcl_subscription_t * subscription,
  rcl_context_t * context,
  size_t max_tries,
  int64_t period_ms);

#endif  // RCL__WAIT_FOR_ENTITY_HELPERS_HPP_
