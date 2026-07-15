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

#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "rcutils/snprintf.h"

#include "rcl/rcl.h"
#include "rcl/validate_enclave_name.h"

#include "rcl/error_handling.h"

#include "rmw/validate_namespace.h"

#include "../mocking_utils/patch.hpp"

TEST(TestValidateEnclaveName, test_validate) {
  int validation_result;
  size_t invalid_index;

  EXPECT_EQ(
    RMW_RET_INVALID_ARGUMENT,
    rcl_validate_enclave_name(nullptr, &validation_result, &invalid_index));

  EXPECT_EQ(
    RMW_RET_INVALID_ARGUMENT,
    rcl_validate_enclave_name_with_size(nullptr, 20, &validation_result, &invalid_index));

  EXPECT_EQ(
    RMW_RET_INVALID_ARGUMENT,
    rcl_validate_enclave_name_with_size("/foo", 20, nullptr, &invalid_index));

  EXPECT_EQ(
    RCL_RET_OK,
    rcl_validate_enclave_name("/foo", &validation_result, &invalid_index));
  EXPECT_EQ(RCL_ENCLAVE_NAME_VALID, validation_result);

  EXPECT_EQ(
    RCL_RET_OK,
    rcl_validate_enclave_name("/foo/bar", &validation_result, &invalid_index));
  EXPECT_EQ(RCL_ENCLAVE_NAME_VALID, validation_result);

  {
    auto mock = mocking_utils::patch(
      "lib:rcl", rmw_validate_namespace_with_size,
      [&](auto, auto, int * result, size_t * index) {
        if (index) {
          *index = 0u;
        }
        *result = RMW_NAMESPACE_INVALID_TOO_LONG;
        return RMW_RET_OK;
      });

    // When applying RMW namespace validation rules, an enclave name may be too
    // long for an RMW namespace but not necessarily for an enclave name.
    EXPECT_EQ(
      RCL_RET_OK,
      rcl_validate_enclave_name("/foo/baz", &validation_result, &invalid_index));
    EXPECT_EQ(RCL_ENCLAVE_NAME_VALID, validation_result);
  }
}

TEST(TestValidateEnclaveName, test_validate_on_internal_error) {
  int validation_result;
  size_t invalid_index;

  {
    auto mock = mocking_utils::patch_to_fail(
      "lib:rcl", rmw_validate_namespace_with_size, "internal error", RMW_RET_ERROR);

    EXPECT_EQ(
      RCL_RET_ERROR,
      rcl_validate_enclave_name("/foo", &validation_result, &invalid_index));
    EXPECT_TRUE(rcl_error_is_set());
    rcl_reset_error();
  }

  {
    auto mock = mocking_utils::patch(
      "lib:rcl", rmw_validate_namespace_with_size,
      [&](auto, auto, int * result, size_t * index) {
        if (index) {
          *index = 0u;
        }
        *result = -1;
        return RMW_RET_OK;
      });

    EXPECT_EQ(
      RCL_RET_ERROR,
      rcl_validate_enclave_name("/foo", &validation_result, &invalid_index));
    EXPECT_TRUE(rcl_error_is_set());
    rcl_reset_error();
  }
}

TEST(TestValidateEnclaveName, test_validation_string) {
  struct enclave_case
  {
    std::string enclave;
    int expected_validation_result;
    size_t expected_invalid_index;
  };
  std::vector<enclave_case> enclave_cases_that_should_fail = {
    // TODO(blast_545): Look for naming conventions doc for enclave_names
    {"", RCL_ENCLAVE_NAME_INVALID_IS_EMPTY_STRING, 0},
    {"~/foo", RCL_ENCLAVE_NAME_INVALID_NOT_ABSOLUTE, 0},
    {"/foo/", RCL_ENCLAVE_NAME_INVALID_ENDS_WITH_FORWARD_SLASH, 4},
    {"/foo/$", RCL_ENCLAVE_NAME_INVALID_CONTAINS_UNALLOWED_CHARACTERS, 5},
    {"/bar#", RCL_ENCLAVE_NAME_INVALID_CONTAINS_UNALLOWED_CHARACTERS, 4},
    {"/foo//bar", RCL_ENCLAVE_NAME_INVALID_CONTAINS_REPEATED_FORWARD_SLASH, 5},
    {"/1bar", RCL_ENCLAVE_NAME_INVALID_NAME_TOKEN_STARTS_WITH_NUMBER, 1},
    {"/" + std::string(RCL_ENCLAVE_NAME_MAX_LENGTH, 'o'),
      RCL_ENCLAVE_NAME_INVALID_TOO_LONG,
      RCL_ENCLAVE_NAME_MAX_LENGTH - 1}
  };
  for (const auto & case_tuple : enclave_cases_that_should_fail) {
    std::string enclave = case_tuple.enclave;
    int expected_validation_result = case_tuple.expected_validation_result;
    size_t expected_invalid_index = case_tuple.expected_invalid_index;
    int validation_result;
    size_t invalid_index = 0;
    rcl_ret_t ret = rcl_validate_enclave_name(enclave.c_str(), &validation_result, &invalid_index);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    EXPECT_EQ(expected_validation_result, validation_result) <<
      "'" << enclave << "' should have failed with '" <<
      expected_validation_result << "' but got '" << validation_result << "'.\n" <<
      " " << std::string(invalid_index, ' ') << "^";
    EXPECT_EQ(expected_invalid_index, invalid_index) <<
      "Enclave '" << enclave << "' failed with '" << validation_result << "'.";
    EXPECT_NE(nullptr, rcl_enclave_name_validation_result_string(validation_result)) << enclave;
  }
  EXPECT_STREQ(
    "unknown result code for rcl context name validation",
    rcl_enclave_name_validation_result_string(-1));  // invalid result
  EXPECT_EQ(nullptr, rcl_enclave_name_validation_result_string(RCL_ENCLAVE_NAME_VALID));
}
