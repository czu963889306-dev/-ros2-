// Copyright 2019 Open Source Robotics Foundation, Inc.
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

#include <ament_cmake_gen_version_h/version_custom.h>
#include <gtest/gtest.h>
#include <sstream>

TEST(test_ament_cmake_gen_version_h, version_custom) {
  EXPECT_TRUE(AMENT_CMAKE_GEN_VERSION_H_VERSION_GTE(0, 0, 0));
  EXPECT_TRUE(AMENT_CMAKE_GEN_VERSION_H_VERSION_GTE(0, 1, 2));
  EXPECT_TRUE(AMENT_CMAKE_GEN_VERSION_H_VERSION_GTE(1, 2, 3));
  EXPECT_FALSE(AMENT_CMAKE_GEN_VERSION_H_VERSION_GTE(1, 2, 4));
  EXPECT_FALSE(AMENT_CMAKE_GEN_VERSION_H_VERSION_GTE(1, 3, 2));
  EXPECT_FALSE(AMENT_CMAKE_GEN_VERSION_H_VERSION_GTE(2, 1, 2));
  EXPECT_EQ(AMENT_CMAKE_GEN_VERSION_H_VERSION_MAJOR, 1);
  EXPECT_EQ(AMENT_CMAKE_GEN_VERSION_H_VERSION_MINOR, 2);
  EXPECT_EQ(AMENT_CMAKE_GEN_VERSION_H_VERSION_PATCH, 3);
  EXPECT_STREQ(AMENT_CMAKE_GEN_VERSION_H_VERSION_STR, "1.2.3");

  std::stringstream version;
  version << AMENT_CMAKE_GEN_VERSION_H_VERSION_MAJOR << ".";
  version << AMENT_CMAKE_GEN_VERSION_H_VERSION_MINOR << ".";
  version << AMENT_CMAKE_GEN_VERSION_H_VERSION_PATCH;
  EXPECT_STREQ(AMENT_CMAKE_GEN_VERSION_H_VERSION_STR, version.str().c_str());
}
