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

#if __cplusplus
extern "C"
{
#endif /* if __cplusplus */

#include "rclc_parameter/parameter_utils.h"

rcl_ret_t
rclc_parameter_value_copy(
  ParameterValue * dst,
  const ParameterValue * src)
{
  RCL_CHECK_ARGUMENT_FOR_NULL(dst, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(src, RCL_RET_INVALID_ARGUMENT);

  dst->type = src->type;

  switch (src->type) {
    case RCLC_PARAMETER_BOOL:
      dst->bool_value = src->bool_value;
      return RCL_RET_OK;
    case RCLC_PARAMETER_INT:
      dst->integer_value = src->integer_value;
      return RCL_RET_OK;
    case RCLC_PARAMETER_DOUBLE:
      dst->double_value = src->double_value;
      return RCL_RET_OK;
    case RCLC_PARAMETER_NOT_SET:
    default:
      dst->type = RCLC_PARAMETER_NOT_SET;
      return RCL_RET_ERROR;
  }
  return RCL_RET_ERROR;
}

rcl_ret_t
rclc_parameter_copy(
  Parameter * dst,
  const Parameter * src)
{
  RCL_CHECK_ARGUMENT_FOR_NULL(dst, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(src, RCL_RET_INVALID_ARGUMENT);

  if (!rclc_parameter_set_string(&dst->name, src->name.data)) {
    return RCL_RET_ERROR;
  }

  return rclc_parameter_value_copy(&dst->value, &src->value);
}

rcl_ret_t
rclc_parameter_descriptor_copy(
  ParameterDescriptor * dst,
  const ParameterDescriptor * src,
  bool low_mem)
{
  RCL_CHECK_ARGUMENT_FOR_NULL(dst, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(src, RCL_RET_INVALID_ARGUMENT);

  if (!low_mem) {
    if (!rclc_parameter_set_string(&dst->name, src->name.data)) {
      return RCL_RET_ERROR;
    }

    if (!rclc_parameter_set_string(&dst->description, src->description.data)) {
      return RCL_RET_ERROR;
    }

    if (!rclc_parameter_set_string(
        &dst->additional_constraints,
        src->additional_constraints.data))
    {
      return RCL_RET_ERROR;
    }
  }

  dst->type = src->type;
  dst->read_only = src->read_only;

  dst->floating_point_range.data[0].from_value = src->floating_point_range.data[0].from_value;
  dst->floating_point_range.data[0].to_value = src->floating_point_range.data[0].to_value;
  dst->floating_point_range.data[0].step = src->floating_point_range.data[0].step;
  dst->floating_point_range.size = src->floating_point_range.size;

  dst->integer_range.data[0].from_value = src->integer_range.data[0].from_value;
  dst->integer_range.data[0].to_value = src->integer_range.data[0].to_value;
  dst->integer_range.data[0].step = src->integer_range.data[0].step;
  dst->integer_range.size = src->integer_range.size;

  return RCL_RET_OK;
}

Parameter *
rclc_parameter_search(
  Parameter__Sequence * parameter_list,
  const char * param_name)
{
  RCL_CHECK_ARGUMENT_FOR_NULL(parameter_list, NULL);
  RCL_CHECK_ARGUMENT_FOR_NULL(param_name, NULL);

  for (size_t i = 0; i < parameter_list->size; ++i) {
    if (!strcmp(param_name, parameter_list->data[i].name.data)) {
      return &parameter_list->data[i];
    }
  }

  return NULL;
}

size_t
rclc_parameter_search_index(
  Parameter__Sequence * parameter_list,
  const char * param_name)
{
  RCL_CHECK_ARGUMENT_FOR_NULL(parameter_list, SIZE_MAX);
  RCL_CHECK_ARGUMENT_FOR_NULL(param_name, SIZE_MAX);

  size_t index = SIZE_MAX;
  for (size_t i = 0; i < parameter_list->size; ++i) {
    if (!strcmp(param_name, parameter_list->data[i].name.data)) {
      index = i;
      break;
    }
  }

  return index;
}

bool
rclc_parameter_set_string(
  rosidl_runtime_c__String * str,
  const char * value)
{
  RCL_CHECK_ARGUMENT_FOR_NULL(str, false);
  RCL_CHECK_ARGUMENT_FOR_NULL(value, false);

  if (str->capacity >= (strlen(value) + 1)) {
    memcpy(str->data, value, strlen(value) + 1);
    str->size = strlen(str->data);
    return true;
  }

  return false;
}

rcl_ret_t rclc_parameter_prepare_new_event(
  ParameterEvent * event,
  Parameter * parameter)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    event, "event is a null pointer", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    parameter, "parameter is a null pointer", return RCL_RET_INVALID_ARGUMENT);

  rclc_parameter_reset_parameter_event(event);
  event->new_parameters.data = parameter;
  event->new_parameters.capacity = 1;
  event->new_parameters.size = 1;
  return RCL_RET_OK;
}

rcl_ret_t rclc_parameter_prepare_changed_event(
  ParameterEvent * event,
  Parameter * parameter)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    event, "event is a null pointer", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    parameter, "parameter is a null pointer", return RCL_RET_INVALID_ARGUMENT);

  rclc_parameter_reset_parameter_event(event);
  event->changed_parameters.data = parameter;
  event->changed_parameters.capacity = 1;
  event->changed_parameters.size = 1;
  return RCL_RET_OK;
}

rcl_ret_t rclc_parameter_prepare_deleted_event(
  ParameterEvent * event,
  Parameter * parameter)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    event, "event is a null pointer", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    parameter, "parameter is a null pointer", return RCL_RET_INVALID_ARGUMENT);

  rclc_parameter_reset_parameter_event(event);
  event->deleted_parameters.data = parameter;
  event->deleted_parameters.capacity = 1;
  event->deleted_parameters.size = 1;
  return RCL_RET_OK;
}

rcl_ret_t rclc_parameter_reset_parameter_event(
  ParameterEvent * event)
{
  RCL_CHECK_ARGUMENT_FOR_NULL(event, RCL_RET_INVALID_ARGUMENT);

  memset(&event->new_parameters, 0, sizeof(Parameter__Sequence));
  memset(&event->changed_parameters, 0, sizeof(Parameter__Sequence));
  memset(&event->deleted_parameters, 0, sizeof(Parameter__Sequence));
  return RCL_RET_OK;
}

rcl_ret_t rclc_parameter_initialize_empty_string(
  rosidl_runtime_c__String * str,
  size_t capacity)
{
  RCL_CHECK_ARGUMENT_FOR_NULL(str, RCL_RET_INVALID_ARGUMENT);

  if (capacity < 1) {
    return RCL_RET_INVALID_ARGUMENT;
  }

  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  str->data = allocator.allocate(sizeof(char) * capacity, allocator.state);

  if (str->data == NULL) {
    return RCL_RET_ERROR;
  }

  str->data[0] = '\0';
  str->capacity = capacity;
  str->size = 0;

  return RCL_RET_OK;
}

bool rclc_parameter_descriptor_initialize_string(rosidl_runtime_c__String * str)
{
  RCL_CHECK_ARGUMENT_FOR_NULL(str, false);

  static char empty_string[RCLC_PARAMETER_MAX_STRING_LENGTH] = "";
  size_t string_capacity = RCLC_PARAMETER_MAX_STRING_LENGTH - 1;

  bool ret = rosidl_runtime_c__String__assignn(
    str,
    (const char *) empty_string,
    string_capacity);

  str->size = 0;
  return ret;
}

#if __cplusplus
}
#endif /* if __cplusplus */
