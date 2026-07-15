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

#include "gmock/gmock.h"
#include "rmw/allocators.h"

TEST(test_rmw_allocators, rmw_allocate_free) {
  void * ptr = rmw_allocate(100u);
  EXPECT_NE(ptr, nullptr);
  rmw_free(ptr);
}

TEST(test_rmw_allocators, rmw_node_allocate_free) {
  rmw_node_t * node = rmw_node_allocate();
  EXPECT_NE(node, nullptr);
  rmw_node_free(node);
}

TEST(test_rmw_allocators, rmw_publisher_allocate_free) {
  rmw_publisher_t * publisher = rmw_publisher_allocate();
  EXPECT_NE(publisher, nullptr);
  rmw_publisher_free(publisher);
}

TEST(test_rmw_allocators, rmw_subscription_allocate_free) {
  rmw_subscription_t * subscriber = rmw_subscription_allocate();
  EXPECT_NE(subscriber, nullptr);
  rmw_subscription_free(subscriber);
}

TEST(test_rmw_allocators, rmw_guard_condition_allocate_free) {
  rmw_guard_condition_t * guard_condition = rmw_guard_condition_allocate();
  EXPECT_NE(guard_condition, nullptr);
  rmw_guard_condition_free(guard_condition);
}

TEST(test_rmw_allocators, rmw_client_allocate_free) {
  rmw_client_t * client = rmw_client_allocate();
  EXPECT_NE(client, nullptr);
  rmw_client_free(client);
}

TEST(test_rmw_allocators, rmw_service_allocate_free) {
  rmw_service_t * service = rmw_service_allocate();
  EXPECT_NE(service, nullptr);
  rmw_service_free(service);
}

TEST(test_rmw_allocators, rmw_wait_set_allocate_free) {
  rmw_wait_set_t * wait_set = rmw_wait_set_allocate();
  EXPECT_NE(wait_set, nullptr);
  rmw_wait_set_free(wait_set);
}
