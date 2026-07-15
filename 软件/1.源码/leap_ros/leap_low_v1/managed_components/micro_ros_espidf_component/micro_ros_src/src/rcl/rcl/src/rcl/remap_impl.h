// Copyright 2018 Open Source Robotics Foundation, Inc.
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

#ifndef RCL__REMAP_IMPL_H_
#define RCL__REMAP_IMPL_H_

#include "rcl/allocator.h"
#include "rcl/macros.h"
#include "rcl/remap.h"
#include "rcl/types.h"
#include "rcl/visibility_control.h"

#ifdef __cplusplus
extern "C"
{
#endif

/// Enum doubles as a bitmask for rule sthat apply to both topics and services.
typedef enum rcl_remap_type_t
{
  RCL_UNKNOWN_REMAP = 0,
  RCL_TOPIC_REMAP = 1u << 0,
  RCL_SERVICE_REMAP = 1u << 1,
  RCL_NODENAME_REMAP = 1u << 2,
  RCL_NAMESPACE_REMAP = 1u << 3
} rcl_remap_type_t;

struct rcl_remap_impl_s
{
  /// Bitmask indicating what type of rule this is.
  rcl_remap_type_t type;
  /// A node name that this rule is limited to, or NULL if it applies to any node.
  char * node_name;
  /// Match portion of a rule, or NULL if node name or namespace replacement.
  char * match;
  /// Replacement portion of a rule.
  char * replacement;

  /// Allocator used to allocate objects in this struct
  rcl_allocator_t allocator;
};

RCL_LOCAL
rcl_ret_t
rcl_remap_name(
  const rcl_arguments_t * local_arguments,
  const rcl_arguments_t * global_arguments,
  rcl_remap_type_t type_bitmask,
  const char * name,
  const char * node_name,
  const char * node_namespace,
  const rcutils_string_map_t * substitutions,
  rcl_allocator_t allocator,
  char ** output_name);

#ifdef __cplusplus
}
#endif

#endif  // RCL__REMAP_IMPL_H_
