// Copyright 2018 Apex.AI, Inc.
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

#include "./impl/node_params.h"
#include "./impl/types.h"
#include "./impl/yaml_variant.h"

#define INIT_NUM_PARAMS_PER_NODE 128U

rcutils_ret_t node_params_init(
  rcl_node_params_t * node_params,
  const rcutils_allocator_t allocator)
{
  return node_params_init_with_capacity(node_params, INIT_NUM_PARAMS_PER_NODE, allocator);
}

rcutils_ret_t node_params_init_with_capacity(
  rcl_node_params_t * node_params,
  size_t capacity,
  const rcutils_allocator_t allocator)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(node_params, RCUTILS_RET_INVALID_ARGUMENT);
  RCUTILS_CHECK_ALLOCATOR_WITH_MSG(
    &allocator, "invalid allocator", return RCUTILS_RET_INVALID_ARGUMENT);
  if (capacity == 0) {
    RCUTILS_SET_ERROR_MSG("capacity can't be zero");
    return RCUTILS_RET_INVALID_ARGUMENT;
  }

  node_params->parameter_names = allocator.zero_allocate(
    capacity, sizeof(char *), allocator.state);
  if (NULL == node_params->parameter_names) {
    RCUTILS_SET_ERROR_MSG("Failed to allocate memory for node parameter names");
    return RCUTILS_RET_BAD_ALLOC;
  }

  node_params->parameter_values = allocator.zero_allocate(
    capacity, sizeof(rcl_variant_t), allocator.state);
  if (NULL == node_params->parameter_values) {
    allocator.deallocate(node_params->parameter_names, allocator.state);
    node_params->parameter_names = NULL;
    RCUTILS_SET_ERROR_MSG("Failed to allocate memory for node parameter values");
    return RCUTILS_RET_BAD_ALLOC;
  }

  node_params->num_params = 0U;
  node_params->capacity_params = capacity;
  return RCUTILS_RET_OK;
}

rcutils_ret_t node_params_reallocate(
  rcl_node_params_t * node_params,
  size_t new_capacity,
  const rcutils_allocator_t allocator)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(node_params, RCUTILS_RET_INVALID_ARGUMENT);
  RCUTILS_CHECK_ALLOCATOR_WITH_MSG(
    &allocator, "invalid allocator", return RCUTILS_RET_INVALID_ARGUMENT);
  // invalid if new_capacity is less than num_params
  if (new_capacity < node_params->num_params) {
    RCUTILS_SET_ERROR_MSG_WITH_FORMAT_STRING(
      "new capacity '%zu' must be greater than or equal to '%zu'",
      new_capacity,
      node_params->num_params);
    return RCUTILS_RET_INVALID_ARGUMENT;
  }

  void * parameter_names = allocator.reallocate(
    node_params->parameter_names, new_capacity * sizeof(char *), allocator.state);
  if (NULL == parameter_names) {
    RCUTILS_SET_ERROR_MSG("Failed to reallocate node parameter names");
    return RCUTILS_RET_BAD_ALLOC;
  }
  node_params->parameter_names = parameter_names;
  // zero initialization for the added memory
  if (new_capacity > node_params->capacity_params) {
    memset(
      node_params->parameter_names + node_params->capacity_params, 0,
      (new_capacity - node_params->capacity_params) * sizeof(char *));
  }

  void * parameter_values = allocator.reallocate(
    node_params->parameter_values, new_capacity * sizeof(rcl_variant_t), allocator.state);
  if (NULL == parameter_values) {
    RCUTILS_SET_ERROR_MSG("Failed to reallocate node parameter values");
    return RCUTILS_RET_BAD_ALLOC;
  }
  node_params->parameter_values = parameter_values;
  // zero initialization for the added memory
  if (new_capacity > node_params->capacity_params) {
    memset(
      &node_params->parameter_values[node_params->capacity_params], 0,
      (new_capacity - node_params->capacity_params) * sizeof(rcl_variant_t));
  }

  node_params->capacity_params = new_capacity;
  return RCUTILS_RET_OK;
}

void rcl_yaml_node_params_fini(
  rcl_node_params_t * node_params_st,
  const rcutils_allocator_t allocator)
{
  if (NULL == node_params_st) {
    return;
  }

  if (NULL != node_params_st->parameter_names) {
    for (size_t parameter_idx = 0U; parameter_idx < node_params_st->num_params;
      parameter_idx++)
    {
      char * param_name = node_params_st->parameter_names[parameter_idx];
      if (NULL != param_name) {
        allocator.deallocate(param_name, allocator.state);
      }
    }
    allocator.deallocate(node_params_st->parameter_names, allocator.state);
    node_params_st->parameter_names = NULL;
  }

  if (NULL != node_params_st->parameter_values) {
    for (size_t parameter_idx = 0U; parameter_idx < node_params_st->num_params;
      parameter_idx++)
    {
      rcl_yaml_variant_fini(&(node_params_st->parameter_values[parameter_idx]), allocator);
    }

    allocator.deallocate(node_params_st->parameter_values, allocator.state);
    node_params_st->parameter_values = NULL;
  }

  node_params_st->num_params = 0;
  node_params_st->capacity_params = 0;
}
