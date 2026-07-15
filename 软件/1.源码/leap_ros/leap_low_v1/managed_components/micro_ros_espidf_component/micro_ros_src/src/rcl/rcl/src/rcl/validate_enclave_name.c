// Copyright 2020 Open Source Robotics Foundation, Inc.
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

#include "rcl/validate_enclave_name.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include <rcutils/macros.h>
#include <rcutils/snprintf.h>

#include "rmw/validate_namespace.h"

#include "rcl/error_handling.h"

#include "./common.h"

rcl_ret_t
rcl_validate_enclave_name(
  const char * enclave,
  int * validation_result,
  size_t * invalid_index)
{
  RCL_CHECK_ARGUMENT_FOR_NULL(enclave, RCL_RET_INVALID_ARGUMENT);
  return rcl_validate_enclave_name_with_size(
    enclave, strlen(enclave), validation_result, invalid_index);
}

rcl_ret_t
rcl_validate_enclave_name_with_size(
  const char * enclave,
  size_t enclave_length,
  int * validation_result,
  size_t * invalid_index)
{
  RCUTILS_CAN_SET_MSG_AND_RETURN_WITH_ERROR_OF(RCL_RET_INVALID_ARGUMENT);
  RCUTILS_CAN_SET_MSG_AND_RETURN_WITH_ERROR_OF(RCL_RET_ERROR);

  RCL_CHECK_ARGUMENT_FOR_NULL(enclave, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(validation_result, RCL_RET_INVALID_ARGUMENT);

  int tmp_validation_result;
  size_t tmp_invalid_index;
  rmw_ret_t ret = rmw_validate_namespace_with_size(
    enclave, enclave_length, &tmp_validation_result, &tmp_invalid_index);
  if (ret != RMW_RET_OK) {
    return rcl_convert_rmw_ret_to_rcl_ret(ret);
  }

  if (tmp_validation_result != RMW_NAMESPACE_VALID &&
    tmp_validation_result != RMW_NAMESPACE_INVALID_TOO_LONG)
  {
    switch (tmp_validation_result) {
      case RMW_NAMESPACE_INVALID_IS_EMPTY_STRING:
        *validation_result = RCL_ENCLAVE_NAME_INVALID_IS_EMPTY_STRING;
        break;
      case RMW_NAMESPACE_INVALID_NOT_ABSOLUTE:
        *validation_result = RCL_ENCLAVE_NAME_INVALID_NOT_ABSOLUTE;
        break;
      case RMW_NAMESPACE_INVALID_ENDS_WITH_FORWARD_SLASH:
        *validation_result = RCL_ENCLAVE_NAME_INVALID_ENDS_WITH_FORWARD_SLASH;
        break;
      case RMW_NAMESPACE_INVALID_CONTAINS_UNALLOWED_CHARACTERS:
        *validation_result = RCL_ENCLAVE_NAME_INVALID_CONTAINS_UNALLOWED_CHARACTERS;
        break;
      case RMW_NAMESPACE_INVALID_CONTAINS_REPEATED_FORWARD_SLASH:
        *validation_result = RCL_ENCLAVE_NAME_INVALID_CONTAINS_REPEATED_FORWARD_SLASH;
        break;
      case RMW_NAMESPACE_INVALID_NAME_TOKEN_STARTS_WITH_NUMBER:
        *validation_result = RCL_ENCLAVE_NAME_INVALID_NAME_TOKEN_STARTS_WITH_NUMBER;
        break;
      default:
        {
          char default_err_msg[256];
          // explicitly not taking return value which is number of bytes written
          int ret = rcutils_snprintf(
            default_err_msg, sizeof(default_err_msg),
            "rcl_validate_enclave_name_with_size(): "
            "unknown rmw_validate_namespace_with_size() result '%d'",
            tmp_validation_result);
          if (ret < 0) {
            RCL_SET_ERROR_MSG(
              "rcl_validate_enclave_name_with_size(): "
              "rcutils_snprintf() failed while reporting an unknown validation result");
          } else {
            RCL_SET_ERROR_MSG(default_err_msg);
          }
        }
        return RCL_RET_ERROR;
    }
    if (invalid_index) {
      *invalid_index = tmp_invalid_index;
    }
    return RCL_RET_OK;
  }

  // enclave might be longer that namespace length, check false positives and correct
  if (RMW_NAMESPACE_INVALID_TOO_LONG == tmp_validation_result) {
    if (RCL_ENCLAVE_NAME_MAX_LENGTH >= enclave_length) {
      *validation_result = RCL_ENCLAVE_NAME_VALID;
    } else {
      *validation_result = RCL_ENCLAVE_NAME_INVALID_TOO_LONG;
      if (invalid_index) {
        *invalid_index = RCL_ENCLAVE_NAME_MAX_LENGTH - 1;
      }
    }
    return RCL_RET_OK;
  }

  // everything was ok, set result to valid namespace, avoid setting invalid_index, and return
  *validation_result = RCL_ENCLAVE_NAME_VALID;
  return RCL_RET_OK;
}

const char *
rcl_enclave_name_validation_result_string(int validation_result)
{
  switch (validation_result) {
    case RCL_ENCLAVE_NAME_VALID:
      return NULL;
    case RCL_ENCLAVE_NAME_INVALID_IS_EMPTY_STRING:
      return "context name must not be empty";
    case RCL_ENCLAVE_NAME_INVALID_NOT_ABSOLUTE:
      return "context name must be absolute, it must lead with a '/'";
    case RCL_ENCLAVE_NAME_INVALID_ENDS_WITH_FORWARD_SLASH:
      return "context name must not end with a '/', unless only a '/'";
    case RCL_ENCLAVE_NAME_INVALID_CONTAINS_UNALLOWED_CHARACTERS:
      return "context name must not contain characters other than alphanumerics, '_', or '/'";
    case RCL_ENCLAVE_NAME_INVALID_CONTAINS_REPEATED_FORWARD_SLASH:
      return "context name must not contain repeated '/'";
    case RCL_ENCLAVE_NAME_INVALID_NAME_TOKEN_STARTS_WITH_NUMBER:
      return "context name must not have a token that starts with a number";
    case RCL_ENCLAVE_NAME_INVALID_TOO_LONG:
      return "context name should not exceed '"
             RCUTILS_STRINGIFY(RCL_ENCLAVE_NAME_MAX_NAME_LENGTH) "'";
    default:
      return "unknown result code for rcl context name validation";
  }
}
