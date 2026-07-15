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

#ifndef COM_INTERFACE_H_
#define COM_INTERFACE_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "rcl/macros.h"

#include "rcl_lifecycle/data_types.h"

rcl_lifecycle_com_interface_t
rcl_lifecycle_get_zero_initialized_com_interface();

rcl_ret_t
RCL_WARN_UNUSED
rcl_lifecycle_com_interface_init(
  rcl_lifecycle_com_interface_t * com_interface,
  rcl_node_t * node_handle,
  const rosidl_message_type_support_t * ts_pub_notify,
  const rosidl_service_type_support_t * ts_srv_change_state,
  const rosidl_service_type_support_t * ts_srv_get_state,
  const rosidl_service_type_support_t * ts_srv_get_available_states,
  const rosidl_service_type_support_t * ts_srv_get_available_transitions,
  const rosidl_service_type_support_t * ts_srv_get_transition_graph);

rcl_ret_t
RCL_WARN_UNUSED
rcl_lifecycle_com_interface_publisher_init(
  rcl_lifecycle_com_interface_t * com_interface,
  rcl_node_t * node_handle,
  const rosidl_message_type_support_t * ts_pub_notify);

rcl_ret_t
RCL_WARN_UNUSED
rcl_lifecycle_com_interface_publisher_fini(
  rcl_lifecycle_com_interface_t * com_interface,
  rcl_node_t * node_handle);

rcl_ret_t
RCL_WARN_UNUSED
rcl_lifecycle_com_interface_services_init(
  rcl_lifecycle_com_interface_t * com_interface,
  rcl_node_t * node_handle,
  const rosidl_service_type_support_t * ts_srv_change_state,
  const rosidl_service_type_support_t * ts_srv_get_state,
  const rosidl_service_type_support_t * ts_srv_get_available_states,
  const rosidl_service_type_support_t * ts_srv_get_available_transitions,
  const rosidl_service_type_support_t * ts_srv_get_transition_graph);

rcl_ret_t
RCL_WARN_UNUSED
rcl_lifecycle_com_interface_services_fini(
  rcl_lifecycle_com_interface_t * com_interface,
  rcl_node_t * node_handle);

rcl_ret_t
RCL_WARN_UNUSED
rcl_lifecycle_com_interface_fini(
  rcl_lifecycle_com_interface_t * com_interface,
  rcl_node_t * node_handle);

rcl_ret_t
RCL_WARN_UNUSED
rcl_lifecycle_com_interface_publish_notification(
  rcl_lifecycle_com_interface_t * com_interface,
  const rcl_lifecycle_state_t * start, const rcl_lifecycle_state_t * goal);

#ifdef __cplusplus
}
#endif

#endif  // COM_INTERFACE_H_
