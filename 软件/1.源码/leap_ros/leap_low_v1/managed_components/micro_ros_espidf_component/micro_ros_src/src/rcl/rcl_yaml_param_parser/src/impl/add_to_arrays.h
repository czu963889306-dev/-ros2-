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

#ifndef IMPL__ADD_TO_ARRAYS_H_
#define IMPL__ADD_TO_ARRAYS_H_

#include <yaml.h>

#include "rcutils/types.h"

#include "./types.h"
#include "rcl_yaml_param_parser/types.h"
#include "rcl_yaml_param_parser/visibility_control.h"

#ifdef __cplusplus
extern "C"
{
#endif

///
/// Add a value to a bool array. Create the array if it does not exist
///
RCL_YAML_PARAM_PARSER_PUBLIC
RCUTILS_WARN_UNUSED
rcutils_ret_t add_val_to_bool_arr(
  rcl_bool_array_t * const val_array,
  bool * value,
  const rcutils_allocator_t allocator);

///
/// Add a value to an integer array. Create the array if it does not exist
///
RCL_YAML_PARAM_PARSER_PUBLIC
RCUTILS_WARN_UNUSED
rcutils_ret_t add_val_to_int_arr(
  rcl_int64_array_t * const val_array,
  int64_t * value,
  const rcutils_allocator_t allocator);

///
/// Add a value to a double array. Create the array if it does not exist
///
RCL_YAML_PARAM_PARSER_PUBLIC
RCUTILS_WARN_UNUSED
rcutils_ret_t add_val_to_double_arr(
  rcl_double_array_t * const val_array,
  double * value,
  const rcutils_allocator_t allocator);

///
/// Add a value to a string array. Create the array if it does not exist
///
RCL_YAML_PARAM_PARSER_PUBLIC
RCUTILS_WARN_UNUSED
rcutils_ret_t add_val_to_string_arr(
  rcutils_string_array_t * const val_array,
  char * value,
  const rcutils_allocator_t allocator);

///
/// TODO (anup.pemmaiah): Support byte array
///

#ifdef __cplusplus
}
#endif

#endif  // IMPL__ADD_TO_ARRAYS_H_
