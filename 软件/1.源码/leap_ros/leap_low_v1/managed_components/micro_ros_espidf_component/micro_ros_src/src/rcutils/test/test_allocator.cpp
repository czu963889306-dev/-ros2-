// Copyright 2015 Open Source Robotics Foundation, Inc.
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

#include "./allocator_testing_utils.h"
#include "rcutils/allocator.h"
#include "rcutils/testing/fault_injection.h"

#include "osrf_testing_tools_cpp/memory_tools/memory_tools.hpp"
#include "osrf_testing_tools_cpp/scope_exit.hpp"

#ifdef RMW_IMPLEMENTATION
# define CLASSNAME_(NAME, SUFFIX) NAME ## __ ## SUFFIX
# define CLASSNAME(NAME, SUFFIX) CLASSNAME_(NAME, SUFFIX)
#else
# define CLASSNAME(NAME, SUFFIX) NAME
#endif

using osrf_testing_tools_cpp::memory_tools::disable_monitoring_in_all_threads;
using osrf_testing_tools_cpp::memory_tools::enable_monitoring_in_all_threads;
using osrf_testing_tools_cpp::memory_tools::on_unexpected_calloc;
using osrf_testing_tools_cpp::memory_tools::on_unexpected_free;
using osrf_testing_tools_cpp::memory_tools::on_unexpected_malloc;
using osrf_testing_tools_cpp::memory_tools::on_unexpected_realloc;

class CLASSNAME (TestAllocatorFixture, RMW_IMPLEMENTATION) : public ::testing::Test
{
public:
  CLASSNAME(TestAllocatorFixture, RMW_IMPLEMENTATION)() {}

  void SetUp()
  {
    osrf_testing_tools_cpp::memory_tools::initialize();
    enable_monitoring_in_all_threads();
  }

  void TearDown()
  {
    disable_monitoring_in_all_threads();
    osrf_testing_tools_cpp::memory_tools::uninitialize();
  }
};

/* Tests the default allocator.
 */
TEST_F(CLASSNAME(TestAllocatorFixture, RMW_IMPLEMENTATION), test_default_allocator_normal) {
  size_t mallocs = 0;
  size_t reallocs = 0;
  size_t callocs = 0;
  size_t frees = 0;
  on_unexpected_malloc([&mallocs]() {mallocs++;});
  on_unexpected_realloc([&reallocs]() {reallocs++;});
  on_unexpected_calloc([&callocs]() {callocs++;});
  on_unexpected_free([&frees]() {frees++;});

  rcutils_allocator_t allocator;
  EXPECT_NO_MEMORY_OPERATIONS(
  {
    allocator = rcutils_get_default_allocator();
  });
  EXPECT_EQ(0u, mallocs);
  EXPECT_EQ(0u, reallocs);
  EXPECT_EQ(0u, callocs);
  EXPECT_EQ(0u, frees);

  void * allocated_memory = nullptr;
  EXPECT_NO_MEMORY_OPERATIONS(
  {
    allocated_memory = allocator.allocate(1024, allocator.state);
  });
  EXPECT_EQ(1u, mallocs);
  EXPECT_NE(nullptr, allocated_memory);
  EXPECT_NO_MEMORY_OPERATIONS(
  {
    allocated_memory = allocator.reallocate(allocated_memory, 2048, allocator.state);
  });
  EXPECT_EQ(1u, reallocs);
  EXPECT_NE(nullptr, allocated_memory);
  EXPECT_NO_MEMORY_OPERATIONS(
  {
    allocator.deallocate(allocated_memory, allocator.state);
    allocated_memory = allocator.zero_allocate(1024, sizeof(void *), allocator.state);
  });
  EXPECT_EQ(1u, callocs);
  EXPECT_NE(nullptr, allocated_memory);
  EXPECT_NO_MEMORY_OPERATIONS(
  {
    allocator.deallocate(allocated_memory, allocator.state);
  });
  EXPECT_EQ(1u, mallocs);
  EXPECT_EQ(1u, reallocs);
  EXPECT_EQ(1u, callocs);
  EXPECT_EQ(2u, frees);
}

TEST(test_allocator, realloc_failing_allocators) {
  void * allocated_memory = nullptr;
  EXPECT_EQ(nullptr, rcutils_reallocf(allocated_memory, 1024, nullptr));

  auto failing_allocator = get_failing_allocator();
  EXPECT_EQ(nullptr, rcutils_reallocf(allocated_memory, 1024, &failing_allocator));
}

TEST(test_allocator, default_allocator_maybe_fail) {
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  // Check allocate
  rcutils_fault_injection_set_count(RCUTILS_FAULT_INJECTION_FAIL_NOW);
  EXPECT_EQ(nullptr, allocator.allocate(1u, allocator.state));
  EXPECT_EQ(RCUTILS_FAULT_INJECTION_NEVER_FAIL, rcutils_fault_injection_get_count());

  void * pointer = allocator.allocate(1u, allocator.state);
  ASSERT_NE(nullptr, pointer);
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    allocator.deallocate(pointer, allocator.state);
  });

  // Check reallocate
  rcutils_fault_injection_set_count(RCUTILS_FAULT_INJECTION_FAIL_NOW);
  EXPECT_EQ(nullptr, allocator.reallocate(pointer, 1u, allocator.state));
  EXPECT_EQ(RCUTILS_FAULT_INJECTION_NEVER_FAIL, rcutils_fault_injection_get_count());

  // Check zero_allocate
  rcutils_fault_injection_set_count(RCUTILS_FAULT_INJECTION_FAIL_NOW);
  EXPECT_EQ(nullptr, allocator.zero_allocate(1u, 1u, allocator.state));
  EXPECT_EQ(RCUTILS_FAULT_INJECTION_NEVER_FAIL, rcutils_fault_injection_get_count());
}
