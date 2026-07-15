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

#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <yaml.h>

#include "rcl_yaml_param_parser/parser.h"
#include "rcl_yaml_param_parser/types.h"

#include "rcutils/allocator.h"
#include "rcutils/error_handling.h"
#include "rcutils/strdup.h"
#include "rcutils/types.h"

#include "./impl/types.h"
#include "./impl/parse.h"
#include "./impl/node_params.h"
#include "./impl/yaml_variant.h"

#define INIT_NUM_NODE_ENTRIES 128U

///
/// Create the rcl_params_t parameter structure
///
rcl_params_t * rcl_yaml_node_struct_init(
  const rcutils_allocator_t allocator)
{
  return rcl_yaml_node_struct_init_with_capacity(INIT_NUM_NODE_ENTRIES, allocator);
}

rcl_params_t * rcl_yaml_node_struct_init_with_capacity(
  size_t capacity,
  const rcutils_allocator_t allocator)
{
  RCUTILS_CHECK_ALLOCATOR_WITH_MSG(&allocator, "invalid allocator", return NULL);
  if (capacity == 0) {
    RCUTILS_SET_ERROR_MSG("capacity can't be zero");
    return NULL;
  }
  rcl_params_t * params_st = allocator.zero_allocate(1, sizeof(rcl_params_t), allocator.state);
  if (NULL == params_st) {
    RCUTILS_SET_ERROR_MSG("Failed to allocate memory for parameters");
    return NULL;
  }

  params_st->allocator = allocator;

  params_st->node_names = allocator.zero_allocate(
    capacity, sizeof(char *), allocator.state);
  if (NULL == params_st->node_names) {
    RCUTILS_SET_ERROR_MSG("Failed to allocate memory for parameter node names");
    goto clean;
  }

  params_st->params = allocator.zero_allocate(
    capacity, sizeof(rcl_node_params_t), allocator.state);
  if (NULL == params_st->params) {
    allocator.deallocate(params_st->node_names, allocator.state);
    params_st->node_names = NULL;
    RCUTILS_SET_ERROR_MSG("Failed to allocate memory for parameter values");
    goto clean;
  }

  params_st->num_nodes = 0U;
  params_st->capacity_nodes = capacity;
  return params_st;

clean:
  allocator.deallocate(params_st, allocator.state);
  return NULL;
}

rcutils_ret_t rcl_yaml_node_struct_reallocate(
  rcl_params_t * params_st,
  size_t new_capacity,
  const rcutils_allocator_t allocator)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(params_st, RCUTILS_RET_INVALID_ARGUMENT);
  RCUTILS_CHECK_ALLOCATOR_WITH_MSG(
    &allocator, "invalid allocator", return RCUTILS_RET_INVALID_ARGUMENT);
  // invalid if new_capacity is less than num_nodes
  if (new_capacity < params_st->num_nodes) {
    RCUTILS_SET_ERROR_MSG_WITH_FORMAT_STRING(
      "new capacity '%zu' must be greater than or equal to '%zu'",
      new_capacity,
      params_st->num_nodes);
    return RCUTILS_RET_INVALID_ARGUMENT;
  }

  void * node_names = allocator.reallocate(
    params_st->node_names, new_capacity * sizeof(char *), allocator.state);
  if (NULL == node_names) {
    RCUTILS_SET_ERROR_MSG("Failed to reallocate memory for parameter node names");
    return RCUTILS_RET_BAD_ALLOC;
  }
  params_st->node_names = node_names;
  // zero initialization for the added memory
  if (new_capacity > params_st->capacity_nodes) {
    memset(
      params_st->node_names + params_st->capacity_nodes, 0,
      (new_capacity - params_st->capacity_nodes) * sizeof(char *));
  }

  void * params = allocator.reallocate(
    params_st->params, new_capacity * sizeof(rcl_node_params_t), allocator.state);
  if (NULL == params) {
    RCUTILS_SET_ERROR_MSG("Failed to reallocate memory for parameter values");
    return RCUTILS_RET_BAD_ALLOC;
  }
  params_st->params = params;
  // zero initialization for the added memory
  if (new_capacity > params_st->capacity_nodes) {
    memset(
      &params_st->params[params_st->capacity_nodes], 0,
      (new_capacity - params_st->capacity_nodes) * sizeof(rcl_node_params_t));
  }

  params_st->capacity_nodes = new_capacity;
  return RCUTILS_RET_OK;
}

///
/// Copy the rcl_params_t parameter structure
///
rcl_params_t * rcl_yaml_node_struct_copy(
  const rcl_params_t * params_st)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(params_st, NULL);

  rcutils_allocator_t allocator = params_st->allocator;
  rcl_params_t * out_params_st = rcl_yaml_node_struct_init_with_capacity(
    params_st->capacity_nodes,
    allocator);

  if (NULL == out_params_st) {
    RCUTILS_SAFE_FWRITE_TO_STDERR("Error allocating mem\n");
    return NULL;
  }

  rcutils_ret_t ret;
  for (size_t node_idx = 0U; node_idx < params_st->num_nodes; ++node_idx) {
    out_params_st->node_names[node_idx] =
      rcutils_strdup(params_st->node_names[node_idx], allocator);
    if (NULL == out_params_st->node_names[node_idx]) {
      RCUTILS_SAFE_FWRITE_TO_STDERR("Error allocating mem\n");
      goto fail;
    }
    out_params_st->num_nodes++;

    rcl_node_params_t * node_params_st = &(params_st->params[node_idx]);
    rcl_node_params_t * out_node_params_st = &(out_params_st->params[node_idx]);
    ret = node_params_init_with_capacity(
      out_node_params_st,
      node_params_st->capacity_params,
      allocator);
    if (RCUTILS_RET_OK != ret) {
      if (RCUTILS_RET_BAD_ALLOC == ret) {
        RCUTILS_SAFE_FWRITE_TO_STDERR("Error allocating mem\n");
      }
      goto fail;
    }
    for (size_t parameter_idx = 0U; parameter_idx < node_params_st->num_params; ++parameter_idx) {
      out_node_params_st->parameter_names[parameter_idx] =
        rcutils_strdup(node_params_st->parameter_names[parameter_idx], allocator);
      if (NULL == out_node_params_st->parameter_names[parameter_idx]) {
        RCUTILS_SAFE_FWRITE_TO_STDERR("Error allocating mem\n");
        goto fail;
      }
      out_node_params_st->num_params++;

      const rcl_variant_t * param_var = &(node_params_st->parameter_values[parameter_idx]);
      rcl_variant_t * out_param_var = &(out_node_params_st->parameter_values[parameter_idx]);
      if (!rcl_yaml_variant_copy(out_param_var, param_var, allocator)) {
        goto fail;
      }
    }
  }
  return out_params_st;

fail:
  rcl_yaml_node_struct_fini(out_params_st);
  return NULL;
}

///
/// Free param structure
/// NOTE: If there is an error, would recommend just to safely exit the process instead
/// of calling this free function and continuing
///
void rcl_yaml_node_struct_fini(
  rcl_params_t * params_st)
{
  if (NULL == params_st) {
    return;
  }
  rcutils_allocator_t allocator = params_st->allocator;

  if (NULL != params_st->node_names) {
    for (size_t node_idx = 0U; node_idx < params_st->num_nodes; node_idx++) {
      char * node_name = params_st->node_names[node_idx];
      if (NULL != node_name) {
        allocator.deallocate(node_name, allocator.state);
      }
    }

    allocator.deallocate(params_st->node_names, allocator.state);
    params_st->node_names = NULL;
  }

  if (NULL != params_st->params) {
    for (size_t node_idx = 0U; node_idx < params_st->num_nodes; node_idx++) {
      rcl_yaml_node_params_fini(&(params_st->params[node_idx]), allocator);
    }  // for (node_idx)

    allocator.deallocate(params_st->params, allocator.state);
    params_st->params = NULL;
  }  // if (params)

  params_st->num_nodes = 0U;
  params_st->capacity_nodes = 0U;
  allocator.deallocate(params_st, allocator.state);
}

///
/// TODO (anup.pemmaiah): Support Mutiple yaml files
///
///
/// Parse the YAML file and populate params_st
///
bool rcl_parse_yaml_file(
  const char * file_path,
  rcl_params_t * params_st)
{
  RCUTILS_CHECK_FOR_NULL_WITH_MSG(
    file_path, "YAML file path is NULL", return false);

  if (NULL == params_st) {
    RCUTILS_SAFE_FWRITE_TO_STDERR("Pass an initialized parameter structure");
    return false;
  }

  yaml_parser_t parser;
  int success = yaml_parser_initialize(&parser);
  if (0 == success) {
    RCUTILS_SET_ERROR_MSG("Could not initialize the parser");
    return false;
  }

  FILE * yaml_file = fopen(file_path, "r");
  if (NULL == yaml_file) {
    yaml_parser_delete(&parser);
    RCUTILS_SET_ERROR_MSG("Error opening YAML file");
    return false;
  }

  yaml_parser_set_input_file(&parser, yaml_file);

  namespace_tracker_t ns_tracker;
  memset(&ns_tracker, 0, sizeof(namespace_tracker_t));
  rcutils_ret_t ret = parse_file_events(&parser, &ns_tracker, params_st);

  fclose(yaml_file);

  yaml_parser_delete(&parser);

  rcutils_allocator_t allocator = params_st->allocator;
  if (NULL != ns_tracker.node_ns) {
    allocator.deallocate(ns_tracker.node_ns, allocator.state);
  }
  if (NULL != ns_tracker.parameter_ns) {
    allocator.deallocate(ns_tracker.parameter_ns, allocator.state);
  }

  return RCUTILS_RET_OK == ret;
}

///
/// Parse a YAML string and populate params_st
///
bool rcl_parse_yaml_value(
  const char * node_name,
  const char * param_name,
  const char * yaml_value,
  rcl_params_t * params_st)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(node_name, false);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(param_name, false);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(yaml_value, false);

  if (0U == strlen(node_name) || 0U == strlen(param_name) || 0U == strlen(yaml_value)) {
    return false;
  }

  if (NULL == params_st) {
    RCUTILS_SAFE_FWRITE_TO_STDERR("Pass an initialized parameter structure");
    return false;
  }

  size_t node_idx = 0U;
  rcutils_ret_t ret = find_node(node_name, params_st, &node_idx);
  if (RCUTILS_RET_OK != ret) {
    return false;
  }

  size_t parameter_idx = 0U;
  ret = find_parameter(node_idx, param_name, params_st, &parameter_idx);
  if (RCUTILS_RET_OK != ret) {
    return false;
  }

  yaml_parser_t parser;
  int success = yaml_parser_initialize(&parser);
  if (0 == success) {
    RCUTILS_SET_ERROR_MSG("Could not initialize the parser");
    return false;
  }

  yaml_parser_set_input_string(
    &parser, (const unsigned char *)yaml_value, strlen(yaml_value));

  ret = parse_value_events(&parser, node_idx, parameter_idx, params_st);

  yaml_parser_delete(&parser);

  return RCUTILS_RET_OK == ret;
}

rcl_variant_t * rcl_yaml_node_struct_get(
  const char * node_name,
  const char * param_name,
  rcl_params_t * params_st)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(node_name, NULL);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(param_name, NULL);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(params_st, NULL);

  rcl_variant_t * param_value = NULL;

  size_t node_idx = 0U;
  rcutils_ret_t ret = find_node(node_name, params_st, &node_idx);
  if (RCUTILS_RET_OK == ret) {
    size_t parameter_idx = 0U;
    ret = find_parameter(node_idx, param_name, params_st, &parameter_idx);
    if (RCUTILS_RET_OK == ret) {
      param_value = &(params_st->params[node_idx].parameter_values[parameter_idx]);
    }
  }
  return param_value;
}

///
/// Dump the param structure
///
void rcl_yaml_node_struct_print(
  const rcl_params_t * const params_st)
{
  if (NULL == params_st) {
    return;
  }

  printf("\n Node Name\t\t\t\tParameters\n");
  for (size_t node_idx = 0U; node_idx < params_st->num_nodes; node_idx++) {
    int32_t param_col = 50;
    const char * const node_name = params_st->node_names[node_idx];
    if (NULL != node_name) {
      printf("%s\n", node_name);
    }

    if (NULL != params_st->params) {
      rcl_node_params_t * node_params_st = &(params_st->params[node_idx]);
      for (size_t parameter_idx = 0U; parameter_idx < node_params_st->num_params; parameter_idx++) {
        if (
          (NULL != node_params_st->parameter_names) &&
          (NULL != node_params_st->parameter_values))
        {
          char * param_name = node_params_st->parameter_names[parameter_idx];
          rcl_variant_t * param_var = &(node_params_st->parameter_values[parameter_idx]);
          if (NULL != param_name) {
            printf("%*s", param_col, param_name);
          }

          if (NULL != param_var) {
            if (NULL != param_var->bool_value) {
              printf(": %s\n", *(param_var->bool_value) ? "true" : "false");
            } else if (NULL != param_var->integer_value) {
              printf(": %" PRId64 "\n", *(param_var->integer_value));
            } else if (NULL != param_var->double_value) {
              printf(": %lf\n", *(param_var->double_value));
            } else if (NULL != param_var->string_value) {
              printf(": %s\n", param_var->string_value);
            } else if (NULL != param_var->bool_array_value) {
              printf(": ");
              for (size_t i = 0; i < param_var->bool_array_value->size; i++) {
                if (param_var->bool_array_value->values) {
                  printf(
                    "%s, ",
                    (param_var->bool_array_value->values[i]) ? "true" : "false");
                }
              }
              printf("\n");
            } else if (NULL != param_var->integer_array_value) {
              printf(": ");
              for (size_t i = 0; i < param_var->integer_array_value->size; i++) {
                if (param_var->integer_array_value->values) {
                  printf("%" PRId64 ", ", param_var->integer_array_value->values[i]);
                }
              }
              printf("\n");
            } else if (NULL != param_var->double_array_value) {
              printf(": ");
              for (size_t i = 0; i < param_var->double_array_value->size; i++) {
                if (param_var->double_array_value->values) {
                  printf("%lf, ", param_var->double_array_value->values[i]);
                }
              }
              printf("\n");
            } else if (NULL != param_var->string_array_value) {
              printf(": ");
              for (size_t i = 0; i < param_var->string_array_value->size; i++) {
                if (param_var->string_array_value->data[i]) {
                  printf("%s, ", param_var->string_array_value->data[i]);
                }
              }
              printf("\n");
            }
          }
        }
      }
    }
  }
}
