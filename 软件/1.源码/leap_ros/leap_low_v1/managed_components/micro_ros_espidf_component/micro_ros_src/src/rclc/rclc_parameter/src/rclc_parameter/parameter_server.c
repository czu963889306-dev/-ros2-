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

#include <time.h>

#include <rcutils/time.h>
#include <rclc_parameter/rclc_parameter.h>
#include <rcl_interfaces/msg/floating_point_range.h>
#include <rcl_interfaces/msg/integer_range.h>

#include "./parameter_utils.h"

#define RCLC_SET_ERROR_MAX_STRING_LENGTH 25

rcl_ret_t
rclc_parameter_server_init_service(
  rcl_service_t * service,
  rcl_node_t * node,
  char * service_name,
  const rosidl_service_type_support_t * srv_type);

rcl_ret_t rclc_parameter_service_publish_event(
  rclc_parameter_server_t * parameter_server);

rcl_ret_t rclc_add_parameter_undeclared(
  rclc_parameter_server_t * parameter_server,
  Parameter * parameter);

rcl_ret_t rclc_parameter_execute_callback(
  rclc_parameter_server_t * parameter_server,
  const Parameter * old_param,
  const Parameter * new_param);

void
rclc_parameter_server_describe_service_callback(
  const void * req,
  void * res,
  void * parameter_server)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    req, "req is a null pointer", return );
  RCL_CHECK_FOR_NULL_WITH_MSG(
    res, "res is a null pointer", return );
  RCL_CHECK_FOR_NULL_WITH_MSG(
    parameter_server, "parameter_server is a null pointer", return );

  rclc_parameter_server_t * param_server = (rclc_parameter_server_t *) parameter_server;
  DescribeParameters_Request * request = (DescribeParameters_Request *) req;
  DescribeParameters_Response * response = (DescribeParameters_Response *) res;

  if (request->names.size > response->descriptors.capacity) {
    response->descriptors.size = 0;
    return;
  }

  response->descriptors.size = request->names.size;

  for (size_t i = 0; i < request->names.size; ++i) {
    size_t index = rclc_parameter_search_index(
      &param_server->parameter_list,
      request->names.data[i].data);

    ParameterDescriptor * response_descriptor = &response->descriptors.data[i];

    // Reset response values
    response_descriptor->type = RCLC_PARAMETER_NOT_SET;
    response_descriptor->read_only = false;
    response_descriptor->floating_point_range.size = 0;
    response_descriptor->integer_range.size = 0;

    // Copy request name
    if (param_server->low_mem_mode) {
      response_descriptor->name.data = request->names.data[i].data;
      response_descriptor->name.size = request->names.data[i].size;
      response_descriptor->name.capacity = request->names.data[i].capacity;
    }

    if (index < param_server->parameter_descriptors.size) {
      ParameterDescriptor * parameter_descriptor = &param_server->parameter_descriptors.data[index];
      rclc_parameter_descriptor_copy(
        response_descriptor, parameter_descriptor,
        param_server->low_mem_mode);
    } else if (!param_server->low_mem_mode) {
      rclc_parameter_set_string(&response_descriptor->name, request->names.data[i].data);
      rclc_parameter_set_string(&response_descriptor->description, "");
      rclc_parameter_set_string(&response_descriptor->additional_constraints, "");
    }
  }
}

void
rclc_parameter_server_list_service_callback(
  const void * req,
  void * res,
  void * parameter_server)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    res, "res is a null pointer", return );
  RCL_CHECK_FOR_NULL_WITH_MSG(
    parameter_server, "parameter_server is a null pointer", return );

  (void) req;

  rclc_parameter_server_t * param_server = (rclc_parameter_server_t *) parameter_server;
  ListParameters_Response * response = (ListParameters_Response *) res;

  response->result.names.size = param_server->parameter_list.size;

  for (size_t i = 0; i < response->result.names.size; ++i) {
    if (!param_server->low_mem_mode) {
      rclc_parameter_set_string(
        &response->result.names.data[i],
        param_server->parameter_list.data[i].name.data);
    } else {
      response->result.names.data[i].size = param_server->parameter_list.data[i].name.size;
    }
  }
}

void
rclc_parameter_server_get_service_callback(
  const void * req,
  void * res,
  void * parameter_server)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    req, "req is a null pointer", return );
  RCL_CHECK_FOR_NULL_WITH_MSG(
    res, "res is a null pointer", return );
  RCL_CHECK_FOR_NULL_WITH_MSG(
    parameter_server, "parameter_server is a null pointer", return );

  const GetParameters_Request * request = (const GetParameters_Request *) req;
  GetParameters_Response * response = (GetParameters_Response *) res;
  rclc_parameter_server_t * param_server = (rclc_parameter_server_t *) parameter_server;

  if (request->names.size > response->values.capacity) {
    response->values.size = 0;
    return;
  }

  size_t size = (request->names.size > param_server->parameter_list.size) ?
    param_server->parameter_list.size :
    request->names.size;

  response->values.size = size;

  for (size_t i = 0; i < size; ++i) {
    Parameter * parameter = rclc_parameter_search(
      &param_server->parameter_list,
      request->names.data[i].data);

    if (parameter != NULL) {
      rclc_parameter_value_copy(&response->values.data[i], &parameter->value);
    } else {
      response->values.data[i].type = RCLC_PARAMETER_NOT_SET;
    }
  }
}

void
rclc_parameter_server_get_types_service_callback(
  const void * req,
  void * res,
  void * parameter_server)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    req, "req is a null pointer", return );
  RCL_CHECK_FOR_NULL_WITH_MSG(
    res, "res is a null pointer", return );
  RCL_CHECK_FOR_NULL_WITH_MSG(
    parameter_server, "parameter_server is a null pointer", return );

  const GetParameterTypes_Request * request = (const GetParameterTypes_Request *)  req;
  GetParameterTypes_Response * response = (GetParameterTypes_Response *) res;
  rclc_parameter_server_t * param_server = (rclc_parameter_server_t *) parameter_server;

  if (request->names.size > response->types.capacity) {
    response->types.size = 0;
    return;
  }

  response->types.size = request->names.size;

  for (size_t i = 0; i < response->types.size; ++i) {
    Parameter * parameter = rclc_parameter_search(
      &param_server->parameter_list,
      request->names.data[i].data);

    if (parameter != NULL) {
      response->types.data[i] = parameter->value.type;
    } else {
      response->types.data[i] = RCLC_PARAMETER_NOT_SET;
    }
  }
}

void
rclc_parameter_server_set_service_callback(
  const void * req,
  void * res,
  void * parameter_server)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    req, "req is a null pointer", return );
  RCL_CHECK_FOR_NULL_WITH_MSG(
    res, "res is a null pointer", return );
  RCL_CHECK_FOR_NULL_WITH_MSG(
    parameter_server, "parameter_server is a null pointer", return );

  const SetParameters_Request * request = (const SetParameters_Request *) req;
  SetParameters_Response * response = (SetParameters_Response *) res;
  rclc_parameter_server_t * param_server = (rclc_parameter_server_t *) parameter_server;

  if (request->parameters.size > response->results.capacity) {
    response->results.size = 0;
    return;
  }

  response->results.size = request->parameters.size;

  for (size_t i = 0; i < response->results.size; ++i) {
    rosidl_runtime_c__String * message =
      (rosidl_runtime_c__String *) &response->results.data[i].reason;
    size_t index = rclc_parameter_search_index(
      &param_server->parameter_list,
      request->parameters.data[i].name.data);

    rcl_ret_t ret = RCL_RET_OK;

    // Clean previous response msg
    response->results.data[i].successful = false;
    rclc_parameter_set_string(message, "");

    if (index < param_server->parameter_list.size) {
      if (param_server->parameter_descriptors.data[index].read_only) {
        rclc_parameter_set_string(message, "Read only parameter");
        continue;
      }

      Parameter * parameter = &param_server->parameter_list.data[index];

      switch (request->parameters.data[i].value.type) {
        case RCLC_PARAMETER_NOT_SET:
          ret = rclc_parameter_execute_callback(param_server, parameter, NULL);
          if (ret == RCL_RET_OK) {
            ret = rclc_delete_parameter(param_server, parameter->name.data);
          }
          break;

        case RCLC_PARAMETER_BOOL:
          ret =
            rclc_parameter_set_bool(
            param_server, parameter->name.data,
            request->parameters.data[i].value.bool_value);
          break;
        case RCLC_PARAMETER_INT:
          ret =
            rclc_parameter_set_int(
            param_server, parameter->name.data,
            request->parameters.data[i].value.integer_value);
          break;
        case RCLC_PARAMETER_DOUBLE:
          ret =
            rclc_parameter_set_double(
            param_server, parameter->name.data,
            request->parameters.data[i].value.double_value);
          break;

        default:
          rclc_parameter_set_string(message, "Type not supported");
          break;
      }

      if (ret == RCL_RET_INVALID_ARGUMENT) {
        rclc_parameter_set_string(message, "Set parameter error");
      } else if (ret == RCLC_PARAMETER_MODIFICATION_REJECTED) {
        rclc_parameter_set_string(message, "Rejected by server");
      } else if (ret == RCLC_PARAMETER_TYPE_MISMATCH) {
        rclc_parameter_set_string(message, "Type mismatch");
      } else {
        response->results.data[i].successful = true;
      }
    } else if (param_server->allow_undeclared_parameters && // NOLINT
      request->parameters.data[i].value.type != RCLC_PARAMETER_NOT_SET)
    {
      size_t remaining_capacity = param_server->parameter_list.capacity -
        param_server->parameter_list.size;

      if (0 == remaining_capacity) {
        // Check parameter server capacity
        rclc_parameter_set_string(message, "Parameter server is full");

      } else if (RCL_RET_OK != // NOLINT
        rclc_parameter_execute_callback(param_server, NULL, &request->parameters.data[i]))
      {
        // Check server callback
        rclc_parameter_set_string(message, "New parameter rejected");
      } else if (RCL_RET_OK != // NOLINT
        rclc_add_parameter_undeclared(param_server, &request->parameters.data[i]))
      {
        // Check add parameter
        rclc_parameter_set_string(message, "Add parameter failed");
      } else {
        rclc_parameter_set_string(message, "New parameter added");
        response->results.data[i].successful = true;
      }
    } else {
      rclc_parameter_set_string(message, "Parameter not found");
    }
  }
}

const rclc_parameter_options_t DEFAULT_PARAMETER_SERVER_OPTIONS = {
  .notify_changed_over_dds = true,
  .max_params = 4,
  .allow_undeclared_parameters = false,
  .low_mem_mode = false
};

rcl_ret_t rclc_parameter_server_init_default(
  rclc_parameter_server_t * parameter_server,
  rcl_node_t * node)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    parameter_server, "parameter is a null pointer", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    node, "node is a null pointer", return RCL_RET_INVALID_ARGUMENT);

  return rclc_parameter_server_init_with_option(
    parameter_server, node, &DEFAULT_PARAMETER_SERVER_OPTIONS);
}

rcl_ret_t
init_parameter_server_memory(
  rclc_parameter_server_t * parameter_server,
  rcl_node_t * node,
  const rclc_parameter_options_t * options)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    parameter_server, "parameter is a null pointer", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    node, "node is a null pointer", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    options, "options is a null pointer", return RCL_RET_INVALID_ARGUMENT);

  rcl_ret_t ret = RCL_RET_OK;

  bool mem_allocs_ok = true;
  mem_allocs_ok &= rcl_interfaces__msg__Parameter__Sequence__init(
    &parameter_server->parameter_list,
    options->max_params);
  parameter_server->parameter_list.size = 0;

  // Pre-init strings
  for (size_t i = 0; i < options->max_params; ++i) {
    mem_allocs_ok &= rclc_parameter_descriptor_initialize_string(
      &parameter_server->parameter_list.data[i].name);
  }

  // Init list service msgs
  mem_allocs_ok &=
    rcl_interfaces__srv__ListParameters_Request__init(&parameter_server->list_request);
  mem_allocs_ok &= rcl_interfaces__srv__ListParameters_Response__init(
    &parameter_server->list_response);
  mem_allocs_ok &= rosidl_runtime_c__String__Sequence__init(
    &parameter_server->list_response.result.names,
    options->max_params);
  parameter_server->list_response.result.names.size = 0;

  // Pre-init strings
  for (size_t i = 0; i < options->max_params; ++i) {
    mem_allocs_ok &= rclc_parameter_descriptor_initialize_string(
      &parameter_server->list_response.result.names.data[i]);
  }

  // Init Get service msgs
  mem_allocs_ok &= rcl_interfaces__srv__GetParameters_Request__init(&parameter_server->get_request);
  mem_allocs_ok &=
    rcl_interfaces__srv__GetParameters_Response__init(&parameter_server->get_response);
  mem_allocs_ok &= rosidl_runtime_c__String__Sequence__init(
    &parameter_server->get_request.names,
    options->max_params);
  parameter_server->get_request.names.size = 0;
  mem_allocs_ok &= rcl_interfaces__msg__ParameterValue__Sequence__init(
    &parameter_server->get_response.values,
    options->max_params);
  parameter_server->get_response.values.size = 0;

  // Pre-init strings
  for (size_t i = 0; i < options->max_params; ++i) {
    mem_allocs_ok &= rclc_parameter_descriptor_initialize_string(
      &parameter_server->get_request.names.data[i]);
  }

  // Init Set service msgs
  mem_allocs_ok &= rcl_interfaces__srv__SetParameters_Request__init(&parameter_server->set_request);
  mem_allocs_ok &=
    rcl_interfaces__srv__SetParameters_Response__init(&parameter_server->set_response);
  mem_allocs_ok &= rcl_interfaces__msg__Parameter__Sequence__init(
    &parameter_server->set_request.parameters,
    options->max_params);
  parameter_server->set_request.parameters.size = 0;
  mem_allocs_ok &= rcl_interfaces__msg__SetParametersResult__Sequence__init(
    &parameter_server->set_response.results,
    options->max_params);
  parameter_server->set_response.results.size = 0;

  // Pre-init strings
  for (size_t i = 0; i < options->max_params; ++i) {
    mem_allocs_ok &= rclc_parameter_descriptor_initialize_string(
      &parameter_server->set_request.parameters.data[i].name);
    mem_allocs_ok &= rclc_parameter_descriptor_initialize_string(
      &parameter_server->set_response.results.data[i].reason);
  }

  // Init Get types service msgs
  mem_allocs_ok &= rcl_interfaces__srv__GetParameterTypes_Request__init(
    &parameter_server->get_types_request);
  mem_allocs_ok &= rcl_interfaces__srv__GetParameterTypes_Response__init(
    &parameter_server->get_types_response);
  mem_allocs_ok &= rosidl_runtime_c__String__Sequence__init(
    &parameter_server->get_types_request.names,
    options->max_params);
  parameter_server->get_types_request.names.size = 0;
  mem_allocs_ok &= rosidl_runtime_c__uint8__Sequence__init(
    &parameter_server->get_types_response.types,
    options->max_params);
  parameter_server->get_types_response.types.size = 0;

  for (size_t i = 0; i < options->max_params; ++i) {
    mem_allocs_ok &= rclc_parameter_descriptor_initialize_string(
      &parameter_server->get_types_request.names.data[i]);
  }

  // Init Describe service msgs
  mem_allocs_ok &= rcl_interfaces__srv__DescribeParameters_Request__init(
    &parameter_server->describe_request);
  mem_allocs_ok &= rcl_interfaces__srv__DescribeParameters_Response__init(
    &parameter_server->describe_response);
  mem_allocs_ok &= rosidl_runtime_c__String__Sequence__init(
    &parameter_server->describe_request.names,
    options->max_params);
  parameter_server->describe_request.names.size = 0;

  mem_allocs_ok &= rcl_interfaces__msg__ParameterDescriptor__Sequence__init(
    &parameter_server->describe_response.descriptors,
    options->max_params);
  parameter_server->describe_response.descriptors.size = 0;

  mem_allocs_ok &= rcl_interfaces__msg__ParameterDescriptor__Sequence__init(
    &parameter_server->parameter_descriptors,
    options->max_params);
  parameter_server->parameter_descriptors.size = 0;

  for (size_t i = 0; i < options->max_params; ++i) {
    // Init describe_request members
    mem_allocs_ok &= rclc_parameter_descriptor_initialize_string(
      &parameter_server->describe_request.names.data[i]);

    // Init describe_response members
    mem_allocs_ok &= rclc_parameter_descriptor_initialize_string(
      &parameter_server->describe_response.descriptors.data[i].name);
    mem_allocs_ok &= rclc_parameter_descriptor_initialize_string(
      &parameter_server->describe_response.descriptors.data[i].description);
    mem_allocs_ok &= rclc_parameter_descriptor_initialize_string(
      &parameter_server->describe_response.descriptors.data[i].additional_constraints);
    mem_allocs_ok &= rcl_interfaces__msg__FloatingPointRange__Sequence__init(
      &parameter_server->describe_response.descriptors.data[i].floating_point_range,
      rcl_interfaces__msg__ParameterDescriptor__floating_point_range__MAX_SIZE);
    parameter_server->describe_response.descriptors.data[i].floating_point_range.size = 0;
    mem_allocs_ok &= rcl_interfaces__msg__IntegerRange__Sequence__init(
      &parameter_server->describe_response.descriptors.data[i].integer_range,
      rcl_interfaces__msg__ParameterDescriptor__integer_range__MAX_SIZE);
    parameter_server->describe_response.descriptors.data[i].integer_range.size = 0;

    // Init parameter_descriptors members
    mem_allocs_ok &= rclc_parameter_descriptor_initialize_string(
      &parameter_server->parameter_descriptors.data[i].name);
    mem_allocs_ok &= rclc_parameter_descriptor_initialize_string(
      &parameter_server->parameter_descriptors.data[i].description);
    mem_allocs_ok &= rclc_parameter_descriptor_initialize_string(
      &parameter_server->parameter_descriptors.data[i].additional_constraints);
    mem_allocs_ok &= rcl_interfaces__msg__FloatingPointRange__Sequence__init(
      &parameter_server->parameter_descriptors.data[i].floating_point_range,
      rcl_interfaces__msg__ParameterDescriptor__floating_point_range__MAX_SIZE);
    parameter_server->parameter_descriptors.data[i].floating_point_range.size = 0;
    mem_allocs_ok &= rcl_interfaces__msg__IntegerRange__Sequence__init(
      &parameter_server->parameter_descriptors.data[i].integer_range,
      rcl_interfaces__msg__ParameterDescriptor__integer_range__MAX_SIZE);
    parameter_server->parameter_descriptors.data[i].integer_range.size = 0;
  }

  // Init event publisher msgs
  if (parameter_server->notify_changed_over_dds) {
    mem_allocs_ok &= rcl_interfaces__msg__ParameterEvent__init(&parameter_server->event_list);
    mem_allocs_ok &= rosidl_runtime_c__String__assign(
      &parameter_server->event_list.node,
      rcl_node_get_name(node));
  }

  if (!mem_allocs_ok) {
    ret = RCL_RET_ERROR;
  }

  return ret;
}

rcl_ret_t init_parameter_server_memory_low(
  rclc_parameter_server_t * parameter_server,
  rcl_node_t * node,
  const rclc_parameter_options_t * options)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    parameter_server, "parameter_server is a null pointer", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    node, "node is a null pointer", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    options, "options is a null pointer", return RCL_RET_INVALID_ARGUMENT);

  rcl_ret_t ret = RCL_RET_OK;

  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  // Init a parameter sequence
  parameter_server->parameter_list.data =
    allocator.zero_allocate(options->max_params, sizeof(Parameter), allocator.state);
  parameter_server->parameter_list.size = 0;
  parameter_server->parameter_list.capacity = options->max_params;

  // Init a parameter descriptor sequence
  parameter_server->parameter_descriptors.data = allocator.zero_allocate(
    options->max_params,
    sizeof(ParameterDescriptor),
    allocator.state);
  parameter_server->parameter_descriptors.size = 0;
  parameter_server->parameter_descriptors.capacity = options->max_params;

  for (size_t i = 0; i < options->max_params; ++i) {
    ret |= rclc_parameter_initialize_empty_string(
      &parameter_server->parameter_list.data[i].name,
      RCLC_PARAMETER_MAX_STRING_LENGTH);
    ret |=
      rclc_parameter_initialize_empty_string(
      &parameter_server->parameter_list.data[i].value.string_value, 1);
    ret |=
      rclc_parameter_initialize_empty_string(
      &parameter_server->parameter_descriptors.data[i].description, 1);
    ret |= rclc_parameter_initialize_empty_string(
      &parameter_server->parameter_descriptors.data[i].additional_constraints, 1);

    parameter_server->parameter_descriptors.data[i].floating_point_range.data =
      allocator.zero_allocate(
      1, sizeof(rcl_interfaces__msg__FloatingPointRange__Sequence),
      allocator.state);
    parameter_server->parameter_descriptors.data[i].floating_point_range.capacity = 1;
    parameter_server->parameter_descriptors.data[i].floating_point_range.size = 0;

    parameter_server->parameter_descriptors.data[i].integer_range.data = allocator.zero_allocate(
      1,
      sizeof(
        rcl_interfaces__msg__IntegerRange__Sequence), allocator.state);
    parameter_server->parameter_descriptors.data[i].integer_range.capacity = 1;
    parameter_server->parameter_descriptors.data[i].integer_range.size = 0;
  }

  // Parameter descriptors

  // Initialize empty string value

  // List parameters:
  //    - The request has no prefixes enabled nor depth.
  //    - The response has a sequence of names taken from the names of each parameter
  parameter_server->list_request.prefixes.data = NULL;
  parameter_server->list_request.prefixes.size = 0;
  parameter_server->list_request.prefixes.capacity = 0;

  parameter_server->list_response.result.names.data =
    allocator.allocate(sizeof(rosidl_runtime_c__String) * options->max_params, allocator.state);
  parameter_server->list_response.result.names.size = 0;
  parameter_server->list_response.result.names.capacity = options->max_params;
  for (size_t i = 0; i < options->max_params; ++i) {
    parameter_server->list_response.result.names.data[i].data =
      parameter_server->parameter_list.data[i].name.data;
    parameter_server->list_response.result.names.data[i].capacity =
      parameter_server->parameter_list.data[i].name.capacity;
    parameter_server->list_response.result.names.data[i].size =
      parameter_server->parameter_list.data[i].name.size;
  }

  parameter_server->list_response.result.prefixes.data = NULL;
  parameter_server->list_response.result.prefixes.size = 0;
  parameter_server->list_response.result.prefixes.capacity = 0;

  // Get parameters:
  //    - Only one parameter can be retrieved per request
  parameter_server->get_request.names.data = allocator.allocate(
    sizeof(rosidl_runtime_c__String),
    allocator.state);
  parameter_server->get_request.names.size = 0;
  parameter_server->get_request.names.capacity = 1;

  ret |= rclc_parameter_initialize_empty_string(
    &parameter_server->get_request.names.data[0],
    RCLC_PARAMETER_MAX_STRING_LENGTH);

  parameter_server->get_response.values.data = allocator.zero_allocate(
    1, sizeof(ParameterValue),
    allocator.state);
  parameter_server->get_response.values.size = 0;
  parameter_server->get_response.values.capacity = 1;

  ret |= rclc_parameter_initialize_empty_string(
    &parameter_server->get_response.values.data[0].string_value, 1);

  // Set parameters:
  //    - Only one parameter can be set, created or deleted per request
  // TODO(acuadros95): Check if alloc ParameterValue string_value
  parameter_server->set_request.parameters.data = allocator.zero_allocate(
    1, sizeof(Parameter),
    allocator.state);
  parameter_server->set_request.parameters.size = 0;
  parameter_server->set_request.parameters.capacity = 1;

  ret |= rclc_parameter_initialize_empty_string(
    &parameter_server->set_request.parameters.data[0].name,
    RCLC_PARAMETER_MAX_STRING_LENGTH);

  parameter_server->set_response.results.data =
    allocator.zero_allocate(1, sizeof(SetParameters_Result), allocator.state);
  parameter_server->set_response.results.size = 0;
  parameter_server->set_response.results.capacity = 1;

  ret |= rclc_parameter_initialize_empty_string(
    &parameter_server->set_response.results.data[0].reason,
    RCLC_SET_ERROR_MAX_STRING_LENGTH);

  // Get parameter types:
  //    - Only one parameter type can be retrieved per request
  parameter_server->get_types_request.names.data =
    allocator.allocate(sizeof(rosidl_runtime_c__String), allocator.state);
  parameter_server->get_types_request.names.size = 0;
  parameter_server->get_types_request.names.capacity = 1;

  ret |= rclc_parameter_initialize_empty_string(
    &parameter_server->get_types_request.names.data[0],
    RCLC_PARAMETER_MAX_STRING_LENGTH);

  parameter_server->get_types_response.types.data = allocator.zero_allocate(
    1, sizeof(uint8_t),
    allocator.state);
  parameter_server->get_types_response.types.size = 0;
  parameter_server->get_types_response.types.capacity = 1;

  // Describe parameters:
  //    - Only one description can be retrieved per request
  parameter_server->describe_request.names.data = allocator.allocate(
    sizeof(rosidl_runtime_c__String), allocator.state);
  parameter_server->describe_request.names.size = 0;
  parameter_server->describe_request.names.capacity = 1;

  ret |= rclc_parameter_initialize_empty_string(
    &parameter_server->describe_request.names.data[0],
    RCLC_PARAMETER_MAX_STRING_LENGTH);

  parameter_server->describe_response.descriptors.data =
    allocator.zero_allocate(1, sizeof(ParameterDescriptor), allocator.state);
  parameter_server->describe_response.descriptors.size = 0;
  parameter_server->describe_response.descriptors.capacity = 1;

  ret |= rclc_parameter_initialize_empty_string(
    &parameter_server->describe_response.descriptors.data[0].description,
    RCLC_PARAMETER_MAX_STRING_LENGTH);
  ret |= rclc_parameter_initialize_empty_string(
    &parameter_server->describe_response.descriptors.data[0].additional_constraints, 1);

  parameter_server->describe_response.descriptors.data[0].floating_point_range.data =
    allocator.zero_allocate(
    1,
    sizeof(rcl_interfaces__msg__FloatingPointRange__Sequence),
    allocator.state);
  parameter_server->describe_response.descriptors.data[0].floating_point_range.capacity = 1;
  parameter_server->describe_response.descriptors.data[0].floating_point_range.size = 0;

  parameter_server->describe_response.descriptors.data[0].integer_range.data =
    allocator.zero_allocate(
    1,
    sizeof(
      rcl_interfaces__msg__IntegerRange__Sequence), allocator.state);
  parameter_server->describe_response.descriptors.data[0].integer_range.capacity = 1;
  parameter_server->describe_response.descriptors.data[0].integer_range.size = 0;

  // Parameter events msg
  if (parameter_server->notify_changed_over_dds) {
    // Parameter node info
    parameter_server->event_list.node.data = (char *) rcl_node_get_name(node);
    parameter_server->event_list.node.size = strlen(rcl_node_get_name(node));
    parameter_server->event_list.node.capacity = parameter_server->event_list.node.size + 1;

    // Parameters event
    parameter_server->event_list.new_parameters.size = 0;
    parameter_server->event_list.new_parameters.capacity = 1;
    parameter_server->event_list.changed_parameters.size = 0;
    parameter_server->event_list.changed_parameters.capacity = 1;
    parameter_server->event_list.deleted_parameters.size = 0;
    parameter_server->event_list.deleted_parameters.capacity = 1;
  }

  return ret;
}

rcl_ret_t
rclc_parameter_server_init_with_option(
  rclc_parameter_server_t * parameter_server,
  rcl_node_t * node,
  const rclc_parameter_options_t * options)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    parameter_server, "parameter is a null pointer", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    node, "node is a null pointer", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    options, "options is a null pointer", return RCL_RET_INVALID_ARGUMENT);

  rcl_ret_t ret = RCL_RET_OK;

  const rosidl_service_type_support_t * get_ts = ROSIDL_GET_SRV_TYPE_SUPPORT(
    rcl_interfaces,
    srv,
    GetParameters);
  ret |= rclc_parameter_server_init_service(
    &parameter_server->get_service, node, "/get_parameters",
    get_ts);

  const rosidl_service_type_support_t * get_types_ts = ROSIDL_GET_SRV_TYPE_SUPPORT(
    rcl_interfaces,
    srv,
    GetParameterTypes);
  ret |= rclc_parameter_server_init_service(
    &parameter_server->get_types_service, node,
    "/get_parameter_types", get_types_ts);

  const rosidl_service_type_support_t * set_ts = ROSIDL_GET_SRV_TYPE_SUPPORT(
    rcl_interfaces,
    srv,
    SetParameters);
  ret |= rclc_parameter_server_init_service(
    &parameter_server->set_service, node, "/set_parameters",
    set_ts);

  const rosidl_service_type_support_t * list_ts = ROSIDL_GET_SRV_TYPE_SUPPORT(
    rcl_interfaces,
    srv,
    ListParameters);
  ret |= rclc_parameter_server_init_service(
    &parameter_server->list_service, node,
    "/list_parameters", list_ts);

  const rosidl_service_type_support_t * describe_ts = ROSIDL_GET_SRV_TYPE_SUPPORT(
    rcl_interfaces,
    srv,
    DescribeParameters);
  ret |= rclc_parameter_server_init_service(
    &parameter_server->describe_service, node,
    "/describe_parameters", describe_ts);

  parameter_server->on_callback = false;
  parameter_server->notify_changed_over_dds = options->notify_changed_over_dds;
  parameter_server->allow_undeclared_parameters = options->allow_undeclared_parameters;
  parameter_server->low_mem_mode = options->low_mem_mode;

  if (parameter_server->notify_changed_over_dds) {
    const rosidl_message_type_support_t * event_ts = ROSIDL_GET_MSG_TYPE_SUPPORT(
      rcl_interfaces,
      msg,
      ParameterEvent);
    ret |= rclc_publisher_init(
      &parameter_server->event_publisher, node, event_ts,
      "/parameter_events",
      &rmw_qos_profile_parameter_events);
  }

  if (parameter_server->low_mem_mode) {
    ret |= init_parameter_server_memory_low(parameter_server, node, options);
  } else {
    ret |= init_parameter_server_memory(parameter_server, node, options);
  }

  return ret;
}

void
rclc_parameter_server_fini_memory_low(
  rclc_parameter_server_t * parameter_server)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    parameter_server, "parameter_server is a null pointer", return );

  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  // Get request
  allocator.deallocate(parameter_server->get_request.names.data[0].data, allocator.state);
  allocator.deallocate(parameter_server->get_request.names.data, allocator.state);
  parameter_server->get_request.names.capacity = 0;
  parameter_server->get_request.names.size = 0;

  // Get response
  allocator.deallocate(
    parameter_server->get_response.values.data[0].string_value.data,
    allocator.state);
  allocator.deallocate(parameter_server->get_response.values.data, allocator.state);
  parameter_server->get_response.values.capacity = 0;
  parameter_server->get_response.values.size = 0;

  // Set request
  allocator.deallocate(parameter_server->set_request.parameters.data[0].name.data, allocator.state);
  allocator.deallocate(parameter_server->set_request.parameters.data, allocator.state);
  parameter_server->set_request.parameters.capacity = 0;
  parameter_server->set_request.parameters.size = 0;

  // Set response
  allocator.deallocate(parameter_server->set_response.results.data[0].reason.data, allocator.state);
  allocator.deallocate(parameter_server->set_response.results.data, allocator.state);
  parameter_server->set_response.results.capacity = 0;
  parameter_server->set_response.results.size = 0;

  // List response
  for (size_t i = 0; i < parameter_server->list_response.result.names.capacity; ++i) {
    parameter_server->list_response.result.names.data[i].data = NULL;
    parameter_server->list_response.result.names.data[i].capacity = 0;
    parameter_server->list_response.result.names.data[i].size = 0;
  }
  allocator.deallocate(parameter_server->list_response.result.names.data, allocator.state);

  // Get types request
  allocator.deallocate(parameter_server->get_types_request.names.data[0].data, allocator.state);
  allocator.deallocate(parameter_server->get_types_request.names.data, allocator.state);
  parameter_server->get_types_request.names.capacity = 0;
  parameter_server->get_types_request.names.size = 0;

  // Get types response
  allocator.deallocate(parameter_server->get_types_response.types.data, allocator.state);
  parameter_server->get_types_response.types.capacity = 0;
  parameter_server->get_types_response.types.size = 0;

  // Describe parameters request
  allocator.deallocate(parameter_server->describe_request.names.data[0].data, allocator.state);
  allocator.deallocate(parameter_server->describe_request.names.data, allocator.state);
  parameter_server->describe_request.names.size = 0;
  parameter_server->describe_request.names.capacity = 0;

  // Describe parameters response
  allocator.deallocate(
    parameter_server->describe_response.descriptors.data[0].floating_point_range.data,
    allocator.state);
  allocator.deallocate(
    parameter_server->describe_response.descriptors.data[0].integer_range.data,
    allocator.state);
  allocator.deallocate(
    parameter_server->describe_response.descriptors.data[0].description.data,
    allocator.state);
  allocator.deallocate(
    parameter_server->describe_response.descriptors.data[0].additional_constraints.data,
    allocator.state);
  allocator.deallocate(parameter_server->describe_response.descriptors.data, allocator.state);
  parameter_server->describe_response.descriptors.size = 0;
  parameter_server->describe_response.descriptors.capacity = 0;

  // Parameter list and parameter descriptors
  for (size_t i = 0; i < parameter_server->parameter_list.capacity; ++i) {
    allocator.deallocate(parameter_server->parameter_list.data[i].name.data, allocator.state);
    allocator.deallocate(
      parameter_server->parameter_list.data[i].value.string_value.data,
      allocator.state);
    allocator.deallocate(
      parameter_server->parameter_descriptors.data[i].description.data,
      allocator.state);
    allocator.deallocate(
      parameter_server->parameter_descriptors.data[i].additional_constraints.data, allocator.state);
    allocator.deallocate(
      parameter_server->parameter_descriptors.data[i].floating_point_range.data,
      allocator.state);
    allocator.deallocate(
      parameter_server->parameter_descriptors.data[i].integer_range.data,
      allocator.state);
  }

  allocator.deallocate(parameter_server->parameter_list.data, allocator.state);
  parameter_server->parameter_list.capacity = 0;
  parameter_server->parameter_list.size = 0;

  allocator.deallocate(parameter_server->parameter_descriptors.data, allocator.state);
  parameter_server->parameter_descriptors.size = 0;
  parameter_server->parameter_descriptors.capacity = 0;


  if (parameter_server->notify_changed_over_dds) {
    parameter_server->event_list.node.data = NULL;
    parameter_server->event_list.node.capacity = 0;
    parameter_server->event_list.node.size = 0;
  }
}

void
rclc_parameter_server_fini_memory(
  rclc_parameter_server_t * parameter_server)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    parameter_server, "parameter_server is a null pointer", return );

  // Fini describe msgs
  for (size_t i = 0; i < parameter_server->describe_request.names.capacity; ++i) {
    rosidl_runtime_c__String__fini(&parameter_server->describe_request.names.data[i]);
    rosidl_runtime_c__String__fini(&parameter_server->describe_response.descriptors.data[i].name);
    rosidl_runtime_c__String__fini(
      &parameter_server->describe_response.descriptors.data[i].description);
    rosidl_runtime_c__String__fini(
      &parameter_server->describe_response.descriptors.data[i].additional_constraints);
    rcl_interfaces__msg__IntegerRange__Sequence__fini(
      &parameter_server->describe_response.descriptors.data[i].integer_range);
    rcl_interfaces__msg__FloatingPointRange__Sequence__fini(
      &parameter_server->describe_response.descriptors.data[i].floating_point_range);
  }

  rcl_interfaces__msg__ParameterDescriptor__Sequence__fini(
    &parameter_server->describe_response.descriptors);
  rosidl_runtime_c__String__Sequence__fini(&parameter_server->describe_request.names);
  rcl_interfaces__srv__DescribeParameters_Response__fini(&parameter_server->describe_response);
  rcl_interfaces__srv__DescribeParameters_Request__fini(&parameter_server->describe_request);

  // Fini get types msgs
  for (size_t i = 0; i < parameter_server->get_types_request.names.capacity; ++i) {
    rosidl_runtime_c__String__fini(&parameter_server->get_types_request.names.data[i]);
  }

  rosidl_runtime_c__uint8__Sequence__fini(&parameter_server->get_types_response.types);
  rosidl_runtime_c__String__Sequence__fini(&parameter_server->get_types_request.names);
  rcl_interfaces__srv__GetParameterTypes_Response__fini(&parameter_server->get_types_response);
  rcl_interfaces__srv__GetParameterTypes_Request__fini(&parameter_server->get_types_request);

  // Finish set msgs
  for (size_t i = 0; i < parameter_server->set_request.parameters.capacity; ++i) {
    rosidl_runtime_c__String__fini(&parameter_server->set_request.parameters.data[i].name);
    rosidl_runtime_c__String__fini(&parameter_server->set_response.results.data[i].reason);
  }

  rcl_interfaces__msg__SetParametersResult__Sequence__fini(&parameter_server->set_response.results);
  rcl_interfaces__msg__Parameter__Sequence__fini(&parameter_server->set_request.parameters);
  rcl_interfaces__srv__SetParameters_Response__fini(&parameter_server->set_response);
  rcl_interfaces__srv__SetParameters_Request__fini(&parameter_server->set_request);

  // Finish get msgs
  for (size_t i = 0; i < parameter_server->get_request.names.capacity; ++i) {
    rosidl_runtime_c__String__fini(&parameter_server->get_request.names.data[i]);
  }

  rcl_interfaces__msg__ParameterValue__Sequence__fini(&parameter_server->get_response.values);
  rosidl_runtime_c__String__Sequence__fini(&parameter_server->get_request.names);
  rcl_interfaces__srv__GetParameters_Response__fini(&parameter_server->get_response);
  rcl_interfaces__srv__GetParameters_Request__fini(&parameter_server->get_request);

  // Finish list msgs
  for (size_t i = 0; i < parameter_server->list_response.result.names.capacity; ++i) {
    rosidl_runtime_c__String__fini(&parameter_server->list_response.result.names.data[i]);
  }

  rosidl_runtime_c__String__Sequence__fini(&parameter_server->list_response.result.names);
  rcl_interfaces__srv__ListParameters_Response__fini(&parameter_server->list_response);
  rcl_interfaces__srv__ListParameters_Request__fini(&parameter_server->list_request);

  // Free parameter list
  for (size_t i = 0; i < parameter_server->parameter_list.capacity; ++i) {
    rosidl_runtime_c__String__fini(&parameter_server->parameter_list.data[i].name);
  }

  rcl_interfaces__msg__Parameter__Sequence__fini(&parameter_server->parameter_list);

  // Free parameter descriptor list
  for (size_t i = 0; i < parameter_server->parameter_descriptors.capacity; ++i) {
    rosidl_runtime_c__String__fini(&parameter_server->parameter_descriptors.data[i].name);
    rosidl_runtime_c__String__fini(&parameter_server->parameter_descriptors.data[i].description);
    rosidl_runtime_c__String__fini(
      &parameter_server->parameter_descriptors.data[i].additional_constraints);
    rcl_interfaces__msg__IntegerRange__Sequence__fini(
      &parameter_server->parameter_descriptors.data[i].integer_range);
    rcl_interfaces__msg__FloatingPointRange__Sequence__fini(
      &parameter_server->parameter_descriptors.data[i].floating_point_range);
  }

  if (parameter_server->notify_changed_over_dds) {
    rosidl_runtime_c__String__fini(&parameter_server->event_list.node);
  }

  rcl_interfaces__msg__ParameterDescriptor__Sequence__fini(
    &parameter_server->parameter_descriptors);
}

rcl_ret_t
rclc_parameter_server_fini(
  rclc_parameter_server_t * parameter_server,
  rcl_node_t * node)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    parameter_server, "parameter server is a null pointer", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    node, "node is a null pointer", return RCL_RET_INVALID_ARGUMENT);

  rcl_ret_t ret = RCL_RET_OK;

  ret |= rcl_service_fini(&parameter_server->list_service, node);
  ret |= rcl_service_fini(&parameter_server->set_service, node);
  ret |= rcl_service_fini(&parameter_server->get_service, node);
  ret |= rcl_service_fini(&parameter_server->get_types_service, node);
  ret |= rcl_service_fini(&parameter_server->describe_service, node);

  if (parameter_server->notify_changed_over_dds) {
    ret |= rcl_publisher_fini(&parameter_server->event_publisher, node);
  }

  if (parameter_server->low_mem_mode) {
    rclc_parameter_server_fini_memory_low(parameter_server);
  } else {
    rclc_parameter_server_fini_memory(parameter_server);
  }
  return ret;
}

rcl_ret_t
rclc_executor_add_parameter_server(
  rclc_executor_t * executor,
  rclc_parameter_server_t * parameter_server,
  rclc_parameter_callback_t on_modification)
{
  return rclc_executor_add_parameter_server_with_context(
    executor, parameter_server,
    on_modification, NULL);
}

rcl_ret_t
rclc_executor_add_parameter_server_with_context(
  rclc_executor_t * executor,
  rclc_parameter_server_t * parameter_server,
  rclc_parameter_callback_t on_modification,
  void * context)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    executor, "executor is a null pointer", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    parameter_server, "parameter_server is a null pointer", return RCL_RET_INVALID_ARGUMENT);

  rcl_ret_t ret;

  parameter_server->on_modification = on_modification;
  parameter_server->context = context;

  ret = rclc_executor_add_service_with_context(
    executor, &parameter_server->list_service,
    &parameter_server->list_request, &parameter_server->list_response,
    rclc_parameter_server_list_service_callback, parameter_server);

  ret |= rclc_executor_add_service_with_context(
    executor, &parameter_server->get_types_service,
    &parameter_server->get_types_request, &parameter_server->get_types_response,
    rclc_parameter_server_get_types_service_callback, parameter_server);

  ret |= rclc_executor_add_service_with_context(
    executor, &parameter_server->set_service,
    &parameter_server->set_request, &parameter_server->set_response,
    rclc_parameter_server_set_service_callback,
    parameter_server);

  ret |= rclc_executor_add_service_with_context(
    executor, &parameter_server->get_service,
    &parameter_server->get_request, &parameter_server->get_response,
    rclc_parameter_server_get_service_callback,
    parameter_server);

  ret |= rclc_executor_add_service_with_context(
    executor, &parameter_server->describe_service,
    &parameter_server->describe_request, &parameter_server->describe_response,
    rclc_parameter_server_describe_service_callback,
    parameter_server);

  return ret;
}

rcl_ret_t
rclc_add_parameter(
  rclc_parameter_server_t * parameter_server,
  const char * parameter_name,
  rclc_parameter_type_t type)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    parameter_server, "parameter_server is a null pointer", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    parameter_name, "parameter_name is a null pointer", return RCL_RET_INVALID_ARGUMENT);

  if (parameter_server->on_callback) {
    return RCLC_PARAMETER_DISABLED_ON_CALLBACK;
  }

  size_t index = parameter_server->parameter_list.size;

  if (index >= parameter_server->parameter_list.capacity ||
    rclc_parameter_search(&parameter_server->parameter_list, parameter_name) != NULL)
  {
    return RCL_RET_ERROR;
  }

  if (!rclc_parameter_set_string(
      &parameter_server->parameter_list.data[index].name,
      parameter_name))
  {
    return RCL_RET_ERROR;
  }

  parameter_server->parameter_list.data[index].value.type = type;
  ++parameter_server->parameter_list.size;

  // Add to parameter descriptors
  if (!parameter_server->low_mem_mode && !rclc_parameter_set_string(
      &parameter_server->parameter_descriptors.data[index].name,
      parameter_name))
  {
    return RCL_RET_ERROR;
  }

  parameter_server->parameter_descriptors.data[index].type = type;
  ++parameter_server->parameter_descriptors.size;

  if (parameter_server->notify_changed_over_dds) {
    rclc_parameter_prepare_new_event(
      &parameter_server->event_list,
      &parameter_server->parameter_list.data[index]);
    return rclc_parameter_service_publish_event(parameter_server);
  }

  return RCL_RET_OK;
}

rcl_ret_t
rclc_add_parameter_undeclared(
  rclc_parameter_server_t * parameter_server,
  Parameter * parameter)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    parameter_server, "parameter_server is a null pointer", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    parameter, "parameter is a null pointer", return RCL_RET_INVALID_ARGUMENT);

  size_t index = parameter_server->parameter_list.size;

  if (index >= parameter_server->parameter_list.capacity ||
    rclc_parameter_search(&parameter_server->parameter_list, parameter->name.data) != NULL)
  {
    return RCL_RET_ERROR;
  }

  if (RCL_RET_OK != rclc_parameter_copy(
      &parameter_server->parameter_list.data[index],
      parameter))
  {
    return RCL_RET_ERROR;
  }

  ++parameter_server->parameter_list.size;

  // Add to parameter descriptors
  if (!parameter_server->low_mem_mode && !rclc_parameter_set_string(
      &parameter_server->parameter_descriptors.data[index].name,
      parameter->name.data))
  {
    return RCL_RET_ERROR;
  }

  parameter_server->parameter_descriptors.data[index].type =
    parameter->value.type;
  ++parameter_server->parameter_descriptors.size;

  if (parameter_server->notify_changed_over_dds) {
    rclc_parameter_prepare_new_event(
      &parameter_server->event_list,
      &parameter_server->parameter_list.data[index]);
    return rclc_parameter_service_publish_event(parameter_server);
  }

  return RCL_RET_OK;
}

rcl_ret_t
rclc_delete_parameter(
  rclc_parameter_server_t * parameter_server,
  const char * parameter_name)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    parameter_server, "parameter_server is a null pointer", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    parameter_name, "parameter_name is a null pointer", return RCL_RET_INVALID_ARGUMENT);

  if (parameter_server->on_callback) {
    return RCLC_PARAMETER_DISABLED_ON_CALLBACK;
  }

  // Find parameter
  size_t index = rclc_parameter_search_index(&parameter_server->parameter_list, parameter_name);

  if (index >= parameter_server->parameter_list.size) {
    return RCL_RET_ERROR;
  }


  Parameter * param = &parameter_server->parameter_list.data[index];
  ParameterDescriptor * param_description = &parameter_server->parameter_descriptors.data[index];

  if (parameter_server->notify_changed_over_dds) {
    rclc_parameter_prepare_deleted_event(&parameter_server->event_list, param);
    rclc_parameter_service_publish_event(parameter_server);
  }

  // Reset parameter
  rclc_parameter_set_string(&param->name, "");
  param->value.type = RCLC_PARAMETER_NOT_SET;

  // Reset parameter description
  param_description->type = RCLC_PARAMETER_NOT_SET;
  param_description->floating_point_range.size = 0;
  param_description->integer_range.size = 0;
  if (!parameter_server->low_mem_mode) {
    rclc_parameter_set_string(&param_description->description, "");
    rclc_parameter_set_string(&param_description->additional_constraints, "");
  }

  for (size_t i = index; i < (parameter_server->parameter_list.size - 1); ++i) {
    // Move parameter list
    rclc_parameter_copy(
      &parameter_server->parameter_list.data[i],
      &parameter_server->parameter_list.data[i + 1]);

    // Move descriptors
    rclc_parameter_descriptor_copy(
      &parameter_server->parameter_descriptors.data[i],
      &parameter_server->parameter_descriptors.data[i + 1],
      parameter_server->low_mem_mode);
  }

  parameter_server->parameter_descriptors.size--;
  parameter_server->parameter_list.size--;

  return RCL_RET_OK;
}

rcl_ret_t
rclc_parameter_set_bool(
  rclc_parameter_server_t * parameter_server,
  const char * parameter_name,
  bool value)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    parameter_server, "parameter_server is a null pointer", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    parameter_name, "parameter_name is a null pointer", return RCL_RET_INVALID_ARGUMENT);

  if (parameter_server->on_callback) {
    return RCLC_PARAMETER_DISABLED_ON_CALLBACK;
  }

  Parameter * parameter =
    rclc_parameter_search(&parameter_server->parameter_list, parameter_name);

  if (parameter == NULL) {
    return RCL_RET_ERROR;
  }

  if (parameter->value.type != RCLC_PARAMETER_BOOL) {
    return RCLC_PARAMETER_TYPE_MISMATCH;
  }

  Parameter new_parameter = *parameter;
  new_parameter.value.bool_value = value;

  if (RCL_RET_OK !=
    rclc_parameter_execute_callback(parameter_server, parameter, &new_parameter))
  {
    return RCLC_PARAMETER_MODIFICATION_REJECTED;
  }

  parameter->value.bool_value = value;

  if (parameter_server->notify_changed_over_dds) {
    rclc_parameter_prepare_changed_event(&parameter_server->event_list, parameter);
    rclc_parameter_service_publish_event(parameter_server);
  }

  return RCL_RET_OK;
}

rcl_ret_t
rclc_parameter_set_int(
  rclc_parameter_server_t * parameter_server,
  const char * parameter_name,
  int64_t value)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    parameter_server, "parameter_server is a null pointer", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    parameter_name, "parameter_name is a null pointer", return RCL_RET_INVALID_ARGUMENT);

  if (parameter_server->on_callback) {
    return RCLC_PARAMETER_DISABLED_ON_CALLBACK;
  }

  Parameter * parameter =
    rclc_parameter_search(&parameter_server->parameter_list, parameter_name);

  if (parameter == NULL) {
    return RCL_RET_ERROR;
  }

  if (parameter->value.type != RCLC_PARAMETER_INT) {
    return RCLC_PARAMETER_TYPE_MISMATCH;
  }

  Parameter new_parameter = *parameter;
  new_parameter.value.integer_value = value;

  if (RCL_RET_OK !=
    rclc_parameter_execute_callback(parameter_server, parameter, &new_parameter))
  {
    return RCLC_PARAMETER_MODIFICATION_REJECTED;
  }

  parameter->value.integer_value = value;

  if (parameter_server->notify_changed_over_dds) {
    rclc_parameter_prepare_changed_event(&parameter_server->event_list, parameter);
    rclc_parameter_service_publish_event(parameter_server);
  }

  return RCL_RET_OK;
}

rcl_ret_t
rclc_parameter_set_double(
  rclc_parameter_server_t * parameter_server,
  const char * parameter_name,
  double value)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    parameter_server, "parameter_server is a null pointer", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    parameter_name, "parameter_name is a null pointer", return RCL_RET_INVALID_ARGUMENT);

  if (parameter_server->on_callback) {
    return RCLC_PARAMETER_DISABLED_ON_CALLBACK;
  }

  Parameter * parameter =
    rclc_parameter_search(&parameter_server->parameter_list, parameter_name);

  if (parameter == NULL) {
    return RCL_RET_ERROR;
  }

  if (parameter->value.type != RCLC_PARAMETER_DOUBLE) {
    return RCLC_PARAMETER_TYPE_MISMATCH;
  }

  Parameter new_parameter = *parameter;
  new_parameter.value.double_value = value;

  if (RCL_RET_OK !=
    rclc_parameter_execute_callback(parameter_server, parameter, &new_parameter))
  {
    return RCLC_PARAMETER_MODIFICATION_REJECTED;
  }

  parameter->value.double_value = value;

  if (parameter_server->notify_changed_over_dds) {
    rclc_parameter_prepare_changed_event(&parameter_server->event_list, parameter);
    rclc_parameter_service_publish_event(parameter_server);
  }

  return RCL_RET_OK;
}

rcl_ret_t
rclc_parameter_get_bool(
  rclc_parameter_server_t * parameter_server,
  const char * parameter_name,
  bool * output)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    parameter_server, "parameter_server is a null pointer", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    parameter_name, "parameter_name is a null pointer", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    output, "output is a null pointer", return RCL_RET_INVALID_ARGUMENT);

  Parameter * parameter =
    rclc_parameter_search(&parameter_server->parameter_list, parameter_name);

  rcl_ret_t ret = RCL_RET_OK;

  if (parameter == NULL) {
    ret = RCL_RET_ERROR;
  } else if (parameter->value.type != RCLC_PARAMETER_BOOL) {
    ret = RCL_RET_INVALID_ARGUMENT;
  } else if (parameter != NULL) {
    *output = parameter->value.bool_value;
  }

  return ret;
}

rcl_ret_t
rclc_parameter_get_int(
  rclc_parameter_server_t * parameter_server,
  const char * parameter_name,
  int64_t * output)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    parameter_server, "parameter_server is a null pointer", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    parameter_name, "parameter_name is a null pointer", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    output, "output is a null pointer", return RCL_RET_INVALID_ARGUMENT);

  Parameter * parameter =
    rclc_parameter_search(&parameter_server->parameter_list, parameter_name);

  rcl_ret_t ret = RCL_RET_OK;

  if (parameter == NULL) {
    ret = RCL_RET_ERROR;
  } else if (parameter->value.type != RCLC_PARAMETER_INT) {
    ret = RCL_RET_INVALID_ARGUMENT;
  } else if (parameter != NULL) {
    *output = parameter->value.integer_value;
  }

  return ret;
}

rcl_ret_t
rclc_parameter_get_double(
  rclc_parameter_server_t * parameter_server,
  const char * parameter_name,
  double * output)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    parameter_server, "parameter_server is a null pointer", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    parameter_name, "parameter_name is a null pointer", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    output, "output is a null pointer", return RCL_RET_INVALID_ARGUMENT);

  Parameter * parameter =
    rclc_parameter_search(&parameter_server->parameter_list, parameter_name);

  rcl_ret_t ret = RCL_RET_OK;

  if (parameter == NULL) {
    ret = RCL_RET_ERROR;
  } else if (parameter->value.type != RCLC_PARAMETER_DOUBLE) {
    ret = RCL_RET_INVALID_ARGUMENT;
  } else if (parameter != NULL) {
    *output = parameter->value.double_value;
  }

  return ret;
}

rcl_ret_t
rclc_parameter_service_publish_event(
  rclc_parameter_server_t * parameter_server)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    parameter_server, "parameter_server is a null pointer", return RCL_RET_INVALID_ARGUMENT);

  rcl_ret_t ret = RCL_RET_OK;

  rcutils_time_point_value_t now;
  ret |= rcutils_system_time_now(&now);

  parameter_server->event_list.stamp.sec = RCUTILS_NS_TO_S(now);
  parameter_server->event_list.stamp.nanosec =
    now - RCUTILS_S_TO_NS(parameter_server->event_list.stamp.sec);

  ret |= rcl_publish(
    &parameter_server->event_publisher, &parameter_server->event_list,
    NULL);

  return ret;
}

rcl_ret_t
rclc_parameter_server_init_service(
  rcl_service_t * service,
  rcl_node_t * node,
  char * service_name,
  const rosidl_service_type_support_t * srv_type)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    service, "service is a null pointer", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    node, "node is a null pointer", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    service_name, "service_name is a null pointer", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    srv_type, "srv_type is a null pointer", return RCL_RET_INVALID_ARGUMENT);

  const char * node_name = rcl_node_get_name(node);

  static char get_service_name[RCLC_PARAMETER_MAX_STRING_LENGTH];
  memset(get_service_name, 0, RCLC_PARAMETER_MAX_STRING_LENGTH);
  memcpy(get_service_name, node_name, strlen(node_name) + 1);
  memcpy((get_service_name + strlen(node_name)), service_name, strlen(service_name) + 1);
  return rclc_service_init(service, node, srv_type, get_service_name, &rmw_qos_profile_parameters);
}

rcl_ret_t rclc_add_parameter_description(
  rclc_parameter_server_t * parameter_server,
  const char * parameter_name,
  const char * parameter_description,
  const char * additional_constraints)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    parameter_server, "parameter_server is a null pointer", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    parameter_name, "parameter_name is a null pointer", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    parameter_description, "parameter_description is a null pointer",
    return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    additional_constraints, "additional_constraints is a null pointer",
    return RCL_RET_INVALID_ARGUMENT);

  if (parameter_server->low_mem_mode) {
    return RCLC_PARAMETER_UNSUPORTED_ON_LOW_MEM;
  }

  size_t index = rclc_parameter_search_index(&parameter_server->parameter_list, parameter_name);

  if (index >= parameter_server->parameter_list.size) {
    return RCL_RET_ERROR;
  }

  ParameterDescriptor * parameter_descriptor = &parameter_server->parameter_descriptors.data[index];

  // Set parameter description
  if (!rclc_parameter_set_string(&parameter_descriptor->description, parameter_description)) {
    return RCL_RET_ERROR;
  }

  // Set constraint description
  if (!rclc_parameter_set_string(
      &parameter_descriptor->additional_constraints,
      additional_constraints))
  {
    return RCL_RET_ERROR;
  }

  return RCL_RET_OK;
}

rcl_ret_t rclc_set_parameter_read_only(
  rclc_parameter_server_t * parameter_server,
  const char * parameter_name,
  bool read_only)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    parameter_server, "parameter_server is a null pointer", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    parameter_name, "parameter_name is a null pointer", return RCL_RET_INVALID_ARGUMENT);

  if (parameter_server->on_callback) {
    return RCLC_PARAMETER_DISABLED_ON_CALLBACK;
  }

  size_t index = rclc_parameter_search_index(&parameter_server->parameter_list, parameter_name);

  if (index >= parameter_server->parameter_list.size) {
    return RCL_RET_ERROR;
  }

  ParameterDescriptor * parameter_descriptor = &parameter_server->parameter_descriptors.data[index];

  // Set flag
  parameter_descriptor->read_only = read_only;

  return RCL_RET_OK;
}

rcl_ret_t rclc_add_parameter_constraint_double(
  rclc_parameter_server_t * parameter_server,
  const char * parameter_name,
  double from_value,
  double to_value,
  double step)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    parameter_server, "parameter_server is a null pointer", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    parameter_name, "parameter_name is a null pointer", return RCL_RET_INVALID_ARGUMENT);

  if (parameter_server->on_callback) {
    return RCLC_PARAMETER_DISABLED_ON_CALLBACK;
  }

  size_t index = rclc_parameter_search_index(&parameter_server->parameter_list, parameter_name);

  if (index >= parameter_server->parameter_list.size) {
    return RCL_RET_ERROR;
  }

  ParameterDescriptor * parameter_descriptor = &parameter_server->parameter_descriptors.data[index];

  if (parameter_descriptor->type != RCLC_PARAMETER_DOUBLE) {
    return RCL_RET_ERROR;
  }

  parameter_descriptor->floating_point_range.data->from_value = from_value;
  parameter_descriptor->floating_point_range.data->to_value = to_value;
  parameter_descriptor->floating_point_range.data->step = step;
  parameter_descriptor->floating_point_range.size = 1;

  return RCL_RET_OK;
}

rcl_ret_t rclc_add_parameter_constraint_integer(
  rclc_parameter_server_t * parameter_server,
  const char * parameter_name, int64_t from_value,
  int64_t to_value, uint64_t step)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    parameter_server, "parameter_server is a null pointer", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    parameter_name, "parameter_name is a null pointer", return RCL_RET_INVALID_ARGUMENT);

  if (parameter_server->on_callback) {
    return RCLC_PARAMETER_DISABLED_ON_CALLBACK;
  }

  size_t index = rclc_parameter_search_index(&parameter_server->parameter_list, parameter_name);

  if (index >= parameter_server->parameter_list.size) {
    return RCL_RET_ERROR;
  }

  ParameterDescriptor * parameter_descriptor = &parameter_server->parameter_descriptors.data[index];

  if (parameter_descriptor->type != RCLC_PARAMETER_INT) {
    return RCL_RET_ERROR;
  }

  parameter_descriptor->integer_range.data->from_value = from_value;
  parameter_descriptor->integer_range.data->to_value = to_value;
  parameter_descriptor->integer_range.data->step = step;
  parameter_descriptor->integer_range.size = 1;

  return RCL_RET_OK;
}

rcl_ret_t rclc_parameter_execute_callback(
  rclc_parameter_server_t * parameter_server,
  const Parameter * old_param,
  const Parameter * new_param)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    parameter_server, "parameter_server is a null pointer", return RCL_RET_INVALID_ARGUMENT);

  bool ret = true;

  if (parameter_server->on_modification) {
    parameter_server->on_callback = true;
    ret = parameter_server->on_modification(old_param, new_param, parameter_server->context);
    parameter_server->on_callback = false;
  }

  return ret ? RCL_RET_OK : RCLC_PARAMETER_MODIFICATION_REJECTED;
}

#if __cplusplus
}
#endif /* if __cplusplus */
