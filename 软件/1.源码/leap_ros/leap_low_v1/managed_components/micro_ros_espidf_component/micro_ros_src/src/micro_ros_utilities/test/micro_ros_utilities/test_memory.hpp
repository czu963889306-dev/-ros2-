// Copyright (c) 2021 - for information on the respective copyright owner
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

#ifndef MICRO_ROS_UTILITIES__TEST_MEMORY_HPP_
#define MICRO_ROS_UTILITIES__TEST_MEMORY_HPP_

#include <gtest/gtest.h>

#include <micro_ros_utilities/type_utilities.h>
#include <micro_ros_utilities/string_utilities.h>

#include <utility>
#include <string>
#include <vector>
#include <map>

class MicroROSTypeUtilities : public ::testing::Test
{
public:
  MicroROSTypeUtilities()
  : dyn_msg_(nullptr),
    static_buffer_(nullptr),
    allocated_memory_map_()
  {}

  ~MicroROSTypeUtilities() {}

  void SetUp() override
  {
    allocators_ = {
      allocate,
      deallocate,
      reallocate,
      zero_allocate,
      static_cast<void *>(this)
    };

    conf_.allocator = &allocators_;

    ASSERT_EQ(allocated_memory(), 0UL);
  }

  void TearDown() override
  {
    if (static_buffer_ != nullptr) {
      free(static_buffer_);
    }

    // Ensure not memory leak in memory handling
    ASSERT_EQ(allocated_memory(), 0UL);

    // Ensure that realloc and calloc are not used;
    ASSERT_EQ(reallocate_count, 0UL);
    ASSERT_EQ(zero_allocate_count, 0UL);
  }

  // Handle messages
  template<typename T>
  bool create_memory_dynamic(T * msg, rcutils_allocator_t * override_allocators = nullptr)
  {
    size_t initial_allocated_memory = allocated_memory();

    if (override_allocators != nullptr) {
      conf_.allocator = override_allocators;
    }

    allow_dynamic_operations_ = true;
    EXPECT_NE(msg, nullptr);
    EXPECT_EQ(dyn_msg_, nullptr);

    dyn_msg_ = msg;

    size_t size = micro_ros_utilities_get_dynamic_size(
      typesupport_,
      conf_
    );

    bool ret = micro_ros_utilities_create_message_memory(
      typesupport_,
      msg,
      conf_);

    if (override_allocators == nullptr) {
      EXPECT_EQ(allocated_memory() - initial_allocated_memory, size);
    }

    return ret;
  }

  void destroy_memory_dynamic()
  {
    if (dyn_msg_ != nullptr) {
      allow_dynamic_operations_ = true;
      ASSERT_TRUE(
        micro_ros_utilities_destroy_message_memory(
          typesupport_,
          dyn_msg_,
          conf_)
      );
    }
  }

  template<typename T>
  bool create_memory_static(T * msg)
  {
    allow_dynamic_operations_ = rules_.size() > 0;
    EXPECT_NE(msg, nullptr);

    size_t size = micro_ros_utilities_get_static_size(
      typesupport_,
      conf_
    );

    static_buffer_ = static_cast<uint8_t *>(malloc(size));

    return micro_ros_utilities_create_static_message_memory(
      typesupport_,
      msg,
      conf_,
      static_buffer_,
      size);
  }

  // Handle rules
  void add_rule(const char * name, const size_t size)
  {
    micro_ros_utilities_memory_rule_t rule = {name, size};
    rules_.push_back(std::move(rule));
    conf_.rules = rules_.data();
    conf_.n_rules = rules_.size();
  }

  size_t get_rule_size(const std::string name)
  {
    for (const auto & rule : rules_) {
      if (strcmp(rule.rule, name.c_str()) == 0) {
        return rule.size;
      }
    }
    return 0;
  }

  size_t allocated_memory()
  {
    size_t allocated_memory = 0;
    for (auto const & x : allocated_memory_map_) {
      allocated_memory += x.second;
    }
    return allocated_memory;
  }

  // Allocators

  static void * allocate(size_t size, void * state)
  {
    MicroROSTypeUtilities * self = static_cast<MicroROSTypeUtilities *>(state);
    EXPECT_TRUE(self->allow_dynamic_operations_);

    void * ptr = malloc(size);
    self->allocated_memory_map_.emplace(std::make_pair(ptr, size));
    self->allocate_count++;

    return ptr;
  }

  static void deallocate(void * pointer, void * state)
  {
    MicroROSTypeUtilities * self = static_cast<MicroROSTypeUtilities *>(state);
    EXPECT_TRUE(self->allow_dynamic_operations_);

    auto it = self->allocated_memory_map_.find(pointer);
    self->allocated_memory_map_.erase(it);
    self->deallocate_count++;

    free(pointer);
  }

  static void * reallocate(void * pointer, size_t size, void * state)
  {
    MicroROSTypeUtilities * self = static_cast<MicroROSTypeUtilities *>(state);
    EXPECT_TRUE(self->allow_dynamic_operations_);

    auto it = self->allocated_memory_map_.find(pointer);
    self->allocated_memory_map_.erase(it);
    void * ptr = realloc(pointer, size);
    self->allocated_memory_map_.emplace(std::pair<void *, size_t>(ptr, size));
    self->reallocate_count++;

    return ptr;
  }

  static void * zero_allocate(
    size_t number_of_elements, size_t size_of_element,
    void * state)
  {
    MicroROSTypeUtilities * self = static_cast<MicroROSTypeUtilities *>(state);
    EXPECT_TRUE(self->allow_dynamic_operations_);

    void * ptr = calloc(number_of_elements, size_of_element);
    self->allocated_memory_map_.emplace(
      std::pair<void *, size_t>(
        ptr,
        number_of_elements * size_of_element));
    self->zero_allocate_count++;

    return ptr;
  }

protected:
  const rosidl_message_type_support_t * typesupport_ = {};
  rcutils_allocator_t allocators_ = {};
  micro_ros_utilities_memory_conf_t conf_ = {};
  std::vector<micro_ros_utilities_memory_rule_t> rules_ = {};
  void * dyn_msg_ = {};
  uint8_t * static_buffer_ = {};

  bool allow_dynamic_operations_ = false;

  std::map<void *, size_t> allocated_memory_map_;

  size_t allocate_count = 0;
  size_t deallocate_count = 0;
  size_t reallocate_count = 0;
  size_t zero_allocate_count = 0;
};

#endif  // MICRO_ROS_UTILITIES__TEST_MEMORY_HPP_
