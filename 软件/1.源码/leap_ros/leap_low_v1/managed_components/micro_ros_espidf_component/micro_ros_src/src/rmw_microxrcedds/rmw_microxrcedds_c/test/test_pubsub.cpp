// Copyright 2018 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

#include <memory>
#include <string>
#include <chrono>
#include <thread>
#include <vector>

#include "rmw/error_handling.h"
#include "rmw/rmw.h"
#include "rmw/validate_namespace.h"
#include "rmw/validate_node_name.h"

#include "./rmw_base_test.hpp"
#include "./test_utils.hpp"

#include "rosidl_runtime_c/string.h"

#define MICROXRCEDDS_PADDING sizeof(uint32_t)

class TestPubSub : public ::testing::Test
{
public:
  void SetUp() override
  {
    ASSERT_EQ(rmw_init_options_init(&options_pub, rcutils_get_default_allocator()), RMW_RET_OK);
    ASSERT_EQ(rmw_init(&options_pub, &context_pub), RMW_RET_OK);

    ASSERT_EQ(rmw_init_options_init(&options_sub, rcutils_get_default_allocator()), RMW_RET_OK);
    ASSERT_EQ(rmw_init(&options_sub, &context_sub), RMW_RET_OK);

    configure_typesupport();

    node_pub = rmw_create_node(&context_pub, "node_pub", "/ns");
    node_sub = rmw_create_node(&context_sub, "node_sub", "/ns");

    EXPECT_NE(node_pub, nullptr);
    EXPECT_NE(node_sub, nullptr);
  }

  void TearDown() override
  {
    for (auto pub : publishers) {
      EXPECT_EQ(rmw_destroy_publisher(node_pub, pub), RMW_RET_OK);
    }

    for (auto sub : subscribers) {
      EXPECT_EQ(rmw_destroy_subscription(node_sub, sub), RMW_RET_OK);
    }

    EXPECT_EQ(rmw_destroy_node(node_pub), RMW_RET_OK);
    EXPECT_EQ(rmw_destroy_node(node_sub), RMW_RET_OK);

    ASSERT_EQ(rmw_init_options_fini(&options_pub), RMW_RET_OK);
    ASSERT_EQ(rmw_init_options_fini(&options_sub), RMW_RET_OK);

    ASSERT_EQ(rmw_shutdown(&context_pub), RMW_RET_OK);
    ASSERT_EQ(rmw_shutdown(&context_sub), RMW_RET_OK);
  }

  void configure_typesupport()
  {
    ConfigureDummyTypeSupport(
      topic_type,
      topic_name,
      message_namespace,
      id_gen++,
      &dummy_type_support);

    dummy_type_support.callbacks.cdr_serialize =
      [](const void * untyped_ros_message, ucdrBuffer * cdr) -> bool {
        EXPECT_NE(untyped_ros_message, nullptr);
        const rosidl_runtime_c__String * ros_message =
          reinterpret_cast<const rosidl_runtime_c__String *>(untyped_ros_message);

        bool ret = ucdr_serialize_string(cdr, ros_message->data);
        EXPECT_TRUE(ret);
        return ret;
      };

    dummy_type_support.callbacks.cdr_deserialize =
      [](ucdrBuffer * cdr, void * untyped_ros_message) -> bool {
        EXPECT_NE(untyped_ros_message, nullptr);
        rosidl_runtime_c__String * ros_message =
          reinterpret_cast<rosidl_runtime_c__String *>(untyped_ros_message);

        bool ret = ucdr_deserialize_string(cdr, ros_message->data, ros_message->capacity);
        if (ret) {
          ros_message->size = strlen(ros_message->data);
        }
        EXPECT_TRUE(ret);
        return ret;
      };
    dummy_type_support.callbacks.get_serialized_size =
      [](const void * untyped_ros_message) -> uint32_t {
        EXPECT_NE(untyped_ros_message, nullptr);
        const rosidl_runtime_c__String * ros_message =
          reinterpret_cast<const rosidl_runtime_c__String *>(untyped_ros_message);

        return MICROXRCEDDS_PADDING +
               ucdr_alignment(0, MICROXRCEDDS_PADDING) + ros_message->size + 8;
      };
    dummy_type_support.callbacks.max_serialized_size =
      []() -> size_t {
        return static_cast<size_t>(MICROXRCEDDS_PADDING +
               ucdr_alignment(0, MICROXRCEDDS_PADDING) + 1);
      };
  }

  rmw_publisher_t * create_publisher(const rmw_qos_profile_t qos)
  {
    rmw_publisher_options_t default_publisher_options = rmw_get_default_publisher_options();
    rmw_publisher_t * pub = rmw_create_publisher(
      node_pub, &dummy_type_support.type_support, topic_name,
      &qos, &default_publisher_options);
    EXPECT_NE(pub, nullptr);
    publishers.push_back(pub);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return pub;
  }

  rmw_subscription_t * create_subscriber(const rmw_qos_profile_t qos)
  {
    rmw_subscription_options_t default_subscription_options =
      rmw_get_default_subscription_options();
    rmw_subscription_t * sub = rmw_create_subscription(
      node_sub, &dummy_type_support.type_support,
      topic_name, &qos,
      &default_subscription_options);
    EXPECT_NE(sub, nullptr);
    subscribers.push_back(sub);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return sub;
  }

  void publish_string(const char * data, rmw_publisher_t * pub)
  {
    rosidl_runtime_c__String ros_message;
    ros_message.data = const_cast<char *>(data);
    ros_message.capacity = strlen(ros_message.data);
    ros_message.size = ros_message.capacity;

    EXPECT_EQ(rmw_publish(pub, &ros_message, NULL), RMW_RET_OK);
  }

  rmw_ret_t wait_for_subscription(rmw_subscription_t * sub)
  {
    rmw_subscriptions_t subscriptions;
    void * subs[1] = {sub->data};
    subscriptions.subscribers = subs;
    subscriptions.subscriber_count = 1;

    rmw_time_t wait_timeout = (rmw_time_t) {2LL, 1LL};

    return rmw_wait(&subscriptions, NULL, NULL, NULL, NULL, NULL, &wait_timeout);
  }

  rmw_ret_t take_from_subscription(
    rmw_subscription_t * sub, char * buff, size_t buff_size,
    bool & taken)
  {
    rosidl_runtime_c__String read_ros_message;
    read_ros_message.data = buff;
    read_ros_message.capacity = buff_size;
    read_ros_message.size = 0;

    return rmw_take_with_info(sub, &read_ros_message, &taken, NULL, NULL);
  }

protected:
  size_t id_gen = 0;

  const char * topic_type = "topic_type";
  const char * topic_name = "topic_name";
  const char * message_namespace = "package_name";

  dummy_type_support_t dummy_type_support;

  rmw_context_t context_pub = rmw_get_zero_initialized_context();
  rmw_init_options_t options_pub = rmw_get_zero_initialized_init_options();
  rmw_node_t * node_pub;

  rmw_context_t context_sub = rmw_get_zero_initialized_context();
  rmw_init_options_t options_sub = rmw_get_zero_initialized_init_options();
  rmw_node_t * node_sub;

  std::vector<rmw_publisher_t *> publishers;
  std::vector<rmw_subscription_t *> subscribers;
};

TEST_F(TestPubSub, publish_and_receive)
{
  rmw_publisher_t * pub = create_publisher(rmw_qos_profile_default);
  rmw_subscription_t * sub = create_subscriber(rmw_qos_profile_default);

  std::string send_data = "hello";
  publish_string(send_data.c_str(), pub);

  EXPECT_EQ(wait_for_subscription(sub), RMW_RET_OK);

  bool taken = false;
  char recv_data[100];
  ASSERT_EQ(take_from_subscription(sub, recv_data, sizeof(recv_data), taken), RMW_RET_OK);

  ASSERT_TRUE(taken);
  ASSERT_EQ(strcmp(send_data.c_str(), recv_data), 0);
}

TEST_F(TestPubSub, wait_timeout)
{
  rmw_subscription_t * sub = create_subscriber(rmw_qos_profile_default);

  EXPECT_EQ(wait_for_subscription(sub), RMW_RET_TIMEOUT);

  bool taken = false;
  char recv_data[100] = {0};
  ASSERT_EQ(take_from_subscription(sub, recv_data, sizeof(recv_data), taken), RMW_RET_ERROR);

  ASSERT_FALSE(taken);
}

TEST_F(TestPubSub, take_expired)
{
  rmw_publisher_t * pub = create_publisher(rmw_qos_profile_default);

  rmw_qos_profile_t qos = rmw_qos_profile_default;
  qos.lifespan = (rmw_time_t) {1LL, 0LL};
  rmw_subscription_t * sub = create_subscriber(qos);

  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  std::string send_data = "hello";
  publish_string(send_data.c_str(), pub);

  EXPECT_EQ(wait_for_subscription(sub), RMW_RET_OK);

  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  bool taken = false;
  char recv_data[100] = {0};
  ASSERT_EQ(take_from_subscription(sub, recv_data, sizeof(recv_data), taken), RMW_RET_ERROR);

  ASSERT_FALSE(taken);
  ASSERT_NE(strcmp(send_data.c_str(), recv_data), 0);
}

TEST_F(TestPubSub, take_expired_two_subscriber)
{
  rmw_publisher_t * pub = create_publisher(rmw_qos_profile_default);

  rmw_subscription_t * sub_1 = create_subscriber(rmw_qos_profile_default);

  rmw_qos_profile_t qos = rmw_qos_profile_default;
  qos.lifespan = (rmw_time_t) {1LL, 0LL};
  rmw_subscription_t * sub_2 = create_subscriber(qos);

  std::string send_data = "hello";
  publish_string(send_data.c_str(), pub);

  bool received_1 = false, received_2 = false;
  size_t iterations = 0;
  while ((!received_1 || !received_2) && iterations < 100) {
    rmw_subscriptions_t subscriptions;
    void * subs[] = {sub_1->data, sub_2->data};
    subscriptions.subscribers = subs;
    subscriptions.subscriber_count = 2;
    rmw_time_t wait_timeout = (rmw_time_t) {0LL, 1000000LL};
    (void) !rmw_wait(&subscriptions, NULL, NULL, NULL, NULL, NULL, &wait_timeout);
    received_1 = subscriptions.subscribers[0] != NULL;
    received_2 = subscriptions.subscribers[1] != NULL;
    iterations++;
  }

  bool taken = false;
  char recv_data[100];
  EXPECT_EQ(take_from_subscription(sub_1, recv_data, sizeof(recv_data), taken), RMW_RET_OK);

  EXPECT_TRUE(taken);
  EXPECT_EQ(strcmp(send_data.c_str(), recv_data), 0);

  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  taken = false;
  memset(recv_data, 0, sizeof(recv_data));
  EXPECT_EQ(take_from_subscription(sub_2, recv_data, sizeof(recv_data), taken), RMW_RET_ERROR);

  EXPECT_FALSE(taken);
  EXPECT_NE(strcmp(send_data.c_str(), recv_data), 0);
}

TEST_F(TestPubSub, take_order_with_expired)
{
  rmw_publisher_t * pub = create_publisher(rmw_qos_profile_default);

  rmw_qos_profile_t qos = rmw_qos_profile_default;
  qos.history = RMW_QOS_POLICY_HISTORY_KEEP_ALL;
  qos.depth = 0;
  qos.lifespan = (rmw_time_t) {0LL, 500000000LL};
  rmw_subscription_t * sub = create_subscriber(qos);

  for (size_t i = 0; i < 5; i++) {
    std::string send_data = "hello_" + std::to_string(i);
    publish_string(send_data.c_str(), pub);
  }

  for (size_t i = 0; i < 5; i++) {
    EXPECT_EQ(wait_for_subscription(sub), RMW_RET_OK);
  }

  for (size_t i = 0; i < 2; i++) {
    bool taken = false;
    char recv_data[100] = {0};
    EXPECT_EQ(take_from_subscription(sub, recv_data, sizeof(recv_data), taken), RMW_RET_OK);

    EXPECT_TRUE(taken);
    std::string send_data = "hello_" + std::to_string(i);
    EXPECT_EQ(strcmp(send_data.c_str(), recv_data), 0);
    std::cout << "recv_data: " << recv_data << std::endl;
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  for (int i = 2; i < 5; i++) {
    bool taken = false;
    char recv_data[100] = {0};
    EXPECT_EQ(take_from_subscription(sub, recv_data, sizeof(recv_data), taken), RMW_RET_ERROR);

    EXPECT_FALSE(taken);
    std::string send_data = "hello_" + std::to_string(i);
    EXPECT_NE(strcmp(send_data.c_str(), recv_data), 0);
    std::cout << "expired recv_data: " << send_data << std::endl;
  }
}

TEST_F(TestPubSub, subscriber_depth_keep_all)
{
  rmw_publisher_t * pub = create_publisher(rmw_qos_profile_default);

  rmw_qos_profile_t qos = rmw_qos_profile_default;
  qos.history = RMW_QOS_POLICY_HISTORY_KEEP_ALL;
  qos.depth = 4;
  rmw_subscription_t * sub = create_subscriber(qos);

  for (size_t i = 0; i < 10; i++) {
    std::string send_data = "hello_" + std::to_string(i);
    publish_string(send_data.c_str(), pub);
  }

  for (size_t i = 0; i < 10; i++) {
    wait_for_subscription(sub);
  }

  for (size_t i = 0; i < 10; i++) {
    bool taken = false;
    char recv_data[100] = {0};
    EXPECT_EQ(
      take_from_subscription(
        sub, recv_data, sizeof(recv_data),
        taken), (i < qos.depth) ? RMW_RET_OK : RMW_RET_ERROR);

    EXPECT_EQ(taken, (i < qos.depth) ? true : false);
    std::string send_data = "hello_" + std::to_string(i);
    EXPECT_EQ(0 == strcmp(send_data.c_str(), recv_data), (i < qos.depth) ? true : false);
  }
}

TEST_F(TestPubSub, subscriber_depth_keep_last)
{
  rmw_publisher_t * pub = create_publisher(rmw_qos_profile_default);

  rmw_qos_profile_t qos = rmw_qos_profile_default;
  qos.history = RMW_QOS_POLICY_HISTORY_KEEP_LAST;
  qos.depth = 4;
  rmw_subscription_t * sub = create_subscriber(qos);

  size_t sent_topics = 10;
  for (size_t i = 0; i < sent_topics; i++) {
    std::string send_data = "hello_" + std::to_string(i);
    publish_string(send_data.c_str(), pub);
  }

  for (size_t i = 0; i < qos.depth; i++) {
    EXPECT_EQ(wait_for_subscription(sub), RMW_RET_OK);
  }

  for (size_t i = 0; i < sent_topics; i++) {
    size_t expected_no = (sent_topics - qos.depth) + i;
    bool taken = false;
    char recv_data[100] = {0};
    EXPECT_EQ(
      take_from_subscription(
        sub, recv_data, sizeof(recv_data),
        taken), (i < qos.depth) ? RMW_RET_OK : RMW_RET_ERROR);

    EXPECT_EQ(taken, (i < qos.depth) ? true : false);
    std::string send_data = "hello_" + std::to_string(expected_no);
    EXPECT_EQ(0 == strcmp(send_data.c_str(), recv_data), (i < qos.depth) ? true : false);
  }
}
