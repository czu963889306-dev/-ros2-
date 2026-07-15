// Copyright 2019 Robert Bosch GmbH
// Copyright 2020 Christophe Bedard
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

// Provide fake header guard for cpplint
#undef TRACETOOLS__TP_CALL_H_
#ifndef TRACETOOLS__TP_CALL_H_
#define TRACETOOLS__TP_CALL_H_

#undef TRACEPOINT_PROVIDER
#define TRACEPOINT_PROVIDER ros2

#undef TRACEPOINT_INCLUDE
#define TRACEPOINT_INCLUDE "tracetools/tp_call.h"

#if !defined(_TRACETOOLS__TP_CALL_H_) || defined(TRACEPOINT_HEADER_MULTI_READ)
#define _TRACETOOLS__TP_CALL_H_

#include <lttng/tracepoint.h>

#include <stdint.h>
#include <stdbool.h>

/// See RMW_GID_STORAGE_SIZE in rmw.
#define TRACETOOLS_GID_STORAGE_SIZE 24u

TRACEPOINT_EVENT(
  TRACEPOINT_PROVIDER,
  rcl_init,
  TP_ARGS(
    const void *, context_handle_arg
  ),
  TP_FIELDS(
    ctf_integer_hex(const void *, context_handle, context_handle_arg)
    ctf_string(version, tracetools_VERSION)
  )
)

TRACEPOINT_EVENT(
  TRACEPOINT_PROVIDER,
  rcl_node_init,
  TP_ARGS(
    const void *, node_handle_arg,
    const void *, rmw_handle_arg,
    const char *, node_name_arg,
    const char *, namespace_arg
  ),
  TP_FIELDS(
    ctf_integer_hex(const void *, node_handle, node_handle_arg)
    ctf_integer_hex(const void *, rmw_handle, rmw_handle_arg)
    ctf_string(node_name, node_name_arg)
    ctf_string(namespace, namespace_arg)
  )
)

TRACEPOINT_EVENT(
  TRACEPOINT_PROVIDER,
  rmw_publisher_init,
  TP_ARGS(
    const void *, rmw_publisher_handle_arg,
    const uint8_t *, gid_arg
  ),
  TP_FIELDS(
    ctf_integer_hex(const void *, rmw_publisher_handle, rmw_publisher_handle_arg)
    ctf_array(uint8_t, gid, gid_arg, TRACETOOLS_GID_STORAGE_SIZE)
  )
)

TRACEPOINT_EVENT(
  TRACEPOINT_PROVIDER,
  rcl_publisher_init,
  TP_ARGS(
    const void *, publisher_handle_arg,
    const void *, node_handle_arg,
    const void *, rmw_publisher_handle_arg,
    const char *, topic_name_arg,
    const size_t, queue_depth_arg
  ),
  TP_FIELDS(
    ctf_integer_hex(const void *, publisher_handle, publisher_handle_arg)
    ctf_integer_hex(const void *, node_handle, node_handle_arg)
    ctf_integer_hex(const void *, rmw_publisher_handle, rmw_publisher_handle_arg)
    ctf_string(topic_name, topic_name_arg)
    ctf_integer(const size_t, queue_depth, queue_depth_arg)
  )
)

TRACEPOINT_EVENT(
  TRACEPOINT_PROVIDER,
  rclcpp_publish,
  TP_ARGS(
    const void *, message_arg
  ),
  TP_FIELDS(
    ctf_integer_hex(const void *, message, message_arg)
  )
)

TRACEPOINT_EVENT(
  TRACEPOINT_PROVIDER,
  rcl_publish,
  TP_ARGS(
    const void *, publisher_handle_arg,
    const void *, message_arg
  ),
  TP_FIELDS(
    ctf_integer_hex(const void *, publisher_handle, publisher_handle_arg)
    ctf_integer_hex(const void *, message, message_arg)
  )
)

TRACEPOINT_EVENT(
  TRACEPOINT_PROVIDER,
  rmw_publish,
  TP_ARGS(
    const void *, message_arg
  ),
  TP_FIELDS(
    ctf_integer_hex(const void *, message, message_arg)
  )
)

TRACEPOINT_EVENT(
  TRACEPOINT_PROVIDER,
  rmw_subscription_init,
  TP_ARGS(
    const void *, rmw_subscription_handle_arg,
    const uint8_t *, gid_arg
  ),
  TP_FIELDS(
    ctf_integer_hex(const void *, rmw_subscription_handle, rmw_subscription_handle_arg)
    ctf_array(uint8_t, gid, gid_arg, TRACETOOLS_GID_STORAGE_SIZE)
  )
)

TRACEPOINT_EVENT(
  TRACEPOINT_PROVIDER,
  rcl_subscription_init,
  TP_ARGS(
    const void *, subscription_handle_arg,
    const void *, node_handle_arg,
    const void *, rmw_subscription_handle_arg,
    const char *, topic_name_arg,
    const size_t, queue_depth_arg
  ),
  TP_FIELDS(
    ctf_integer_hex(const void *, subscription_handle, subscription_handle_arg)
    ctf_integer_hex(const void *, node_handle, node_handle_arg)
    ctf_integer_hex(const void *, rmw_subscription_handle, rmw_subscription_handle_arg)
    ctf_string(topic_name, topic_name_arg)
    ctf_integer(const size_t, queue_depth, queue_depth_arg)
  )
)

TRACEPOINT_EVENT(
  TRACEPOINT_PROVIDER,
  rclcpp_subscription_init,
  TP_ARGS(
    const void *, subscription_handle_arg,
    const void *, subscription_arg
  ),
  TP_FIELDS(
    ctf_integer_hex(const void *, subscription_handle, subscription_handle_arg)
    ctf_integer_hex(const void *, subscription, subscription_arg)
  )
)

TRACEPOINT_EVENT(
  TRACEPOINT_PROVIDER,
  rclcpp_subscription_callback_added,
  TP_ARGS(
    const void *, subscription_arg,
    const void *, callback_arg
  ),
  TP_FIELDS(
    ctf_integer_hex(const void *, subscription, subscription_arg)
    ctf_integer_hex(const void *, callback, callback_arg)
  )
)

TRACEPOINT_EVENT(
  TRACEPOINT_PROVIDER,
  rmw_take,
  TP_ARGS(
    const void *, rmw_subscription_handle_arg,
    const void *, message_arg,
    int64_t, source_timestamp_arg,
    const bool, taken_arg
  ),
  TP_FIELDS(
    ctf_integer_hex(const void *, rmw_subscription_handle, rmw_subscription_handle_arg)
    ctf_integer_hex(const void *, message, message_arg)
    ctf_integer(int64_t, source_timestamp, source_timestamp_arg)
    ctf_integer(int, taken, (taken_arg ? 1 : 0))
  )
)

TRACEPOINT_EVENT(
  TRACEPOINT_PROVIDER,
  rcl_take,
  TP_ARGS(
    const void *, message_arg
  ),
  TP_FIELDS(
    ctf_integer_hex(const void *, message, message_arg)
  )
)

TRACEPOINT_EVENT(
  TRACEPOINT_PROVIDER,
  rclcpp_take,
  TP_ARGS(
    const void *, message_arg
  ),
  TP_FIELDS(
    ctf_integer_hex(const void *, message, message_arg)
  )
)

TRACEPOINT_EVENT(
  TRACEPOINT_PROVIDER,
  rcl_service_init,
  TP_ARGS(
    const void *, service_handle_arg,
    const void *, node_handle_arg,
    const void *, rmw_service_handle_arg,
    const char *, service_name_arg
  ),
  TP_FIELDS(
    ctf_integer_hex(const void *, service_handle, service_handle_arg)
    ctf_integer_hex(const void *, node_handle, node_handle_arg)
    ctf_integer_hex(const void *, rmw_service_handle, rmw_service_handle_arg)
    ctf_string(service_name, service_name_arg)
  )
)

TRACEPOINT_EVENT(
  TRACEPOINT_PROVIDER,
  rclcpp_service_callback_added,
  TP_ARGS(
    const void *, service_handle_arg,
    const void *, callback_arg
  ),
  TP_FIELDS(
    ctf_integer_hex(const void *, service_handle, service_handle_arg)
    ctf_integer_hex(const void *, callback, callback_arg)
  )
)

TRACEPOINT_EVENT(
  TRACEPOINT_PROVIDER,
  rcl_client_init,
  TP_ARGS(
    const void *, client_handle_arg,
    const void *, node_handle_arg,
    const void *, rmw_client_handle_arg,
    const char *, service_name_arg
  ),
  TP_FIELDS(
    ctf_integer_hex(const void *, client_handle, client_handle_arg)
    ctf_integer_hex(const void *, node_handle, node_handle_arg)
    ctf_integer_hex(const void *, rmw_client_handle, rmw_client_handle_arg)
    ctf_string(service_name, service_name_arg)
  )
)

TRACEPOINT_EVENT(
  TRACEPOINT_PROVIDER,
  rcl_timer_init,
  TP_ARGS(
    const void *, timer_handle_arg,
    int64_t, period_arg
  ),
  TP_FIELDS(
    ctf_integer_hex(const void *, timer_handle, timer_handle_arg)
    ctf_integer(int64_t, period, period_arg)
  )
)

TRACEPOINT_EVENT(
  TRACEPOINT_PROVIDER,
  rclcpp_timer_callback_added,
  TP_ARGS(
    const void *, timer_handle_arg,
    const void *, callback_arg
  ),
  TP_FIELDS(
    ctf_integer_hex(const void *, timer_handle, timer_handle_arg)
    ctf_integer_hex(const void *, callback, callback_arg)
  )
)

TRACEPOINT_EVENT(
  TRACEPOINT_PROVIDER,
  rclcpp_timer_link_node,
  TP_ARGS(
    const void *, timer_handle_arg,
    const void *, node_handle_arg
  ),
  TP_FIELDS(
    ctf_integer_hex(const void *, timer_handle, timer_handle_arg)
    ctf_integer_hex(const void *, node_handle, node_handle_arg)
  )
)

TRACEPOINT_EVENT(
  TRACEPOINT_PROVIDER,
  rclcpp_callback_register,
  TP_ARGS(
    const void *, callback_arg,
    const char *, symbol_arg
  ),
  TP_FIELDS(
    ctf_integer_hex(const void *, callback, callback_arg)
    ctf_string(symbol, symbol_arg)
  )
)

TRACEPOINT_EVENT(
  TRACEPOINT_PROVIDER,
  callback_start,
  TP_ARGS(
    const void *, callback_arg,
    const bool, is_intra_process_arg
  ),
  TP_FIELDS(
    ctf_integer_hex(const void *, callback, callback_arg)
    ctf_integer(int, is_intra_process, (is_intra_process_arg ? 1 : 0))
  )
)

TRACEPOINT_EVENT(
  TRACEPOINT_PROVIDER,
  callback_end,
  TP_ARGS(
    const void *, callback_arg
  ),
  TP_FIELDS(
    ctf_integer_hex(const void *, callback, callback_arg)
  )
)

TRACEPOINT_EVENT(
  TRACEPOINT_PROVIDER,
  rcl_lifecycle_state_machine_init,
  TP_ARGS(
    const void *, node_handle_arg,
    const void *, state_machine_arg
  ),
  TP_FIELDS(
    ctf_integer_hex(const void *, node_handle, node_handle_arg)
    ctf_integer_hex(const void *, state_machine, state_machine_arg)
  )
)

TRACEPOINT_EVENT(
  TRACEPOINT_PROVIDER,
  rcl_lifecycle_transition,
  TP_ARGS(
    const void *, state_machine_arg,
    const char *, start_label_arg,
    const char *, goal_label_arg
  ),
  TP_FIELDS(
    ctf_integer_hex(const void *, state_machine, state_machine_arg)
    ctf_string(start_label, start_label_arg)
    ctf_string(goal_label, goal_label_arg)
  )
)

TRACEPOINT_EVENT(
  TRACEPOINT_PROVIDER,
  rclcpp_executor_get_next_ready,
  TP_ARGS(),
  TP_FIELDS()
)

TRACEPOINT_EVENT(
  TRACEPOINT_PROVIDER,
  rclcpp_executor_wait_for_work,
  TP_ARGS(
    const int64_t, timeout_arg
  ),
  TP_FIELDS(
    ctf_integer(int64_t, timeout, timeout_arg)
  )
)

TRACEPOINT_EVENT(
  TRACEPOINT_PROVIDER,
  rclcpp_executor_execute,
  TP_ARGS(
    const void *, handle_arg
  ),
  TP_FIELDS(
    ctf_integer_hex(const void *, handle, handle_arg)
  )
)

#endif  // _TRACETOOLS__TP_CALL_H_

#include <lttng/tracepoint-event.h>

#endif  // TRACETOOLS__TP_CALL_H_
