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

#ifndef IMPL__NODE_PARAMS_H_
#define IMPL__NODE_PARAMS_H_

#include "rcl_yaml_param_parser/types.h"
#include "rcl_yaml_param_parser/visibility_control.h"

#ifdef __cplusplus
extern "C"
{
#endif

///
/// Create rcl_node_params_t structure
///
RCL_YAML_PARAM_PARSER_PUBLIC
RCUTILS_WARN_UNUSED
rcutils_ret_t node_params_init(
  rcl_node_params_t * node_params,
  const rcutils_allocator_t allocator);

///
/// Create rcl_node_params_t structure with a capacity
///
RCL_YAML_PARAM_PARSER_PUBLIC
RCUTILS_WARN_UNUSED
rcutils_ret_t node_params_init_with_capacity(
  rcl_node_params_t * node_params,
  size_t capacity,
  const rcutils_allocator_t allocator);

///
/// Reallocate rcl_node_params_t structure with a new capacity
/// \post the address of \p parameter_names in \p node_params might be changed
/// even if the result value is `RCL_RET_BAD_ALLOC`.
///
RCL_YAML_PARAM_PARSER_PUBLIC
RCUTILS_WARN_UNUSED
rcutils_ret_t node_params_reallocate(
  rcl_node_params_t * node_params,
  size_t new_capacity,
  const rcutils_allocator_t allocator);

///
/// Finalize rcl_node_params_t structure
///
RCL_YAML_PARAM_PARSER_PUBLIC
void rcl_yaml_node_params_fini(
  rcl_node_params_t * node_params,
  const rcutils_allocator_t allocator);

#ifdef __cplusplus
}
#endif

#endif  // IMPL__NODE_PARAMS_H_
