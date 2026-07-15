// Copyright (c) 2019 - for information on the respective copyright owner
// see the NOTICE file and/or the repository https://github.com/ros2/rclc.
// Copyright 2014 Open Source Robotics Foundation, Inc.
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

extern "C"
{
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <example_interfaces/action/fibonacci.h>
}

#include <chrono>
#include <thread>
#include <memory>
#include <map>
#include <vector>
#include <utility>

#include <rclcpp/rclcpp.hpp>
#include <rclcpp_action/rclcpp_action.hpp>
#include <example_interfaces/action/fibonacci.hpp>

#define RCLC_MAX_GOALS 10

using namespace std::chrono_literals;

TEST(Test, rclc_action_server) {
  rclc_support_t support;
  rcl_node_t node;
  rcl_ret_t rc;

  // preliminary setup
  rcl_allocator_t allocator = rcl_get_default_allocator();
  rc = rclc_support_init(&support, 0, nullptr, &allocator);
  rc = rclc_node_init_default(&node, "my_node", "my_namespace", &support);
  EXPECT_EQ(RCL_RET_OK, rc);

  // test action server with valid arguments
  rclc_action_server_t action_server;
  rc = rclc_action_server_init_default(
    &action_server,
    &node,
    &support,
    ROSIDL_GET_ACTION_TYPE_SUPPORT(example_interfaces, Fibonacci),
    "fibonacci"
  );
  EXPECT_EQ(RCL_RET_OK, rc);

  // test action server with invalid arguments
  rclc_action_server_t invalid_action_server;
  rc = rclc_action_server_init_default(
    nullptr,
    &node,
    &support,
    ROSIDL_GET_ACTION_TYPE_SUPPORT(example_interfaces, Fibonacci),
    "fibonacci"
  );
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc);
  rcutils_reset_error();

  rc = rclc_action_server_init_default(
    &invalid_action_server,
    nullptr,
    &support,
    ROSIDL_GET_ACTION_TYPE_SUPPORT(example_interfaces, Fibonacci),
    "fibonacci"
  );
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc);
  rcutils_reset_error();

  rc = rclc_action_server_init_default(
    &invalid_action_server,
    &node,
    nullptr,
    ROSIDL_GET_ACTION_TYPE_SUPPORT(example_interfaces, Fibonacci),
    "fibonacci"
  );
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc);
  rcutils_reset_error();

  rc = rclc_action_server_init_default(
    &invalid_action_server,
    &node,
    &support,
    nullptr,
    "fibonacci"
  );
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc);
  rcutils_reset_error();

  rc = rclc_action_server_init_default(
    &invalid_action_server,
    &node,
    &support,
    ROSIDL_GET_ACTION_TYPE_SUPPORT(example_interfaces, Fibonacci),
    nullptr
  );
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc);
  rcutils_reset_error();

  // Create executor
  rclc_executor_t executor;
  rclc_executor_init(&executor, &support.context, 1, &allocator);

  example_interfaces__action__Fibonacci_SendGoal_Request ros_goal_request[RCLC_MAX_GOALS];

  rc = rclc_executor_add_action_server(
    &executor,
    &action_server,
    RCLC_MAX_GOALS,
    ros_goal_request,
    sizeof(example_interfaces__action__Fibonacci_SendGoal_Request),
    [](rclc_action_goal_handle_t * /* goal_handle */, void * /* context */) -> rcl_ret_t {
      return RCL_RET_ACTION_GOAL_REJECTED;
    },
    [](rclc_action_goal_handle_t * /* goal_handle */, void * /* context */) -> bool {
      return false;
    },
    &action_server);

  EXPECT_EQ(RCL_RET_OK, rc);

  rc = rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100));
  EXPECT_EQ(RCL_RET_OK, rc);

  // clean up
  rc = rclc_action_server_fini(&action_server, &node);
  EXPECT_EQ(RCL_RET_OK, rc);
  rc = rcl_node_fini(&node);
  EXPECT_EQ(RCL_RET_OK, rc);
  rc = rclc_support_fini(&support);
  EXPECT_EQ(RCL_RET_OK, rc);
}

using Fibonacci = example_interfaces::action::Fibonacci;
using GoalHandleFibonacci = rclcpp_action::ClientGoalHandle<Fibonacci>;

class ActionServerTest : public ::testing::Test
{
public:
  ActionServerTest()
  : rclcpp_timeout(100000) {}

  ~ActionServerTest() {}

  void SetUp() override
  {
    rcl_ret_t rc;

    // Init support
    allocator = rcl_get_default_allocator();
    rc = rclc_support_init(&support, 0, nullptr, &allocator);
    EXPECT_EQ(RCL_RET_OK, rc);

    node = rcl_get_zero_initialized_node();
    rc = rclc_node_init_default(&node, "my_node", "", &support);
    EXPECT_EQ(RCL_RET_OK, rc);

    // Init action server
    rc = rclc_action_server_init_default(
      &action_server,
      &node,
      &support,
      ROSIDL_GET_ACTION_TYPE_SUPPORT(example_interfaces, Fibonacci),
      "fibonacci"
    );
    EXPECT_EQ(RCL_RET_OK, rc);

    // Init executor
    rclc_executor_init(&executor, &support.context, 1, &allocator);

    rc = rclc_executor_add_action_server(
      &executor,
      &action_server,
      RCLC_MAX_GOALS,
      ros_goal_request,
      sizeof(example_interfaces__action__Fibonacci_SendGoal_Request),
      handle_goal_dispatcher,
      handle_cancel_dispatcher,
      this);

    EXPECT_EQ(RCL_RET_OK, rc);

    run_server = true;
    server_thread = std::thread(
      [&]() {
        while (run_server) {
          rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100));
        }
      });

    // Init RCLCPP
    rclcpp::init(0, NULL);
    action_client_node = rclcpp::Node::make_shared("action_aux_client");
    action_client = rclcpp_action::create_client<Fibonacci>(action_client_node, "fibonacci");

    EXPECT_EQ(action_client->wait_for_action_server(rclcpp_timeout), true);

    send_goal_options = rclcpp_action::Client<Fibonacci>::SendGoalOptions();
  }

  void TearDown() override
  {
    rcl_ret_t rc;

    run_server = false;
    server_thread.join();

    rc = rclc_action_server_fini(&action_server, &node);
    EXPECT_EQ(RCL_RET_OK, rc);
    rc = rcl_node_fini(&node);
    EXPECT_EQ(RCL_RET_OK, rc);
    rc = rclc_support_fini(&support);
    EXPECT_EQ(RCL_RET_OK, rc);

    rclcpp::shutdown();
  }

  static rcl_ret_t handle_goal_dispatcher(rclc_action_goal_handle_t * goal_handle, void * context)
  {
    return static_cast<ActionServerTest *>(context)->handle_goal(goal_handle, context);
  }

  static bool handle_cancel_dispatcher(rclc_action_goal_handle_t * goal_handle, void * context)
  {
    return static_cast<ActionServerTest *>(context)->handle_cancel(goal_handle, context);
  }

protected:
  // RCLC members
  rclc_support_t support;
  rcl_allocator_t allocator;

  rcl_node_t node;
  rclc_action_server_t action_server;
  rclc_executor_t executor;

  example_interfaces__action__Fibonacci_SendGoal_Request ros_goal_request[RCLC_MAX_GOALS];

  std::function<rcl_ret_t(rclc_action_goal_handle_t *, void *)> handle_goal;
  std::function<rcl_ret_t(rclc_action_goal_handle_t *, void *)> handle_cancel;

  bool run_server;
  std::thread server_thread;

  // RCLCPP members
  std::shared_ptr<rclcpp::Node> action_client_node;
  rclcpp_action::Client<Fibonacci>::SharedPtr action_client;

  rclcpp_action::Client<Fibonacci>::SendGoalOptions send_goal_options;

  std::chrono::duration<int64_t, std::milli> rclcpp_timeout;
};

TEST_F(ActionServerTest, goal_accept) {
  auto goal_msg = Fibonacci::Goal();
  goal_msg.order = 10;

  // Prepare RCLC
  handle_goal = [&](rclc_action_goal_handle_t * goal_handle, void * /* context */) -> rcl_ret_t {
      example_interfaces__action__Fibonacci_SendGoal_Request * req =
        reinterpret_cast<example_interfaces__action__Fibonacci_SendGoal_Request *>(goal_handle->
        ros_goal_request);
      EXPECT_EQ(req->goal.order, goal_msg.order);
      return RCL_RET_ACTION_GOAL_ACCEPTED;
    };

  // Run RCLCPP
  auto promise = std::make_shared<std::promise<uint8_t>>();
  auto future = promise->get_future().share();

  send_goal_options.goal_response_callback =
    [&](GoalHandleFibonacci::SharedPtr goal_handle) -> void {
      ASSERT_NE(nullptr, goal_handle);
      promise->set_value(goal_handle->get_status());
    };

  action_client->async_send_goal(goal_msg, send_goal_options);
  ASSERT_EQ(
    rclcpp::spin_until_future_complete(
      action_client_node, future,
      rclcpp_timeout), rclcpp::FutureReturnCode::SUCCESS);
  ASSERT_EQ(future.get(), 1);
}

TEST_F(ActionServerTest, goal_reject) {
  // Prepare RCLC
  handle_goal =
    [](rclc_action_goal_handle_t * /* goal_handle */, void * /* context */) -> rcl_ret_t {
      return RCL_RET_ACTION_GOAL_REJECTED;
    };

  // Run RCLCPP
  auto promise = std::make_shared<std::promise<void>>();
  auto future = promise->get_future().share();

  auto goal_msg = Fibonacci::Goal();
  goal_msg.order = 10;

  send_goal_options.goal_response_callback =
    [&](GoalHandleFibonacci::SharedPtr goal_handle) -> void {
      ASSERT_EQ(nullptr, goal_handle);
      promise->set_value();
    };

  action_client->async_send_goal(goal_msg, send_goal_options);
  ASSERT_EQ(
    rclcpp::spin_until_future_complete(
      action_client_node, future,
      rclcpp_timeout), rclcpp::FutureReturnCode::SUCCESS);
}

TEST_F(ActionServerTest, goal_accept_feedback_and_result) {
  // Prepare RCLC
  std::thread feedback_thread;

  handle_goal = [&](rclc_action_goal_handle_t * goal_handle, void * /* context */) -> rcl_ret_t {
      feedback_thread = std::thread(
        [ = ]() {
          std::this_thread::sleep_for(100ms);

          int32_t data[] = {0, 1, 2};
          example_interfaces__action__Fibonacci_FeedbackMessage feedback;
          feedback.feedback.sequence.capacity = sizeof(data) / sizeof(data[0]);
          feedback.feedback.sequence.size = feedback.feedback.sequence.capacity;
          feedback.feedback.sequence.data = data;
          for (size_t i = 0; i < 10; i++) {
            rcl_ret_t rc = rclc_action_publish_feedback(goal_handle, &feedback);
            EXPECT_EQ(RCL_RET_OK, rc);
            std::this_thread::sleep_for(10ms);
          }

          example_interfaces__action__Fibonacci_GetResult_Response response;
          response.result.sequence.capacity = feedback.feedback.sequence.capacity;
          response.result.sequence.size = feedback.feedback.sequence.size;
          response.result.sequence.data = feedback.feedback.sequence.data;
          rcl_ret_t rc = rclc_action_send_result(goal_handle, GOAL_STATE_SUCCEEDED, &response);
          EXPECT_EQ(RCL_RET_OK, rc);
        });

      return RCL_RET_ACTION_GOAL_ACCEPTED;
    };

  // Run RCLCPP
  auto promise = std::make_shared<std::promise<void>>();
  auto future = promise->get_future().share();

  auto goal_msg = Fibonacci::Goal();
  goal_msg.order = 10;

  bool accepted = false;

  send_goal_options.goal_response_callback =
    [&](GoalHandleFibonacci::SharedPtr goal_handle) -> void {
      ASSERT_NE(nullptr, goal_handle);
      accepted = true;
    };

  size_t feedback_received = 0;
  send_goal_options.feedback_callback =
    [&](GoalHandleFibonacci::SharedPtr,
      const std::shared_ptr<const Fibonacci::Feedback> feedback) -> void {
      feedback_received++;
      ASSERT_EQ(feedback->sequence.size(), 3U);
    };

  send_goal_options.result_callback =
    [&](const GoalHandleFibonacci::WrappedResult & result) -> void {
      ASSERT_EQ(result.code, rclcpp_action::ResultCode::SUCCEEDED);
      promise->set_value();
    };

  auto goal_handle = action_client->async_send_goal(goal_msg, send_goal_options);
  rclcpp::spin_until_future_complete(action_client_node, goal_handle);
  action_client->async_get_result(goal_handle.get());
  ASSERT_EQ(
    rclcpp::spin_until_future_complete(
      action_client_node, future,
      rclcpp_timeout), rclcpp::FutureReturnCode::SUCCESS);

  feedback_thread.join();

  ASSERT_TRUE(accepted);
  ASSERT_EQ(feedback_received, 10U);
}

TEST_F(ActionServerTest, goal_accept_feedback_and_abort) {
  // Prepare RCLC
  std::thread feedback_thread;

  handle_goal = [&](rclc_action_goal_handle_t * goal_handle, void * /* context */) -> rcl_ret_t {
      feedback_thread = std::thread(
        [ = ]() {
          std::this_thread::sleep_for(100ms);

          int32_t data[] = {0, 1, 2};
          example_interfaces__action__Fibonacci_FeedbackMessage feedback;
          feedback.feedback.sequence.capacity = sizeof(data) / sizeof(data[0]);
          feedback.feedback.sequence.size = feedback.feedback.sequence.capacity;
          feedback.feedback.sequence.data = data;
          for (size_t i = 0; i < 5; i++) {
            rcl_ret_t rc = rclc_action_publish_feedback(goal_handle, &feedback);
            EXPECT_EQ(RCL_RET_OK, rc);
            std::this_thread::sleep_for(10ms);
          }

          example_interfaces__action__Fibonacci_GetResult_Response response = {};
          rcl_ret_t rc = rclc_action_send_result(goal_handle, GOAL_STATE_ABORTED, &response);
          EXPECT_EQ(RCL_RET_OK, rc);
        });

      return RCL_RET_ACTION_GOAL_ACCEPTED;
    };

  // Run RCLCPP
  auto promise = std::make_shared<std::promise<void>>();
  auto future = promise->get_future().share();

  auto goal_msg = Fibonacci::Goal();
  goal_msg.order = 10;

  bool accepted = false;

  send_goal_options.goal_response_callback =
    [&](GoalHandleFibonacci::SharedPtr goal_handle) -> void {
      ASSERT_NE(nullptr, goal_handle);
      accepted = true;
    };

  size_t feedback_received = 0;
  send_goal_options.feedback_callback =
    [&](GoalHandleFibonacci::SharedPtr,
      const std::shared_ptr<const Fibonacci::Feedback> feedback) -> void {
      feedback_received++;
      ASSERT_EQ(feedback->sequence.size(), 3U);
    };

  send_goal_options.result_callback =
    [&](const GoalHandleFibonacci::WrappedResult & result) -> void {
      ASSERT_EQ(result.code, rclcpp_action::ResultCode::ABORTED);
      promise->set_value();
    };

  auto goal_handle = action_client->async_send_goal(goal_msg, send_goal_options);
  ASSERT_EQ(
    rclcpp::spin_until_future_complete(
      action_client_node, goal_handle,
      rclcpp_timeout), rclcpp::FutureReturnCode::SUCCESS);
  action_client->async_get_result(goal_handle.get());
  ASSERT_EQ(
    rclcpp::spin_until_future_complete(
      action_client_node, future,
      rclcpp_timeout), rclcpp::FutureReturnCode::SUCCESS);

  feedback_thread.join();

  ASSERT_TRUE(accepted);
  ASSERT_EQ(feedback_received, 5U);
}

TEST_F(ActionServerTest, goal_accept_cancel_success) {
  // Prepare RCLC
  std::thread feedback_thread;
  handle_goal =
    [&](rclc_action_goal_handle_t * goal_handle, void * /* context */) -> rcl_ret_t {
      feedback_thread = std::thread(
        [ = ]() {
          std::this_thread::sleep_for(100ms);
          for (size_t i = 0; i < 5 && !goal_handle->goal_cancelled; i++) {
            std::this_thread::sleep_for(100ms);
          }

          ASSERT_TRUE(goal_handle->goal_cancelled);

          example_interfaces__action__Fibonacci_GetResult_Response response = {};
          rcl_ret_t rc = rclc_action_send_result(goal_handle, GOAL_STATE_CANCELED, &response);
          EXPECT_EQ(RCL_RET_OK, rc);
        });

      return RCL_RET_ACTION_GOAL_ACCEPTED;
    };

  handle_cancel = [&](rclc_action_goal_handle_t * /* goal_handle */, void * /* context */) -> bool {
      return true;
    };

  // Run RCLCPP
  auto promise = std::make_shared<std::promise<void>>();
  auto future = promise->get_future().share();

  auto promise_result = std::make_shared<std::promise<void>>();
  auto future_result = promise_result->get_future().share();

  auto goal_msg = Fibonacci::Goal();
  goal_msg.order = 10;

  bool accepted = false;
  send_goal_options.goal_response_callback =
    [&](GoalHandleFibonacci::SharedPtr goal_handle) -> void {
      ASSERT_NE(nullptr, goal_handle);
      accepted = true;
    };

  send_goal_options.result_callback =
    [&](const GoalHandleFibonacci::WrappedResult & result) -> void {
      ASSERT_EQ(result.code, rclcpp_action::ResultCode::CANCELED);
      promise_result->set_value();
    };

  auto goal_handle = action_client->async_send_goal(goal_msg, send_goal_options);
  rclcpp::spin_until_future_complete(action_client_node, goal_handle);
  action_client->async_get_result(goal_handle.get());

  std::this_thread::sleep_for(100ms);

  action_client->async_cancel_goal(
    goal_handle.get(),
    [&](auto ans) -> void {
      EXPECT_EQ(ans->return_code, action_msgs__srv__CancelGoal_Response__ERROR_NONE);
      promise->set_value();
    });

  ASSERT_EQ(
    rclcpp::spin_until_future_complete(
      action_client_node, future,
      rclcpp_timeout), rclcpp::FutureReturnCode::SUCCESS);
  ASSERT_EQ(
    rclcpp::spin_until_future_complete(
      action_client_node, future_result,
      rclcpp_timeout), rclcpp::FutureReturnCode::SUCCESS);

  feedback_thread.join();
  ASSERT_TRUE(accepted);
}

TEST_F(ActionServerTest, goal_accept_cancel_reject) {
  // Prepare RCLC
  handle_goal =
    [&](rclc_action_goal_handle_t * /* goal_handle */, void * /* context */) -> rcl_ret_t {
      return RCL_RET_ACTION_GOAL_ACCEPTED;
    };

  handle_cancel = [&](rclc_action_goal_handle_t * /* goal_handle */, void * /* context */) -> bool {
      return false;
    };

  // Run RCLCPP
  auto promise = std::make_shared<std::promise<void>>();
  auto future = promise->get_future().share();

  auto goal_msg = Fibonacci::Goal();
  goal_msg.order = 10;

  bool accepted = false;
  send_goal_options.goal_response_callback =
    [&](GoalHandleFibonacci::SharedPtr goal_handle) -> void {
      ASSERT_NE(nullptr, goal_handle);
      accepted = true;
    };

  auto goal_handle = action_client->async_send_goal(goal_msg, send_goal_options);
  rclcpp::spin_until_future_complete(action_client_node, goal_handle);
  action_client->async_get_result(goal_handle.get());

  std::this_thread::sleep_for(100ms);

  action_client->async_cancel_goal(
    goal_handle.get(),
    [&](auto ans) -> void {
      EXPECT_EQ(ans->return_code, action_msgs__srv__CancelGoal_Response__ERROR_REJECTED);
      promise->set_value();
    });

  ASSERT_EQ(
    rclcpp::spin_until_future_complete(
      action_client_node, future,
      rclcpp_timeout), rclcpp::FutureReturnCode::SUCCESS);

  ASSERT_TRUE(accepted);
}

TEST_F(ActionServerTest, multi_goal_accept_feedback_and_result) {
  // Prepare RCLC
  std::vector<std::thread> feedback_thread_pool;

  size_t feedback_per_goal = 10;

  handle_goal = [&](rclc_action_goal_handle_t * goal_handle, void * /* context */) -> rcl_ret_t {
      std::thread worker = std::thread(
        [ = ]() {
          std::this_thread::sleep_for(100ms);

          example_interfaces__action__Fibonacci_SendGoal_Request * req =
          reinterpret_cast<example_interfaces__action__Fibonacci_SendGoal_Request *>(goal_handle->
          ros_goal_request);

          std::vector<int32_t> data(req->goal.order);

          example_interfaces__action__Fibonacci_FeedbackMessage feedback;
          feedback.feedback.sequence.capacity = data.size();
          feedback.feedback.sequence.size = feedback.feedback.sequence.capacity;
          feedback.feedback.sequence.data = &data[0];
          for (size_t i = 0; i < feedback_per_goal; i++) {
            rcl_ret_t rc = rclc_action_publish_feedback(goal_handle, &feedback);
            EXPECT_EQ(RCL_RET_OK, rc);
            std::this_thread::sleep_for(10ms);
          }

          example_interfaces__action__Fibonacci_GetResult_Response response;
          response.result.sequence.capacity = feedback.feedback.sequence.capacity;
          response.result.sequence.size = feedback.feedback.sequence.size;
          response.result.sequence.data = feedback.feedback.sequence.data;
          rclc_action_send_result(goal_handle, GOAL_STATE_SUCCEEDED, &response);
        });

      feedback_thread_pool.push_back(std::move(worker));

      return RCL_RET_ACTION_GOAL_ACCEPTED;
    };

  // Run RCLCPP
  auto promise = std::make_shared<std::promise<void>>();
  auto future = promise->get_future().share();

  std::map<rclcpp_action::GoalUUID, int> goals;
  std::map<rclcpp_action::GoalUUID, bool> goals_accepted;

  send_goal_options.goal_response_callback =
    [&](GoalHandleFibonacci::SharedPtr goal_handle) -> void {
      ASSERT_NE(nullptr, goal_handle);
      goals_accepted[goal_handle->get_goal_id()] = true;
    };

  size_t feedback_received = 0;
  send_goal_options.feedback_callback =
    [&](GoalHandleFibonacci::SharedPtr goal_handle,
      const std::shared_ptr<const Fibonacci::Feedback> feedback) -> void {
      feedback_received++;
      size_t feedback_size = goals[goal_handle->get_goal_id()];
      ASSERT_EQ(feedback->sequence.size(), feedback_size);
    };

  send_goal_options.result_callback =
    [&](const GoalHandleFibonacci::WrappedResult & result) -> void {
      ASSERT_EQ(result.code, rclcpp_action::ResultCode::SUCCEEDED);
      ASSERT_TRUE(goals_accepted[result.goal_id]);
      goals.erase(result.goal_id);
      if (goals.size() == 0) {
        promise->set_value();
      }
    };

  size_t num_goals = std::floor(RCLC_MAX_GOALS / 2);

  for (size_t i = 0; i < num_goals; i++) {
    auto goal_msg = Fibonacci::Goal();
    goal_msg.order = 10 * (i + 1);

    auto goal_handle_future = action_client->async_send_goal(goal_msg, send_goal_options);
    rclcpp::spin_until_future_complete(action_client_node, goal_handle_future);
    auto goal_handle = goal_handle_future.get();

    action_client->async_get_result(goal_handle);

    goals.insert({goal_handle->get_goal_id(), goal_msg.order});
    goals_accepted.insert({goal_handle->get_goal_id(), false});
  }

  ASSERT_EQ(
    rclcpp::spin_until_future_complete(
      action_client_node, future,
      rclcpp_timeout), rclcpp::FutureReturnCode::SUCCESS);

  for (auto & thread : feedback_thread_pool) {
    thread.join();
  }

  ASSERT_EQ(goals_accepted.size(), num_goals);

  for (auto const & x : goals_accepted) {
    ASSERT_TRUE(x.second);
  }

  ASSERT_EQ(feedback_received, num_goals * feedback_per_goal);
  ASSERT_EQ(goals.size(), 0U);
}

TEST(Test, rclc_action_server_regression_1) {
  rclc_support_t support;
  rcl_node_t node;
  rcl_ret_t rc;

  rcl_allocator_t allocator = rcl_get_default_allocator();
  rc = rclc_support_init(&support, 0, nullptr, &allocator);
  rc = rclc_node_init_default(&node, "my_node", "my_namespace", &support);
  EXPECT_EQ(RCL_RET_OK, rc);

  rclc_action_server_t action_server;
  rc = rclc_action_server_init_default(
    &action_server,
    &node,
    &support,
    ROSIDL_GET_ACTION_TYPE_SUPPORT(example_interfaces, Fibonacci),
    "fibonacci"
  );
  EXPECT_EQ(RCL_RET_OK, rc);

  rclc_executor_t executor;
  rclc_executor_init(&executor, &support.context, 1, &allocator);

  example_interfaces__action__Fibonacci_SendGoal_Request ros_goal_request[RCLC_MAX_GOALS];

  rc = rclc_executor_add_action_server(
    &executor,
    &action_server,
    RCLC_MAX_GOALS,
    ros_goal_request,
    sizeof(example_interfaces__action__Fibonacci_SendGoal_Request),
    [](rclc_action_goal_handle_t * /* goal_handle */, void * /* context */) -> rcl_ret_t {
      return RCL_RET_ACTION_GOAL_REJECTED;
    },
    [](rclc_action_goal_handle_t * /* goal_handle */, void * /* context */) -> bool {
      return false;
    },
    &action_server);

  EXPECT_EQ(RCL_RET_OK, rc);

  EXPECT_EQ(RCL_RET_OK, rclc_action_server_fini(&action_server, &node));

  // Second time should be safe
  EXPECT_EQ(RCL_RET_OK, rclc_action_server_fini(&action_server, &node));
}

int main(int args, char ** argv)
{
  ::testing::InitGoogleTest(&args, argv);
  return RUN_ALL_TESTS();
}
