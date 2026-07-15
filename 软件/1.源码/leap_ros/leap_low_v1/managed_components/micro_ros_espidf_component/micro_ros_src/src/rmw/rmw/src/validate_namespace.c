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

#include "rmw/validate_namespace.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include <rcutils/snprintf.h>

#include "rmw/error_handling.h"
#include "rmw/validate_full_topic_name.h"

rmw_ret_t
rmw_validate_namespace(
  const char * namespace_,
  int * validation_result,
  size_t * invalid_index)
{
  if (!namespace_) {
    return RMW_RET_INVALID_ARGUMENT;
  }
  return rmw_validate_namespace_with_size(
    namespace_, strlen(namespace_), validation_result, invalid_index);
}

rmw_ret_t
rmw_validate_namespace_with_size(
  const char * namespace_,
  size_t namespace_length,
  int * validation_result,
  size_t * invalid_index)
{
  if (!namespace_) {
    return RMW_RET_INVALID_ARGUMENT;
  }
  if (!validation_result) {
    return RMW_RET_INVALID_ARGUMENT;
  }

  // Special case for root namepsace
  if (namespace_length == 1 && namespace_[0] == '/') {
    // Ok to return here, it is valid and will not exceed RMW_NAMESPACE_MAX_LENGTH.
    *validation_result = RMW_NAMESPACE_VALID;
    return RMW_RET_OK;
  }

  // All other cases should pass the validate topic name test.
  int t_validation_result;
  size_t t_invalid_index;
  rmw_ret_t ret = rmw_validate_full_topic_name(namespace_, &t_validation_result, &t_invalid_index);
  if (ret != RMW_RET_OK) {
    // error already set
    return ret;
  }

  if (t_validation_result != RMW_TOPIC_VALID && t_validation_result != RMW_TOPIC_INVALID_TOO_LONG) {
    switch (t_validation_result) {
      case RMW_TOPIC_INVALID_IS_EMPTY_STRING:
        *validation_result = RMW_NAMESPACE_INVALID_IS_EMPTY_STRING;
        break;
      case RMW_TOPIC_INVALID_NOT_ABSOLUTE:
        *validation_result = RMW_NAMESPACE_INVALID_NOT_ABSOLUTE;
        break;
      case RMW_TOPIC_INVALID_ENDS_WITH_FORWARD_SLASH:
        *validation_result = RMW_NAMESPACE_INVALID_ENDS_WITH_FORWARD_SLASH;
        break;
      case RMW_TOPIC_INVALID_CONTAINS_UNALLOWED_CHARACTERS:
        *validation_result = RMW_NAMESPACE_INVALID_CONTAINS_UNALLOWED_CHARACTERS;
        break;
      case RMW_TOPIC_INVALID_CONTAINS_REPEATED_FORWARD_SLASH:
        *validation_result = RMW_NAMESPACE_INVALID_CONTAINS_REPEATED_FORWARD_SLASH;
        break;
      case RMW_TOPIC_INVALID_NAME_TOKEN_STARTS_WITH_NUMBER:
        *validation_result = RMW_NAMESPACE_INVALID_NAME_TOKEN_STARTS_WITH_NUMBER;
        break;
      default:
        {
          char default_err_msg[256];
          // explicitly not taking return value which is number of bytes written
          int ret = rcutils_snprintf(
            default_err_msg, sizeof(default_err_msg),
            "rmw_validate_namespace(): unknown rmw_validate_full_topic_name() result '%d'",
            t_validation_result);
          if (ret < 0) {
            RMW_SET_ERROR_MSG("rmw_validate_namespace(): rcutils_snprintf() failed");
          } else {
            RMW_SET_ERROR_MSG(default_err_msg);
          }
        }
        return RMW_RET_ERROR;
    }
    if (invalid_index) {
      *invalid_index = t_invalid_index;
    }
    return RMW_RET_OK;
  }

  // check if the namespace is too long last, since it might be a soft invalidation
  if (namespace_length > RMW_NAMESPACE_MAX_LENGTH) {
    *validation_result = RMW_NAMESPACE_INVALID_TOO_LONG;
    if (invalid_index) {
      *invalid_index = RMW_NAMESPACE_MAX_LENGTH - 1;
    }
    return RMW_RET_OK;
  }

  // everything was ok, set result to valid namespace, avoid setting invalid_index, and return
  *validation_result = RMW_NAMESPACE_VALID;
  return RMW_RET_OK;
}

const char *
rmw_namespace_validation_result_string(int validation_result)
{
  switch (validation_result) {
    case RMW_NAMESPACE_VALID:
      return NULL;
    case RMW_NAMESPACE_INVALID_IS_EMPTY_STRING:
      return "namespace must not be empty";
    case RMW_NAMESPACE_INVALID_NOT_ABSOLUTE:
      return "namespace must be absolute, it must lead with a '/'";
    case RMW_NAMESPACE_INVALID_ENDS_WITH_FORWARD_SLASH:
      return "namespace must not end with a '/', unless only a '/'";
    case RMW_NAMESPACE_INVALID_CONTAINS_UNALLOWED_CHARACTERS:
      return "namespace must not contain characters other than alphanumerics, '_', or '/'";
    case RMW_NAMESPACE_INVALID_CONTAINS_REPEATED_FORWARD_SLASH:
      return "namespace must not contain repeated '/'";
    case RMW_NAMESPACE_INVALID_NAME_TOKEN_STARTS_WITH_NUMBER:
      return "namespace must not have a token that starts with a number";
    case RMW_NAMESPACE_INVALID_TOO_LONG:
      return "namespace should not exceed '" RMW_STRINGIFY(RMW_NAMESPACE_MAX_NAME_LENGTH) "'";
    default:
      return "unknown result code for rmw namespace validation";
  }
}
