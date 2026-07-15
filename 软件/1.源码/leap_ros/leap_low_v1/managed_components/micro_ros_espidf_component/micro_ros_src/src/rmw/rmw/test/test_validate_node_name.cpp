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
#include "rmw/validate_node_name.h"

TEST(test_validate_node_name, invalid_parameters) {
  int validation_result;
  size_t invalid_index;
  rmw_ret_t ret = rmw_validate_node_name(nullptr, &validation_result, &invalid_index);
  ASSERT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  ret = rmw_validate_node_name("test", nullptr, &invalid_index);
  ASSERT_EQ(RMW_RET_INVALID_ARGUMENT, ret);

  // name is null pointer,
  ret = rmw_validate_node_name_with_size(nullptr, 0u, &validation_result, &invalid_index);
  ASSERT_EQ(RMW_RET_INVALID_ARGUMENT, ret);
  rmw_reset_error();

  // Invalid validation result
  ASSERT_STREQ(
    "unknown result code for rmw node name validation",
    rmw_node_name_validation_result_string(-1));
  rmw_reset_error();
}

TEST(test_validate_node_name, valid_node_name) {
  int validation_result;
  size_t invalid_index;
  rmw_ret_t ret;

  ret = rmw_validate_node_name("nodename", &validation_result, &invalid_index);
  ASSERT_EQ(RMW_RET_OK, ret);
  ASSERT_EQ(RMW_NODE_NAME_VALID, validation_result);

  validation_result = -1;
  ret = rmw_validate_node_name("node_name", &validation_result, &invalid_index);
  ASSERT_EQ(RMW_RET_OK, ret);
  ASSERT_EQ(RMW_NODE_NAME_VALID, validation_result);

  // with invalid_index as NULL
  validation_result = -1;
  ret = rmw_validate_node_name("node_name", &validation_result, nullptr);
  ASSERT_EQ(RMW_RET_OK, ret);
  ASSERT_EQ(RMW_NODE_NAME_VALID, validation_result);

  ASSERT_EQ((char *)nullptr, rmw_node_name_validation_result_string(validation_result));
}

TEST(test_validate_node_name, empty_node_name) {
  int validation_result;
  size_t invalid_index;
  rmw_ret_t ret;

  ret = rmw_validate_node_name("", &validation_result, &invalid_index);
  ASSERT_EQ(RMW_RET_OK, ret);
  ASSERT_EQ(RMW_NODE_NAME_INVALID_IS_EMPTY_STRING, validation_result);
  ASSERT_EQ(0ul, invalid_index);

  // with invalid_index as NULL
  ret = rmw_validate_node_name("", &validation_result, nullptr);
  ASSERT_EQ(RMW_RET_OK, ret);
  ASSERT_EQ(RMW_NODE_NAME_INVALID_IS_EMPTY_STRING, validation_result);
  ASSERT_EQ(0ul, invalid_index);

  ASSERT_NE((char *)nullptr, rmw_node_name_validation_result_string(validation_result));
}

TEST(test_validate_node_name, unallowed_characters) {
  int validation_result;
  size_t invalid_index;
  rmw_ret_t ret;

  ret = rmw_validate_node_name("node/name", &validation_result, &invalid_index);
  ASSERT_EQ(RMW_RET_OK, ret);
  ASSERT_EQ(RMW_NODE_NAME_INVALID_CONTAINS_UNALLOWED_CHARACTERS, validation_result);
  ASSERT_EQ(4ul, invalid_index);

  ret = rmw_validate_node_name("node_{name}", &validation_result, &invalid_index);
  ASSERT_EQ(RMW_RET_OK, ret);
  ASSERT_EQ(RMW_NODE_NAME_INVALID_CONTAINS_UNALLOWED_CHARACTERS, validation_result);
  ASSERT_EQ(5ul, invalid_index);

  ret = rmw_validate_node_name("~node_name", &validation_result, &invalid_index);
  ASSERT_EQ(RMW_RET_OK, ret);
  ASSERT_EQ(RMW_NODE_NAME_INVALID_CONTAINS_UNALLOWED_CHARACTERS, validation_result);
  ASSERT_EQ(0ul, invalid_index);

  ret = rmw_validate_node_name("with spaces", &validation_result, &invalid_index);
  ASSERT_EQ(RMW_RET_OK, ret);
  ASSERT_EQ(RMW_NODE_NAME_INVALID_CONTAINS_UNALLOWED_CHARACTERS, validation_result);
  ASSERT_EQ(4ul, invalid_index);

  ret = rmw_validate_node_name("with.periods", &validation_result, &invalid_index);
  ASSERT_EQ(RMW_RET_OK, ret);
  ASSERT_EQ(RMW_NODE_NAME_INVALID_CONTAINS_UNALLOWED_CHARACTERS, validation_result);
  ASSERT_EQ(4ul, invalid_index);

  // with invalid_index as NULL
  ret = rmw_validate_node_name("with.periods", &validation_result, nullptr);
  ASSERT_EQ(RMW_RET_OK, ret);
  ASSERT_EQ(RMW_NODE_NAME_INVALID_CONTAINS_UNALLOWED_CHARACTERS, validation_result);
  ASSERT_EQ(4ul, invalid_index);

  ASSERT_NE((char *)nullptr, rmw_node_name_validation_result_string(validation_result));
}

TEST(test_validate_node_name, starts_with_number) {
  int validation_result;
  size_t invalid_index;
  rmw_ret_t ret;

  ret = rmw_validate_node_name("42node", &validation_result, &invalid_index);
  ASSERT_EQ(RMW_RET_OK, ret);
  ASSERT_EQ(RMW_NODE_NAME_INVALID_STARTS_WITH_NUMBER, validation_result);
  ASSERT_EQ(0ul, invalid_index);

  // with invalid_index as NULL
  ret = rmw_validate_node_name("42node", &validation_result, nullptr);
  ASSERT_EQ(RMW_RET_OK, ret);
  ASSERT_EQ(RMW_NODE_NAME_INVALID_STARTS_WITH_NUMBER, validation_result);
  ASSERT_EQ(0ul, invalid_index);

  ASSERT_NE((char *)nullptr, rmw_node_name_validation_result_string(validation_result));
}

TEST(test_validate_node_name, node_name_too_long) {
  int validation_result;
  size_t invalid_index;
  rmw_ret_t ret;

  // Ensure the length is not the first error
  std::string invalid_and_long_node_name(RMW_NODE_NAME_MAX_NAME_LENGTH + 1, '0');
  ret = rmw_validate_node_name(
    invalid_and_long_node_name.c_str(), &validation_result, &invalid_index);
  ASSERT_EQ(RMW_RET_OK, ret);
  ASSERT_EQ(RMW_NODE_NAME_INVALID_STARTS_WITH_NUMBER, validation_result);
  ASSERT_EQ(0ul, invalid_index);

  // with invalid_index as NULL
  ret = rmw_validate_node_name(
    invalid_and_long_node_name.c_str(), &validation_result, nullptr);
  ASSERT_EQ(RMW_RET_OK, ret);
  ASSERT_EQ(RMW_NODE_NAME_INVALID_STARTS_WITH_NUMBER, validation_result);
  ASSERT_EQ(0ul, invalid_index);

  // Ensure length check works when there are no other issues
  std::string valid_but_long_node_name(RMW_NODE_NAME_MAX_NAME_LENGTH + 1, 'a');
  ret = rmw_validate_node_name(
    valid_but_long_node_name.c_str(), &validation_result, &invalid_index);
  ASSERT_EQ(RMW_RET_OK, ret);
  EXPECT_EQ(RMW_NODE_NAME_INVALID_TOO_LONG, validation_result);
  EXPECT_EQ(RMW_NODE_NAME_MAX_NAME_LENGTH - 1U, invalid_index);

  // with invalid_index as NULL
  ret = rmw_validate_node_name(
    valid_but_long_node_name.c_str(), &validation_result, nullptr);
  ASSERT_EQ(RMW_RET_OK, ret);
  EXPECT_EQ(RMW_NODE_NAME_INVALID_TOO_LONG, validation_result);
  EXPECT_EQ(RMW_NODE_NAME_MAX_NAME_LENGTH - 1U, invalid_index);

  ASSERT_NE((char *)nullptr, rmw_node_name_validation_result_string(validation_result));
}
