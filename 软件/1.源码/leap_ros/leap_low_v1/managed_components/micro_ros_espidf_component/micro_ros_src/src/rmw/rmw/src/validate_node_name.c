// Copyright 2017 Open Source Robotics Foundation, Inc.
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

#include <rmw/validate_node_name.h>

#include <ctype.h>
#include <string.h>

#include "rcutils/isalnum_no_locale.h"

rmw_ret_t
rmw_validate_node_name(
  const char * node_name,
  int * validation_result,
  size_t * invalid_index)
{
  if (!node_name) {
    return RMW_RET_INVALID_ARGUMENT;
  }
  return rmw_validate_node_name_with_size(
    node_name, strlen(node_name), validation_result, invalid_index);
}

rmw_ret_t
rmw_validate_node_name_with_size(
  const char * node_name,
  size_t node_name_length,
  int * validation_result,
  size_t * invalid_index)
{
  if (!node_name) {
    return RMW_RET_INVALID_ARGUMENT;
  }
  if (!validation_result) {
    return RMW_RET_INVALID_ARGUMENT;
  }
  if (node_name_length == 0) {
    *validation_result = RMW_NODE_NAME_INVALID_IS_EMPTY_STRING;
    if (invalid_index) {
      *invalid_index = 0;
    }
    return RMW_RET_OK;
  }
  // check for unallowed characters
  for (size_t i = 0; i < node_name_length; ++i) {
    if (rcutils_isalnum_no_locale(node_name[i])) {
      // if it is an alpha numeric character, i.e. [0-9|A-Z|a-z], continue
      continue;
    } else if (node_name[i] == '_') {
      // if it is an underscore, continue
      continue;
    } else {
      // if it is none of these, then it is an unallowed character in a node name
      *validation_result = RMW_NODE_NAME_INVALID_CONTAINS_UNALLOWED_CHARACTERS;
      if (invalid_index) {
        *invalid_index = i;
      }
      return RMW_RET_OK;
    }
  }
  if (isdigit(node_name[0]) != 0) {
    // this is the case where the name starts with a number, i.e. [0-9]
    *validation_result = RMW_NODE_NAME_INVALID_STARTS_WITH_NUMBER;
    if (invalid_index) {
      *invalid_index = 0;
    }
    return RMW_RET_OK;
  }
  // check if the node name is too long last, since it might be a soft invalidation
  if (node_name_length > RMW_NODE_NAME_MAX_NAME_LENGTH) {
    *validation_result = RMW_NODE_NAME_INVALID_TOO_LONG;
    if (invalid_index) {
      *invalid_index = RMW_NODE_NAME_MAX_NAME_LENGTH - 1;
    }
    return RMW_RET_OK;
  }
  // everything was ok, set result to valid node name, avoid setting invalid_index, and return
  *validation_result = RMW_NODE_NAME_VALID;
  return RMW_RET_OK;
}

const char *
rmw_node_name_validation_result_string(int validation_result)
{
  switch (validation_result) {
    case RMW_NODE_NAME_VALID:
      return NULL;
    case RMW_NODE_NAME_INVALID_IS_EMPTY_STRING:
      return "node name must not be empty";
    case RMW_NODE_NAME_INVALID_CONTAINS_UNALLOWED_CHARACTERS:
      return "node name must not contain characters other than alphanumerics or '_'";
    case RMW_NODE_NAME_INVALID_STARTS_WITH_NUMBER:
      return "node name must not start with a number";
    case RMW_NODE_NAME_INVALID_TOO_LONG:
      return
        "node name length should not exceed '" RMW_STRINGIFY(RMW_NODE_NAME_MAX_NAME_LENGTH) "'";
    default:
      return "unknown result code for rmw node name validation";
  }
}
