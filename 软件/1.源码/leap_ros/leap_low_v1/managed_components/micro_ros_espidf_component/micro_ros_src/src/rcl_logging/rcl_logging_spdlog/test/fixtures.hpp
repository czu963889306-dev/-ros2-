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

#ifndef FIXTURES_HPP_
#define FIXTURES_HPP_

#include <limits.h>
#ifdef _WIN32
# include <windows.h>  // MAX_PATH
#endif
#include <string>

#include "gtest/gtest.h"

#include "rcpputils/filesystem_helper.hpp"
#include "rcutils/allocator.h"
#include "rcutils/env.h"
#include "rcutils/error_handling.h"
#include "rcutils/process.h"
#include "rcutils/types/string_array.h"

#ifdef _WIN32
#define popen _popen
#define pclose _pclose
#define DIR_CMD "dir /B"
#else
#define DIR_CMD "ls -d"
#endif

namespace fs = rcpputils::fs;

class AllocatorTest : public ::testing::Test
{
public:
  AllocatorTest()
  : allocator(rcutils_get_default_allocator()),
    bad_allocator(get_bad_allocator()),
    invalid_allocator(rcutils_get_zero_initialized_allocator())
  {
  }

  rcutils_allocator_t allocator;
  rcutils_allocator_t bad_allocator;
  rcutils_allocator_t invalid_allocator;

private:
  static rcutils_allocator_t get_bad_allocator()
  {
    rcutils_allocator_t bad_allocator = rcutils_get_default_allocator();
    bad_allocator.allocate = AllocatorTest::bad_malloc;
    bad_allocator.reallocate = AllocatorTest::bad_realloc;
    return bad_allocator;
  }

  static void * bad_malloc(size_t, void *)
  {
    return nullptr;
  }

  static void * bad_realloc(void *, size_t, void *)
  {
    return nullptr;
  }
};

class LoggingTest : public AllocatorTest
{
public:
  LoggingTest()
  : AllocatorTest()
  {
  }

  fs::path find_single_log()
  {
    fs::path log_dir = get_log_dir();
    std::stringstream dir_command;
    dir_command << DIR_CMD << " " << (log_dir / get_expected_log_prefix()).string() << "*";

    FILE * fp = popen(dir_command.str().c_str(), "r");
    if (nullptr == fp) {
      throw std::runtime_error("Failed to glob for log files");
    }

#ifdef _WIN32
    char raw_line[MAX_PATH];
#else
    char raw_line[PATH_MAX];
#endif
    char * ret = fgets(raw_line, sizeof(raw_line), fp);
    pclose(fp);
    if (nullptr == ret) {
      throw std::runtime_error("No log files were found");
    }

    std::string line(raw_line);
    fs::path line_path(line.substr(0, line.find_last_not_of(" \t\r\n") + 1));
    // This should be changed once ros2/rcpputils#68 is resolved
    return line_path.is_absolute() ? line_path : log_dir / line_path;
  }

private:
  std::string get_expected_log_prefix()
  {
    char * exe_name = rcutils_get_executable_name(allocator);
    if (nullptr == exe_name) {
      throw std::runtime_error("Failed to determine executable name");
    }
    std::stringstream prefix;
    prefix << exe_name << "_" << rcutils_get_pid() << "_";
    allocator.deallocate(exe_name, allocator.state);
    return prefix.str();
  }

  fs::path get_log_dir()
  {
    return fs::path(rcutils_get_home_dir()) / ".ros" / "log";
  }
};

#endif  // FIXTURES_HPP_
