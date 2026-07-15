// Copyright 2018-2019 Open Source Robotics Foundation, Inc.
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

#include "rcl/allocator.h"
#include "rcl/error_handling.h"
#include "rcl/logging_rosout.h"
#include "rcl/node.h"
#include "rcl/publisher.h"
#include "rcl/time.h"
#include "rcl/types.h"
#include "rcl/visibility_control.h"
#include "rcl_interfaces/msg/log.h"
#include "rcutils/allocator.h"
#include "rcutils/logging_macros.h"
#include "rcutils/macros.h"
#include "rcutils/types/hash_map.h"
#include "rcutils/types/rcutils_ret.h"
#include "rosidl_runtime_c/string_functions.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define ROSOUT_TOPIC_NAME "/rosout"

/* Return RCL_RET_OK from this macro because we won't check throughout rcl if rosout is
 * initialized or not and in the case it's not we want things to continue working.
 */
#define RCL_LOGGING_ROSOUT_VERIFY_INITIALIZED \
  if (!__is_initialized) { \
    return RCL_RET_OK; \
  }

#define RCL_RET_FROM_RCUTIL_RET(rcl_ret_var, rcutils_expr) \
  { \
    rcutils_ret_t rcutils_ret = rcutils_expr; \
    if (RCUTILS_RET_OK != rcutils_ret) { \
      if (rcutils_error_is_set()) { \
        RCL_SET_ERROR_MSG(rcutils_get_error_string().str); \
      } else { \
        RCL_SET_ERROR_MSG_WITH_FORMAT_STRING("rcutils_ret_t code: %i", rcutils_ret); \
      } \
    } \
    switch (rcutils_ret) { \
      case RCUTILS_RET_OK: \
        rcl_ret_var = RCL_RET_OK; \
        break; \
      case RCUTILS_RET_ERROR: \
        rcl_ret_var = RCL_RET_ERROR; \
        break; \
      case RCUTILS_RET_BAD_ALLOC: \
        rcl_ret_var = RCL_RET_BAD_ALLOC; \
        break; \
      case RCUTILS_RET_INVALID_ARGUMENT: \
        rcl_ret_var = RCL_RET_INVALID_ARGUMENT; \
        break; \
      case RCUTILS_RET_NOT_INITIALIZED: \
        rcl_ret_var = RCL_RET_NOT_INIT; \
        break; \
      default: \
        rcl_ret_var = RCUTILS_RET_ERROR; \
    } \
  }

typedef struct rosout_map_entry_t
{
  rcl_node_t * node;
  rcl_publisher_t publisher;
} rosout_map_entry_t;

static rcutils_hash_map_t __logger_map;
static bool __is_initialized = false;
static rcl_allocator_t __rosout_allocator;

rcl_ret_t rcl_logging_rosout_init(
  const rcl_allocator_t * allocator)
{
  RCL_CHECK_ARGUMENT_FOR_NULL(allocator, RCL_RET_INVALID_ARGUMENT);
  rcl_ret_t status = RCL_RET_OK;
  if (__is_initialized) {
    return RCL_RET_OK;
  }
  __logger_map = rcutils_get_zero_initialized_hash_map();
  RCL_RET_FROM_RCUTIL_RET(
    status,
    rcutils_hash_map_init(
      &__logger_map, 2, sizeof(const char *), sizeof(rosout_map_entry_t),
      rcutils_hash_map_string_hash_func, rcutils_hash_map_string_cmp_func, allocator));
  if (RCL_RET_OK == status) {
    __rosout_allocator = *allocator;
    __is_initialized = true;
  }
  return status;
}

rcl_ret_t rcl_logging_rosout_fini()
{
  RCL_LOGGING_ROSOUT_VERIFY_INITIALIZED
  rcl_ret_t status = RCL_RET_OK;
  char * key = NULL;
  rosout_map_entry_t entry;

  // fini all the outstanding publishers
  rcutils_ret_t hashmap_ret = rcutils_hash_map_get_next_key_and_data(
    &__logger_map, NULL, &key, &entry);
  while (RCL_RET_OK == status && RCUTILS_RET_OK == hashmap_ret) {
    // Teardown publisher
    status = rcl_publisher_fini(&entry.publisher, entry.node);

    if (RCL_RET_OK == status) {
      RCL_RET_FROM_RCUTIL_RET(status, rcutils_hash_map_unset(&__logger_map, &key));
    }

    if (RCL_RET_OK == status) {
      hashmap_ret = rcutils_hash_map_get_next_key_and_data(&__logger_map, NULL, &key, &entry);
    }
  }
  if (RCUTILS_RET_HASH_MAP_NO_MORE_ENTRIES != hashmap_ret) {
    RCL_RET_FROM_RCUTIL_RET(status, hashmap_ret);
  }

  if (RCL_RET_OK == status) {
    RCL_RET_FROM_RCUTIL_RET(status, rcutils_hash_map_fini(&__logger_map));
  }

  if (RCL_RET_OK == status) {
    __is_initialized = false;
  }
  return status;
}

rcl_ret_t rcl_logging_rosout_init_publisher_for_node(
  rcl_node_t * node)
{
  RCL_LOGGING_ROSOUT_VERIFY_INITIALIZED
  const char * logger_name = NULL;
  rosout_map_entry_t new_entry;
  rcl_ret_t status = RCL_RET_OK;

  // Verify input and make sure it's not already initialized
  RCL_CHECK_ARGUMENT_FOR_NULL(node, RCL_RET_NODE_INVALID);
  logger_name = rcl_node_get_logger_name(node);
  if (NULL == logger_name) {
    RCL_SET_ERROR_MSG("Logger name was null.");
    return RCL_RET_ERROR;
  }
  if (rcutils_hash_map_key_exists(&__logger_map, &logger_name)) {
    // @TODO(nburek) Update behavior to either enforce unique names or work with non-unique
    // names based on the outcome here: https://github.com/ros2/design/issues/187
    RCUTILS_LOG_WARN_NAMED(
      "rcl.logging_rosout",
      "Publisher already registered for provided node name. If this is due to multiple nodes "
      "with the same name then all logs for that logger name will go out over the existing "
      "publisher. As soon as any node with that name is destructed it will unregister the "
      "publisher, preventing any further logs for that name from being published on the rosout "
      "topic.");
    return RCL_RET_OK;
  }

  // Create a new Log message publisher on the node
  const rosidl_message_type_support_t * type_support =
    rosidl_typesupport_c__get_message_type_support_handle__rcl_interfaces__msg__Log();
  rcl_publisher_options_t options = rcl_publisher_get_default_options();

  // Late joining subscriptions get the user's setting of rosout qos options.
  const rcl_node_options_t * node_options = rcl_node_get_options(node);
  RCL_CHECK_FOR_NULL_WITH_MSG(node_options, "Node options was null.", return RCL_RET_ERROR);

  options.qos = node_options->rosout_qos;
  options.allocator = node_options->allocator;
  new_entry.publisher = rcl_get_zero_initialized_publisher();
  status =
    rcl_publisher_init(&new_entry.publisher, node, type_support, ROSOUT_TOPIC_NAME, &options);

  // Add the new publisher to the map
  if (RCL_RET_OK == status) {
    new_entry.node = node;
    RCL_RET_FROM_RCUTIL_RET(status, rcutils_hash_map_set(&__logger_map, &logger_name, &new_entry));
    if (RCL_RET_OK != status) {
      RCL_SET_ERROR_MSG("Failed to add publisher to map.");
      // We failed to add to the map so destroy the publisher that we created
      rcl_ret_t fini_status = rcl_publisher_fini(&new_entry.publisher, new_entry.node);
      // ignore the return status in favor of the failure from set
      RCL_UNUSED(fini_status);
    }
  }

  return status;
}

rcl_ret_t rcl_logging_rosout_fini_publisher_for_node(
  rcl_node_t * node)
{
  RCL_LOGGING_ROSOUT_VERIFY_INITIALIZED
  rosout_map_entry_t entry;
  const char * logger_name = NULL;
  rcl_ret_t status = RCL_RET_OK;

  // Verify input and make sure it's initialized
  RCL_CHECK_ARGUMENT_FOR_NULL(node, RCL_RET_NODE_INVALID);
  logger_name = rcl_node_get_logger_name(node);
  if (NULL == logger_name) {
    return RCL_RET_ERROR;
  }
  if (!rcutils_hash_map_key_exists(&__logger_map, &logger_name)) {
    return RCL_RET_OK;
  }

  // fini the publisher and remove the entry from the map
  RCL_RET_FROM_RCUTIL_RET(status, rcutils_hash_map_get(&__logger_map, &logger_name, &entry));
  if (RCL_RET_OK == status) {
    status = rcl_publisher_fini(&entry.publisher, entry.node);
  }
  if (RCL_RET_OK == status) {
    RCL_RET_FROM_RCUTIL_RET(status, rcutils_hash_map_unset(&__logger_map, &logger_name));
  }

  return status;
}

void rcl_logging_rosout_output_handler(
  const rcutils_log_location_t * location,
  int severity,
  const char * name,
  rcutils_time_point_value_t timestamp,
  const char * format,
  va_list * args)
{
  rosout_map_entry_t entry;
  rcl_ret_t status = RCL_RET_OK;
  if (!__is_initialized) {
    return;
  }
  rcutils_ret_t rcutils_ret = rcutils_hash_map_get(&__logger_map, &name, &entry);
  if (RCUTILS_RET_OK == rcutils_ret) {
    char msg_buf[1024] = "";
    rcutils_char_array_t msg_array = {
      .buffer = msg_buf,
      .owns_buffer = false,
      .buffer_length = 0u,
      .buffer_capacity = sizeof(msg_buf),
      .allocator = __rosout_allocator
    };

    RCL_RET_FROM_RCUTIL_RET(status, rcutils_char_array_vsprintf(&msg_array, format, *args));
    if (RCL_RET_OK != status) {
      RCUTILS_SAFE_FWRITE_TO_STDERR("Failed to format log string: ");
      RCUTILS_SAFE_FWRITE_TO_STDERR(rcl_get_error_string().str);
      rcl_reset_error();
      RCUTILS_SAFE_FWRITE_TO_STDERR("\n");
    } else {
      rcl_interfaces__msg__Log * log_message = rcl_interfaces__msg__Log__create();
      if (NULL != log_message) {
        log_message->stamp.sec = (int32_t) RCL_NS_TO_S(timestamp);
        log_message->stamp.nanosec = (timestamp % RCL_S_TO_NS(1));
        log_message->level = severity;
        log_message->line = (int32_t) location->line_number;
        rosidl_runtime_c__String__assign(&log_message->name, name);
        rosidl_runtime_c__String__assign(&log_message->msg, msg_array.buffer);
        rosidl_runtime_c__String__assign(&log_message->file, location->file_name);
        rosidl_runtime_c__String__assign(&log_message->function, location->function_name);
        status = rcl_publish(&entry.publisher, log_message, NULL);
        if (RCL_RET_OK != status) {
          RCUTILS_SAFE_FWRITE_TO_STDERR("Failed to publish log message to rosout: ");
          RCUTILS_SAFE_FWRITE_TO_STDERR(rcl_get_error_string().str);
          rcl_reset_error();
          RCUTILS_SAFE_FWRITE_TO_STDERR("\n");
        }

        rcl_interfaces__msg__Log__destroy(log_message);
      }
    }

    RCL_RET_FROM_RCUTIL_RET(status, rcutils_char_array_fini(&msg_array));
    if (RCL_RET_OK != status) {
      RCUTILS_SAFE_FWRITE_TO_STDERR("failed to fini char_array: ");
      RCUTILS_SAFE_FWRITE_TO_STDERR(rcl_get_error_string().str);
      rcl_reset_error();
      RCUTILS_SAFE_FWRITE_TO_STDERR("\n");
    }
  }
}


#ifdef __cplusplus
}
#endif
