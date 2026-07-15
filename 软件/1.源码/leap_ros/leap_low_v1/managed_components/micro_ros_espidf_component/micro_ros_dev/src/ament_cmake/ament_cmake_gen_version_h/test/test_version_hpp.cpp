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
#include <ament_generate_version_header/version.hpp>
#include <gtest/gtest.h>
#include <sstream>

TEST(test_ament_generate_version_header, version_hpp) {
  EXPECT_TRUE(AMENT_CMAKE_GEN_VERSION_H_VERSION_GTE(0, 0, 0));
  std::stringstream version;
  version << AMENT_CMAKE_GEN_VERSION_H_VERSION_MAJOR << ".";
  version << AMENT_CMAKE_GEN_VERSION_H_VERSION_MINOR << ".";
  version << AMENT_CMAKE_GEN_VERSION_H_VERSION_PATCH;
  EXPECT_STREQ(AMENT_CMAKE_GEN_VERSION_H_VERSION_STR, version.str().c_str());
}
