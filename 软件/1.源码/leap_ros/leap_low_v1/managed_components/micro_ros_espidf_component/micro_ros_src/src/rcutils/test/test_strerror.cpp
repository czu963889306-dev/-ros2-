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

#include <gtest/gtest.h>

#include <sys/types.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <dirent.h>
#endif

#include "rcutils/macros.h"
#include "rcutils/strerror.h"

#include "mimick/mimick.h"

TEST(test_strerror, get_error) {
  // cleaning possible errors
  errno = 0;

  char error_string[1024];
  rcutils_strerror(error_string, sizeof(error_string));
#ifdef _WIN32
  ASSERT_STREQ(error_string, "No error");
#elif __APPLE__
  ASSERT_STREQ(error_string, "Undefined error: 0");
#else
  ASSERT_STREQ(error_string, "Success");
#endif
  // Set the error "No such file or directory"
  errno = 2;

  rcutils_strerror(error_string, sizeof(error_string));
  ASSERT_STREQ(error_string, "No such file or directory");

#if (!defined(_WIN32)) && (!( \
    defined(_GNU_SOURCE) && (!defined(ANDROID) || __ANDROID_API__ >= 23)))
  // Hopefully this does not become a valid errno.
  errno = 12345;
  rcutils_strerror(error_string, sizeof(error_string));
  ASSERT_STREQ(error_string, "Failed to get error") <<
    "Calling rcutils_strerror with an errno of '" << errno <<
    "' did not cause the expected error message.";
#endif
}

/*
   Define the blueprint of a mock identified by `strerror_r_proto`
   strerror_r possible signatures:

   * int strerror_r(int errnum, char *buf, size_t buflen); (Case 1)
   * char *strerror_r(int errnum, char *buf, size_t buflen); (Case 2)
   * errno_t strerror_s( char *buf, rsize_t bufsz, errno_t errnum ); (Case 3)
*/

#if defined(_WIN32)
const char expected_error_msg[] = "Failed to get error";
mmk_mock_define(strerror_s_mock, errno_t, char *, rsize_t, errno_t);

errno_t mocked_windows_strerror(char * buf, rsize_t bufsz, errno_t errnum)
{
  (void) errnum;
  strncpy_s(buf, static_cast<size_t>(bufsz), expected_error_msg, static_cast<size_t>(bufsz));
  return errnum;
}

// Mocking test example
TEST(test_strerror, test_mock) {
  // Mock the strerror_s function in the current module using
  // the `strerror_s_mock` blueprint.
  strerror_s_mock mock = mmk_mock(RCUTILS_STRINGIFY(strerror_s) "@lib:rcutils", strerror_s_mock);
  // Tell the mock to call mocked_windows_strerror instead
  mmk_when(
    strerror_s(mmk_any(char *), mmk_any(rsize_t), mmk_any(errno_t)),
    .then_call = (mmk_fn) mocked_windows_strerror);

  // Set the error (not used by the mock)
  errno = 2;
  char error_string[1024];
  rcutils_strerror(error_string, sizeof(error_string));
  ASSERT_STREQ(error_string, "Failed to get error");
  mmk_reset(mock);
}

#elif defined(_GNU_SOURCE) && (!defined(ANDROID) || __ANDROID_API__ >= 23)
const char expected_error_msg[] = "Failed to get error";
mmk_mock_define(strerror_r_mock, char *, int, char *, size_t);

char * mocked_gnu_strerror(int errnum, char * buf, size_t buflen)
{
  (void) errnum;
  strncpy(buf, expected_error_msg, buflen);
  return buf;
}

// Mocking test example
TEST(test_strerror, test_mock) {
  // Mock the strerror_r function in the current module using
  // the `strerror_r_mock` blueprint.
  mmk_mock(RCUTILS_STRINGIFY(strerror_r) "@lib:rcutils", strerror_r_mock);
  // Tell the mock to call mocked_gnu_strerror instead
  mmk_when(
    strerror_r(mmk_any(int), mmk_any(char *), mmk_any(size_t) ),
    .then_call = (mmk_fn) mocked_gnu_strerror);

  // Set the error (not used by the mock)
  errno = 2;
  char error_string[1024];
  rcutils_strerror(error_string, sizeof(error_string));
  ASSERT_STREQ(error_string, "Failed to get error");
  mmk_reset(strerror_r);
}

#else
mmk_mock_define(strerror_r_mock, int, int, char *, size_t);

// Mocking test example
TEST(test_strerror, test_mock) {
  // Mock the strerror_r function in the current module using
  // the `strerror_r_mock` blueprint.
  mmk_mock(RCUTILS_STRINGIFY(strerror_r) "@lib:rcutils", strerror_r_mock);
  // Tell the mock to return NULL and set errno to EINVAL
  // whatever the given parameter is.
  mmk_when(
    strerror_r(mmk_any(int), mmk_any(char *), mmk_any(size_t) ),
    .then_return = mmk_val(int, EINVAL));

  // Set the error "No such file or directory" (not used by the mock)
  errno = 2;
  char error_string[1024];
  rcutils_strerror(error_string, sizeof(error_string));
  ASSERT_STREQ(error_string, "Failed to get error");
  mmk_reset(strerror_r);
}
#endif
