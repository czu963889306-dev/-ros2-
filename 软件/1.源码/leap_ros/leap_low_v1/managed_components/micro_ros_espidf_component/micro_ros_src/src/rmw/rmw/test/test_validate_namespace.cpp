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

#include <string>

#include "gmock/gmock.h"

#include "rmw/error_handling.h"
#include "rmw/validate_namespace.h"

TEST(test_validate_namespace, invalid_parameters) {
  int validation_result;
  size_t invalid_index;
  rmw_ret_t ret = rmw_validate_namespace(nullptr, &validation_result, &invalid_index);
  ASSERT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  ret = rmw_validate_namespace("/test", nullptr, &invalid_index);
  ASSERT_EQ(RMW_RET_INVALID_ARGUMENT, ret);

  // name is null pointer,
  ret = rmw_validate_namespace_with_size(nullptr, 0u, &validation_result, &invalid_index);
  ASSERT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();

  // Invalid validation result
  ASSERT_STREQ(
    "unknown result code for rmw namespace validation",
    rmw_namespace_validation_result_string(-1));
  rmw_reset_error();
}

TEST(test_validate_namespace, valid_namespace) {
  int validation_result;
  size_t invalid_index;
  rmw_ret_t ret;

  ret = rmw_validate_namespace("/", &validation_result, &invalid_index);
  ASSERT_EQ(RMW_RET_OK, ret);
  ASSERT_EQ(RMW_NAMESPACE_VALID, validation_result);

  validation_result = -1;
  ret = rmw_validate_namespace("/basename_only", &validation_result, &invalid_index);
  ASSERT_EQ(RMW_RET_OK, ret);
  ASSERT_EQ(RMW_NAMESPACE_VALID, validation_result);

  validation_result = -1;
  ret = rmw_validate_namespace("/with_one/heirarchy", &validation_result, &invalid_index);
  ASSERT_EQ(RMW_RET_OK, ret);
  ASSERT_EQ(RMW_NAMESPACE_VALID, validation_result);

  // with invalid_index as NULL
  validation_result = -1;
  ret = rmw_validate_namespace("/with_one/heirarchy", &validation_result, nullptr);
  ASSERT_EQ(RMW_RET_OK, ret);
  ASSERT_EQ(RMW_NAMESPACE_VALID, validation_result);

  ASSERT_EQ((char *)nullptr, rmw_namespace_validation_result_string(validation_result));
}

TEST(test_validate_namespace, empty_namespace) {
  int validation_result;
  size_t invalid_index;
  rmw_ret_t ret;

  ret = rmw_validate_namespace("", &validation_result, &invalid_index);
  ASSERT_EQ(RMW_RET_OK, ret);
  ASSERT_EQ(RMW_NAMESPACE_INVALID_IS_EMPTY_STRING, validation_result);
  ASSERT_EQ(0ul, invalid_index);

  // with invalid_index as NULL
  ret = rmw_validate_namespace("", &validation_result, nullptr);
  ASSERT_EQ(RMW_RET_OK, ret);
  ASSERT_EQ(RMW_NAMESPACE_INVALID_IS_EMPTY_STRING, validation_result);
  ASSERT_EQ(0ul, invalid_index);

  ASSERT_NE((char *)nullptr, rmw_namespace_validation_result_string(validation_result));
}

TEST(test_validate_namespace, not_absolute) {
  int validation_result;
  size_t invalid_index;
  rmw_ret_t ret;

  ret = rmw_validate_namespace("not_absolute", &validation_result, &invalid_index);
  ASSERT_EQ(RMW_RET_OK, ret);
  ASSERT_EQ(RMW_NAMESPACE_INVALID_NOT_ABSOLUTE, validation_result);
  ASSERT_EQ(0ul, invalid_index);

  ret = rmw_validate_namespace("not/absolute", &validation_result, &invalid_index);
  ASSERT_EQ(RMW_RET_OK, ret);
  ASSERT_EQ(RMW_NAMESPACE_INVALID_NOT_ABSOLUTE, validation_result);
  ASSERT_EQ(0ul, invalid_index);

  // with invalid_index as NULL
  ret = rmw_validate_namespace("not/absolute", &validation_result, nullptr);
  ASSERT_EQ(RMW_RET_OK, ret);
  ASSERT_EQ(RMW_NAMESPACE_INVALID_NOT_ABSOLUTE, validation_result);
  ASSERT_EQ(0ul, invalid_index);

  ASSERT_NE((char *)nullptr, rmw_namespace_validation_result_string(validation_result));
}

TEST(test_validate_namespace, ends_with_forward_slash) {
  int validation_result;
  size_t invalid_index;
  rmw_ret_t ret;

  ret = rmw_validate_namespace("/ends/with/", &validation_result, &invalid_index);
  ASSERT_EQ(RMW_RET_OK, ret);
  ASSERT_EQ(RMW_NAMESPACE_INVALID_ENDS_WITH_FORWARD_SLASH, validation_result);
  ASSERT_EQ(10ul, invalid_index);

  // with invalid_index as NULL
  ret = rmw_validate_namespace("/ends/with/", &validation_result, nullptr);
  ASSERT_EQ(RMW_RET_OK, ret);
  ASSERT_EQ(RMW_NAMESPACE_INVALID_ENDS_WITH_FORWARD_SLASH, validation_result);
  ASSERT_EQ(10ul, invalid_index);

  ASSERT_NE((char *)nullptr, rmw_namespace_validation_result_string(validation_result));
}

TEST(test_validate_namespace, unallowed_characters) {
  int validation_result;
  size_t invalid_index;
  rmw_ret_t ret;

  ret = rmw_validate_namespace("/~/unexpanded_tilde", &validation_result, &invalid_index);
  ASSERT_EQ(RMW_RET_OK, ret);
  ASSERT_EQ(RMW_NAMESPACE_INVALID_CONTAINS_UNALLOWED_CHARACTERS, validation_result);
  ASSERT_EQ(1ul, invalid_index);

  ret = rmw_validate_namespace("/unexpanded_sub/{node}", &validation_result, &invalid_index);
  ASSERT_EQ(RMW_RET_OK, ret);
  ASSERT_EQ(RMW_NAMESPACE_INVALID_CONTAINS_UNALLOWED_CHARACTERS, validation_result);
  ASSERT_EQ(16ul, invalid_index);

  ret = rmw_validate_namespace("/question?", &validation_result, &invalid_index);
  ASSERT_EQ(RMW_RET_OK, ret);
  ASSERT_EQ(RMW_NAMESPACE_INVALID_CONTAINS_UNALLOWED_CHARACTERS, validation_result);
  ASSERT_EQ(9ul, invalid_index);

  ret = rmw_validate_namespace("/with spaces", &validation_result, &invalid_index);
  ASSERT_EQ(RMW_RET_OK, ret);
  ASSERT_EQ(RMW_NAMESPACE_INVALID_CONTAINS_UNALLOWED_CHARACTERS, validation_result);
  ASSERT_EQ(5ul, invalid_index);

  // with invalid_index as NULL
  ret = rmw_validate_namespace("/with spaces", &validation_result, nullptr);
  ASSERT_EQ(RMW_RET_OK, ret);
  ASSERT_EQ(RMW_NAMESPACE_INVALID_CONTAINS_UNALLOWED_CHARACTERS, validation_result);
  ASSERT_EQ(5ul, invalid_index);

  ASSERT_NE((char *)nullptr, rmw_namespace_validation_result_string(validation_result));
}

TEST(test_validate_namespace, repeated_forward_slashes) {
  int validation_result;
  size_t invalid_index;
  rmw_ret_t ret;

  ret = rmw_validate_namespace("/repeated//slashes", &validation_result, &invalid_index);
  ASSERT_EQ(RMW_RET_OK, ret);
  ASSERT_EQ(RMW_NAMESPACE_INVALID_CONTAINS_REPEATED_FORWARD_SLASH, validation_result);
  ASSERT_EQ(10ul, invalid_index);

  // with invalid_index as NULL
  ret = rmw_validate_namespace("/repeated//slashes", &validation_result, nullptr);
  ASSERT_EQ(RMW_RET_OK, ret);
  ASSERT_EQ(RMW_NAMESPACE_INVALID_CONTAINS_REPEATED_FORWARD_SLASH, validation_result);
  ASSERT_EQ(10ul, invalid_index);

  ASSERT_NE((char *)nullptr, rmw_namespace_validation_result_string(validation_result));
}

TEST(test_validate_namespace, starts_with_number) {
  int validation_result;
  size_t invalid_index;
  rmw_ret_t ret;

  ret = rmw_validate_namespace("/9starts_with_number", &validation_result, &invalid_index);
  ASSERT_EQ(RMW_RET_OK, ret);
  ASSERT_EQ(RMW_NAMESPACE_INVALID_NAME_TOKEN_STARTS_WITH_NUMBER, validation_result);
  ASSERT_EQ(1ul, invalid_index);

  ret = rmw_validate_namespace("/starts/42with/number", &validation_result, &invalid_index);
  ASSERT_EQ(RMW_RET_OK, ret);
  ASSERT_EQ(RMW_NAMESPACE_INVALID_NAME_TOKEN_STARTS_WITH_NUMBER, validation_result);
  ASSERT_EQ(8ul, invalid_index);

  // with invalid_index as NULL
  ret = rmw_validate_namespace("/starts/42with/number", &validation_result, nullptr);
  ASSERT_EQ(RMW_RET_OK, ret);
  ASSERT_EQ(RMW_NAMESPACE_INVALID_NAME_TOKEN_STARTS_WITH_NUMBER, validation_result);
  ASSERT_EQ(8ul, invalid_index);

  ASSERT_NE((char *)nullptr, rmw_namespace_validation_result_string(validation_result));
}

TEST(test_validate_namespace, topic_too_long) {
  int validation_result;
  size_t invalid_index;
  rmw_ret_t ret;

  // Ensure the length is not the first error
  std::string invalid_and_long_topic(RMW_NAMESPACE_MAX_LENGTH + 1, 'a');
  ret = rmw_validate_namespace(
    invalid_and_long_topic.c_str(), &validation_result, &invalid_index);
  ASSERT_EQ(RMW_RET_OK, ret);
  ASSERT_EQ(RMW_NAMESPACE_INVALID_NOT_ABSOLUTE, validation_result);
  ASSERT_EQ(0ul, invalid_index);

  // with invalid_index as NULL
  ret = rmw_validate_namespace(
    invalid_and_long_topic.c_str(), &validation_result, nullptr);
  ASSERT_EQ(RMW_RET_OK, ret);
  ASSERT_EQ(RMW_NAMESPACE_INVALID_NOT_ABSOLUTE, validation_result);
  ASSERT_EQ(0ul, invalid_index);

  std::string valid_but_long_topic = "/" + invalid_and_long_topic;
  ret = rmw_validate_namespace(
    valid_but_long_topic.c_str(), &validation_result, &invalid_index);
  ASSERT_EQ(RMW_RET_OK, ret);
  EXPECT_EQ(RMW_NAMESPACE_INVALID_TOO_LONG, validation_result);
  EXPECT_EQ(RMW_NAMESPACE_MAX_LENGTH - 1, invalid_index);

  // with invalid_index as NULL
  ret = rmw_validate_namespace(
    valid_but_long_topic.c_str(), &validation_result, nullptr);
  ASSERT_EQ(RMW_RET_OK, ret);
  EXPECT_EQ(RMW_NAMESPACE_INVALID_TOO_LONG, validation_result);
  EXPECT_EQ(RMW_NAMESPACE_MAX_LENGTH - 1, invalid_index);

  ASSERT_NE((char *)nullptr, rmw_namespace_validation_result_string(validation_result));
}
