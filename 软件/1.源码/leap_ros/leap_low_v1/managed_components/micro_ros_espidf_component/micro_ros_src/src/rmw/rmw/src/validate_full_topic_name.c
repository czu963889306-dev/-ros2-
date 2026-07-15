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

#include <rmw/validate_full_topic_name.h>

#include <ctype.h>
#include <string.h>

#include "rcutils/isalnum_no_locale.h"

rmw_ret_t
rmw_validate_full_topic_name(
  const char * topic_name,
  int * validation_result,
  size_t * invalid_index)
{
  if (!topic_name) {
    return RMW_RET_INVALID_ARGUMENT;
  }
  return rmw_validate_full_topic_name_with_size(
    topic_name, strlen(topic_name), validation_result, invalid_index);
}

rmw_ret_t
rmw_validate_full_topic_name_with_size(
  const char * topic_name,
  size_t topic_name_length,
  int * validation_result,
  size_t * invalid_index)
{
  if (!topic_name) {
    return RMW_RET_INVALID_ARGUMENT;
  }
  if (!validation_result) {
    return RMW_RET_INVALID_ARGUMENT;
  }
  if (topic_name_length == 0) {
    *validation_result = RMW_TOPIC_INVALID_IS_EMPTY_STRING;
    if (invalid_index) {
      *invalid_index = 0;
    }
    return RMW_RET_OK;
  }
  if (topic_name[0] != '/') {
    *validation_result = RMW_TOPIC_INVALID_NOT_ABSOLUTE;
    if (invalid_index) {
      *invalid_index = 0;
    }
    return RMW_RET_OK;
  }
  // note topic_name_length is >= 1 at this point
  if (topic_name[topic_name_length - 1] == '/') {
    // catches both "/foo/" and "/"
    *validation_result = RMW_TOPIC_INVALID_ENDS_WITH_FORWARD_SLASH;
    if (invalid_index) {
      *invalid_index = topic_name_length - 1;
    }
    return RMW_RET_OK;
  }
  // check for unallowed characters
  for (size_t i = 0; i < topic_name_length; ++i) {
    if (rcutils_isalnum_no_locale(topic_name[i])) {
      // if it is an alpha numeric character, i.e. [0-9|A-Z|a-z], continue
      continue;
    } else if (topic_name[i] == '_') {
      // if it is an underscore, continue
      continue;
    } else if (topic_name[i] == '/') {
      // if it is a forward slash, continue
      continue;
    } else {
      // if it is none of these, then it is an unallowed character in a FQN topic name
      *validation_result = RMW_TOPIC_INVALID_CONTAINS_UNALLOWED_CHARACTERS;
      if (invalid_index) {
        *invalid_index = i;
      }
      return RMW_RET_OK;
    }
  }
  // check for double '/' and tokens that start with a number
  for (size_t i = 0; i < topic_name_length; ++i) {
    if (i == topic_name_length - 1) {
      // if this is the last character, then nothing to check
      continue;
    }
    // past this point, assuming i+1 is a valid index
    if (topic_name[i] == '/') {
      if (topic_name[i + 1] == '/') {
        *validation_result = RMW_TOPIC_INVALID_CONTAINS_REPEATED_FORWARD_SLASH;
        if (invalid_index) {
          *invalid_index = i + 1;
        }
        return RMW_RET_OK;
      }
      if (isdigit(topic_name[i + 1]) != 0) {
        // this is the case where a '/' if followed by a number, i.e. [0-9]
        *validation_result = RMW_TOPIC_INVALID_NAME_TOKEN_STARTS_WITH_NUMBER;
        if (invalid_index) {
          *invalid_index = i + 1;
        }
        return RMW_RET_OK;
      }
    }
  }
  // check if the topic name is too long last, since it might be a soft invalidation
  if (topic_name_length > RMW_TOPIC_MAX_NAME_LENGTH) {
    *validation_result = RMW_TOPIC_INVALID_TOO_LONG;
    if (invalid_index) {
      *invalid_index = RMW_TOPIC_MAX_NAME_LENGTH - 1;
    }
    return RMW_RET_OK;
  }
  // everything was ok, set result to valid topic, avoid setting invalid_index, and return
  *validation_result = RMW_TOPIC_VALID;
  return RMW_RET_OK;
}

const char *
rmw_full_topic_name_validation_result_string(int validation_result)
{
  switch (validation_result) {
    case RMW_TOPIC_VALID:
      return NULL;
    case RMW_TOPIC_INVALID_IS_EMPTY_STRING:
      return "topic name must not be empty";
    case RMW_TOPIC_INVALID_NOT_ABSOLUTE:
      return "topic name must be absolute, it must lead with a '/'";
    case RMW_TOPIC_INVALID_ENDS_WITH_FORWARD_SLASH:
      return "topic name must not end with a '/'";
    case RMW_TOPIC_INVALID_CONTAINS_UNALLOWED_CHARACTERS:
      return "topic name must not contain characters other than alphanumerics, '_', or '/'";
    case RMW_TOPIC_INVALID_CONTAINS_REPEATED_FORWARD_SLASH:
      return "topic name must not contain repeated '/'";
    case RMW_TOPIC_INVALID_NAME_TOKEN_STARTS_WITH_NUMBER:
      return "topic name must not have a token that starts with a number";
    case RMW_TOPIC_INVALID_TOO_LONG:
      return "topic length should not exceed '" RMW_STRINGIFY(RMW_TOPIC_MAX_NAME_LENGTH) "'";
    default:
      return "unknown result code for rwm topic name validation";
  }
}
