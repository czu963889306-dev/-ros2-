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

#include "rcl/validate_topic_name.h"

#include <ctype.h>
#include <string.h>

#include "rcl/allocator.h"
#include "rcl/error_handling.h"
#include "rcutils/isalnum_no_locale.h"

rcl_ret_t
rcl_validate_topic_name(
  const char * topic_name,
  int * validation_result,
  size_t * invalid_index)
{
  RCL_CHECK_ARGUMENT_FOR_NULL(topic_name, RCL_RET_INVALID_ARGUMENT);
  return rcl_validate_topic_name_with_size(
    topic_name, strlen(topic_name), validation_result, invalid_index);
}

rcl_ret_t
rcl_validate_topic_name_with_size(
  const char * topic_name,
  size_t topic_name_length,
  int * validation_result,
  size_t * invalid_index)
{
  RCL_CHECK_ARGUMENT_FOR_NULL(topic_name, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(validation_result, RCL_RET_INVALID_ARGUMENT);

  if (topic_name_length == 0) {
    *validation_result = RCL_TOPIC_NAME_INVALID_IS_EMPTY_STRING;
    if (invalid_index) {
      *invalid_index = 0;
    }
    return RCL_RET_OK;
  }
  // check that the first character is not a number
  if (isdigit(topic_name[0]) != 0) {
    // this is the case where the topic is relative and the first token starts with a number
    // e.g. 7foo/bar is invalid
    *validation_result = RCL_TOPIC_NAME_INVALID_NAME_TOKEN_STARTS_WITH_NUMBER;
    if (invalid_index) {
      *invalid_index = 0;
    }
    return RCL_RET_OK;
  }
  // note topic_name_length is >= 1 at this point
  if (topic_name[topic_name_length - 1] == '/') {
    // catches both "/foo/" and "/"
    *validation_result = RCL_TOPIC_NAME_INVALID_ENDS_WITH_FORWARD_SLASH;
    if (invalid_index) {
      *invalid_index = topic_name_length - 1;
    }
    return RCL_RET_OK;
  }
  // check for unallowed characters, nested and unmatched {} too
  bool in_open_curly_brace = false;
  size_t opening_curly_brace_index = 0;
  for (size_t i = 0; i < topic_name_length; ++i) {
    if (rcutils_isalnum_no_locale(topic_name[i])) {
      // if within curly braces and the first character is a number, error
      // e.g. foo/{4bar} is invalid
      if (
        isdigit(topic_name[i]) != 0 &&
        in_open_curly_brace &&
        i > 0 &&
        (i - 1 == opening_curly_brace_index))
      {
        *validation_result = RCL_TOPIC_NAME_INVALID_SUBSTITUTION_STARTS_WITH_NUMBER;
        if (invalid_index) {
          *invalid_index = i;
        }
        return RCL_RET_OK;
      }
      // if it is an alpha numeric character, i.e. [0-9|A-Z|a-z], continue
      continue;
    } else if (topic_name[i] == '_') {
      // if it is an underscore, continue
      continue;
    } else if (topic_name[i] == '/') {
      // if it is a forward slash within {}, error
      if (in_open_curly_brace) {
        *validation_result = RCL_TOPIC_NAME_INVALID_SUBSTITUTION_CONTAINS_UNALLOWED_CHARACTERS;
        if (invalid_index) {
          *invalid_index = i;
        }
        return RCL_RET_OK;
      }
      // if it is a forward slash outside of {}, continue
      continue;
    } else if (topic_name[i] == '~') {
      // if it is a tilde not in the first position, validation fails
      if (i != 0) {
        *validation_result = RCL_TOPIC_NAME_INVALID_MISPLACED_TILDE;
        if (invalid_index) {
          *invalid_index = i;
        }
        return RCL_RET_OK;
      }
      // if it is a tilde in the first position, continue
      continue;
    } else if (topic_name[i] == '{') {
      opening_curly_brace_index = i;
      // if starting a nested curly brace, error
      // e.g. foo/{{bar}_baz} is invalid
      //           ^
      if (in_open_curly_brace) {
        *validation_result = RCL_TOPIC_NAME_INVALID_SUBSTITUTION_CONTAINS_UNALLOWED_CHARACTERS;
        if (invalid_index) {
          *invalid_index = i;
        }
        return RCL_RET_OK;
      }
      in_open_curly_brace = true;
      // if it is a new, open curly brace, continue
      continue;
    } else if (topic_name[i] == '}') {
      // if not preceded by a {, error
      if (!in_open_curly_brace) {
        *validation_result = RCL_TOPIC_NAME_INVALID_UNMATCHED_CURLY_BRACE;
        if (invalid_index) {
          *invalid_index = i;
        }
        return RCL_RET_OK;
      }
      in_open_curly_brace = false;
      // if it is a closing curly brace, continue
      continue;
    } else {
      // if it is none of these, then it is an unallowed character in a topic name
      if (in_open_curly_brace) {
        *validation_result = RCL_TOPIC_NAME_INVALID_SUBSTITUTION_CONTAINS_UNALLOWED_CHARACTERS;
      } else {
        *validation_result = RCL_TOPIC_NAME_INVALID_CONTAINS_UNALLOWED_CHARACTERS;
      }
      if (invalid_index) {
        *invalid_index = i;
      }
      return RCL_RET_OK;
    }
  }
  // check to make sure substitutions were properly closed
  if (in_open_curly_brace) {
    // case where a substitution is never closed, e.g. 'foo/{bar'
    *validation_result = RCL_TOPIC_NAME_INVALID_UNMATCHED_CURLY_BRACE;
    if (invalid_index) {
      *invalid_index = opening_curly_brace_index;
    }
    return RCL_RET_OK;
  }
  // check for tokens (other than the first) that start with a number
  for (size_t i = 0; i < topic_name_length; ++i) {
    if (i == topic_name_length - 1) {
      // if this is the last character, then nothing to check
      continue;
    }
    // past this point, assuming i+1 is a valid index
    if (topic_name[i] == '/') {
      if (isdigit(topic_name[i + 1]) != 0) {
        // this is the case where a '/' if followed by a number, i.e. [0-9]
        *validation_result = RCL_TOPIC_NAME_INVALID_NAME_TOKEN_STARTS_WITH_NUMBER;
        if (invalid_index) {
          *invalid_index = i + 1;
        }
        return RCL_RET_OK;
      }
    } else if (i == 1 && topic_name[0] == '~') {
      // special case where first character is ~ but second character is not /
      // e.g. ~foo is invalid
      *validation_result = RCL_TOPIC_NAME_INVALID_TILDE_NOT_FOLLOWED_BY_FORWARD_SLASH;
      if (invalid_index) {
        *invalid_index = 1;
      }
      return RCL_RET_OK;
    }
  }
  // everything was ok, set result to valid topic, avoid setting invalid_index, and return
  *validation_result = RCL_TOPIC_NAME_VALID;
  return RCL_RET_OK;
}

const char *
rcl_topic_name_validation_result_string(int validation_result)
{
  switch (validation_result) {
    case RCL_TOPIC_NAME_VALID:
      return NULL;
    case RCL_TOPIC_NAME_INVALID_IS_EMPTY_STRING:
      return "topic name must not be empty string";
    case RCL_TOPIC_NAME_INVALID_ENDS_WITH_FORWARD_SLASH:
      return "topic name must not end with a forward slash";
    case RCL_TOPIC_NAME_INVALID_CONTAINS_UNALLOWED_CHARACTERS:
      return
        "topic name must not contain characters other than alphanumerics, '_', '~', '{', or '}'";
    case RCL_TOPIC_NAME_INVALID_NAME_TOKEN_STARTS_WITH_NUMBER:
      return "topic name token must not start with a number";
    case RCL_TOPIC_NAME_INVALID_UNMATCHED_CURLY_BRACE:
      return "topic name must not have unmatched (unbalanced) curly braces '{}'";
    case RCL_TOPIC_NAME_INVALID_MISPLACED_TILDE:
      return "topic name must not have tilde '~' unless it is the first character";
    case RCL_TOPIC_NAME_INVALID_TILDE_NOT_FOLLOWED_BY_FORWARD_SLASH:
      return "topic name must not have a tilde '~' that is not followed by a forward slash '/'";
    case RCL_TOPIC_NAME_INVALID_SUBSTITUTION_CONTAINS_UNALLOWED_CHARACTERS:
      return "substitution name must not contain characters other than alphanumerics or '_'";
    case RCL_TOPIC_NAME_INVALID_SUBSTITUTION_STARTS_WITH_NUMBER:
      return "substitution name must not start with a number";
    default:
      return "unknown result code for rcl topic name validation";
  }
}

#ifdef __cplusplus
}
#endif
