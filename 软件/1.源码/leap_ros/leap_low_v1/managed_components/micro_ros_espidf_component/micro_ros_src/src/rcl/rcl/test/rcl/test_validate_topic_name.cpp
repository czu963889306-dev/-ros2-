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

#include <gtest/gtest.h>

#include <string>
#include <tuple>
#include <vector>

#include "rcl/validate_topic_name.h"

#include "rcl/error_handling.h"

TEST(test_validate_topic_name, normal) {
  rcl_ret_t ret;

  // passing without invalid_index
  {
    int validation_result;
    ret = rcl_validate_topic_name("topic", &validation_result, nullptr);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    EXPECT_EQ(RCL_TOPIC_NAME_VALID, validation_result);
    EXPECT_EQ(nullptr, rcl_topic_name_validation_result_string(validation_result));
  }

  // passing with invalid_index
  {
    int validation_result;
    size_t invalid_index = 42;
    ret = rcl_validate_topic_name("topic", &validation_result, &invalid_index);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    EXPECT_EQ(RCL_TOPIC_NAME_VALID, validation_result);
    EXPECT_EQ(42u, invalid_index);  // ensure invalid_index is not assigned on success
    EXPECT_EQ(nullptr, rcl_topic_name_validation_result_string(validation_result));
  }

  // failing with invalid_index
  {
    int validation_result;
    size_t invalid_index = 42;
    ret = rcl_validate_topic_name("", &validation_result, &invalid_index);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    EXPECT_EQ(RCL_TOPIC_NAME_INVALID_IS_EMPTY_STRING, validation_result);
    EXPECT_EQ(0u, invalid_index);
    EXPECT_NE(nullptr, rcl_topic_name_validation_result_string(validation_result));
  }
}

TEST(test_validate_topic_name, invalid_arguments) {
  rcl_ret_t ret;

  // pass null for topic string
  {
    int validation_result;
    ret = rcl_validate_topic_name(nullptr, &validation_result, nullptr);
    EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret);
    rcl_reset_error();
  }

  // pass null for validation_result
  {
    ret = rcl_validate_topic_name("topic", nullptr, nullptr);
    EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret);
    rcl_reset_error();
  }
}

TEST(test_validate_topic_name, various_valid_topics) {
  std::vector<std::string> topics_that_should_pass = {
    // examples from the design doc:
    //   http://design.ros2.org/articles/topic_and_service_names.html#ros-2-name-examples
    "foo",
    "abc123",
    "_foo",
    "Foo",
    "BAR",
    "~",
    "foo/bar",
    "~/foo",
    "{foo}_bar",
    "foo/{ping}/bar",
    "foo/_bar",
    "foo_/bar",
    "foo_",
    // these two are skipped because their prefixes should be removed before this is called
    // "rosservice:///foo",
    // "rostopic://foo/bar",
    "/foo",
    "/bar/baz",
    // same reason as above, URL should have been removed already
    // "rostopic:///ping",
    "/_private/thing",
    "/public_namespace/_private/thing",
    // these are further corner cases identified:
    "/foo",
    "{foo1}",
    "{foo_bar}",
    "{_bar}",
  };
  for (const auto & topic : topics_that_should_pass) {
    int validation_result;
    size_t invalid_index = 42;
    rcl_ret_t ret = rcl_validate_topic_name(topic.c_str(), &validation_result, &invalid_index);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    EXPECT_EQ(RCL_TOPIC_NAME_VALID, validation_result) <<
      "'" << topic << "' should have passed: " <<
      rcl_topic_name_validation_result_string(validation_result) << "\n" <<
      " " << std::string(invalid_index, ' ') << "^";
    EXPECT_EQ(42u, invalid_index);
    EXPECT_STREQ(nullptr, rcl_topic_name_validation_result_string(validation_result));
  }

  int not_valid_validation_result = 5600;
  EXPECT_STREQ(
    "unknown result code for rcl topic name validation",
    rcl_topic_name_validation_result_string(not_valid_validation_result));
}

TEST(test_validate_topic_name, various_invalid_topics) {
  struct topic_case
  {
    std::string topic;
    int expected_validation_result;
    size_t expected_invalid_index;
  };
  std::vector<topic_case> topic_cases_that_should_fail = {
    // examples from the design doc:
    //   http://design.ros2.org/articles/topic_and_service_names.html#ros-2-name-examples
    {"123abc", RCL_TOPIC_NAME_INVALID_NAME_TOKEN_STARTS_WITH_NUMBER, 0},
    {"123", RCL_TOPIC_NAME_INVALID_NAME_TOKEN_STARTS_WITH_NUMBER, 0},
    {" ", RCL_TOPIC_NAME_INVALID_CONTAINS_UNALLOWED_CHARACTERS, 0},
    {"foo bar", RCL_TOPIC_NAME_INVALID_CONTAINS_UNALLOWED_CHARACTERS, 3},
    // this one is skipped because it tested later, after expansion
    // "foo//bar",
    {"/~", RCL_TOPIC_NAME_INVALID_MISPLACED_TILDE, 1},
    {"~foo", RCL_TOPIC_NAME_INVALID_TILDE_NOT_FOLLOWED_BY_FORWARD_SLASH, 1},
    {"foo~", RCL_TOPIC_NAME_INVALID_MISPLACED_TILDE, 3},
    {"foo~/bar", RCL_TOPIC_NAME_INVALID_MISPLACED_TILDE, 3},
    {"foo/~bar", RCL_TOPIC_NAME_INVALID_MISPLACED_TILDE, 4},
    {"foo/~/bar", RCL_TOPIC_NAME_INVALID_MISPLACED_TILDE, 4},
    {"foo/", RCL_TOPIC_NAME_INVALID_ENDS_WITH_FORWARD_SLASH, 3},
    // these are further corner cases identified:
    {"", RCL_TOPIC_NAME_INVALID_IS_EMPTY_STRING, 0},
    {"foo/123bar", RCL_TOPIC_NAME_INVALID_NAME_TOKEN_STARTS_WITH_NUMBER, 4},
    {"foo/bar}/baz", RCL_TOPIC_NAME_INVALID_UNMATCHED_CURLY_BRACE, 7},
    {"foo/{bar", RCL_TOPIC_NAME_INVALID_UNMATCHED_CURLY_BRACE, 4},
    {"{$}", RCL_TOPIC_NAME_INVALID_SUBSTITUTION_CONTAINS_UNALLOWED_CHARACTERS, 1},
    {"{{bar}_baz}", RCL_TOPIC_NAME_INVALID_SUBSTITUTION_CONTAINS_UNALLOWED_CHARACTERS, 1},
    {"foo/{bar/baz}", RCL_TOPIC_NAME_INVALID_SUBSTITUTION_CONTAINS_UNALLOWED_CHARACTERS, 8},
    {"{1foo}", RCL_TOPIC_NAME_INVALID_SUBSTITUTION_STARTS_WITH_NUMBER, 1},
  };
  for (const auto & case_tuple : topic_cases_that_should_fail) {
    std::string topic = case_tuple.topic;
    int expected_validation_result = case_tuple.expected_validation_result;
    size_t expected_invalid_index = case_tuple.expected_invalid_index;
    int validation_result;
    size_t invalid_index = 0;
    rcl_ret_t ret = rcl_validate_topic_name(topic.c_str(), &validation_result, &invalid_index);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    EXPECT_EQ(expected_validation_result, validation_result) <<
      "'" << topic << "' should have failed with '" <<
      expected_validation_result << "' but got '" << validation_result << "'.\n" <<
      " " << std::string(invalid_index, ' ') << "^";
    EXPECT_EQ(expected_invalid_index, invalid_index) <<
      "Topic '" << topic << "' failed with '" << validation_result << "'.";
    EXPECT_NE(nullptr, rcl_topic_name_validation_result_string(validation_result)) << topic;
  }
}
