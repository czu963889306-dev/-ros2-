// Copyright 2022 Open Source Robotics Foundation, Inc.
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

#ifndef ROSIDL_TYPESUPPORT_INTROSPECTION_TESTS__GTEST__SHARED_LIBRARY_TEST_HPP_
#define ROSIDL_TYPESUPPORT_INTROSPECTION_TESTS__GTEST__SHARED_LIBRARY_TEST_HPP_

#include <gtest/gtest.h>

#include <string>

#include <rcpputils/shared_library.hpp>

namespace rosidl_typesupport_introspection_tests
{
namespace testing
{

/// A GTest fixture for tests that involve a
/// dynamically loaded shared library.
class SharedLibraryTest : public ::testing::Test
{
public:
  explicit SharedLibraryTest(const std::string & name)
  : library_(rcpputils::get_platform_library_name(name))
  {
  }

  rcpputils::SharedLibrary & GetSharedLibrary() {return library_;}

private:
  rcpputils::SharedLibrary library_;
};

}  // namespace testing
}  // namespace rosidl_typesupport_introspection_tests

#endif  // ROSIDL_TYPESUPPORT_INTROSPECTION_TESTS__GTEST__SHARED_LIBRARY_TEST_HPP_
