// Copyright 2018 Open Source Robotics Foundation, Inc.
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

#include <iostream>

#include "rcl_action/names.h"

#include "rcl/allocator.h"
#include "rcl/error_handling.h"
#include "rcl/types.h"

struct ActionDerivedNameTestSubject
{
  const char * action_name;
  const char * expected_action_derived_name;
  rcl_ret_t (* get_action_derived_name)(const char *, rcl_allocator_t, char **);
  const char * subject_name;
};

void * bad_malloc(size_t, void *)
{
  return NULL;
}

std::ostream & operator<<(std::ostream & os, const ActionDerivedNameTestSubject & test_subject)
{
  return os << test_subject.subject_name;
}

class TestActionDerivedName
  : public ::testing::TestWithParam<ActionDerivedNameTestSubject>
{
protected:
  void SetUp() override
  {
    test_subject = GetParam();
  }

  ActionDerivedNameTestSubject test_subject;
};

TEST_P(TestActionDerivedName, validate_action_derived_getter)
{
  rcl_allocator_t default_allocator = rcl_get_default_allocator();

  char * action_derived_name = NULL;
  const char * const null_action_name = NULL;
  rcl_ret_t ret = test_subject.get_action_derived_name(
    null_action_name, default_allocator,
    &action_derived_name);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret);
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  action_derived_name = NULL;
  const char * const invalid_action_name = "";
  ret = test_subject.get_action_derived_name(
    invalid_action_name, default_allocator,
    &action_derived_name);
  EXPECT_EQ(RCL_RET_ACTION_NAME_INVALID, ret) << rcl_get_error_string().str;
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  action_derived_name = NULL;
  rcl_allocator_t invalid_allocator =
    (rcl_allocator_t)rcutils_get_zero_initialized_allocator();
  ret = test_subject.get_action_derived_name(
    test_subject.action_name, invalid_allocator,
    &action_derived_name);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret) << rcl_get_error_string().str;
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  action_derived_name = NULL;
  char ** invalid_ptr_to_action_derived_name = NULL;
  ret = test_subject.get_action_derived_name(
    test_subject.action_name, default_allocator,
    invalid_ptr_to_action_derived_name);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret) << rcl_get_error_string().str;
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  char dummy_char = '\0';
  action_derived_name = &dummy_char;
  ret = test_subject.get_action_derived_name(
    test_subject.action_name, default_allocator,
    &action_derived_name);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret) << rcl_get_error_string().str;
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  action_derived_name = NULL;
  ret = test_subject.get_action_derived_name(
    test_subject.action_name, default_allocator,
    &action_derived_name);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  EXPECT_STREQ(test_subject.expected_action_derived_name, action_derived_name);
  default_allocator.deallocate(action_derived_name, default_allocator.state);

  rcl_allocator_t failing_allocator = rcl_get_default_allocator();
  failing_allocator.allocate = bad_malloc;
  action_derived_name = NULL;
  ret = test_subject.get_action_derived_name(
    test_subject.action_name, failing_allocator,
    &action_derived_name);
  EXPECT_EQ(RCL_RET_BAD_ALLOC, ret) << rcl_get_error_string().str;
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();
}

const ActionDerivedNameTestSubject action_service_and_topic_subjects[] = {
  {
    "test_it",                         // action_name
    "test_it/_action/send_goal",       // expected_action_derived_name
    rcl_action_get_goal_service_name,  // get_action_derived_name
    "goal_service_name_test"           // subject_name
  },
  {
    "test_it",                           // action_name
    "test_it/_action/cancel_goal",       // expected_action_derived_name
    rcl_action_get_cancel_service_name,  // get_action_derived_name
    "cancel_service_name_test"           // subject_name
  },
  {
    "test_it",                           // action_name
    "test_it/_action/get_result",        // expected_action_derived_name
    rcl_action_get_result_service_name,  // get_action_derived_name
    "result_service_name_test"           // subject_name
  },
  {
    "test_it",                           // action_name
    "test_it/_action/feedback",          // expected_action_derived_name
    rcl_action_get_feedback_topic_name,  // get_action_derived_name
    "feedback_topic_name_test"           // subject_name
  },
  {
    "test_it",                         // action_name
    "test_it/_action/status",          // expected_action_derived_name
    rcl_action_get_status_topic_name,  // get_action_derived_name
    "status_topic_name_test"           // subject_name
  }
};

INSTANTIATE_TEST_SUITE_P(
  TestActionServiceAndTopicNames, TestActionDerivedName,
  ::testing::ValuesIn(action_service_and_topic_subjects),
  ::testing::PrintToStringParamName());
