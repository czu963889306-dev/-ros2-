// Copyright 2016 Open Source Robotics Foundation, Inc.
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

#ifdef __cplusplus
extern "C"
{
#endif

#include "com_interface.h"  // NOLINT

#include <stdio.h>
#include <string.h>
#include <stddef.h>

#include "lifecycle_msgs/msg/transition_event.h"

#include "rcl/error_handling.h"

#include "rcutils/format_string.h"
#include "rcutils/logging_macros.h"

#include "rmw/validate_full_topic_name.h"

#include "rosidl_runtime_c/message_type_support_struct.h"
#include "rosidl_runtime_c/string_functions.h"

#include "rcl_lifecycle/data_types.h"

static const char * pub_transition_event_topic = "~/transition_event";
static const char * srv_change_state_service = "~/change_state";
static const char * srv_get_state_service = "~/get_state";
static const char * srv_get_available_states_service = "~/get_available_states";
static const char * srv_get_available_transitions_service = "~/get_available_transitions";
static const char * srv_get_transition_graph = "~/get_transition_graph";

rcl_lifecycle_com_interface_t
rcl_lifecycle_get_zero_initialized_com_interface()
{
  rcl_lifecycle_com_interface_t com_interface;
  com_interface.node_handle = NULL;
  com_interface.pub_transition_event = rcl_get_zero_initialized_publisher();
  com_interface.srv_change_state = rcl_get_zero_initialized_service();
  com_interface.srv_get_state = rcl_get_zero_initialized_service();
  com_interface.srv_get_available_states = rcl_get_zero_initialized_service();
  com_interface.srv_get_available_transitions = rcl_get_zero_initialized_service();
  com_interface.srv_get_transition_graph = rcl_get_zero_initialized_service();
  lifecycle_msgs__msg__TransitionEvent msg = {0};
  com_interface.msg = msg;
  return com_interface;
}

rcl_ret_t
rcl_lifecycle_com_interface_init(
  rcl_lifecycle_com_interface_t * com_interface,
  rcl_node_t * node_handle,
  const rosidl_message_type_support_t * ts_pub_notify,
  const rosidl_service_type_support_t * ts_srv_change_state,
  const rosidl_service_type_support_t * ts_srv_get_state,
  const rosidl_service_type_support_t * ts_srv_get_available_states,
  const rosidl_service_type_support_t * ts_srv_get_available_transitions,
  const rosidl_service_type_support_t * ts_srv_get_transition_graph)
{
  rcl_ret_t ret = rcl_lifecycle_com_interface_publisher_init(
    com_interface, node_handle, ts_pub_notify);
  if (ret != RCL_RET_OK) {
    return ret;
  }

  ret = rcl_lifecycle_com_interface_services_init(
    com_interface,
    node_handle,
    ts_srv_change_state,
    ts_srv_get_state,
    ts_srv_get_available_states,
    ts_srv_get_available_transitions,
    ts_srv_get_transition_graph);

  if (RCL_RET_OK != ret) {
    // cleanup the publisher, which was correctly initialized
    rcl_ret_t ret_fini = rcl_lifecycle_com_interface_publisher_fini(com_interface, node_handle);
    // warning is already set, no need to log anything here
    (void) ret_fini;
  }

  return ret;
}

rcl_ret_t
rcl_lifecycle_com_interface_publisher_init(
  rcl_lifecycle_com_interface_t * com_interface,
  rcl_node_t * node_handle,
  const rosidl_message_type_support_t * ts_pub_notify)
{
  RCL_CHECK_ARGUMENT_FOR_NULL(com_interface, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(node_handle, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(ts_pub_notify, RCL_RET_INVALID_ARGUMENT);

  // initialize publisher
  rcl_publisher_options_t publisher_options = rcl_publisher_get_default_options();
  rcl_ret_t ret = rcl_publisher_init(
    &com_interface->pub_transition_event, node_handle,
    ts_pub_notify, pub_transition_event_topic, &publisher_options);

  if (ret != RCL_RET_OK) {
    goto fail;
  }

  // initialize static message for notification
  lifecycle_msgs__msg__TransitionEvent__init(&com_interface->msg);

  return RCL_RET_OK;

fail:
  // error message is already logged on failure
  ret = rcl_lifecycle_com_interface_publisher_fini(com_interface, node_handle);
  (void) ret;
  return RCL_RET_ERROR;
}

rcl_ret_t
rcl_lifecycle_com_interface_publisher_fini(
  rcl_lifecycle_com_interface_t * com_interface,
  rcl_node_t * node_handle)
{
  lifecycle_msgs__msg__TransitionEvent__fini(&com_interface->msg);

  rcl_ret_t ret = rcl_publisher_fini(
    &com_interface->pub_transition_event, node_handle);
  if (ret != RCL_RET_OK) {
    RCUTILS_LOG_ERROR_NAMED(ROS_PACKAGE_NAME, "Failed to destroy transition_event publisher");
  }

  return ret;
}

rcl_ret_t
rcl_lifecycle_com_interface_services_init(
  rcl_lifecycle_com_interface_t * com_interface,
  rcl_node_t * node_handle,
  const rosidl_service_type_support_t * ts_srv_change_state,
  const rosidl_service_type_support_t * ts_srv_get_state,
  const rosidl_service_type_support_t * ts_srv_get_available_states,
  const rosidl_service_type_support_t * ts_srv_get_available_transitions,
  const rosidl_service_type_support_t * ts_srv_get_transition_graph)
{
  RCL_CHECK_ARGUMENT_FOR_NULL(com_interface, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(node_handle, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(ts_srv_change_state, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(ts_srv_get_state, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(ts_srv_get_available_states, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(ts_srv_get_available_transitions, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(ts_srv_get_transition_graph, RCL_RET_INVALID_ARGUMENT);

  rcl_ret_t ret = RCL_RET_OK;

  // initialize change state service
  {
    rcl_service_options_t service_options = rcl_service_get_default_options();
    ret = rcl_service_init(
      &com_interface->srv_change_state, node_handle,
      ts_srv_change_state, srv_change_state_service, &service_options);

    if (ret != RCL_RET_OK) {
      goto fail;
    }
  }

  // initialize get state service
  {
    rcl_service_options_t service_options = rcl_service_get_default_options();
    ret = rcl_service_init(
      &com_interface->srv_get_state, node_handle,
      ts_srv_get_state, srv_get_state_service, &service_options);

    if (ret != RCL_RET_OK) {
      goto fail;
    }
  }

  // initialize get available states service
  {
    rcl_service_options_t service_options = rcl_service_get_default_options();
    ret = rcl_service_init(
      &com_interface->srv_get_available_states, node_handle,
      ts_srv_get_available_states, srv_get_available_states_service, &service_options);

    if (ret != RCL_RET_OK) {
      goto fail;
    }
  }

  // initialize get available transitions service
  {
    rcl_service_options_t service_options = rcl_service_get_default_options();
    ret = rcl_service_init(
      &com_interface->srv_get_available_transitions, node_handle,
      ts_srv_get_available_transitions, srv_get_available_transitions_service, &service_options);

    if (ret != RCL_RET_OK) {
      goto fail;
    }
  }

  // initialize get transition graph service
  {
    rcl_service_options_t service_options = rcl_service_get_default_options();
    ret = rcl_service_init(
      &com_interface->srv_get_transition_graph, node_handle,
      ts_srv_get_transition_graph, srv_get_transition_graph, &service_options);

    if (ret != RCL_RET_OK) {
      goto fail;
    }
  }
  return RCL_RET_OK;

fail:
  // error messages already logged on failure
  ret = rcl_lifecycle_com_interface_services_fini(com_interface, node_handle);
  (void) ret;
  return RCL_RET_ERROR;
}

rcl_ret_t
rcl_lifecycle_com_interface_services_fini(
  rcl_lifecycle_com_interface_t * com_interface,
  rcl_node_t * node_handle)
{
  rcl_ret_t fcn_ret = RCL_RET_OK;

  // destroy get transition graph srv
  {
    rcl_ret_t ret = rcl_service_fini(
      &com_interface->srv_get_transition_graph, node_handle);
    if (ret != RCL_RET_OK) {
      RCUTILS_LOG_ERROR_NAMED(
        ROS_PACKAGE_NAME, "Failed to destroy get_transition_graph service");
      fcn_ret = RCL_RET_ERROR;
    }
  }

  // destroy get available transitions srv
  {
    rcl_ret_t ret = rcl_service_fini(
      &com_interface->srv_get_available_transitions, node_handle);
    if (ret != RCL_RET_OK) {
      RCUTILS_LOG_ERROR_NAMED(
        ROS_PACKAGE_NAME, "Failed to destroy get_available_transitions service");
      fcn_ret = RCL_RET_ERROR;
    }
  }

  // destroy get available states srv
  {
    rcl_ret_t ret = rcl_service_fini(
      &com_interface->srv_get_available_states, node_handle);
    if (ret != RCL_RET_OK) {
      RCUTILS_LOG_ERROR_NAMED(ROS_PACKAGE_NAME, "Failed to destroy get_available_states service");
      fcn_ret = RCL_RET_ERROR;
    }
  }

  // destroy get state srv
  {
    rcl_ret_t ret = rcl_service_fini(
      &com_interface->srv_get_state, node_handle);
    if (ret != RCL_RET_OK) {
      RCUTILS_LOG_ERROR_NAMED(ROS_PACKAGE_NAME, "Failed to destroy get_state service");
      fcn_ret = RCL_RET_ERROR;
    }
  }

  // destroy change state srv
  {
    rcl_ret_t ret = rcl_service_fini(
      &com_interface->srv_change_state, node_handle);
    if (ret != RCL_RET_OK) {
      RCUTILS_LOG_ERROR_NAMED(ROS_PACKAGE_NAME, "Failed to destroy change_state service");
      fcn_ret = RCL_RET_ERROR;
    }
  }

  return fcn_ret;
}

rcl_ret_t
rcl_lifecycle_com_interface_fini(
  rcl_lifecycle_com_interface_t * com_interface,
  rcl_node_t * node_handle)
{
  rcl_ret_t fcn_ret = RCL_RET_OK;

  // destroy the services
  {
    rcl_ret_t ret = rcl_lifecycle_com_interface_services_fini(
      com_interface, node_handle);
    if (RCL_RET_OK != ret) {
      fcn_ret = RCL_RET_ERROR;
    }
  }

  // destroy the event publisher
  {
    rcl_ret_t ret = rcl_lifecycle_com_interface_publisher_fini(
      com_interface, node_handle);
    if (RCL_RET_OK != ret) {
      fcn_ret = RCL_RET_ERROR;
    }
  }

  return fcn_ret;
}

rcl_ret_t
rcl_lifecycle_com_interface_publish_notification(
  rcl_lifecycle_com_interface_t * com_interface,
  const rcl_lifecycle_state_t * start, const rcl_lifecycle_state_t * goal)
{
  com_interface->msg.start_state.id = start->id;
  rosidl_runtime_c__String__assign(&com_interface->msg.start_state.label, start->label);
  com_interface->msg.goal_state.id = goal->id;
  rosidl_runtime_c__String__assign(&com_interface->msg.goal_state.label, goal->label);

  return rcl_publish(&com_interface->pub_transition_event, &com_interface->msg, NULL);
}

#ifdef __cplusplus
}
#endif
