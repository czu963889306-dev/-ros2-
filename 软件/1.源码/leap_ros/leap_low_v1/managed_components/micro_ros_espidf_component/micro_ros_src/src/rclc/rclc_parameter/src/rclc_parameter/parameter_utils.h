// Copyright (c) 2021 - for information on the respective copyright owner
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

#ifndef RCLC_PARAMETER__PARAMETER_UTILS_H_
#define RCLC_PARAMETER__PARAMETER_UTILS_H_

#if __cplusplus
extern "C"
{
#endif  // if __cplusplus

#include <rclc_parameter/rclc_parameter.h>
#include <rosidl_runtime_c/string_functions.h>

#include <rcl/error_handling.h>
#include <rcl/types.h>

rcl_ret_t
rclc_parameter_value_copy(
  ParameterValue * dst,
  const ParameterValue * src);

rcl_ret_t
rclc_parameter_copy(
  Parameter * dst,
  const Parameter * src);

rcl_ret_t
rclc_parameter_descriptor_copy(
  ParameterDescriptor * dst,
  const ParameterDescriptor * src,
  bool low_mem);

Parameter *
rclc_parameter_search(
  Parameter__Sequence * parameter_list,
  const char * param_name);

size_t
rclc_parameter_search_index(
  Parameter__Sequence * parameter_list,
  const char * param_name);

bool rclc_parameter_set_string(
  rosidl_runtime_c__String * str,
  const char * value);

rcl_ret_t rclc_parameter_prepare_new_event(
  ParameterEvent * event,
  Parameter * parameter);

rcl_ret_t rclc_parameter_prepare_changed_event(
  ParameterEvent * event,
  Parameter * parameter);

rcl_ret_t rclc_parameter_prepare_deleted_event(
  ParameterEvent * event,
  Parameter * parameter);

rcl_ret_t rclc_parameter_reset_parameter_event(
  ParameterEvent * event);

rcl_ret_t rclc_parameter_initialize_empty_string(
  rosidl_runtime_c__String * str,
  size_t capacity);

bool rclc_parameter_descriptor_initialize_string(
  rosidl_runtime_c__String * str);

#if __cplusplus
}
#endif  // if __cplusplus

#endif  // RCLC_PARAMETER__PARAMETER_UTILS_H_
