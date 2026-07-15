// Copyright (c) 2019 - for information on the respective copyright owner
// see the NOTICE file and/or the repository https://github.com/ros2/rclc.
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

TEST(Test, rclc_action_client) {
  rclc_support_t support;
  rcl_node_t node;
  rcl_ret_t rc;

  // preliminary setup
  rcl_allocator_t allocator = rcl_get_default_allocator();
  rc = rclc_support_init(&support, 0, nullptr, &allocator);
  rc = rclc_node_init_default(&node, "my_node", "my_namespace", &support);
  EXPECT_EQ(RCL_RET_OK, rc);

  // test action client with valid arguments
  rclc_action_client_t action_client;
  rc = rclc_action_client_init_default(
    &action_client,
    &node,
    ROSIDL_GET_ACTION_TYPE_SUPPORT(example_interfaces, Fibonacci),
    "fibonacci"
  );
  EXPECT_EQ(RCL_RET_OK, rc);

  // test action client with invalid arguments
  rclc_action_client_t invalid_action_client;
  rc = rclc_action_client_init_default(
    nullptr,
    &node,
    ROSIDL_GET_ACTION_TYPE_SUPPORT(example_interfaces, Fibonacci),
    "fibonacci"
  );
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc);
  rcutils_reset_error();

  rc = rclc_action_client_init_default(
    &invalid_action_client,
    nullptr,
    ROSIDL_GET_ACTION_TYPE_SUPPORT(example_interfaces, Fibonacci),
    "fibonacci"
  );
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc);
  rcutils_reset_error();

  rc = rclc_action_client_init_default(
    &invalid_action_client,
    &node,
    nullptr,
    "fibonacci"
  );
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc);
  rcutils_reset_error();

  rc = rclc_action_client_init_default(
    &invalid_action_client,
    &node,
    ROSIDL_GET_ACTION_TYPE_SUPPORT(example_interfaces, Fibonacci),
    nullptr
  );
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, rc);
  rcutils_reset_error();

  // Create executor
  rclc_executor_t executor;
  rclc_executor_init(&executor, &support.context, 1, &allocator);

  example_interfaces__action__Fibonacci_FeedbackMessage ros_feedback;
  example_interfaces__action__Fibonacci_GetResult_Response ros_result_response;

  rc = rclc_executor_add_action_client(
    &executor,
    &action_client,
    RCLC_MAX_GOALS,
    &ros_result_response,
    &ros_feedback,
    [](rclc_action_goal_handle_t * /* goal_handle */, bool /* accepted */,
    void * /* context */) -> void {},
    [](rclc_action_goal_handle_t * /* goal_handle */, void * /* feedback */,
    void * /* context */) -> void {},
    [](rclc_action_goal_handle_t * /* goal_handle */, void * /* result */,
    void * /* context */) -> void {},
    [](rclc_action_goal_handle_t * /* goal_handle */, bool /* canceled */,
    void * /* context */) -> void {},
    this);

  EXPECT_EQ(RCL_RET_OK, rc);

  rc = rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100));
  EXPECT_EQ(RCL_RET_OK, rc);

  // clean up
  rc = rclc_action_client_fini(&action_client, &node);
  EXPECT_EQ(RCL_RET_OK, rc);
  rc = rcl_node_fini(&node);
  EXPECT_EQ(RCL_RET_OK, rc);
  rc = rclc_support_fini(&support);
  EXPECT_EQ(RCL_RET_OK, rc);
}

using Fibonacci = example_interfaces::action::Fibonacci;
using GoalHandleFibonacci = rclcpp_action::ServerGoalHandle<Fibonacci>;

class ActionClientTest : public ::testing::Test
{
public:
  ActionClientTest() {}

  ~ActionClientTest() {}

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

    // Init action client
    rc = rclc_action_client_init_default(
      &action_client,
      &node,
      ROSIDL_GET_ACTION_TYPE_SUPPORT(example_interfaces, Fibonacci),
      "fibonacci"
    );

    EXPECT_EQ(RCL_RET_OK, rc);

    // Init executor
    rclc_executor_init(&executor, &support.context, 1, &allocator);

    ros_feedback.feedback.sequence.capacity = RCLC_MAX_GOALS;
    ros_feedback.feedback.sequence.size = 0;
    ros_feedback.feedback.sequence.data = reinterpret_cast<int32_t *>(malloc(
        ros_feedback.feedback.sequence.capacity * sizeof(int32_t)));

    ros_result_response.result.sequence.capacity = RCLC_MAX_GOALS;
    ros_result_response.result.sequence.size = 0;
    ros_result_response.result.sequence.data = reinterpret_cast<int32_t *>(malloc(
        ros_result_response.result.sequence.capacity * sizeof(int32_t)));

    rc = rclc_executor_add_action_client(
      &executor,
      &action_client,
      RCLC_MAX_GOALS,
      &ros_result_response,
      &ros_feedback,
      handle_goal_dispatcher,
      feedback_dispatcher,
      result_dispatcher,
      handle_cancel_dispatcher,
      this);

    EXPECT_EQ(RCL_RET_OK, rc);

    handle_feedback =
      [&](rclc_action_goal_handle_t * /* goal_handle */, void * /* ros_feedback */,
        void * /* context */) {};

    // Init RCLCPP
    using namespace std::placeholders;

    rclcpp::init(0, NULL);
    action_server_node = rclcpp::Node::make_shared("action_aux_client");
    action_server = rclcpp_action::create_server<Fibonacci>(
      action_server_node,
      "fibonacci",
      std::bind(&ActionClientTest::server_handle_goal_dispatcher, this, _1, _2),
      std::bind(&ActionClientTest::server_handle_cancel_dispatcher, this, _1),
      std::bind(&ActionClientTest::server_handle_accepted_dispatcher, this, _1));

    server_handle_accepted = [](const std::shared_ptr<GoalHandleFibonacci>/* goal_handle */) {};

    run_server = true;
    server_thread = std::thread(
      [&]() {
        while (run_server) {
          rclcpp::spin_some(action_server_node);
        }
      });

    // Wait for action server match
    bool server_matched = false;
    for (size_t i = 0; i < 10; i++) {
      rc = rcl_action_server_is_available(
        &node, &action_client.rcl_handle, &server_matched);

      if (server_matched) {
        break;
      }

      std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    EXPECT_EQ(RCL_RET_OK, rc);
    EXPECT_TRUE(server_matched);
  }

  void TearDown() override
  {
    rcl_ret_t rc;

    run_server = false;
    server_thread.join();

    rc = rclc_action_client_fini(&action_client, &node);
    EXPECT_EQ(RCL_RET_OK, rc);
    rc = rcl_node_fini(&node);
    EXPECT_EQ(RCL_RET_OK, rc);
    rc = rclc_support_fini(&support);
    EXPECT_EQ(RCL_RET_OK, rc);

    free(ros_feedback.feedback.sequence.data);
    free(ros_result_response.result.sequence.data);

    rclcpp::shutdown();
  }

  rclcpp_action::GoalResponse server_handle_goal_dispatcher(
    const rclcpp_action::GoalUUID & uuid,
    std::shared_ptr<const Fibonacci::Goal> goal)
  {
    return server_handle_goal(uuid, goal);
  }

  rclcpp_action::CancelResponse server_handle_cancel_dispatcher(
    const std::shared_ptr<GoalHandleFibonacci> goal_handle)
  {
    return server_handle_cancel(goal_handle);
  }

  void server_handle_accepted_dispatcher(const std::shared_ptr<GoalHandleFibonacci> goal_handle)
  {
    return server_handle_accepted(goal_handle);
  }

  static void handle_goal_dispatcher(
    rclc_action_goal_handle_t * goal_handle, bool accepted,
    void * context)
  {
    static_cast<ActionClientTest *>(context)->handle_goal(goal_handle, accepted, context);
  }

  static void feedback_dispatcher(
    rclc_action_goal_handle_t * goal_handle, void * ros_feedback,
    void * context)
  {
    static_cast<ActionClientTest *>(context)->handle_feedback(goal_handle, ros_feedback, context);
  }

  static void result_dispatcher(
    rclc_action_goal_handle_t * goal_handle, void * ros_result_response,
    void * context)
  {
    static_cast<ActionClientTest *>(context)->handle_result(
      goal_handle, ros_result_response,
      context);
  }

  static void handle_cancel_dispatcher(
    rclc_action_goal_handle_t * goal_handle, bool cancelled,
    void * context)
  {
    static_cast<ActionClientTest *>(context)->handle_cancel(goal_handle, cancelled, context);
  }

  unique_identifier_msgs__msg__UUID get_raw_uuid(const rclcpp_action::GoalUUID & uuid)
  {
    unique_identifier_msgs__msg__UUID raw_uuid;
    for (size_t i = 0; i < uuid.size(); ++i) {
      raw_uuid.uuid[i] = uuid[i];
    }
    return raw_uuid;
  }

protected:
  // RCLC members
  rclc_support_t support;
  rcl_allocator_t allocator;

  rcl_node_t node;
  rclc_action_client_t action_client;
  rclc_executor_t executor;

  example_interfaces__action__Fibonacci_FeedbackMessage ros_feedback;
  example_interfaces__action__Fibonacci_GetResult_Response ros_result_response;

  std::function<void(rclc_action_goal_handle_t *, bool, void *)> handle_goal;
  std::function<void(rclc_action_goal_handle_t *, void *, void *)> handle_feedback;
  std::function<void(rclc_action_goal_handle_t *, void *, void *)> handle_result;
  std::function<void(rclc_action_goal_handle_t *, bool, void *)> handle_cancel;

  // RCLCPP members
  bool run_server;
  std::thread server_thread;

  std::shared_ptr<rclcpp::Node> action_server_node;
  rclcpp_action::Server<Fibonacci>::SharedPtr action_server;

  std::function<rclcpp_action::GoalResponse(
      const rclcpp_action::GoalUUID & uuid,
      std::shared_ptr<const Fibonacci::Goal> goal)> server_handle_goal;
  std::function<rclcpp_action::CancelResponse(
      const std::shared_ptr<GoalHandleFibonacci> goal_handle)> server_handle_cancel;
  std::function<void(const std::shared_ptr<GoalHandleFibonacci> goal_handle)>
  server_handle_accepted;
};

TEST_F(ActionClientTest, goal_accept) {
  example_interfaces__action__Fibonacci_SendGoal_Request ros_goal_request;
  ros_goal_request.goal.order = 10;

  // Prepare RCLCPP
  server_handle_goal = [ = ](const rclcpp_action::GoalUUID & /* uuid */,
      std::shared_ptr<const Fibonacci::Goal> goal) -> rclcpp_action::GoalResponse {
      EXPECT_EQ(ros_goal_request.goal.order, goal->order);
      return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
    };

  // Prepare RCLC
  bool goal_response_received = false;
  handle_goal =
    [&](rclc_action_goal_handle_t * /* goal_handle */, bool accepted, void * /* context */) {
      ASSERT_TRUE(accepted);
      goal_response_received = true;
    };

  rcl_ret_t rc = rclc_action_send_goal_request(&action_client, &ros_goal_request, NULL);
  EXPECT_EQ(RCL_RET_OK, rc);

  while (!goal_response_received) {
    rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100));
  }

  ASSERT_TRUE(goal_response_received);
}

TEST_F(ActionClientTest, goal_reject) {
  example_interfaces__action__Fibonacci_SendGoal_Request ros_goal_request;
  ros_goal_request.goal.order = 10;

  // Prepare RCLCPP
  server_handle_goal = [ = ](const rclcpp_action::GoalUUID & /* uuid */,
      std::shared_ptr<const Fibonacci::Goal> goal) -> rclcpp_action::GoalResponse {
      EXPECT_EQ(ros_goal_request.goal.order, goal->order);
      return rclcpp_action::GoalResponse::REJECT;
    };

  server_handle_accepted = [](const std::shared_ptr<GoalHandleFibonacci>/* goal_handle */) {
    };

  // Prepare RCLC
  bool goal_response_received = false;
  handle_goal =
    [&](rclc_action_goal_handle_t * /* goal_handle */, bool accepted, void * /* context */) {
      ASSERT_FALSE(accepted);
      goal_response_received = true;
    };

  rcl_ret_t rc = rclc_action_send_goal_request(&action_client, &ros_goal_request, NULL);
  EXPECT_EQ(RCL_RET_OK, rc);

  while (!goal_response_received) {
    rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100));
  }

  ASSERT_TRUE(goal_response_received);
}

TEST_F(ActionClientTest, goal_accept_feedback_and_result) {
  example_interfaces__action__Fibonacci_SendGoal_Request ros_goal_request;
  ros_goal_request.goal.order = 10;

  // Prepare RCLCPP

  server_handle_goal = [&](const rclcpp_action::GoalUUID & /* uuid */,
      std::shared_ptr<const Fibonacci::Goal> goal) -> rclcpp_action::GoalResponse {
      EXPECT_EQ(ros_goal_request.goal.order, goal->order);
      return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
    };

  std::thread feedback_thread;

  server_handle_accepted = [&](const std::shared_ptr<GoalHandleFibonacci> goal_handle) -> void {
      feedback_thread = std::thread(
        [ = ]() -> void {
          const auto goal = goal_handle->get_goal();
          auto feedback = std::make_shared<Fibonacci::Feedback>();
          auto & sequence = feedback->sequence;

          while (sequence.size() < static_cast<size_t>(goal->order)) {
            sequence.push_back(sequence.size());
          }

          size_t sent_feedback = 0;
          while (sent_feedback < static_cast<size_t>(goal->order)) {
            goal_handle->publish_feedback(feedback);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            ++sent_feedback;
          }

          auto result = std::make_shared<Fibonacci::Result>();
          result->sequence = sequence;
          goal_handle->succeed(result);
        });
    };

  // Prepare RCLC
  bool goal_response_received = false;
  handle_goal =
    [&](rclc_action_goal_handle_t * /* goal_handle */, bool accepted, void * /* context */) {
      ASSERT_TRUE(accepted);
      goal_response_received = true;
    };

  size_t feedback_count = 0;
  handle_feedback =
    [&](rclc_action_goal_handle_t * /* goal_handle */, void * ros_feedback, void * /* context */) {
      feedback_count++;

      example_interfaces__action__Fibonacci_FeedbackMessage * feedback =
        reinterpret_cast<example_interfaces__action__Fibonacci_FeedbackMessage *>(ros_feedback);

      ASSERT_EQ(feedback->feedback.sequence.size, (size_t) ros_goal_request.goal.order);
    };

  bool goal_result_received = false;
  handle_result =
    [&](rclc_action_goal_handle_t * /* goal_handle */, void * ros_result_response,
      void * /* context */) {
      goal_result_received = true;

      example_interfaces__action__Fibonacci_GetResult_Response * result =
        reinterpret_cast<example_interfaces__action__Fibonacci_GetResult_Response *>(
        ros_result_response);

      ASSERT_EQ(result->status, action_msgs__msg__GoalStatus__STATUS_SUCCEEDED);
      ASSERT_EQ(result->result.sequence.size, (size_t) ros_goal_request.goal.order);
    };

  rcl_ret_t rc = rclc_action_send_goal_request(&action_client, &ros_goal_request, NULL);
  EXPECT_EQ(RCL_RET_OK, rc);

  while (!goal_response_received || !goal_result_received) {
    rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100));
  }

  feedback_thread.join();

  ASSERT_TRUE(goal_response_received);
  ASSERT_TRUE(goal_result_received);
  ASSERT_EQ(feedback_count, (size_t) ros_goal_request.goal.order);
}

TEST_F(ActionClientTest, goal_accept_feedback_and_abort) {
  example_interfaces__action__Fibonacci_SendGoal_Request ros_goal_request;
  ros_goal_request.goal.order = 10;

  // Prepare RCLCPP

  server_handle_goal = [&](const rclcpp_action::GoalUUID & /* uuid */,
      std::shared_ptr<const Fibonacci::Goal> goal) -> rclcpp_action::GoalResponse {
      EXPECT_EQ(ros_goal_request.goal.order, goal->order);
      return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
    };

  std::thread feedback_thread;

  server_handle_accepted = [&](const std::shared_ptr<GoalHandleFibonacci> goal_handle) -> void {
      feedback_thread = std::thread(
        [ = ]() -> void {
          const auto goal = goal_handle->get_goal();
          auto feedback = std::make_shared<Fibonacci::Feedback>();
          auto & sequence = feedback->sequence;

          while (sequence.size() < static_cast<size_t>(goal->order)) {
            sequence.push_back(sequence.size());
          }

          size_t sent_feedback = 0;
          while (sent_feedback < static_cast<size_t>(goal->order)) {
            goal_handle->publish_feedback(feedback);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            ++sent_feedback;
          }

          auto result = std::make_shared<Fibonacci::Result>();
          result->sequence = sequence;
          goal_handle->abort(result);
        });
    };

  // Prepare RCLC
  bool goal_response_received = false;
  handle_goal =
    [&](rclc_action_goal_handle_t * /* goal_handle */, bool accepted, void * /* context */) {
      ASSERT_TRUE(accepted);
      goal_response_received = true;
    };

  size_t feedback_count = 0;
  handle_feedback =
    [&](rclc_action_goal_handle_t * /* goal_handle */, void * ros_feedback, void * /* context */) {
      feedback_count++;

      example_interfaces__action__Fibonacci_FeedbackMessage * feedback =
        reinterpret_cast<example_interfaces__action__Fibonacci_FeedbackMessage *>(ros_feedback);

      ASSERT_EQ(feedback->feedback.sequence.size, (size_t) ros_goal_request.goal.order);
    };

  bool goal_result_received = false;
  handle_result =
    [&](rclc_action_goal_handle_t * /* goal_handle */, void * ros_result_response,
      void * /* context */) {
      goal_result_received = true;

      example_interfaces__action__Fibonacci_GetResult_Response * result =
        reinterpret_cast<example_interfaces__action__Fibonacci_GetResult_Response *>(
        ros_result_response);

      ASSERT_EQ(result->status, action_msgs__msg__GoalStatus__STATUS_ABORTED);
      ASSERT_EQ(result->result.sequence.size, (size_t) ros_goal_request.goal.order);
    };

  rcl_ret_t rc = rclc_action_send_goal_request(&action_client, &ros_goal_request, NULL);
  EXPECT_EQ(RCL_RET_OK, rc);

  while (!goal_response_received || !goal_result_received) {
    rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100));
  }

  feedback_thread.join();

  ASSERT_TRUE(goal_response_received);
  ASSERT_TRUE(goal_result_received);
  ASSERT_EQ(feedback_count, (size_t) ros_goal_request.goal.order);
}

TEST_F(ActionClientTest, goal_accept_cancel_success) {
  example_interfaces__action__Fibonacci_SendGoal_Request ros_goal_request;
  ros_goal_request.goal.order = 10;

  // Prepare RCLCPP

  server_handle_goal = [&](const rclcpp_action::GoalUUID & /* uuid */,
      std::shared_ptr<const Fibonacci::Goal> goal) -> rclcpp_action::GoalResponse {
      EXPECT_EQ(ros_goal_request.goal.order, goal->order);
      return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
    };

  std::thread feedback_thread;
  bool run_feedback_thread = true;

  server_handle_accepted = [&](const std::shared_ptr<GoalHandleFibonacci> goal_handle) -> void {
      feedback_thread = std::thread(
        [ =, &run_feedback_thread]() -> void {
          const auto goal = goal_handle->get_goal();
          auto feedback = std::make_shared<Fibonacci::Feedback>();
          auto & sequence = feedback->sequence;

          while (sequence.size() < static_cast<size_t>(goal->order)) {
            sequence.push_back(sequence.size());
          }

          while (run_feedback_thread) {
            goal_handle->publish_feedback(feedback);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
          }
        });
    };

  server_handle_cancel =
    [&](const std::shared_ptr<GoalHandleFibonacci>/* goal_handle */) -> rclcpp_action::
    CancelResponse {
      return rclcpp_action::CancelResponse::ACCEPT;
    };

  // Prepare RCLC
  bool goal_response_received = false;
  handle_goal =
    [&](rclc_action_goal_handle_t * /* goal_handle */, bool accepted, void * /* context */) {
      ASSERT_TRUE(accepted);
      goal_response_received = true;
    };

  bool goal_result_received = false;
  handle_result =
    [&](rclc_action_goal_handle_t * /* goal_handle */, void * /* ros_result_response */,
      void * /* context */) {
      goal_result_received = true;
    };

  bool cancel_result_received = false;
  handle_cancel =
    [&](rclc_action_goal_handle_t * /* goal_handle */, bool cancelled, void * /* context */) {
      cancel_result_received = true;
      EXPECT_TRUE(cancelled);
    };

  rclc_action_goal_handle_t * goal_handle;
  rcl_ret_t rc = rclc_action_send_goal_request(&action_client, &ros_goal_request, &goal_handle);
  EXPECT_EQ(RCL_RET_OK, rc);

  while (!goal_response_received) {
    rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100));
  }

  rc = rclc_action_send_cancel_request(goal_handle);
  EXPECT_EQ(RCL_RET_OK, rc);

  while (!cancel_result_received) {
    rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100));
  }

  run_feedback_thread = false;
  feedback_thread.join();

  ASSERT_TRUE(goal_response_received);
  ASSERT_FALSE(goal_result_received);
  ASSERT_TRUE(cancel_result_received);
}

TEST_F(ActionClientTest, goal_accept_cancel_reject) {
  example_interfaces__action__Fibonacci_SendGoal_Request ros_goal_request;
  ros_goal_request.goal.order = 10;

  // Prepare RCLCPP

  server_handle_goal = [&](const rclcpp_action::GoalUUID & /* uuid */,
      std::shared_ptr<const Fibonacci::Goal> goal) -> rclcpp_action::GoalResponse {
      EXPECT_EQ(ros_goal_request.goal.order, goal->order);
      return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
    };

  std::thread feedback_thread;

  server_handle_accepted = [&](const std::shared_ptr<GoalHandleFibonacci> goal_handle) -> void {
      feedback_thread = std::thread(
        [ = ]() -> void {
          const auto goal = goal_handle->get_goal();
          auto feedback = std::make_shared<Fibonacci::Feedback>();
          auto & sequence = feedback->sequence;

          while (sequence.size() < static_cast<size_t>(goal->order)) {
            sequence.push_back(sequence.size());
          }

          size_t sent_feedback = 0;
          while (sent_feedback < static_cast<size_t>(goal->order)) {
            goal_handle->publish_feedback(feedback);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            ++sent_feedback;
          }

          auto result = std::make_shared<Fibonacci::Result>();
          result->sequence = sequence;
          goal_handle->succeed(result);
        });
    };

  server_handle_cancel =
    [&](const std::shared_ptr<GoalHandleFibonacci>/* goal_handle */) -> rclcpp_action::
    CancelResponse {
      return rclcpp_action::CancelResponse::REJECT;
    };

  // Prepare RCLC
  bool goal_response_received = false;
  handle_goal =
    [&](rclc_action_goal_handle_t * /* goal_handle */, bool accepted, void * /* context */) {
      ASSERT_TRUE(accepted);
      goal_response_received = true;
    };

  size_t feedback_count = 0;
  handle_feedback =
    [&](rclc_action_goal_handle_t * /* goal_handle */, void * ros_feedback, void * /* context */) {
      feedback_count++;

      example_interfaces__action__Fibonacci_FeedbackMessage * feedback =
        static_cast<example_interfaces__action__Fibonacci_FeedbackMessage *>(ros_feedback);

      ASSERT_EQ(feedback->feedback.sequence.size, (size_t) ros_goal_request.goal.order);
    };

  bool goal_result_received = false;
  handle_result =
    [&](rclc_action_goal_handle_t * /* goal_handle */, void * ros_result_response,
      void * /* context */) {
      goal_result_received = true;

      example_interfaces__action__Fibonacci_GetResult_Response * result =
        static_cast<example_interfaces__action__Fibonacci_GetResult_Response *>
        (ros_result_response);

      ASSERT_EQ(result->status, action_msgs__msg__GoalStatus__STATUS_SUCCEEDED);
      ASSERT_EQ(result->result.sequence.size, (size_t) ros_goal_request.goal.order);
    };

  bool cancel_result_received = false;
  handle_cancel =
    [&](rclc_action_goal_handle_t * /* goal_handle */, bool cancelled, void * /* context */) {
      cancel_result_received = true;
      EXPECT_FALSE(cancelled);
    };

  rclc_action_goal_handle_t * goal_handle;
  rcl_ret_t rc = rclc_action_send_goal_request(&action_client, &ros_goal_request, &goal_handle);
  EXPECT_EQ(RCL_RET_OK, rc);

  while (!goal_response_received) {
    rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100));
  }

  rc = rclc_action_send_cancel_request(goal_handle);
  EXPECT_EQ(RCL_RET_OK, rc);

  while (!cancel_result_received || !goal_result_received) {
    rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100));
  }

  feedback_thread.join();

  ASSERT_TRUE(goal_response_received);
  ASSERT_TRUE(goal_result_received);
  ASSERT_TRUE(cancel_result_received);
  ASSERT_EQ(feedback_count, (size_t) ros_goal_request.goal.order);
}

bool operator<(
  const unique_identifier_msgs__msg__UUID & lhs,
  const unique_identifier_msgs__msg__UUID & rhs)
{
  uint64_t lhs_high = *(reinterpret_cast<const uint64_t *>(&lhs.uuid[0]));
  uint64_t lhs_low = *(reinterpret_cast<const uint64_t *>(&lhs.uuid[8]));

  uint64_t rhs_high = *(reinterpret_cast<const uint64_t *>(&rhs.uuid[0]));
  uint64_t rhs_low = *(reinterpret_cast<const uint64_t *>(&rhs.uuid[8]));

  return (rhs_high == lhs_high) ? (rhs_low < lhs_low) : (rhs_high < lhs_high);
}

TEST_F(ActionClientTest, multi_goal_accept_feedback_and_result) {
  std::map<unique_identifier_msgs__msg__UUID,
    example_interfaces__action__Fibonacci_SendGoal_Request> ros_goal_request;

  // Prepare RCLCPP
  server_handle_goal = [&](const rclcpp_action::GoalUUID & uuid,
      std::shared_ptr<const Fibonacci::Goal> goal) -> rclcpp_action::GoalResponse {
      EXPECT_EQ(ros_goal_request[get_raw_uuid(uuid)].goal.order, goal->order);

      return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
    };

  std::vector<std::thread> feedback_thread_pool;

  size_t feedback_per_goal = 10;

  server_handle_accepted = [&](const std::shared_ptr<GoalHandleFibonacci> goal_handle) -> void {
      std::thread worker = std::thread(
        [ = ]() -> void {
          const auto goal = goal_handle->get_goal();
          auto feedback = std::make_shared<Fibonacci::Feedback>();
          auto & sequence = feedback->sequence;

          while (sequence.size() < static_cast<size_t>(goal->order)) {
            sequence.push_back(sequence.size());
          }

          size_t sent_feedback = 0;
          while (sent_feedback < feedback_per_goal) {
            goal_handle->publish_feedback(feedback);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            ++sent_feedback;
          }

          auto result = std::make_shared<Fibonacci::Result>();
          result->sequence = sequence;
          goal_handle->succeed(result);
        });

      feedback_thread_pool.push_back(std::move(worker));
    };

  // Prepare RCLC

  std::map<unique_identifier_msgs__msg__UUID, bool> goal_response_received;
  handle_goal = [&](rclc_action_goal_handle_t * goal_handle, bool accepted, void * /* context */) {
      ASSERT_TRUE(accepted);
      goal_response_received[goal_handle->goal_id] = true;
    };

  std::map<unique_identifier_msgs__msg__UUID, size_t> feedback_count;
  handle_feedback =
    [&](rclc_action_goal_handle_t * goal_handle, void * ros_feedback, void * /* context */) {
      feedback_count[goal_handle->goal_id]++;

      example_interfaces__action__Fibonacci_FeedbackMessage * feedback =
        reinterpret_cast<example_interfaces__action__Fibonacci_FeedbackMessage *>(ros_feedback);

      ASSERT_EQ(
        feedback->feedback.sequence.size,
        (size_t) ros_goal_request[goal_handle->goal_id].goal.order);
    };

  std::map<unique_identifier_msgs__msg__UUID, bool> goal_result_received;
  handle_result =
    [&](rclc_action_goal_handle_t * goal_handle, void * ros_result_response, void * /* context */) {
      goal_result_received[goal_handle->goal_id] = true;

      example_interfaces__action__Fibonacci_GetResult_Response * result =
        reinterpret_cast<example_interfaces__action__Fibonacci_GetResult_Response *>(
        ros_result_response);

      ASSERT_EQ(result->status, action_msgs__msg__GoalStatus__STATUS_SUCCEEDED);
      ASSERT_EQ(
        result->result.sequence.size,
        (size_t) ros_goal_request[goal_handle->goal_id].goal.order);
    };

  size_t num_goals = std::floor(RCLC_MAX_GOALS / 2);

  for (size_t i = 0; i < num_goals; i++) {
    example_interfaces__action__Fibonacci_SendGoal_Request req;
    req.goal.order = 10 * (i + 1);

    rclc_action_goal_handle_t * handle;
    rclc_action_send_goal_request(&action_client, &req, &handle);
    ros_goal_request[handle->goal_id] = req;

    goal_response_received[handle->goal_id] = false;
    feedback_count[handle->goal_id] = 0;
    goal_result_received[handle->goal_id] = false;
  }

  auto check_spin = [&]() -> bool {
      for (auto & goal_response_received_pair : goal_response_received) {
        if (!goal_response_received_pair.second) {
          return true;
        }
      }

      for (auto & goal_result_received_pair : goal_result_received) {
        if (!goal_result_received_pair.second) {
          return true;
        }
      }

      return false;
    };

  while (check_spin()) {
    rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100));
  }

  for (auto & thread : feedback_thread_pool) {
    thread.join();
  }

  ASSERT_EQ(goal_response_received.size(), num_goals);
  for (auto const & x : goal_response_received) {
    ASSERT_TRUE(x.second);
  }

  ASSERT_EQ(goal_result_received.size(), num_goals);
  for (auto const & x : goal_result_received) {
    ASSERT_TRUE(x.second);
  }

  ASSERT_EQ(feedback_count.size(), num_goals);
  for (auto const & x : feedback_count) {
    ASSERT_EQ(x.second, feedback_per_goal);
  }
}


// test more than N goals sequentiall
