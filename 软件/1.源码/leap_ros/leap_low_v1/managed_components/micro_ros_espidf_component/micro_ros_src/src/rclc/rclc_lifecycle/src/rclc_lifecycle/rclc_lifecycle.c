// Copyright (c) 2020 - for information on the respective copyright owner
// see the NOTICE file and/or the repository https://github.com/ros2/rclc.
// Copyright 2014 Open Source Robotics Foundation, Inc.
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

#include "rclc_lifecycle/rclc_lifecycle.h"

#include <rcl/error_handling.h>
#include <rcutils/logging_macros.h>

#include <rclc/types.h>
#include <rcl_lifecycle/rcl_lifecycle.h>
#include <rcl_lifecycle/transition_map.h>

#include <rosidl_runtime_c/string_functions.h>
#include <std_msgs/msg/string.h>
#include <lifecycle_msgs/msg/state.h>
#include <lifecycle_msgs/msg/transition_description.h>
#include <lifecycle_msgs/msg/transition_event.h>
#include <lifecycle_msgs/srv/change_state.h>
#include <lifecycle_msgs/srv/get_available_states.h>
#include <lifecycle_msgs/srv/get_available_transitions.h>
#include <lifecycle_msgs/srv/get_state.h>


rcl_ret_t
rclc_make_node_a_lifecycle_node(
  rclc_lifecycle_node_t * lifecycle_node,
  rcl_node_t * node,
  rcl_lifecycle_state_machine_t * state_machine,
  rcl_allocator_t * allocator,
  bool enable_communication_interface
)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    lifecycle_node, "lifecycle_node is a null pointer", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    node, "node is a null pointer", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    allocator, "allocator is a null pointer", return RCL_RET_INVALID_ARGUMENT);

  lifecycle_node->node = node;
  lifecycle_node->publish_transitions = enable_communication_interface;

  rcl_lifecycle_state_machine_options_t state_machine_options =
    rcl_lifecycle_get_default_state_machine_options();
  state_machine_options.enable_com_interface = enable_communication_interface;
  state_machine_options.allocator = *allocator;

  rcl_ret_t rcl_ret = rcl_lifecycle_state_machine_init(
    state_machine,
    node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(lifecycle_msgs, msg, TransitionEvent),
    ROSIDL_GET_SRV_TYPE_SUPPORT(lifecycle_msgs, srv, ChangeState),
    ROSIDL_GET_SRV_TYPE_SUPPORT(lifecycle_msgs, srv, GetState),
    ROSIDL_GET_SRV_TYPE_SUPPORT(lifecycle_msgs, srv, GetAvailableStates),
    ROSIDL_GET_SRV_TYPE_SUPPORT(lifecycle_msgs, srv, GetAvailableTransitions),
    ROSIDL_GET_SRV_TYPE_SUPPORT(lifecycle_msgs, srv, GetAvailableTransitions),
    &state_machine_options);
  if (rcl_ret != RCL_RET_OK) {
    // state machine not initialized, return uninitilized
    RCUTILS_LOG_ERROR(
      "Unable to initialize state machine: %s",
      rcl_get_error_string().str);
    return RCL_RET_ERROR;
  }

  lifecycle_node->state_machine = state_machine;

  // Pre-init messages and strings
  static char empty_string[RCLC_LIFECYCLE_MAX_STRING_LENGTH];
  memset(empty_string, ' ', RCLC_LIFECYCLE_MAX_STRING_LENGTH);
  empty_string[RCLC_LIFECYCLE_MAX_STRING_LENGTH - 1] = '\0';

  lifecycle_msgs__srv__ChangeState_Request__init(&lifecycle_node->cs_req);
  lifecycle_msgs__srv__ChangeState_Response__init(&lifecycle_node->cs_res);

  lifecycle_msgs__srv__GetState_Request__init(&lifecycle_node->gs_req);
  lifecycle_msgs__srv__GetState_Response__init(&lifecycle_node->gs_res);
  rosidl_runtime_c__String__assign(
    &lifecycle_node->gs_res.current_state.label,
    (const char *) empty_string);

  lifecycle_msgs__srv__GetAvailableStates_Request__init(&lifecycle_node->gas_req);
  lifecycle_msgs__srv__GetAvailableStates_Response__init(&lifecycle_node->gas_res);
  lifecycle_msgs__msg__State__Sequence__init(
    &lifecycle_node->gas_res.available_states,
    state_machine->transition_map.states_size);
  lifecycle_node->gas_res.available_states.size = 0;
  for (size_t i = 0; i < state_machine->transition_map.states_size; ++i) {
    rosidl_runtime_c__String__assign(
      &lifecycle_node->gas_res.available_states.data[i].label,
      (const char *) empty_string);
  }

  return RCL_RET_OK;
}

rcl_ret_t
rclc_lifecycle_change_state(
  rclc_lifecycle_node_t * lifecycle_node,
  unsigned int transition_id,
  bool publish_update
)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    lifecycle_node, "lifecycle_node is a null pointer", return RCL_RET_INVALID_ARGUMENT);

  if (rcl_lifecycle_state_machine_is_initialized(lifecycle_node->state_machine) != RCL_RET_OK) {
    RCUTILS_LOG_ERROR(
      "Unable to change state for state machine: %s",
      rcl_get_error_string().str);
    return RCL_RET_ERROR;
  }

  if (
    rcl_lifecycle_trigger_transition_by_id(
      lifecycle_node->state_machine, transition_id, publish_update) != RCL_RET_OK)
  {
    RCUTILS_LOG_ERROR(
      "Unable to start transition %u from current state %s: %s",
      transition_id, lifecycle_node->state_machine->current_state->label,
      rcl_get_error_string().str);
    rcutils_reset_error();
    return RCL_RET_ERROR;
  }

  // Check for callbacks for this transition
  if (rclc_lifecycle_execute_callback(lifecycle_node, transition_id) == RCL_RET_OK) {
    // successful, so transition do according success transition
    switch (transition_id) {
      case lifecycle_msgs__msg__Transition__TRANSITION_CONFIGURE:
        return rcl_lifecycle_trigger_transition_by_id(
          lifecycle_node->state_machine,
          lifecycle_msgs__msg__Transition__TRANSITION_ON_CONFIGURE_SUCCESS,
          publish_update);
        break;
      case lifecycle_msgs__msg__Transition__TRANSITION_ACTIVATE:
        return rcl_lifecycle_trigger_transition_by_id(
          lifecycle_node->state_machine,
          lifecycle_msgs__msg__Transition__TRANSITION_ON_ACTIVATE_SUCCESS,
          publish_update);
        break;
      case lifecycle_msgs__msg__Transition__TRANSITION_DEACTIVATE:
        return rcl_lifecycle_trigger_transition_by_id(
          lifecycle_node->state_machine,
          lifecycle_msgs__msg__Transition__TRANSITION_ON_DEACTIVATE_SUCCESS,
          publish_update);
        break;
      case lifecycle_msgs__msg__Transition__TRANSITION_CLEANUP:
        return rcl_lifecycle_trigger_transition_by_id(
          lifecycle_node->state_machine,
          lifecycle_msgs__msg__Transition__TRANSITION_ON_CLEANUP_SUCCESS,
          publish_update);
        break;
    }
  } else {
    // failed, so transition to fail
    switch (transition_id) {
      case lifecycle_msgs__msg__Transition__TRANSITION_CONFIGURE:
        return rcl_lifecycle_trigger_transition_by_id(
          lifecycle_node->state_machine,
          lifecycle_msgs__msg__Transition__TRANSITION_ON_CONFIGURE_FAILURE,
          publish_update);
        break;
      case lifecycle_msgs__msg__Transition__TRANSITION_ACTIVATE:
        return rcl_lifecycle_trigger_transition_by_id(
          lifecycle_node->state_machine,
          lifecycle_msgs__msg__Transition__TRANSITION_ON_ACTIVATE_FAILURE,
          publish_update);
        break;
      case lifecycle_msgs__msg__Transition__TRANSITION_DEACTIVATE:
        return rcl_lifecycle_trigger_transition_by_id(
          lifecycle_node->state_machine,
          lifecycle_msgs__msg__Transition__TRANSITION_ON_DEACTIVATE_FAILURE,
          publish_update);
        break;
      case lifecycle_msgs__msg__Transition__TRANSITION_CLEANUP:
        return rcl_lifecycle_trigger_transition_by_id(
          lifecycle_node->state_machine,
          lifecycle_msgs__msg__Transition__TRANSITION_ON_CLEANUP_FAILURE,
          publish_update);
        break;
    }
  }

  // This true holds in both cases where the actual callback
  // was successful or not, since at this point we have a valid transistion
  // to either a new primary state or error state
  return RCL_RET_OK;
}

rcl_ret_t
rclc_lifecycle_register_callback(
  rclc_lifecycle_node_t * lifecycle_node,
  unsigned int goal_state,
  int (* cb)(void)
)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    lifecycle_node, "node is a null pointer", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    cb, "callback is a null pointer", return RCL_RET_INVALID_ARGUMENT);

  lifecycle_node->callbacks.goal_states[goal_state] = true;
  lifecycle_node->callbacks.fun_ptrs[goal_state] = cb;

  return 0;
}

rcl_ret_t
rclc_lifecycle_register_on_configure(
  rclc_lifecycle_node_t * node,
  int (* cb)(void)
)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    node, "node is a null pointer", return RCL_RET_INVALID_ARGUMENT);

  return rclc_lifecycle_register_callback(
    node,
    lifecycle_msgs__msg__Transition__TRANSITION_CONFIGURE,
    cb);
}

rcl_ret_t
rclc_lifecycle_register_on_activate(
  rclc_lifecycle_node_t * node,
  int (* cb)(void)
)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    node, "node is a null pointer", return RCL_RET_INVALID_ARGUMENT);

  return rclc_lifecycle_register_callback(
    node,
    lifecycle_msgs__msg__Transition__TRANSITION_ACTIVATE,
    cb);
}

rcl_ret_t
rclc_lifecycle_register_on_deactivate(
  rclc_lifecycle_node_t * node,
  int (* cb)(void)
)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    node, "node is a null pointer", return RCL_RET_INVALID_ARGUMENT);

  return rclc_lifecycle_register_callback(
    node,
    lifecycle_msgs__msg__Transition__TRANSITION_DEACTIVATE,
    cb);
}

rcl_ret_t
rclc_lifecycle_register_on_cleanup(
  rclc_lifecycle_node_t * node,
  int (* cb)(void)
)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    node, "node is a null pointer", return RCL_RET_INVALID_ARGUMENT);

  return rclc_lifecycle_register_callback(
    node,
    lifecycle_msgs__msg__Transition__TRANSITION_CLEANUP,
    cb);
}

rcl_ret_t
rclc_lifecycle_node_fini(
  rclc_lifecycle_node_t * lifecycle_node,
  rcl_allocator_t * allocator
)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    lifecycle_node, "lifecycle_node is a null pointer", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    allocator, "allocator is a null pointer", return RCL_RET_INVALID_ARGUMENT);

  rcl_ret_t rcl_ret = RCL_RET_OK;
  RCLC_UNUSED(allocator);

  // Cleanup statemachine
  rcl_ret = rcl_lifecycle_state_machine_fini(
    lifecycle_node->state_machine,
    lifecycle_node->node);
  if (rcl_ret != RCL_RET_OK) {
    RCUTILS_LOG_ERROR(
      "Unable to clean up state machine: %s",
      rcl_get_error_string().str);
    rcutils_reset_error();
    return RCL_RET_ERROR;
  }

  // Cleanup service messages
  lifecycle_msgs__srv__GetState_Request__fini(&lifecycle_node->gs_req);
  lifecycle_msgs__srv__GetState_Response__fini(&lifecycle_node->gs_res);
  lifecycle_msgs__srv__ChangeState_Request__fini(&lifecycle_node->cs_req);
  lifecycle_msgs__srv__ChangeState_Response__fini(&lifecycle_node->cs_res);
  lifecycle_msgs__srv__GetAvailableStates_Request__fini(&lifecycle_node->gas_req);
  lifecycle_msgs__srv__GetAvailableStates_Response__fini(&lifecycle_node->gas_res);

  return rcl_ret;
}

rcl_ret_t
rclc_lifecycle_execute_callback(
  rclc_lifecycle_node_t * lifecycle_node,
  unsigned int transition_id
)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    lifecycle_node, "lifecycle_node is a null pointer", return RCL_RET_INVALID_ARGUMENT);

  if (!lifecycle_node->callbacks.goal_states[transition_id]) {
    // no callback, so process, all good
    return RCL_RET_OK;
  }

  return (*lifecycle_node->callbacks.fun_ptrs[transition_id])();
}

rcl_ret_t
rclc_lifecycle_init_get_state_server(
  rclc_lifecycle_service_context_t * context,
  rclc_executor_t * executor)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    context, "context is a null pointer", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    executor, "executor is a null pointer", return RCL_RET_INVALID_ARGUMENT);

  rcl_ret_t rc = rclc_executor_add_service_with_context(
    executor,
    &context->lifecycle_node->state_machine->com_interface.srv_get_state,
    &context->lifecycle_node->gs_req,
    &context->lifecycle_node->gs_res,
    rclc_lifecycle_get_state_callback,
    context);
  if (rc != RCL_RET_OK) {
    PRINT_RCLC_ERROR(main, rclc_executor_add_service_with_context);
    return RCL_RET_ERROR;
  }

  return rc;
}

void
rclc_lifecycle_get_state_callback(
  const void * req,
  void * res,
  void * context)
{
  RCL_UNUSED(req);
  lifecycle_msgs__srv__GetState_Response * res_in =
    (lifecycle_msgs__srv__GetState_Response *) res;
  rclc_lifecycle_service_context_t * context_in =
    (rclc_lifecycle_service_context_t *) context;

  rcl_lifecycle_state_machine_t * sm =
    context_in->lifecycle_node->state_machine;

  res_in->current_state.id = sm->current_state->id;
  bool success = rosidl_runtime_c__String__assign(
    &res_in->current_state.label,
    sm->current_state->label);
  if (!success) {
    PRINT_RCLC_ERROR(
      rclc_lifecycle_get_state_callback,
      rosidl_runtime_c__String__assign);
  }
}

rcl_ret_t
rclc_lifecycle_init_get_available_states_server(
  rclc_lifecycle_service_context_t * context,
  rclc_executor_t * executor)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    executor, "executor is a null pointer", return RCL_RET_INVALID_ARGUMENT);

  rcl_ret_t rc = rclc_executor_add_service_with_context(
    executor,
    &context->lifecycle_node->state_machine->com_interface.srv_get_available_states,
    &context->lifecycle_node->gas_req,
    &context->lifecycle_node->gas_res,
    rclc_lifecycle_get_available_states_callback,
    context);
  if (rc != RCL_RET_OK) {
    PRINT_RCLC_ERROR(main, rclc_executor_add_service_with_context);
    return RCL_RET_ERROR;
  }

  return rc;
}

void
rclc_lifecycle_get_available_states_callback(
  const void * req,
  void * res,
  void * context)
{
  RCL_UNUSED(req);
  lifecycle_msgs__srv__GetAvailableStates_Response * res_in =
    (lifecycle_msgs__srv__GetAvailableStates_Response *) res;
  rclc_lifecycle_service_context_t * context_in =
    (rclc_lifecycle_service_context_t *) context;

  rcl_lifecycle_state_machine_t * sm =
    context_in->lifecycle_node->state_machine;

  bool success = true;
  res_in->available_states.size = sm->transition_map.states_size;
  for (unsigned int i = 0; i < sm->transition_map.states_size; ++i) {
    res_in->available_states.data[i].id = sm->transition_map.states[i].id;
    success &= rosidl_runtime_c__String__assign(
      &res_in->available_states.data[i].label,
      sm->transition_map.states[i].label);
  }

  if (!success) {
    PRINT_RCLC_ERROR(
      rclc_lifecycle_get_available_states_callback,
      rosidl_runtime_c__String__assign);
  }
}

rcl_ret_t
rclc_lifecycle_init_change_state_server(
  rclc_lifecycle_service_context_t * context,
  rclc_executor_t * executor)
{
  rcl_ret_t rc = rclc_executor_add_service_with_context(
    executor,
    &context->lifecycle_node->state_machine->com_interface.srv_change_state,
    &context->lifecycle_node->cs_req,
    &context->lifecycle_node->cs_res,
    rclc_lifecycle_change_state_callback,
    context);
  if (rc != RCL_RET_OK) {
    PRINT_RCLC_ERROR(main, rclc_executor_add_service_with_context);
    return RCL_RET_ERROR;
  }

  return rc;
}

void
rclc_lifecycle_change_state_callback(
  const void * req,
  void * res,
  void * context)
{
  lifecycle_msgs__srv__ChangeState_Request * req_in =
    (lifecycle_msgs__srv__ChangeState_Request *) req;
  lifecycle_msgs__srv__ChangeState_Response * res_in =
    (lifecycle_msgs__srv__ChangeState_Response *) res;
  rclc_lifecycle_service_context_t * context_in =
    (rclc_lifecycle_service_context_t *) context;

  rclc_lifecycle_node_t * ln = context_in->lifecycle_node;
  rcl_ret_t rc = rclc_lifecycle_change_state(ln, req_in->transition.id, true);

  lifecycle_msgs__srv__ChangeState_Response__init(res_in);
  if (rc != RCL_RET_OK) {
    PRINT_RCLC_ERROR(
      rclc_lifecycle_change_state_callback,
      rclc_lifecycle_change_state);
    res_in->success = false;
  } else {
    res_in->success = true;
  }
}
