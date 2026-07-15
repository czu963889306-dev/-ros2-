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

/// NOTE: Will allow a max YAML mapping depth of 5
/// map level 1 : Node name mapping
/// map level 2 : Params mapping

#ifndef IMPL__TYPES_H_
#define IMPL__TYPES_H_

#include <inttypes.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define PARAMS_KEY "ros__parameters"
#define NODE_NS_SEPERATOR "/"
#define PARAMETER_NS_SEPERATOR "."

typedef enum yaml_map_lvl_e
{
  MAP_UNINIT_LVL = 0U,
  MAP_NODE_NAME_LVL = 1U,
  MAP_PARAMS_LVL = 2U,
} yaml_map_lvl_t;

/// Basic supported data types in the yaml file
typedef enum data_types_e
{
  DATA_TYPE_UNKNOWN = 0U,
  DATA_TYPE_BOOL = 1U,
  DATA_TYPE_INT64 = 2U,
  DATA_TYPE_DOUBLE = 3U,
  DATA_TYPE_STRING = 4U
} data_types_t;

typedef enum namespace_type_e
{
  NS_TYPE_NODE = 1U,
  NS_TYPE_PARAM = 2U
} namespace_type_t;

/// Keep track of node and parameter name spaces
typedef struct namespace_tracker_s
{
  char * node_ns;
  uint32_t num_node_ns;
  char * parameter_ns;
  uint32_t num_parameter_ns;
} namespace_tracker_t;

#ifdef __cplusplus
}
#endif

#endif  // IMPL__TYPES_H_
