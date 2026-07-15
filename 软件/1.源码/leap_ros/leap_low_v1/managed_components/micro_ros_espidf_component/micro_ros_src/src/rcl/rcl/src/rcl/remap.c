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

#include "rcl/remap.h"

#include "./arguments_impl.h"
#include "./remap_impl.h"
#include "rcl/error_handling.h"
#include "rcl/expand_topic_name.h"
#include "rcutils/allocator.h"
#include "rcutils/macros.h"
#include "rcutils/strdup.h"
#include "rcutils/types/string_map.h"

#ifdef __cplusplus
extern "C"
{
#endif

rcl_remap_t
rcl_get_zero_initialized_remap(void)
{
  static rcl_remap_t default_rule = {
    .impl = NULL
  };
  return default_rule;
}

rcl_ret_t
rcl_remap_copy(
  const rcl_remap_t * rule,
  rcl_remap_t * rule_out)
{
  RCUTILS_CAN_SET_MSG_AND_RETURN_WITH_ERROR_OF(RCL_RET_INVALID_ARGUMENT);
  RCUTILS_CAN_SET_MSG_AND_RETURN_WITH_ERROR_OF(RCL_RET_BAD_ALLOC);

  RCL_CHECK_ARGUMENT_FOR_NULL(rule, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(rule_out, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(rule->impl, RCL_RET_INVALID_ARGUMENT);

  if (NULL != rule_out->impl) {
    RCL_SET_ERROR_MSG("rule_out must be zero initialized");
    return RCL_RET_INVALID_ARGUMENT;
  }

  rcl_allocator_t allocator = rule->impl->allocator;

  rule_out->impl = allocator.allocate(sizeof(rcl_remap_impl_t), allocator.state);
  if (NULL == rule_out->impl) {
    return RCL_RET_BAD_ALLOC;
  }

  rule_out->impl->allocator = allocator;

  // Zero so it's safe to call rcl_remap_fini() if an error occurs while copying.
  rule_out->impl->type = RCL_UNKNOWN_REMAP;
  rule_out->impl->node_name = NULL;
  rule_out->impl->match = NULL;
  rule_out->impl->replacement = NULL;

  rule_out->impl->type = rule->impl->type;
  if (NULL != rule->impl->node_name) {
    rule_out->impl->node_name = rcutils_strdup(rule->impl->node_name, allocator);
    if (NULL == rule_out->impl->node_name) {
      goto fail;
    }
  }
  if (NULL != rule->impl->match) {
    rule_out->impl->match = rcutils_strdup(rule->impl->match, allocator);
    if (NULL == rule_out->impl->match) {
      goto fail;
    }
  }
  if (NULL != rule->impl->replacement) {
    rule_out->impl->replacement = rcutils_strdup(
      rule->impl->replacement, allocator);
    if (NULL == rule_out->impl->replacement) {
      goto fail;
    }
  }
  return RCL_RET_OK;
fail:
  if (RCL_RET_OK != rcl_remap_fini(rule_out)) {
    RCL_SET_ERROR_MSG("Error while finalizing remap rule due to another error");
  }
  return RCL_RET_BAD_ALLOC;
}

/// Get the first matching rule in a chain.
/// \return RCL_RET_OK if no errors occurred while searching for a rule
static
rcl_ret_t
rcl_remap_first_match(
  rcl_remap_t * remap_rules,
  int num_rules,
  rcl_remap_type_t type_bitmask,
  const char * name,
  const char * node_name,
  const char * node_namespace,
  const rcutils_string_map_t * substitutions,
  rcutils_allocator_t allocator,
  rcl_remap_t ** output_rule)
{
  *output_rule = NULL;
  for (int i = 0; i < num_rules; ++i) {
    rcl_remap_t * rule = &(remap_rules[i]);
    if (!(rule->impl->type & type_bitmask)) {
      // Not the type of remap rule we're looking fore
      continue;
    }
    if (rule->impl->node_name != NULL && 0 != strcmp(rule->impl->node_name, node_name)) {
      // Rule has a node name prefix and the supplied node name didn't match
      continue;
    }
    bool matched = false;
    if (rule->impl->type & (RCL_TOPIC_REMAP | RCL_SERVICE_REMAP)) {
      // topic and service rules need the match side to be expanded to a FQN
      char * expanded_match = NULL;
      rcl_ret_t ret = rcl_expand_topic_name(
        rule->impl->match, node_name, node_namespace,
        substitutions, allocator, &expanded_match);
      if (RCL_RET_OK != ret) {
        rcl_reset_error();
        if (
          RCL_RET_NODE_INVALID_NAMESPACE == ret ||
          RCL_RET_NODE_INVALID_NAME == ret ||
          RCL_RET_BAD_ALLOC == ret)
        {
          // these are probably going to happen again. Stop processing rules
          return ret;
        }
        continue;
      }
      if (NULL != name) {
        // this check is to satisfy clang-tidy – name is always not null when type_bitmask is
        // RCL_TOPIC_REMAP or RCL_SERVICE_REMAP. That is guaranteed because rcl_remap_first_match
        // and rcl_remap_name are not public.
        matched = (0 == strcmp(expanded_match, name));
      }
      allocator.deallocate(expanded_match, allocator.state);
    } else {
      // nodename and namespace replacement apply if the type and node name prefix checks passed
      matched = true;
    }
    if (matched) {
      *output_rule = rule;
      break;
    }
  }
  return RCL_RET_OK;
}

/// Remap from one name to another using rules matching a given type bitmask.
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
  char ** output_name)
{
  RCL_CHECK_ARGUMENT_FOR_NULL(node_name, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(output_name, RCL_RET_INVALID_ARGUMENT);
  if (NULL != local_arguments && NULL == local_arguments->impl) {
    local_arguments = NULL;
  }
  if (NULL != global_arguments && NULL == global_arguments->impl) {
    global_arguments = NULL;
  }
  if (NULL == local_arguments && NULL == global_arguments) {
    RCL_SET_ERROR_MSG("local_arguments invalid and not using global arguments");
    return RCL_RET_INVALID_ARGUMENT;
  }

  *output_name = NULL;
  rcl_remap_t * rule = NULL;

  // Look at local rules first
  if (NULL != local_arguments) {
    rcl_ret_t ret = rcl_remap_first_match(
      local_arguments->impl->remap_rules, local_arguments->impl->num_remap_rules, type_bitmask,
      name, node_name, node_namespace, substitutions, allocator, &rule);
    if (ret != RCL_RET_OK) {
      return ret;
    }
  }
  // Check global rules if no local rule matched
  if (NULL == rule && NULL != global_arguments) {
    rcl_ret_t ret = rcl_remap_first_match(
      global_arguments->impl->remap_rules, global_arguments->impl->num_remap_rules, type_bitmask,
      name, node_name, node_namespace, substitutions, allocator, &rule);
    if (ret != RCL_RET_OK) {
      return ret;
    }
  }
  // Do the remapping
  if (NULL != rule) {
    if (rule->impl->type & (RCL_TOPIC_REMAP | RCL_SERVICE_REMAP)) {
      // topic and service rules need the replacement to be expanded to a FQN
      rcl_ret_t ret = rcl_expand_topic_name(
        rule->impl->replacement, node_name, node_namespace, substitutions, allocator, output_name);
      if (RCL_RET_OK != ret) {
        return ret;
      }
    } else {
      // nodename and namespace rules don't need replacment expanded
      *output_name = rcutils_strdup(rule->impl->replacement, allocator);
    }
    if (NULL == *output_name) {
      RCL_SET_ERROR_MSG("Failed to set output");
      return RCL_RET_ERROR;
    }
  }
  return RCL_RET_OK;
}

rcl_ret_t
rcl_remap_topic_name(
  const rcl_arguments_t * local_arguments,
  const rcl_arguments_t * global_arguments,
  const char * topic_name,
  const char * node_name,
  const char * node_namespace,
  rcl_allocator_t allocator,
  char ** output_name)
{
  RCL_CHECK_ALLOCATOR_WITH_MSG(&allocator, "allocator is invalid", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(topic_name, RCL_RET_INVALID_ARGUMENT);

  rcutils_string_map_t substitutions = rcutils_get_zero_initialized_string_map();
  rcutils_ret_t rcutils_ret = rcutils_string_map_init(&substitutions, 0, allocator);
  rcl_ret_t ret = RCL_RET_ERROR;
  if (RCUTILS_RET_OK == rcutils_ret) {
    ret = rcl_get_default_topic_name_substitutions(&substitutions);
    if (RCL_RET_OK == ret) {
      ret = rcl_remap_name(
        local_arguments, global_arguments, RCL_TOPIC_REMAP, topic_name, node_name,
        node_namespace, &substitutions, allocator, output_name);
    }
  }
  if (RCUTILS_RET_OK != rcutils_string_map_fini(&substitutions)) {
    return RCL_RET_ERROR;
  }
  return ret;
}

rcl_ret_t
rcl_remap_service_name(
  const rcl_arguments_t * local_arguments,
  const rcl_arguments_t * global_arguments,
  const char * service_name,
  const char * node_name,
  const char * node_namespace,
  rcl_allocator_t allocator,
  char ** output_name)
{
  RCL_CHECK_ALLOCATOR_WITH_MSG(&allocator, "allocator is invalid", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(service_name, RCL_RET_INVALID_ARGUMENT);

  rcutils_string_map_t substitutions = rcutils_get_zero_initialized_string_map();
  rcutils_ret_t rcutils_ret = rcutils_string_map_init(&substitutions, 0, allocator);
  rcl_ret_t ret = RCL_RET_ERROR;
  if (rcutils_ret == RCUTILS_RET_OK) {
    ret = rcl_get_default_topic_name_substitutions(&substitutions);
    if (ret == RCL_RET_OK) {
      ret = rcl_remap_name(
        local_arguments, global_arguments, RCL_SERVICE_REMAP, service_name, node_name,
        node_namespace, &substitutions, allocator, output_name);
    }
  }
  if (RCUTILS_RET_OK != rcutils_string_map_fini(&substitutions)) {
    return RCL_RET_ERROR;
  }
  return ret;
}

rcl_ret_t
rcl_remap_node_name(
  const rcl_arguments_t * local_arguments,
  const rcl_arguments_t * global_arguments,
  const char * node_name,
  rcl_allocator_t allocator,
  char ** output_name)
{
  RCUTILS_CAN_SET_MSG_AND_RETURN_WITH_ERROR_OF(RCL_RET_INVALID_ARGUMENT);
  RCUTILS_CAN_SET_MSG_AND_RETURN_WITH_ERROR_OF(RCL_RET_NODE_INVALID_NAME);
  RCUTILS_CAN_SET_MSG_AND_RETURN_WITH_ERROR_OF(RCL_RET_BAD_ALLOC);
  RCUTILS_CAN_SET_MSG_AND_RETURN_WITH_ERROR_OF(RCL_RET_ERROR);

  RCL_CHECK_ARGUMENT_FOR_NULL(node_name, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ALLOCATOR_WITH_MSG(&allocator, "allocator is invalid", return RCL_RET_INVALID_ARGUMENT);
  return rcl_remap_name(
    local_arguments, global_arguments, RCL_NODENAME_REMAP, NULL, node_name, NULL, NULL,
    allocator, output_name);
}

rcl_ret_t
rcl_remap_node_namespace(
  const rcl_arguments_t * local_arguments,
  const rcl_arguments_t * global_arguments,
  const char * node_name,
  rcl_allocator_t allocator,
  char ** output_namespace)
{
  RCUTILS_CAN_SET_MSG_AND_RETURN_WITH_ERROR_OF(RCL_RET_INVALID_ARGUMENT);
  RCUTILS_CAN_SET_MSG_AND_RETURN_WITH_ERROR_OF(RCL_RET_NODE_INVALID_NAMESPACE);
  RCUTILS_CAN_SET_MSG_AND_RETURN_WITH_ERROR_OF(RCL_RET_BAD_ALLOC);
  RCUTILS_CAN_SET_MSG_AND_RETURN_WITH_ERROR_OF(RCL_RET_ERROR);

  RCL_CHECK_ARGUMENT_FOR_NULL(node_name, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ALLOCATOR_WITH_MSG(&allocator, "allocator is invalid", return RCL_RET_INVALID_ARGUMENT);
  return rcl_remap_name(
    local_arguments, global_arguments, RCL_NAMESPACE_REMAP, NULL, node_name, NULL, NULL,
    allocator, output_namespace);
}

rcl_ret_t
rcl_remap_fini(
  rcl_remap_t * rule)
{
  RCL_CHECK_ARGUMENT_FOR_NULL(rule, RCL_RET_INVALID_ARGUMENT);
  if (rule->impl) {
    rcl_ret_t ret = RCL_RET_OK;
    if (NULL != rule->impl->node_name) {
      rule->impl->allocator.deallocate(
        rule->impl->node_name, rule->impl->allocator.state);
      rule->impl->node_name = NULL;
    }
    if (NULL != rule->impl->match) {
      rule->impl->allocator.deallocate(
        rule->impl->match, rule->impl->allocator.state);
      rule->impl->match = NULL;
    }
    if (NULL != rule->impl->replacement) {
      rule->impl->allocator.deallocate(
        rule->impl->replacement, rule->impl->allocator.state);
      rule->impl->replacement = NULL;
    }
    rule->impl->allocator.deallocate(rule->impl, rule->impl->allocator.state);
    rule->impl = NULL;
    return ret;
  }
  RCL_SET_ERROR_MSG("rcl_remap_t finalized twice");
  return RCL_RET_ERROR;
}

#ifdef __cplusplus
}
#endif
