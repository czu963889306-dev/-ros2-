// Copyright 2015 Open Source Robotics Foundation, Inc.
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

#include "rcl/node.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef RCL_COMMAND_LINE_ENABLED
#include "rcl/arguments.h"
#endif // RCL_COMMAND_LINE_ENABLED
#include "rcl/error_handling.h"
#include "rcl/init_options.h"
#include "rcl/localhost.h"
#ifdef RCL_LOGGING_ENABLED
#include "rcl/logging.h"
#include "rcl/logging_rosout.h"
#endif // RCL_LOGGING_ENABLED
#include "rcl/rcl.h"
#include "rcl/remap.h"
#include "rcl/security.h"

#include "rcutils/env.h"
#include "rcutils/filesystem.h"
#include "rcutils/find.h"
#include "rcutils/format_string.h"
#include "rcutils/logging_macros.h"
#include "rcutils/macros.h"
#include "rcutils/repl_str.h"
#include "rcutils/snprintf.h"
#include "rcutils/strdup.h"

#include "rmw/error_handling.h"
#include "rmw/security_options.h"
#include "rmw/rmw.h"
#include "rmw/validate_namespace.h"
#include "rmw/validate_node_name.h"
#include "tracetools/tracetools.h"

#include "./context_impl.h"

const char * const RCL_DISABLE_LOANED_MESSAGES_ENV_VAR = "ROS_DISABLE_LOANED_MESSAGES";

struct rcl_node_impl_s
{
  rcl_node_options_t options;
  rmw_node_t * rmw_node_handle;
  rcl_guard_condition_t * graph_guard_condition;
  const char * logger_name;
  const char * fq_name;
};


/// Return the logger name associated with a node given the validated node name and namespace.
/**
 * E.g. for a node named "c" in namespace "/a/b", the logger name will be
 * "a.b.c", assuming logger name separator of ".".
 *
 * \param[in] node_name validated node name (a single token)
 * \param[in] node_namespace validated, absolute namespace (starting with "/")
 * \param[in] allocator the allocator to use for allocation
 * \returns duplicated string or null if there is an error
 */
const char * rcl_create_node_logger_name(
  const char * node_name,
  const char * node_namespace,
  const rcl_allocator_t * allocator)
{
  // If the namespace is the root namespace ("/"), the logger name is just the node name.
  if (strlen(node_namespace) == 1) {
    return rcutils_strdup(node_name, *allocator);
  }

  // Convert the forward slashes in the namespace to the separator used for logger names.
  // The input namespace has already been expanded and therefore will always be absolute,
  // i.e. it will start with a forward slash, which we want to ignore.
  const char * ns_with_separators = rcutils_repl_str(
    node_namespace + 1,  // Ignore the leading forward slash.
    "/", RCUTILS_LOGGING_SEPARATOR_STRING,
    allocator);
  if (NULL == ns_with_separators) {
    return NULL;
  }

  // Join the namespace and node name to create the logger name.
  char * node_logger_name = rcutils_format_string(
    *allocator, "%s%s%s", ns_with_separators, RCUTILS_LOGGING_SEPARATOR_STRING, node_name);
  allocator->deallocate((char *)ns_with_separators, allocator->state);
  return node_logger_name;
}

rcl_node_t
rcl_get_zero_initialized_node()
{
  static rcl_node_t null_node = {
    .context = 0,
    .impl = 0
  };
  return null_node;
}

rcl_ret_t
rcl_node_init(
  rcl_node_t * node,
  const char * name,
  const char * namespace_,
  rcl_context_t * context,
  const rcl_node_options_t * options)
{
  const rmw_guard_condition_t * rmw_graph_guard_condition = NULL;
  rcl_guard_condition_options_t graph_guard_condition_options =
    rcl_guard_condition_get_default_options();
  rcl_ret_t ret;
  rcl_ret_t fail_ret = RCL_RET_ERROR;
  char * remapped_node_name = NULL;

  // Check options and allocator first, so allocator can be used for errors.
  RCL_CHECK_ARGUMENT_FOR_NULL(options, RCL_RET_INVALID_ARGUMENT);
  const rcl_allocator_t * allocator = &options->allocator;
  RCL_CHECK_ALLOCATOR_WITH_MSG(allocator, "invalid allocator", return RCL_RET_INVALID_ARGUMENT);

  RCL_CHECK_ARGUMENT_FOR_NULL(name, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(namespace_, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(node, RCL_RET_INVALID_ARGUMENT);
  RCUTILS_LOG_DEBUG_NAMED(
    ROS_PACKAGE_NAME, "Initializing node '%s' in namespace '%s'", name, namespace_);
  if (node->impl) {
    RCL_SET_ERROR_MSG("node already initialized, or struct memory was unintialized");
    return RCL_RET_ALREADY_INIT;
  }
  // Make sure rcl has been initialized.
  RCL_CHECK_FOR_NULL_WITH_MSG(
    context, "given context in options is NULL", return RCL_RET_INVALID_ARGUMENT);
  if (!rcl_context_is_valid(context)) {
    RCL_SET_ERROR_MSG(
      "the given context is not valid, "
      "either rcl_init() was not called or rcl_shutdown() was called.");
    return RCL_RET_NOT_INIT;
  }
  // Make sure the node name is valid before allocating memory.
  int validation_result = 0;
  ret = rmw_validate_node_name(name, &validation_result, NULL);
  if (ret != RMW_RET_OK) {
    RCL_SET_ERROR_MSG(rmw_get_error_string().str);
    return ret;
  }
  if (validation_result != RMW_NODE_NAME_VALID) {
    const char * msg = rmw_node_name_validation_result_string(validation_result);
    RCL_SET_ERROR_MSG(msg);
    return RCL_RET_NODE_INVALID_NAME;
  }

  // Process the namespace.
  size_t namespace_length = strlen(namespace_);
  const char * local_namespace_ = namespace_;
  bool should_free_local_namespace_ = false;
  // If the namespace is just an empty string, replace with "/"
  if (namespace_length == 0) {
    // Have this special case to avoid a memory allocation when "" is passed.
    local_namespace_ = "/";
  }

  // If the namespace does not start with a /, add one.
  if (namespace_length > 0 && namespace_[0] != '/') {
    local_namespace_ = rcutils_format_string(*allocator, "/%s", namespace_);
    RCL_CHECK_FOR_NULL_WITH_MSG(
      local_namespace_,
      "failed to format node namespace string",
      ret = RCL_RET_BAD_ALLOC; goto cleanup);
    should_free_local_namespace_ = true;
  }
  // Make sure the node namespace is valid.
  validation_result = 0;
  ret = rmw_validate_namespace(local_namespace_, &validation_result, NULL);
  if (ret != RMW_RET_OK) {
    RCL_SET_ERROR_MSG(rmw_get_error_string().str);
    goto cleanup;
  }
  if (validation_result != RMW_NAMESPACE_VALID) {
    const char * msg = rmw_namespace_validation_result_string(validation_result);
    RCL_SET_ERROR_MSG_WITH_FORMAT_STRING("%s, result: %d", msg, validation_result);

    ret = RCL_RET_NODE_INVALID_NAMESPACE;
    goto cleanup;
  }

  // Allocate space for the implementation struct.
  node->impl = (rcl_node_impl_t *)allocator->allocate(sizeof(rcl_node_impl_t), allocator->state);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    node->impl, "allocating memory failed", ret = RCL_RET_BAD_ALLOC; goto cleanup);
  node->impl->rmw_node_handle = NULL;
  node->impl->graph_guard_condition = NULL;
  node->impl->logger_name = NULL;
  node->impl->fq_name = NULL;
  node->impl->options = rcl_node_get_default_options();
  node->context = context;
  // Initialize node impl.
  ret = rcl_node_options_copy(options, &(node->impl->options));
  if (RCL_RET_OK != ret) {
    goto fail;
  }

#ifdef RCL_COMMAND_LINE_ENABLED
  // Remap the node name and namespace if remap rules are given
  rcl_arguments_t * global_args = NULL;
  if (node->impl->options.use_global_arguments) {
    global_args = &(node->context->global_arguments);
  }
  ret = rcl_remap_node_name(
    &(node->impl->options.arguments), global_args, name, *allocator,
    &remapped_node_name);
  if (RCL_RET_OK != ret) {
    goto fail;
  } else if (NULL != remapped_node_name) {
    name = remapped_node_name;
  }
  char * remapped_namespace = NULL;
  ret = rcl_remap_node_namespace(
    &(node->impl->options.arguments), global_args, name,
    *allocator, &remapped_namespace);
  if (RCL_RET_OK != ret) {
    goto fail;
  } else if (NULL != remapped_namespace) {
    if (should_free_local_namespace_) {
      allocator->deallocate((char *)local_namespace_, allocator->state);
    }
    should_free_local_namespace_ = true;
    local_namespace_ = remapped_namespace;
  }
#endif // RCL_COMMAND_LINE_ENABLED

  // compute fully qualfied name of the node.
  if ('/' == local_namespace_[strlen(local_namespace_) - 1]) {
    node->impl->fq_name = rcutils_format_string(*allocator, "%s%s", local_namespace_, name);
  } else {
    node->impl->fq_name = rcutils_format_string(*allocator, "%s/%s", local_namespace_, name);
  }

  // node logger name
#ifdef RCL_LOGGING_ENABLED
  node->impl->logger_name = rcl_create_node_logger_name(name, local_namespace_, allocator);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    node->impl->logger_name, "creating logger name failed", goto fail);
#endif // RCL_LOGGING_ENABLED

  RCUTILS_LOG_DEBUG_NAMED(
    ROS_PACKAGE_NAME, "Using domain ID of '%zu'", context->impl->rmw_context.actual_domain_id);

  node->impl->rmw_node_handle = rmw_create_node(
    &(node->context->impl->rmw_context),
    name, local_namespace_);

  RCL_CHECK_FOR_NULL_WITH_MSG(
    node->impl->rmw_node_handle, rmw_get_error_string().str, goto fail);
  // graph guard condition
  rmw_graph_guard_condition = rmw_node_get_graph_guard_condition(node->impl->rmw_node_handle);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    rmw_graph_guard_condition, rmw_get_error_string().str, goto fail);

  node->impl->graph_guard_condition = (rcl_guard_condition_t *)allocator->allocate(
    sizeof(rcl_guard_condition_t), allocator->state);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    node->impl->graph_guard_condition,
    "allocating memory failed",
    goto fail);
  *node->impl->graph_guard_condition = rcl_get_zero_initialized_guard_condition();
  graph_guard_condition_options.allocator = *allocator;
  ret = rcl_guard_condition_init_from_rmw(
    node->impl->graph_guard_condition,
    rmw_graph_guard_condition,
    context,
    graph_guard_condition_options);
  if (ret != RCL_RET_OK) {
    // error message already set
    goto fail;
  }
#ifdef RCL_LOGGING_ENABLED
  // The initialization for the rosout publisher requires the node to be in initialized to a point
  // that it can create new topic publishers
  if (rcl_logging_rosout_enabled() && node->impl->options.enable_rosout) {
    ret = rcl_logging_rosout_init_publisher_for_node(node);
    if (ret != RCL_RET_OK) {
      // error message already set
      goto fail;
    }
  }
#endif // RCL_LOGGING_ENABLED
  RCUTILS_LOG_DEBUG_NAMED(ROS_PACKAGE_NAME, "Node initialized");
  ret = RCL_RET_OK;
  TRACEPOINT(
    rcl_node_init,
    (const void *)node,
    (const void *)rcl_node_get_rmw_handle(node),
    rcl_node_get_name(node),
    rcl_node_get_namespace(node));
  goto cleanup;
fail:
  if (node->impl) {
#ifdef RCL_LOGGING_ENABLED
    if (rcl_logging_rosout_enabled() &&
      node->impl->options.enable_rosout &&
      node->impl->logger_name)
    {
      ret = rcl_logging_rosout_fini_publisher_for_node(node);
      RCUTILS_LOG_ERROR_EXPRESSION_NAMED(
        (ret != RCL_RET_OK && ret != RCL_RET_NOT_INIT),
        ROS_PACKAGE_NAME, "Failed to fini publisher for node: %i", ret);
      allocator->deallocate((char *)node->impl->logger_name, allocator->state);
    }
#endif // RCL_LOGGING_ENABLED
    if (node->impl->fq_name) {
      allocator->deallocate((char *)node->impl->fq_name, allocator->state);
    }
    if (node->impl->rmw_node_handle) {
      ret = rmw_destroy_node(node->impl->rmw_node_handle);
      if (ret != RMW_RET_OK) {
        RCUTILS_LOG_ERROR_NAMED(
          ROS_PACKAGE_NAME,
          "failed to fini rmw node in error recovery: %s", rmw_get_error_string().str
        );
      }
    }
    if (node->impl->graph_guard_condition) {
      ret = rcl_guard_condition_fini(node->impl->graph_guard_condition);
      if (ret != RCL_RET_OK) {
        RCUTILS_LOG_ERROR_NAMED(
          ROS_PACKAGE_NAME,
          "failed to fini guard condition in error recovery: %s", rcl_get_error_string().str
        );
      }
      allocator->deallocate(node->impl->graph_guard_condition, allocator->state);
    }

#ifdef RCL_COMMAND_LINE_ENABLED
    if (NULL != node->impl->options.arguments.impl) {
      ret = rcl_arguments_fini(&(node->impl->options.arguments));
      if (ret != RCL_RET_OK) {
        RCUTILS_LOG_ERROR_NAMED(
          ROS_PACKAGE_NAME,
          "failed to fini arguments in error recovery: %s", rcl_get_error_string().str
        );
      }
    }
#endif // RCL_COMMAND_LINE_ENABLED
    allocator->deallocate(node->impl, allocator->state);
  }
  *node = rcl_get_zero_initialized_node();

  ret = fail_ret;
  // fall through from fail -> cleanup
cleanup:
  if (should_free_local_namespace_) {
    allocator->deallocate((char *)local_namespace_, allocator->state);
    local_namespace_ = NULL;
  }
  if (NULL != remapped_node_name) {
    allocator->deallocate(remapped_node_name, allocator->state);
  }
  return ret;
}

rcl_ret_t
rcl_node_fini(rcl_node_t * node)
{
  RCUTILS_LOG_DEBUG_NAMED(ROS_PACKAGE_NAME, "Finalizing node");
  RCL_CHECK_ARGUMENT_FOR_NULL(node, RCL_RET_NODE_INVALID);
  if (!node->impl) {
    // Repeat calls to fini or calling fini on a zero initialized node is ok.
    return RCL_RET_OK;
  }
  rcl_allocator_t allocator = node->impl->options.allocator;
  rcl_ret_t result = RCL_RET_OK;
  rcl_ret_t rcl_ret = RCL_RET_OK;
#ifdef RCL_LOGGING_ENABLED
  if (rcl_logging_rosout_enabled() && node->impl->options.enable_rosout) {
    rcl_ret = rcl_logging_rosout_fini_publisher_for_node(node);
    if (rcl_ret != RCL_RET_OK && rcl_ret != RCL_RET_NOT_INIT) {
      RCL_SET_ERROR_MSG("Unable to fini publisher for node.");
      result = RCL_RET_ERROR;
    }
  }
#endif // RCL_LOGGING_ENABLED
  rmw_ret_t rmw_ret = rmw_destroy_node(node->impl->rmw_node_handle);
  if (rmw_ret != RMW_RET_OK) {
    RCL_SET_ERROR_MSG(rmw_get_error_string().str);
    result = RCL_RET_ERROR;
  }
  rcl_ret = rcl_guard_condition_fini(node->impl->graph_guard_condition);
  if (rcl_ret != RCL_RET_OK) {
    RCL_SET_ERROR_MSG(rmw_get_error_string().str);
    result = RCL_RET_ERROR;
  }
  allocator.deallocate(node->impl->graph_guard_condition, allocator.state);
  // assuming that allocate and deallocate are ok since they are checked in init
  allocator.deallocate((char *)node->impl->logger_name, allocator.state);
  allocator.deallocate((char *)node->impl->fq_name, allocator.state);
#ifdef RCL_COMMAND_LINE_ENABLED
  if (NULL != node->impl->options.arguments.impl) {
    rcl_ret_t ret = rcl_arguments_fini(&(node->impl->options.arguments));
    if (ret != RCL_RET_OK) {
      return ret;
    }
  }
#endif // RCL_COMMAND_LINE_ENABLED
  allocator.deallocate(node->impl, allocator.state);
  node->impl = NULL;
  RCUTILS_LOG_DEBUG_NAMED(ROS_PACKAGE_NAME, "Node finalized");
  return result;
}

bool
rcl_node_is_valid_except_context(const rcl_node_t * node)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(node, "rcl node pointer is invalid", return false);
  RCL_CHECK_FOR_NULL_WITH_MSG(node->impl, "rcl node implementation is invalid", return false);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    node->impl->rmw_node_handle, "rcl node's rmw handle is invalid", return false);
  return true;
}

bool
rcl_node_is_valid(const rcl_node_t * node)
{
  bool result = rcl_node_is_valid_except_context(node);
  if (!result) {
    return result;
  }
  if (!rcl_context_is_valid(node->context)) {
    RCL_SET_ERROR_MSG("rcl node's context is invalid");
    return false;
  }
  return true;
}

const char *
rcl_node_get_name(const rcl_node_t * node)
{
  if (!rcl_node_is_valid_except_context(node)) {
    return NULL;  // error already set
  }
  return node->impl->rmw_node_handle->name;
}

const char *
rcl_node_get_namespace(const rcl_node_t * node)
{
  if (!rcl_node_is_valid_except_context(node)) {
    return NULL;  // error already set
  }
  return node->impl->rmw_node_handle->namespace_;
}

const char *
rcl_node_get_fully_qualified_name(const rcl_node_t * node)
{
  if (!rcl_node_is_valid_except_context(node)) {
    return NULL;  // error already set
  }
  return node->impl->fq_name;
}

const rcl_node_options_t *
rcl_node_get_options(const rcl_node_t * node)
{
  if (!rcl_node_is_valid_except_context(node)) {
    return NULL;  // error already set
  }
  return &node->impl->options;
}

rcl_ret_t
rcl_node_get_domain_id(const rcl_node_t * node, size_t * domain_id)
{
  if (!rcl_node_is_valid(node)) {
    return RCL_RET_NODE_INVALID;
  }
  RCL_CHECK_ARGUMENT_FOR_NULL(domain_id, RCL_RET_INVALID_ARGUMENT);
  rcl_ret_t ret = rcl_context_get_domain_id(node->context, domain_id);
  if (RCL_RET_OK != ret) {
    return ret;
  }
  return RCL_RET_OK;
}

rmw_node_t *
rcl_node_get_rmw_handle(const rcl_node_t * node)
{
  if (!rcl_node_is_valid_except_context(node)) {
    return NULL;  // error already set
  }
  return node->impl->rmw_node_handle;
}

uint64_t
rcl_node_get_rcl_instance_id(const rcl_node_t * node)
{
  if (!rcl_node_is_valid_except_context(node)) {
    return 0;  // error already set
  }
  return rcl_context_get_instance_id(node->context);
}

const rcl_guard_condition_t *
rcl_node_get_graph_guard_condition(const rcl_node_t * node)
{
  if (!rcl_node_is_valid_except_context(node)) {
    return NULL;  // error already set
  }
  return node->impl->graph_guard_condition;
}

const char *
rcl_node_get_logger_name(const rcl_node_t * node)
{
  if (!rcl_node_is_valid_except_context(node)) {
    return NULL;  // error already set
  }
  return node->impl->logger_name;
}

rcl_ret_t
rcl_get_disable_loaned_message(bool * disable_loaned_message)
{
  const char * env_val = NULL;
  const char * env_error_str = NULL;

  RCL_CHECK_ARGUMENT_FOR_NULL(disable_loaned_message, RCL_RET_INVALID_ARGUMENT);

  env_error_str = rcutils_get_env(RCL_DISABLE_LOANED_MESSAGES_ENV_VAR, &env_val);
  if (NULL != env_error_str) {
    RCL_SET_ERROR_MSG_WITH_FORMAT_STRING(
      "Error getting env var: '" RCUTILS_STRINGIFY(RCL_DISABLE_LOANED_MESSAGES_ENV_VAR) "': %s\n",
      env_error_str);
    return RCL_RET_ERROR;
  }

  *disable_loaned_message = (strcmp(env_val, "1") == 0);
  return RCL_RET_OK;
}
#ifdef __cplusplus
}
#endif
