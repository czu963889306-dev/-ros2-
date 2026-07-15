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

#include "rcl/error_handling.h"
#include "rcl_action/types.h"

void * bad_realloc(void *, size_t, void *)
{
  return nullptr;
}

void * bad_calloc(size_t, size_t, void *)
{
  return nullptr;
}

TEST(TestActionTypes, test_get_zero_inititalized_goal_info)
{
  rcl_action_goal_info_t goal_info = rcl_action_get_zero_initialized_goal_info();
  ASSERT_EQ(sizeof(goal_info.goal_id.uuid) / sizeof(uint8_t), 16u);
  for (int i = 0; i < 16; ++i) {
    EXPECT_EQ(goal_info.goal_id.uuid[i], 0u);
  }
  EXPECT_EQ(goal_info.stamp.sec, 0);
  EXPECT_EQ(goal_info.stamp.nanosec, 0u);

  // Modify the first and get another zero initialized goal info struct
  // to confirm they are independent objects
  for (int i = 0; i < 16; ++i) {
    goal_info.goal_id.uuid[i] = static_cast<uint8_t>(i);
  }
  goal_info.stamp.sec = 1234;
  goal_info.stamp.nanosec = 4567u;
  rcl_action_goal_info_t another_goal_info = rcl_action_get_zero_initialized_goal_info();
  for (int i = 0; i < 16; ++i) {
    EXPECT_EQ(goal_info.goal_id.uuid[i], i);
    EXPECT_EQ(another_goal_info.goal_id.uuid[i], 0u);
  }
  EXPECT_EQ(goal_info.stamp.sec, 1234);
  EXPECT_EQ(goal_info.stamp.nanosec, 4567u);
  EXPECT_EQ(another_goal_info.stamp.sec, 0);
  EXPECT_EQ(another_goal_info.stamp.nanosec, 0u);
}

TEST(TestActionTypes, test_get_zero_initialized_goal_status_array)
{
  rcl_action_goal_status_array_t status_array =
    rcl_action_get_zero_initialized_goal_status_array();
  EXPECT_EQ(status_array.msg.status_list.size, 0u);
  EXPECT_EQ(status_array.msg.status_list.data, nullptr);
}

TEST(TestActionTypes, test_get_zero_inititalized_cancel_request)
{
  rcl_action_cancel_request_t cancel_request = rcl_action_get_zero_initialized_cancel_request();
  ASSERT_EQ(sizeof(cancel_request.goal_info.goal_id.uuid) / sizeof(uint8_t), 16u);
  for (int i = 0; i < 16; ++i) {
    EXPECT_EQ(cancel_request.goal_info.goal_id.uuid[i], 0u);
  }
  EXPECT_EQ(cancel_request.goal_info.stamp.sec, 0);
  EXPECT_EQ(cancel_request.goal_info.stamp.nanosec, 0u);
}

TEST(TestActionTypes, test_get_zero_initialized_cancel_response)
{
  rcl_action_cancel_response_t cancel_response = rcl_action_get_zero_initialized_cancel_response();
  EXPECT_EQ(cancel_response.msg.goals_canceling.size, 0u);
  EXPECT_EQ(cancel_response.msg.goals_canceling.data, nullptr);
  EXPECT_EQ(cancel_response.msg.return_code, 0);
}

TEST(TestActionTypes, test_init_fini_goal_status_array)
{
  const size_t num_status = 3;
  // Initialize with invalid status array
  rcl_ret_t ret = rcl_action_goal_status_array_init(
    nullptr,
    num_status,
    rcl_get_default_allocator());
  EXPECT_EQ(ret, RCL_RET_INVALID_ARGUMENT);
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  // Initialize with invalid allocator
  rcl_allocator_t invalid_allocator = rcl_get_default_allocator();
  invalid_allocator.allocate = nullptr;
  rcl_action_goal_status_array_t status_array =
    rcl_action_get_zero_initialized_goal_status_array();
  ASSERT_EQ(status_array.msg.status_list.size, 0u);
  ret = rcl_action_goal_status_array_init(&status_array, num_status, invalid_allocator);
  EXPECT_EQ(ret, RCL_RET_INVALID_ARGUMENT);
  EXPECT_EQ(status_array.msg.status_list.size, 0u);
  EXPECT_EQ(status_array.msg.status_list.data, nullptr);
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  // Initialize with a failing allocator
  rcl_allocator_t failing_allocator = rcl_get_default_allocator();
  failing_allocator.zero_allocate = bad_calloc;
  status_array = rcl_action_get_zero_initialized_goal_status_array();
  ASSERT_EQ(status_array.msg.status_list.size, 0u);
  ret = rcl_action_goal_status_array_init(&status_array, num_status, failing_allocator);
  EXPECT_EQ(ret, RCL_RET_BAD_ALLOC);
  EXPECT_EQ(status_array.msg.status_list.size, 0u);
  EXPECT_EQ(status_array.msg.status_list.data, nullptr);
  // Doesn't set error
  EXPECT_FALSE(rcl_error_is_set());
  rcl_reset_error();

  // Initialize with zero size
  status_array = rcl_action_get_zero_initialized_goal_status_array();
  ASSERT_EQ(status_array.msg.status_list.size, 0u);
  ret = rcl_action_goal_status_array_init(&status_array, 0, rcl_get_default_allocator());
  EXPECT_EQ(ret, RCL_RET_INVALID_ARGUMENT);
  EXPECT_EQ(status_array.msg.status_list.size, 0u);
  EXPECT_EQ(status_array.msg.status_list.data, nullptr);
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  // Initialize with non-zero size
  status_array = rcl_action_get_zero_initialized_goal_status_array();
  status_array.msg.status_list.size = 1;
  ret = rcl_action_goal_status_array_init(&status_array, num_status, rcl_get_default_allocator());
  EXPECT_EQ(ret, RCL_RET_ALREADY_INIT);
  // Expect array to be unchanged
  EXPECT_EQ(status_array.msg.status_list.size, 1u);
  EXPECT_EQ(status_array.msg.status_list.data, nullptr);
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  // Initialize with valid arguments
  status_array = rcl_action_get_zero_initialized_goal_status_array();
  ASSERT_EQ(status_array.msg.status_list.size, 0u);
  ret = rcl_action_goal_status_array_init(&status_array, num_status, rcl_get_default_allocator());
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  EXPECT_EQ(status_array.msg.status_list.size, num_status);
  EXPECT_NE(status_array.msg.status_list.data, nullptr);

  // Finalize with invalid status array
  ret = rcl_action_goal_status_array_fini(nullptr);
  EXPECT_EQ(ret, RCL_RET_INVALID_ARGUMENT);
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  // Finalize with valid arguments
  ret = rcl_action_goal_status_array_fini(&status_array);
  EXPECT_EQ(ret, RCL_RET_OK);
}

TEST(TestActionTypes, test_init_fini_cancel_response)
{
  const size_t num_goals_canceling = 3;
  // Initialize with invalid cancel response
  rcl_ret_t ret = rcl_action_cancel_response_init(
    nullptr,
    num_goals_canceling,
    rcl_get_default_allocator());
  EXPECT_EQ(ret, RCL_RET_INVALID_ARGUMENT);
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  // Initialize with invalid allocator
  rcl_allocator_t invalid_allocator = rcl_get_default_allocator();
  invalid_allocator.allocate = nullptr;
  rcl_action_cancel_response_t cancel_response = rcl_action_get_zero_initialized_cancel_response();
  ASSERT_EQ(cancel_response.msg.goals_canceling.size, 0u);
  ret = rcl_action_cancel_response_init(&cancel_response, num_goals_canceling, invalid_allocator);
  EXPECT_EQ(ret, RCL_RET_INVALID_ARGUMENT);
  EXPECT_EQ(cancel_response.msg.goals_canceling.size, 0u);
  EXPECT_EQ(cancel_response.msg.goals_canceling.data, nullptr);
  EXPECT_EQ(cancel_response.msg.return_code, 0);
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  // Initialize with failing allocator
  rcl_allocator_t failing_allocator = rcl_get_default_allocator();
  failing_allocator.zero_allocate = bad_calloc;
  cancel_response = rcl_action_get_zero_initialized_cancel_response();
  ASSERT_EQ(cancel_response.msg.goals_canceling.size, 0u);
  ret = rcl_action_cancel_response_init(&cancel_response, num_goals_canceling, failing_allocator);
  EXPECT_EQ(ret, RCL_RET_BAD_ALLOC);
  EXPECT_EQ(cancel_response.msg.goals_canceling.size, 0u);
  EXPECT_EQ(cancel_response.msg.goals_canceling.data, nullptr);
  EXPECT_EQ(cancel_response.msg.return_code, 0);
  // Bad allocation doesn't set error message
  EXPECT_FALSE(rcl_error_is_set());
  rcl_reset_error();

  // Initialize with zero size
  cancel_response = rcl_action_get_zero_initialized_cancel_response();
  ASSERT_EQ(cancel_response.msg.goals_canceling.size, 0u);
  ret = rcl_action_cancel_response_init(&cancel_response, 0, rcl_get_default_allocator());
  EXPECT_EQ(ret, RCL_RET_INVALID_ARGUMENT);
  EXPECT_EQ(cancel_response.msg.goals_canceling.size, 0u);
  EXPECT_EQ(cancel_response.msg.goals_canceling.data, nullptr);
  EXPECT_EQ(cancel_response.msg.return_code, 0);
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  // Initialize with non-zero size
  cancel_response = rcl_action_get_zero_initialized_cancel_response();
  cancel_response.msg.goals_canceling.size = 1;
  ret = rcl_action_cancel_response_init(&cancel_response, 1, rcl_get_default_allocator());
  EXPECT_EQ(ret, RCL_RET_ALREADY_INIT);
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  // Check unchanged
  EXPECT_EQ(cancel_response.msg.goals_canceling.size, 1u);
  EXPECT_EQ(cancel_response.msg.goals_canceling.data, nullptr);
  EXPECT_EQ(cancel_response.msg.return_code, 0);

  // Initialize with non-zero return code
  cancel_response = rcl_action_get_zero_initialized_cancel_response();
  cancel_response.msg.return_code = 1;
  ret = rcl_action_cancel_response_init(&cancel_response, 0, rcl_get_default_allocator());
  EXPECT_EQ(ret, RCL_RET_INVALID_ARGUMENT);
  // Check unchanged
  EXPECT_EQ(cancel_response.msg.goals_canceling.size, 0u);
  EXPECT_EQ(cancel_response.msg.goals_canceling.data, nullptr);
  EXPECT_EQ(cancel_response.msg.return_code, 1);
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  // Initialize with valid arguments
  cancel_response = rcl_action_get_zero_initialized_cancel_response();
  ASSERT_EQ(cancel_response.msg.goals_canceling.size, 0u);
  ret = rcl_action_cancel_response_init(
    &cancel_response,
    num_goals_canceling,
    rcl_get_default_allocator());
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
  EXPECT_EQ(cancel_response.msg.goals_canceling.size, num_goals_canceling);
  EXPECT_NE(cancel_response.msg.goals_canceling.data, nullptr);
  EXPECT_EQ(cancel_response.msg.return_code, action_msgs__srv__CancelGoal_Response__ERROR_NONE);

  // Finalize with invalid cancel response
  ret = rcl_action_cancel_response_fini(nullptr);
  EXPECT_EQ(ret, RCL_RET_INVALID_ARGUMENT);
  EXPECT_TRUE(rcl_error_is_set());
  rcl_reset_error();

  // Finalize with valid arguments
  ret = rcl_action_cancel_response_fini(&cancel_response);
  EXPECT_EQ(ret, RCL_RET_OK) << rcl_get_error_string().str;
}
