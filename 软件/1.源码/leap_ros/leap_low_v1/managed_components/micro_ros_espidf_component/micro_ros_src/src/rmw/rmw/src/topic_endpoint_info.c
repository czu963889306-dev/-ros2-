// Copyright 2019 Amazon.com, Inc. or its affiliates. All Rights Reserved.
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

#include "rmw/topic_endpoint_info.h"

#include "rcutils/macros.h"
#include "rcutils/strdup.h"
#include "rmw/error_handling.h"
#include "rmw/types.h"

rmw_topic_endpoint_info_t
rmw_get_zero_initialized_topic_endpoint_info(void)
{
#ifdef __clang__
# pragma clang diagnostic push
# pragma clang diagnostic ignored "-Wmissing-field-initializers"
#endif
  rmw_topic_endpoint_info_t zero = {0};
#ifdef __clang__
# pragma clang diagnostic pop
#endif
  return zero;
}

rmw_ret_t
_rmw_topic_endpoint_info_fini_str(
  const char ** topic_endpoint_info_str,
  rcutils_allocator_t * allocator)
{
  allocator->deallocate((char *) *topic_endpoint_info_str, allocator->state);
  *topic_endpoint_info_str = NULL;
  return RMW_RET_OK;
}

rmw_ret_t
_rmw_topic_endpoint_info_fini_node_name(
  rmw_topic_endpoint_info_t * topic_endpoint_info,
  rcutils_allocator_t * allocator)
{
  return _rmw_topic_endpoint_info_fini_str(&topic_endpoint_info->node_name, allocator);
}

rmw_ret_t
_rmw_topic_endpoint_info_fini_node_namespace(
  rmw_topic_endpoint_info_t * topic_endpoint_info,
  rcutils_allocator_t * allocator)
{
  return _rmw_topic_endpoint_info_fini_str(&topic_endpoint_info->node_namespace, allocator);
}

rmw_ret_t
_rmw_topic_endpoint_info_fini_topic_type(
  rmw_topic_endpoint_info_t * topic_endpoint_info,
  rcutils_allocator_t * allocator)
{
  return _rmw_topic_endpoint_info_fini_str(&topic_endpoint_info->topic_type, allocator);
}

rmw_ret_t
rmw_topic_endpoint_info_fini(
  rmw_topic_endpoint_info_t * topic_endpoint_info,
  rcutils_allocator_t * allocator)
{
  RCUTILS_CAN_RETURN_WITH_ERROR_OF(RMW_RET_INVALID_ARGUMENT);

  if (!topic_endpoint_info) {
    RMW_SET_ERROR_MSG("topic_endpoint_info is null");
    return RMW_RET_INVALID_ARGUMENT;
  }
  if (!allocator) {
    RMW_SET_ERROR_MSG("allocator is null");
    return RMW_RET_INVALID_ARGUMENT;
  }

  rmw_ret_t ret;
  ret = _rmw_topic_endpoint_info_fini_node_name(topic_endpoint_info, allocator);
  if (ret != RMW_RET_OK) {
    return ret;
  }
  ret = _rmw_topic_endpoint_info_fini_node_namespace(topic_endpoint_info, allocator);
  if (ret != RMW_RET_OK) {
    return ret;
  }
  ret = _rmw_topic_endpoint_info_fini_topic_type(topic_endpoint_info, allocator);
  if (ret != RMW_RET_OK) {
    return ret;
  }

  *topic_endpoint_info = rmw_get_zero_initialized_topic_endpoint_info();

  return RMW_RET_OK;
}

rmw_ret_t
_rmw_topic_endpoint_info_copy_str(
  const char ** topic_endpoint_info_str,
  const char * str,
  rcutils_allocator_t * allocator)
{
  RCUTILS_CAN_RETURN_WITH_ERROR_OF(RMW_RET_INVALID_ARGUMENT);
  RCUTILS_CAN_RETURN_WITH_ERROR_OF(RMW_RET_BAD_ALLOC);

  if (!str) {
    RMW_SET_ERROR_MSG("str is null");
    return RMW_RET_INVALID_ARGUMENT;
  }

  if (!topic_endpoint_info_str) {
    RMW_SET_ERROR_MSG("topic_endpoint_info_str is null");
    return RMW_RET_INVALID_ARGUMENT;
  }

  if (!allocator) {
    RMW_SET_ERROR_MSG("allocator is null");
    return RMW_RET_INVALID_ARGUMENT;
  }

  *topic_endpoint_info_str = rcutils_strdup(str, *allocator);
  if (NULL == *topic_endpoint_info_str) {
    return RMW_RET_BAD_ALLOC;
  }

  return RMW_RET_OK;
}

rmw_ret_t
rmw_topic_endpoint_info_set_topic_type(
  rmw_topic_endpoint_info_t * topic_endpoint_info,
  const char * topic_type,
  rcutils_allocator_t * allocator)
{
  RCUTILS_CAN_RETURN_WITH_ERROR_OF(RMW_RET_INVALID_ARGUMENT);

  if (!topic_endpoint_info) {
    RMW_SET_ERROR_MSG("topic_endpoint_info is null");
    return RMW_RET_INVALID_ARGUMENT;
  }
  return _rmw_topic_endpoint_info_copy_str(&topic_endpoint_info->topic_type, topic_type, allocator);
}

rmw_ret_t
rmw_topic_endpoint_info_set_node_name(
  rmw_topic_endpoint_info_t * topic_endpoint_info,
  const char * node_name,
  rcutils_allocator_t * allocator)
{
  RCUTILS_CAN_RETURN_WITH_ERROR_OF(RMW_RET_INVALID_ARGUMENT);

  if (!topic_endpoint_info) {
    RMW_SET_ERROR_MSG("topic_endpoint_info is null");
    return RMW_RET_INVALID_ARGUMENT;
  }
  return _rmw_topic_endpoint_info_copy_str(&topic_endpoint_info->node_name, node_name, allocator);
}

rmw_ret_t
rmw_topic_endpoint_info_set_node_namespace(
  rmw_topic_endpoint_info_t * topic_endpoint_info,
  const char * node_namespace,
  rcutils_allocator_t * allocator)
{
  RCUTILS_CAN_RETURN_WITH_ERROR_OF(RMW_RET_INVALID_ARGUMENT);

  if (!topic_endpoint_info) {
    RMW_SET_ERROR_MSG("topic_endpoint_info is null");
    return RMW_RET_INVALID_ARGUMENT;
  }
  return _rmw_topic_endpoint_info_copy_str(
    &topic_endpoint_info->node_namespace,
    node_namespace,
    allocator);
}

rmw_ret_t
rmw_topic_endpoint_info_set_endpoint_type(
  rmw_topic_endpoint_info_t * topic_endpoint_info,
  rmw_endpoint_type_t type)
{
  RCUTILS_CAN_RETURN_WITH_ERROR_OF(RMW_RET_INVALID_ARGUMENT);

  if (!topic_endpoint_info) {
    RMW_SET_ERROR_MSG("topic_endpoint_info is null");
    return RMW_RET_INVALID_ARGUMENT;
  }

  topic_endpoint_info->endpoint_type = type;

  return RMW_RET_OK;
}

rmw_ret_t
rmw_topic_endpoint_info_set_gid(
  rmw_topic_endpoint_info_t * topic_endpoint_info,
  const uint8_t * gid,
  size_t size)
{
  RCUTILS_CAN_RETURN_WITH_ERROR_OF(RMW_RET_INVALID_ARGUMENT);

  if (!topic_endpoint_info) {
    RMW_SET_ERROR_MSG("topic_endpoint_info is null");
    return RMW_RET_INVALID_ARGUMENT;
  }
  if (size > RMW_GID_STORAGE_SIZE) {
    RMW_SET_ERROR_MSG("size is more than RMW_GID_STORAGE_SIZE");
    return RMW_RET_INVALID_ARGUMENT;
  }
  memset(topic_endpoint_info->endpoint_gid, 0, RMW_GID_STORAGE_SIZE);
  memcpy(topic_endpoint_info->endpoint_gid, gid, size);
  return RMW_RET_OK;
}

rmw_ret_t
rmw_topic_endpoint_info_set_qos_profile(
  rmw_topic_endpoint_info_t * topic_endpoint_info,
  const rmw_qos_profile_t * qos_profile)
{
  RCUTILS_CAN_RETURN_WITH_ERROR_OF(RMW_RET_INVALID_ARGUMENT);

  if (!topic_endpoint_info) {
    RMW_SET_ERROR_MSG("topic_endpoint_info is null");
    return RMW_RET_INVALID_ARGUMENT;
  }

  if (!qos_profile) {
    RMW_SET_ERROR_MSG("qos_profile is null");
    return RMW_RET_INVALID_ARGUMENT;
  }

  topic_endpoint_info->qos_profile = *qos_profile;
  return RMW_RET_OK;
}
