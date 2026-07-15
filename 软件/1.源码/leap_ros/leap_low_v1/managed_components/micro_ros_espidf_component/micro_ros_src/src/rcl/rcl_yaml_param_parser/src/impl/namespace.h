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

#ifndef IMPL__NAMESPACE_H_
#define IMPL__NAMESPACE_H_

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
/// Add name to namespace tracker
///
RCL_YAML_PARAM_PARSER_PUBLIC
RCUTILS_WARN_UNUSED
rcutils_ret_t add_name_to_ns(
  namespace_tracker_t * ns_tracker,
  const char * name,
  const namespace_type_t namespace_type,
  rcutils_allocator_t allocator);

///
/// Remove name from namespace tracker
///
RCL_YAML_PARAM_PARSER_PUBLIC
RCUTILS_WARN_UNUSED
rcutils_ret_t rem_name_from_ns(
  namespace_tracker_t * ns_tracker,
  const namespace_type_t namespace_type,
  rcutils_allocator_t allocator);

///
/// Replace namespace in namespace tracker
///
RCL_YAML_PARAM_PARSER_PUBLIC
RCUTILS_WARN_UNUSED
rcutils_ret_t replace_ns(
  namespace_tracker_t * ns_tracker,
  char * const new_ns,
  const uint32_t new_ns_count,
  const namespace_type_t namespace_type,
  rcutils_allocator_t allocator);

#ifdef __cplusplus
}
#endif

#endif  // IMPL__NAMESPACE_H_
