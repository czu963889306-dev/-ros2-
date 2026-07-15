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

/** rcl_yaml_param_parser: Parse a YAML parameter file and populate the C data structure
 *
 *  - Parser
 *  - rcl/parser.h
 *
 * Some useful abstractions and utilities:
 * - Return code types
 *   - rcl/types.h
 * - Macros for controlling symbol visibility on the library
 *   - rcl/visibility_control.h
 */

#ifndef RCL_YAML_PARAM_PARSER__PARSER_H_
#define RCL_YAML_PARAM_PARSER__PARSER_H_

#include <stdlib.h>

#include "rcl_yaml_param_parser/types.h"
#include "rcl_yaml_param_parser/visibility_control.h"

#ifdef __cplusplus
extern "C"
{
#endif

/// \brief Initialize parameter structure
/// \param[in] allocator memory allocator to be used
/// \return a pointer to param structure on success or NULL on failure
RCL_YAML_PARAM_PARSER_PUBLIC
rcl_params_t * rcl_yaml_node_struct_init(
  const rcutils_allocator_t allocator);

/// \brief Initialize parameter structure with a capacity
/// \param[in] capacity a capacity to param structure
/// \param[in] allocator memory allocator to be used
/// \return a pointer to param structure on success or NULL on failure
RCL_YAML_PARAM_PARSER_PUBLIC
rcl_params_t * rcl_yaml_node_struct_init_with_capacity(
  size_t capacity,
  const rcutils_allocator_t allocator);

/// \brief Reallocate parameter structure with a new capacity
/// \post the address of \p node_names in \p params_st might be changed
///   even if the result value is `RCL_RET_BAD_ALLOC`.
/// \param[in] params_st a parameter structure
/// \param[in] new_capacity a new capacity to param structure that must be greater than num_params
/// \param[in] allocator memory allocator to be used
/// \return `RCL_RET_OK` if the structure was reallocated successfully, or
/// \return `RCL_RET_INVALID_ARGUMENT` if params_st is NULL, or
///  allocator is invalid, or
///  new_capacity is less than num_nodes
/// \return `RCL_RET_BAD_ALLOC` if allocating memory failed.
RCL_YAML_PARAM_PARSER_PUBLIC
rcutils_ret_t rcl_yaml_node_struct_reallocate(
  rcl_params_t * params_st,
  size_t new_capacity,
  const rcutils_allocator_t allocator);

/// \brief Copy parameter structure
/// \param[in] params_st points to the parameter struct to be copied
/// \return a pointer to the copied param structure on success or NULL on failure
RCL_YAML_PARAM_PARSER_PUBLIC
rcl_params_t * rcl_yaml_node_struct_copy(
  const rcl_params_t * params_st);

/// \brief Free parameter structure
/// \param[in] params_st points to the populated parameter struct
RCL_YAML_PARAM_PARSER_PUBLIC
void rcl_yaml_node_struct_fini(
  rcl_params_t * params_st);

/// \brief Parse the YAML file and populate \p params_st
/// \pre Given \p params_st must be a valid parameter struct
///   as returned by `rcl_yaml_node_struct_init()`
/// \param[in] file_path is the path to the YAML file
/// \param[inout] params_st points to the struct to be populated
/// \return true on success and false on failure
RCL_YAML_PARAM_PARSER_PUBLIC
bool rcl_parse_yaml_file(
  const char * file_path,
  rcl_params_t * params_st);

/// \brief Parse a parameter value as a YAML string, updating params_st accordingly
/// \param[in] node_name is the name of the node to which the parameter belongs
/// \param[in] param_name is the name of the parameter whose value will be parsed
/// \param[in] yaml_value is the parameter value as a YAML string to be parsed
/// \param[inout] params_st points to the parameter struct
/// \return true on success and false on failure
RCL_YAML_PARAM_PARSER_PUBLIC
bool rcl_parse_yaml_value(
  const char * node_name,
  const char * param_name,
  const char * yaml_value,
  rcl_params_t * params_st);

/// \brief Get the variant value for a given parameter, zero initializing it in the
/// process if not present already
/// \param[in] node_name is the name of the node to which the parameter belongs
/// \param[in] param_name is the name of the parameter whose value is to be retrieved
/// \param[inout] params_st points to the populated (or to be populated) parameter struct
/// \return parameter variant value on success and NULL on failure
RCL_YAML_PARAM_PARSER_PUBLIC
rcl_variant_t * rcl_yaml_node_struct_get(
  const char * node_name,
  const char * param_name,
  rcl_params_t * params_st);

/// \brief Print the parameter structure to stdout
/// \param[in] params_st points to the populated parameter struct
RCL_YAML_PARAM_PARSER_PUBLIC
void rcl_yaml_node_struct_print(
  const rcl_params_t * const params_st);

#ifdef __cplusplus
}
#endif

#endif  // RCL_YAML_PARAM_PARSER__PARSER_H_
