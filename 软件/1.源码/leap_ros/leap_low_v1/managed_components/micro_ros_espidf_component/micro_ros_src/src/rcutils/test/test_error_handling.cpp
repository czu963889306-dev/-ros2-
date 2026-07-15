// Copyright 2016 Open Source Robotics Foundation, Inc.
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

#include <string>

#include "./allocator_testing_utils.h"
#include "gmock/gmock.h"

#include "osrf_testing_tools_cpp/memory_tools/gtest_quickstart.hpp"
#include "rcutils/error_handling.h"

int
count_substrings(const std::string & str, const std::string & substr)
{
  if (substr.length() == 0) {
    return 0;
  }
  int count = 0;
  for (
    size_t offset = str.find(substr);
    offset != std::string::npos;
    offset = str.find(substr, offset + substr.length()))
  {
    ++count;
  }
  return count;
}

TEST(test_error_handling, nominal) {
  osrf_testing_tools_cpp::memory_tools::ScopedQuickstartGtest scoped_quickstart_gtest(true);

  auto failing_allocator = get_failing_allocator();
  failing_allocator.allocate = NULL;
  EXPECT_EQ(
    RCUTILS_RET_INVALID_ARGUMENT,
    rcutils_initialize_error_handling_thread_local_storage(failing_allocator));

  // Note that without the thread-local initialization function below, you might get errors like:
  /**
[ RUN      ] test_error_handling.nominal
/.../osrf_testing_tools_cpp/memory_tools/gtest_quickstart.hpp:178: Failure
Failed
unexpected call to malloc
test_error_handling(36072,0x7fffb391b380) malloc:  malloc  (not expected) 2040 -> 0x7f9b6a002400
Stack trace (most recent call last):
#7    Object "libdyld.dylib", at 0x7fff7ae09969,
  in tlv_allocate_and_initialize_for_key + 322
#6    Object "libmemory_tools_interpose.dylib",
  at 0x105cee37b,
  in apple_replacement_malloc + 27
#5    Object "libmemory_tools_interpose.dylib",
  at 0x105cee43e,
  in unix_replacement_malloc + 174
#4    Object "libmemory_tools.dylib",
  at 0x105d08732,
  in osrf_testing_tools_cpp::memory_tools::custom_malloc_with_original(
    unsigned long, void* (*)(unsigned long), char const*, bool) + 50
#3    Object "libmemory_tools.dylib",
  at 0x105d0896b,
  in osrf_testing_tools_cpp::memory_tools::custom_malloc_with_original_except(
    unsigned long, void* (*)(unsigned long), char const*, bool) + 395
#2    Object "libmemory_tools.dylib",
  at 0x105d094a1,
  in void osrf_testing_tools_cpp::memory_tools::print_backtrace<64>(__sFILE*) + 49
#1    Object "libmemory_tools.dylib",
  at 0x105d097c5,
  in backward::StackTraceImpl<backward::system_tag::darwin_tag>::load_here(unsigned long) + 101
#0    Object "libmemory_tools.dylib",
  at 0x105d09f26,
  in unsigned long backward::details::unwind<backward::StackTraceImpl<
    backward::system_tag::darwin_tag>::callback>(
      backward::StackTraceImpl<backward::system_tag::darwin_tag>::callback, unsigned long) + 38
[  FAILED  ] test_error_handling.nominal (7 ms)
  */
  // Note that the error is in `tlv_allocate_and_initialize_for_key`, which is the thread-local
  // storage setup function, at least for clang on macOS.
  rcutils_ret_t ret =
    rcutils_initialize_error_handling_thread_local_storage(rcutils_get_default_allocator());
  ASSERT_EQ(ret, RCUTILS_RET_OK);
  EXPECT_NO_MEMORY_OPERATIONS(
  {
    rcutils_reset_error();
  });
  const char * test_message = "test message";
  EXPECT_NO_MEMORY_OPERATIONS(
  {
    RCUTILS_SET_ERROR_MSG(test_message);
  });
  using ::testing::StartsWith;
  EXPECT_NO_MEMORY_OPERATIONS_BEGIN();
  rcutils_error_string_t error_string = rcutils_get_error_string();
  EXPECT_NO_MEMORY_OPERATIONS_END();
  ASSERT_THAT(error_string.str, StartsWith(test_message));
  EXPECT_NO_MEMORY_OPERATIONS(
  {
    rcutils_reset_error();
  });
}

TEST(test_error_handling, reset) {
  osrf_testing_tools_cpp::memory_tools::ScopedQuickstartGtest scoped_quickstart_gtest;
  rcutils_ret_t ret =
    rcutils_initialize_error_handling_thread_local_storage(rcutils_get_default_allocator());
  ASSERT_EQ(ret, RCUTILS_RET_OK);
  EXPECT_NO_MEMORY_OPERATIONS(
  {
    rcutils_reset_error();
  });
  {
    const char * test_message = "test message";
    EXPECT_NO_MEMORY_OPERATIONS(
    {
      RCUTILS_SET_ERROR_MSG(test_message);
    });
    using ::testing::StartsWith;
    EXPECT_NO_MEMORY_OPERATIONS_BEGIN();
    rcutils_error_string_t error_string = rcutils_get_error_string();
    EXPECT_NO_MEMORY_OPERATIONS_END();
    ASSERT_THAT(error_string.str, StartsWith(test_message));
  }
  rcutils_reset_error();
  {
    const char * test_message = "different message";
    EXPECT_NO_MEMORY_OPERATIONS(
    {
      RCUTILS_SET_ERROR_MSG(test_message);
    });
    using ::testing::StartsWith;
    EXPECT_NO_MEMORY_OPERATIONS_BEGIN();
    rcutils_error_string_t error_string = rcutils_get_error_string();
    EXPECT_NO_MEMORY_OPERATIONS_END();
    ASSERT_THAT(error_string.str, StartsWith(test_message));
  }
  EXPECT_NO_MEMORY_OPERATIONS(
  {
    rcutils_reset_error();
  });
  {
    EXPECT_NO_MEMORY_OPERATIONS_BEGIN();
    rcutils_error_string_t error_string = rcutils_get_error_string();
    EXPECT_NO_MEMORY_OPERATIONS_END();
    ASSERT_STRNE("", error_string.str);
  }
  {
    EXPECT_NO_MEMORY_OPERATIONS_BEGIN();
    rcutils_error_string_t error_string = rcutils_get_error_string();
    EXPECT_NO_MEMORY_OPERATIONS_END();
    ASSERT_STREQ("error not set", error_string.str);
  }
  EXPECT_NO_MEMORY_OPERATIONS(
  {
    rcutils_reset_error();
  });
}

TEST(test_error_handling, invalid_arguments) {
  osrf_testing_tools_cpp::memory_tools::ScopedQuickstartGtest scoped_quickstart_gtest;
  rcutils_ret_t ret =
    rcutils_initialize_error_handling_thread_local_storage(rcutils_get_default_allocator());
  ASSERT_EQ(ret, RCUTILS_RET_OK);
  EXPECT_NO_MEMORY_OPERATIONS(
  {
    rcutils_reset_error();
  });
  printf("The following error from within error_handling.c is expected.\n");
  EXPECT_NO_MEMORY_OPERATIONS(
  {
    RCUTILS_SET_ERROR_MSG(NULL);
  });
  EXPECT_FALSE(rcutils_error_is_set());
  {
    EXPECT_NO_MEMORY_OPERATIONS_BEGIN();
    rcutils_error_string_t error_string = rcutils_get_error_string();
    EXPECT_NO_MEMORY_OPERATIONS_END();
    ASSERT_STREQ("error not set", error_string.str);
  }

  printf("The following error from within error_handling.c is expected.\n");
  EXPECT_NO_MEMORY_OPERATIONS(
  {
    rcutils_set_error_state("valid error message", NULL, 42);
  });
  EXPECT_FALSE(rcutils_error_is_set());
  {
    EXPECT_NO_MEMORY_OPERATIONS_BEGIN();
    rcutils_error_string_t error_string = rcutils_get_error_string();
    EXPECT_NO_MEMORY_OPERATIONS_END();
    ASSERT_STREQ("error not set", error_string.str);
  }

  EXPECT_NO_MEMORY_OPERATIONS(
  {
    rcutils_reset_error();
  });
}

TEST(test_error_handling, empty) {
  osrf_testing_tools_cpp::memory_tools::ScopedQuickstartGtest scoped_quickstart_gtest;
  rcutils_ret_t ret =
    rcutils_initialize_error_handling_thread_local_storage(rcutils_get_default_allocator());
  ASSERT_EQ(ret, RCUTILS_RET_OK);
  EXPECT_NO_MEMORY_OPERATIONS(
  {
    rcutils_reset_error();
  });
  {
    EXPECT_NO_MEMORY_OPERATIONS_BEGIN();
    rcutils_error_string_t error_string = rcutils_get_error_string();
    EXPECT_NO_MEMORY_OPERATIONS_END();
    ASSERT_STRNE("", error_string.str);
  }
  {
    EXPECT_NO_MEMORY_OPERATIONS_BEGIN();
    rcutils_error_string_t error_string = rcutils_get_error_string();
    EXPECT_NO_MEMORY_OPERATIONS_END();
    ASSERT_STREQ("error not set", error_string.str);
  }
  EXPECT_NO_MEMORY_OPERATIONS(
  {
    rcutils_reset_error();
  });
}

TEST(test_error_handling, recursive) {
  osrf_testing_tools_cpp::memory_tools::ScopedQuickstartGtest scoped_quickstart_gtest;
  rcutils_ret_t ret =
    rcutils_initialize_error_handling_thread_local_storage(rcutils_get_default_allocator());
  ASSERT_EQ(ret, RCUTILS_RET_OK);
  EXPECT_NO_MEMORY_OPERATIONS(
  {
    rcutils_reset_error();
  });
  const char * test_message = "test message";
  EXPECT_NO_MEMORY_OPERATIONS(
  {
    RCUTILS_SET_ERROR_MSG(test_message);
  });
  using ::testing::HasSubstr;
  {
    EXPECT_NO_MEMORY_OPERATIONS_BEGIN();
    rcutils_error_string_t error_string = rcutils_get_error_string();
    EXPECT_NO_MEMORY_OPERATIONS_END();
    ASSERT_THAT(error_string.str, HasSubstr(", at"));
  }
  EXPECT_NO_MEMORY_OPERATIONS(
  {
    RCUTILS_SET_ERROR_MSG(rcutils_get_error_string().str);
  });
  std::string err_msg;
  {
    EXPECT_NO_MEMORY_OPERATIONS_BEGIN();
    rcutils_error_string_t error_string = rcutils_get_error_string();
    EXPECT_NO_MEMORY_OPERATIONS_END();
    err_msg = error_string.str;
  }
  int count = count_substrings(err_msg, ", at");
  EXPECT_EQ(2, count) <<
    "Expected ', at' in the error string twice but got it '" << count << "': " << err_msg;
  rcutils_reset_error();
}

TEST(test_error_handling, copy) {
  osrf_testing_tools_cpp::memory_tools::ScopedQuickstartGtest scoped_quickstart_gtest;
  rcutils_ret_t ret =
    rcutils_initialize_error_handling_thread_local_storage(rcutils_get_default_allocator());
  ASSERT_EQ(ret, RCUTILS_RET_OK);
  EXPECT_NO_MEMORY_OPERATIONS(
  {
    rcutils_reset_error();
  });
  const char * test_message = "test message";
  EXPECT_NO_MEMORY_OPERATIONS(
  {
    RCUTILS_SET_ERROR_MSG(test_message);
  });
  using ::testing::HasSubstr;
  EXPECT_NO_MEMORY_OPERATIONS_BEGIN();
  rcutils_error_string_t error_string = rcutils_get_error_string();
  EXPECT_NO_MEMORY_OPERATIONS_END();
  ASSERT_THAT(error_string.str, HasSubstr(", at"));
  EXPECT_NO_MEMORY_OPERATIONS_BEGIN();
  rcutils_error_state_t copy = *rcutils_get_error_state();
  EXPECT_NO_MEMORY_OPERATIONS_END();
  // ensure the copy is correct
  ASSERT_STREQ(test_message, copy.message);
  ASSERT_STREQ(__FILE__, copy.file);
  // ensure the original is still right too
  EXPECT_NO_MEMORY_OPERATIONS_BEGIN();
  const rcutils_error_state_t * error_state = rcutils_get_error_state();
  EXPECT_NO_MEMORY_OPERATIONS_END();
  ASSERT_NE(nullptr, error_state);
  ASSERT_STREQ(test_message, error_state->message);
  EXPECT_NO_MEMORY_OPERATIONS(
  {
    rcutils_reset_error();
  });
}

TEST(test_error_handling, overwrite) {
  osrf_testing_tools_cpp::memory_tools::ScopedQuickstartGtest scoped_quickstart_gtest;
  rcutils_ret_t ret =
    rcutils_initialize_error_handling_thread_local_storage(rcutils_get_default_allocator());
  ASSERT_EQ(ret, RCUTILS_RET_OK);
  EXPECT_NO_MEMORY_OPERATIONS(
  {
    rcutils_reset_error();
  });
  const char * test_message = "this is expected to cause a warning from error_handling.c";
  EXPECT_NO_MEMORY_OPERATIONS(
  {
    RCUTILS_SET_ERROR_MSG(test_message);
  });
  using ::testing::HasSubstr;
  EXPECT_NO_MEMORY_OPERATIONS_BEGIN();
  rcutils_error_string_t error_string = rcutils_get_error_string();
  EXPECT_NO_MEMORY_OPERATIONS_END();
  ASSERT_THAT(error_string.str, HasSubstr(", at"));

  // force an overwrite error
  const char * test_message2 = "and this too";
  printf("The following warning from error_handling.c is expected...\n");
  EXPECT_NO_MEMORY_OPERATIONS(
  {
    RCUTILS_SET_ERROR_MSG(test_message2);
  });

  EXPECT_NO_MEMORY_OPERATIONS(
  {
    rcutils_reset_error();
  });
}
