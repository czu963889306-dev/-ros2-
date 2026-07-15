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

#ifdef __cplusplus
extern "C"
{
#endif

#include "rcl/expand_topic_name.h"

#include <stdio.h>
#include <string.h>

#include "./common.h"
#include "rcl/error_handling.h"
#include "rcl/types.h"
#include "rcl/validate_topic_name.h"
#include "rcutils/error_handling.h"
#include "rcutils/format_string.h"
#include "rcutils/repl_str.h"
#include "rcutils/strdup.h"
#include "rmw/error_handling.h"
#include "rmw/types.h"
#include "rmw/validate_namespace.h"
#include "rmw/validate_node_name.h"

// built-in substitution strings
#define SUBSTITUION_NODE_NAME "{node}"
#define SUBSTITUION_NAMESPACE "{ns}"
#define SUBSTITUION_NAMESPACE2 "{namespace}"

rcl_ret_t
rcl_expand_topic_name(
  const char * input_topic_name,
  const char * node_name,
  const char * node_namespace,
  const rcutils_string_map_t * substitutions,
  rcl_allocator_t allocator,
  char ** output_topic_name)
{
  RCUTILS_CAN_SET_MSG_AND_RETURN_WITH_ERROR_OF(RCL_RET_INVALID_ARGUMENT);
  RCUTILS_CAN_SET_MSG_AND_RETURN_WITH_ERROR_OF(RCL_RET_TOPIC_NAME_INVALID);
  RCUTILS_CAN_SET_MSG_AND_RETURN_WITH_ERROR_OF(RCL_RET_NODE_INVALID_NAME);
  RCUTILS_CAN_SET_MSG_AND_RETURN_WITH_ERROR_OF(RCL_RET_NODE_INVALID_NAMESPACE);
  RCUTILS_CAN_SET_MSG_AND_RETURN_WITH_ERROR_OF(RCL_RET_UNKNOWN_SUBSTITUTION);
  RCUTILS_CAN_SET_MSG_AND_RETURN_WITH_ERROR_OF(RCL_RET_BAD_ALLOC);

  // check arguments that could be null
  RCL_CHECK_ARGUMENT_FOR_NULL(input_topic_name, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(node_name, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(node_namespace, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(substitutions, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(output_topic_name, RCL_RET_INVALID_ARGUMENT);
  // validate the input topic
  int validation_result;
  rcl_ret_t ret = rcl_validate_topic_name(input_topic_name, &validation_result, NULL);
  if (ret != RCL_RET_OK) {
    // error message already set
    return ret;
  }
  if (validation_result != RCL_TOPIC_NAME_VALID) {
    RCL_SET_ERROR_MSG("topic name is invalid");
    return RCL_RET_TOPIC_NAME_INVALID;
  }
  // validate the node name
  rmw_ret_t rmw_ret;
  rmw_ret = rmw_validate_node_name(node_name, &validation_result, NULL);
  if (rmw_ret != RMW_RET_OK) {
    RCL_SET_ERROR_MSG(rmw_get_error_string().str);
    return rcl_convert_rmw_ret_to_rcl_ret(rmw_ret);
  }
  if (validation_result != RMW_NODE_NAME_VALID) {
    RCL_SET_ERROR_MSG("node name is invalid");
    return RCL_RET_NODE_INVALID_NAME;
  }
  // validate the namespace
  rmw_ret = rmw_validate_namespace(node_namespace, &validation_result, NULL);
  if (rmw_ret != RMW_RET_OK) {
    RCL_SET_ERROR_MSG(rmw_get_error_string().str);
    return rcl_convert_rmw_ret_to_rcl_ret(rmw_ret);
  }
  if (validation_result != RMW_NODE_NAME_VALID) {
    RCL_SET_ERROR_MSG("node namespace is invalid");
    return RCL_RET_NODE_INVALID_NAMESPACE;
  }
  // check if the topic has substitutions to be made
  bool has_a_substitution = strchr(input_topic_name, '{') != NULL;
  bool has_a_namespace_tilde = input_topic_name[0] == '~';
  bool is_absolute = input_topic_name[0] == '/';
  // if absolute and doesn't have any substitution
  if (is_absolute && !has_a_substitution) {
    // nothing to do, duplicate and return
    *output_topic_name = rcutils_strdup(input_topic_name, allocator);
    if (!*output_topic_name) {
      *output_topic_name = NULL;
      RCL_SET_ERROR_MSG("failed to allocate memory for output topic");
      return RCL_RET_BAD_ALLOC;
    }
    return RCL_RET_OK;
  }
  char * local_output = NULL;
  // if has_a_namespace_tilde, replace that first
  if (has_a_namespace_tilde) {
    // special case where node_namespace is just '/'
    // then no additional separating '/' is needed
    const char * fmt = (strlen(node_namespace) == 1) ? "%s%s%s" : "%s/%s%s";
    local_output =
      rcutils_format_string(allocator, fmt, node_namespace, node_name, input_topic_name + 1);
    if (!local_output) {
      *output_topic_name = NULL;
      RCL_SET_ERROR_MSG("failed to allocate memory for output topic");
      return RCL_RET_BAD_ALLOC;
    }
  }
  // if it has any substitutions, replace those
  if (has_a_substitution) {
    // Assumptions entering this scope about the topic string:
    //
    // - All {} are matched and balanced
    // - There is no nesting, i.e. {{}}
    // - There are no empty substitution substr, i.e. '{}' versus '{something}'
    //
    // These assumptions are taken because this is checked in the validation function.
    const char * current_output = (local_output) ? local_output : input_topic_name;
    char * next_opening_brace = NULL;
    // current_output may be replaced on each loop if a substitution is made
    while ((next_opening_brace = strchr(current_output, '{')) != NULL) {
      char * next_closing_brace = strchr(current_output, '}');
      // conclusion based on above assumptions: next_closing_brace - next_opening_brace > 1
      size_t substitution_substr_len = next_closing_brace - next_opening_brace + 1;
      // figure out what the replacement is for this substitution
      const char * replacement = NULL;
      if (strncmp(SUBSTITUION_NODE_NAME, next_opening_brace, substitution_substr_len) == 0) {
        replacement = node_name;
      } else if (  // NOLINT
        strncmp(SUBSTITUION_NAMESPACE, next_opening_brace, substitution_substr_len) == 0 ||
        strncmp(SUBSTITUION_NAMESPACE2, next_opening_brace, substitution_substr_len) == 0)
      {
        replacement = node_namespace;
      } else {
        replacement = rcutils_string_map_getn(
          substitutions,
          // compare {substitution}
          //          ^ until    ^
          next_opening_brace + 1, substitution_substr_len - 2);
        if (!replacement) {
          // in this case, it is neither node name nor ns nor in the substitutions map, so error
          *output_topic_name = NULL;
          char * unmatched_substitution =
            rcutils_strndup(next_opening_brace, substitution_substr_len, allocator);
          if (unmatched_substitution) {
            RCL_SET_ERROR_MSG_WITH_FORMAT_STRING(
              "unknown substitution: %s", unmatched_substitution);
          } else {
            RCUTILS_SAFE_FWRITE_TO_STDERR("failed to allocate memory for unmatched substitution\n");
          }
          allocator.deallocate(unmatched_substitution, allocator.state);
          allocator.deallocate(local_output, allocator.state);
          return RCL_RET_UNKNOWN_SUBSTITUTION;
        }
      }
      // at this point replacement will be set or an error would have returned out
      // do the replacement
      char * next_substitution =
        rcutils_strndup(next_opening_brace, substitution_substr_len, allocator);
      if (!next_substitution) {
        *output_topic_name = NULL;
        RCL_SET_ERROR_MSG("failed to allocate memory for substitution");
        allocator.deallocate(local_output, allocator.state);
        return RCL_RET_BAD_ALLOC;
      }
      char * original_local_output = local_output;
      local_output = rcutils_repl_str(current_output, next_substitution, replacement, &allocator);
      allocator.deallocate(next_substitution, allocator.state);  // free no matter what
      allocator.deallocate(original_local_output, allocator.state);  // free no matter what
      if (!local_output) {
        *output_topic_name = NULL;
        RCL_SET_ERROR_MSG("failed to allocate memory for expanded topic");
        return RCL_RET_BAD_ALLOC;
      }
      current_output = local_output;
      // loop until all substitutions are replaced
    }  // while
  }
  // finally make the name absolute if it isn't already
  if (
    (local_output && local_output[0] != '/') ||
    (!local_output && input_topic_name[0] != '/'))
  {
    char * original_local_output = local_output;
    // special case where node_namespace is just '/'
    // then no additional separating '/' is needed
    const char * fmt = (strlen(node_namespace) == 1) ? "%s%s" : "%s/%s";
    local_output = rcutils_format_string(
      allocator, fmt, node_namespace, (local_output) ? local_output : input_topic_name);
    if (original_local_output) {
      allocator.deallocate(original_local_output, allocator.state);
    }
    if (!local_output) {
      *output_topic_name = NULL;
      RCL_SET_ERROR_MSG("failed to allocate memory for output topic");
      return RCL_RET_BAD_ALLOC;
    }
  }
  // finally store the result in the out pointer and return
  *output_topic_name = local_output;
  return RCL_RET_OK;
}

rcl_ret_t
rcl_get_default_topic_name_substitutions(rcutils_string_map_t * string_map)
{
  RCL_CHECK_ARGUMENT_FOR_NULL(string_map, RCL_RET_INVALID_ARGUMENT);

  // right now there are no default substitutions

  return RCL_RET_OK;
}

#ifdef __cplusplus
}
#endif
