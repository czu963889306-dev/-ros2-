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

#include "rcutils/allocator.h"
#include "rcutils/strdup.h"
#include "rcutils/types.h"

#include "./impl/types.h"
#include "./impl/yaml_variant.h"
#include "rcl_yaml_param_parser/types.h"

#define RCL_YAML_VARIANT_COPY_VALUE(dest_ptr, src_ptr, allocator, var_type) \
  do { \
    dest_ptr = allocator.allocate(sizeof(var_type), allocator.state); \
    if (NULL == dest_ptr) { \
      RCUTILS_SAFE_FWRITE_TO_STDERR( \
        "Error allocating variant mem when copying value of type " #var_type "\n"); \
      return false; \
    } \
    *(dest_ptr) = *(src_ptr); \
  } while (0)

#define RCL_YAML_VARIANT_COPY_ARRAY_VALUE( \
    dest_array, src_array, allocator, var_array_type, var_type) \
  do { \
    dest_array = \
      allocator.allocate(sizeof(var_array_type), allocator.state); \
    if (NULL == dest_array) { \
      RCUTILS_SAFE_FWRITE_TO_STDERR( \
        "Error allocating mem for array of type " #var_array_type "\n"); \
      return false; \
    } \
    if (0U != src_array->size) { \
      dest_array->values = allocator.allocate( \
        sizeof(var_type) * src_array->size, allocator.state); \
      if (NULL == dest_array->values) { \
        RCUTILS_SAFE_FWRITE_TO_STDERR( \
          "Error allocating mem for array values of type " #var_type "\n"); \
        return false; \
      } \
      memcpy( \
        dest_array->values, \
        src_array->values, \
        sizeof(var_type) * src_array->size); \
    } else { \
      dest_array->values = NULL; \
    } \
    dest_array->size = src_array->size; \
  } while (0)

void rcl_yaml_variant_fini(
  rcl_variant_t * param_var,
  const rcutils_allocator_t allocator)
{
  if (NULL == param_var) {
    return;
  }

  if (NULL != param_var->bool_value) {
    allocator.deallocate(param_var->bool_value, allocator.state);
    param_var->bool_value = NULL;
  } else if (NULL != param_var->integer_value) {
    allocator.deallocate(param_var->integer_value, allocator.state);
    param_var->integer_value = NULL;
  } else if (NULL != param_var->double_value) {
    allocator.deallocate(param_var->double_value, allocator.state);
    param_var->double_value = NULL;
  } else if (NULL != param_var->string_value) {
    allocator.deallocate(param_var->string_value, allocator.state);
    param_var->string_value = NULL;
  } else if (NULL != param_var->bool_array_value) {
    if (NULL != param_var->bool_array_value->values) {
      allocator.deallocate(param_var->bool_array_value->values, allocator.state);
    }
    allocator.deallocate(param_var->bool_array_value, allocator.state);
    param_var->bool_array_value = NULL;
  } else if (NULL != param_var->integer_array_value) {
    if (NULL != param_var->integer_array_value->values) {
      allocator.deallocate(param_var->integer_array_value->values, allocator.state);
    }
    allocator.deallocate(param_var->integer_array_value, allocator.state);
    param_var->integer_array_value = NULL;
  } else if (NULL != param_var->double_array_value) {
    if (NULL != param_var->double_array_value->values) {
      allocator.deallocate(param_var->double_array_value->values, allocator.state);
    }
    allocator.deallocate(param_var->double_array_value, allocator.state);
    param_var->double_array_value = NULL;
  } else if (NULL != param_var->string_array_value) {
    if (RCUTILS_RET_OK != rcutils_string_array_fini(param_var->string_array_value)) {
      // Log and continue ...
      RCUTILS_SAFE_FWRITE_TO_STDERR("Error deallocating string array");
    }
    allocator.deallocate(param_var->string_array_value, allocator.state);
    param_var->string_array_value = NULL;
  } else {
    /// Nothing to do to keep pclint happy
  }
}

bool rcl_yaml_variant_copy(
  rcl_variant_t * out_param_var, const rcl_variant_t * param_var, rcutils_allocator_t allocator)
{
  if (NULL == param_var || NULL == out_param_var) {
    return false;
  }
  if (NULL != param_var->bool_value) {
    RCL_YAML_VARIANT_COPY_VALUE(
      out_param_var->bool_value, param_var->bool_value, allocator, bool);
  } else if (NULL != param_var->integer_value) {
    RCL_YAML_VARIANT_COPY_VALUE(
      out_param_var->integer_value, param_var->integer_value, allocator, int64_t);
  } else if (NULL != param_var->double_value) {
    RCL_YAML_VARIANT_COPY_VALUE(
      out_param_var->double_value, param_var->double_value, allocator, double);
  } else if (NULL != param_var->string_value) {
    out_param_var->string_value =
      rcutils_strdup(param_var->string_value, allocator);
    if (NULL == out_param_var->string_value) {
      RCUTILS_SAFE_FWRITE_TO_STDERR("Error allocating variant mem when copying string_value\n");
      return false;
    }
  } else if (NULL != param_var->bool_array_value) {
    RCL_YAML_VARIANT_COPY_ARRAY_VALUE(
      out_param_var->bool_array_value, param_var->bool_array_value, allocator,
      rcl_bool_array_t, bool);
  } else if (NULL != param_var->integer_array_value) {
    RCL_YAML_VARIANT_COPY_ARRAY_VALUE(
      out_param_var->integer_array_value, param_var->integer_array_value, allocator,
      rcl_int64_array_t, int64_t);
  } else if (NULL != param_var->double_array_value) {
    RCL_YAML_VARIANT_COPY_ARRAY_VALUE(
      out_param_var->double_array_value, param_var->double_array_value, allocator,
      rcl_double_array_t, double);
  } else if (NULL != param_var->string_array_value) {
    out_param_var->string_array_value =
      allocator.allocate(sizeof(rcutils_string_array_t), allocator.state);
    if (NULL == out_param_var->string_array_value) {
      RCUTILS_SAFE_FWRITE_TO_STDERR("Error allocating mem\n");
      return false;
    }
    *(out_param_var->string_array_value) = rcutils_get_zero_initialized_string_array();
    rcutils_ret_t ret = rcutils_string_array_init(
      out_param_var->string_array_value,
      param_var->string_array_value->size,
      &(param_var->string_array_value->allocator));
    if (RCUTILS_RET_OK != ret) {
      if (RCUTILS_RET_BAD_ALLOC == ret) {
        RCUTILS_SAFE_FWRITE_TO_STDERR("Error allocating mem for string array\n");
      }
      return false;
    }
    for (size_t str_idx = 0U; str_idx < param_var->string_array_value->size; ++str_idx) {
      out_param_var->string_array_value->data[str_idx] = rcutils_strdup(
        param_var->string_array_value->data[str_idx],
        out_param_var->string_array_value->allocator);
      if (NULL == out_param_var->string_array_value->data[str_idx]) {
        RCUTILS_SAFE_FWRITE_TO_STDERR("Error allocating mem for string array values\n");
        return false;
      }
    }
  } else {
    /// Nothing to do to keep pclint happy
  }
  return true;
}
