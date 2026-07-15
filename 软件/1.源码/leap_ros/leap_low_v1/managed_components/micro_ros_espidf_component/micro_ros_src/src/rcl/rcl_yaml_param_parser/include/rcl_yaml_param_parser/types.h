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
#ifndef RCL_YAML_PARAM_PARSER__TYPES_H_
#define RCL_YAML_PARAM_PARSER__TYPES_H_

#include "rcutils/allocator.h"
#include "rcutils/types/string_array.h"

/// Array of bool values
/*
 * \typedef rcl_bool_array_t
 */
typedef struct rcl_bool_array_s
{
  /// Array with bool values
  bool * values;
  /// Number of values in the array
  size_t size;
} rcl_bool_array_t;

/// Array of int64_t values
/*
 * \typedef rcl_int64_array_t
 */
typedef struct rcl_int64_array_s
{
  /// Array with int64 values
  int64_t * values;
  /// Number of values in the array
  size_t size;
} rcl_int64_array_t;

/// Array of double values
/*
 * \typedef rcl_double_array_t
 */
typedef struct rcl_double_array_s
{
  /// Array with double values
  double * values;
  /// Number of values in the array
  size_t size;
} rcl_double_array_t;

/// Array of byte values
/*
 * \typedef rcl_byte_array_t
 */
typedef struct rcl_byte_array_s
{
  /// Array with uint8_t values
  uint8_t * values;
  /// Number of values in the array
  size_t size;
} rcl_byte_array_t;

/// variant_t stores the value of a parameter
/*
 * Only one pointer in this struct will store the value
 * \typedef rcl_variant_t
 */
typedef struct rcl_variant_s
{
  bool * bool_value;  ///< If bool, gets stored here
  int64_t * integer_value;  ///< If integer, gets stored here
  double * double_value;  ///< If double, gets stored here
  char * string_value;  ///< If string, gets stored here
  rcl_byte_array_t * byte_array_value;  ///< If array of bytes
  rcl_bool_array_t * bool_array_value;  ///< If array of bool's
  rcl_int64_array_t * integer_array_value;  ///< If array of integers
  rcl_double_array_t * double_array_value;  ///< If array of doubles
  rcutils_string_array_t * string_array_value;  ///< If array of strings
} rcl_variant_t;

/// node_params_t stores all the parameters(key:value) of a single node
/*
* \typedef rcl_node_params_t
*/
typedef struct rcl_node_params_s
{
  char ** parameter_names;  ///< Array of parameter names (keys)
  rcl_variant_t * parameter_values;  ///< Array of coressponding parameter values
  size_t num_params;  ///< Number of parameters in the node
  size_t capacity_params;  ///< Capacity of parameters in the node
} rcl_node_params_t;

/// stores all the parameters of all nodes of a process
/*
* \typedef rcl_params_t
*/
typedef struct rcl_params_s
{
  char ** node_names;  ///< List of names of the node
  rcl_node_params_t * params;  ///<  Array of parameters
  size_t num_nodes;       ///< Number of nodes
  size_t capacity_nodes;  ///< Capacity of nodes
  rcutils_allocator_t allocator;  ///< Allocator used
} rcl_params_t;

#endif  // RCL_YAML_PARAM_PARSER__TYPES_H_
