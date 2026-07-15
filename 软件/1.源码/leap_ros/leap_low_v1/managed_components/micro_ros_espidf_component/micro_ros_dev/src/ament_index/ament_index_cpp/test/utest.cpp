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

#include <stdlib.h>

#include <list>
#include <map>
#include <stdexcept>
#include <string>

#include "ament_index_cpp/get_package_prefix.hpp"
#include "ament_index_cpp/get_package_share_directory.hpp"
#include "ament_index_cpp/get_packages_with_prefixes.hpp"
#include "ament_index_cpp/get_resource.hpp"
#include "ament_index_cpp/get_resources.hpp"
#include "ament_index_cpp/get_search_paths.hpp"
#include "ament_index_cpp/has_resource.hpp"

std::string generate_subfolder_path(std::string subfolder)
{
  // Get base path of this file
  std::string base_path = __FILE__;
  const std::string filename = "utest.cpp";
  base_path = base_path.substr(0, base_path.length() - filename.length() - 1);
  // Generate the base path of the subfolder in this directory
  return base_path + "/" + subfolder;
}

void set_ament_prefix_path(std::list<std::string> subfolders)
{
  std::string ament_prefix_path;
  // Generate a joined string of all absolute paths
  // Subfolders later in the list are searched later in the index
  for (auto subfolder : subfolders) {
    std::string path = generate_subfolder_path(subfolder);
    if (!ament_prefix_path.empty()) {
#ifndef _WIN32
      ament_prefix_path += ":";
#else
      ament_prefix_path += ";";
#endif
    }
    ament_prefix_path += path;
  }
  // Set environment variable
#ifndef _WIN32
  int retcode = setenv("AMENT_PREFIX_PATH", ament_prefix_path.c_str(), 1);
#else
  errno_t retcode = _putenv_s("AMENT_PREFIX_PATH", ament_prefix_path.c_str());
#endif
  if (retcode) {
    throw std::runtime_error("Failed to set environment variable 'AMENT_PREFIX_PATH'");
  }
}

TEST(AmentIndexCpp, empty_search_paths) {
  std::list<std::string> subfolders;
  set_ament_prefix_path(subfolders);
  EXPECT_THROW(ament_index_cpp::get_search_paths(), std::runtime_error);
}

TEST(AmentIndexCpp, search_paths) {
  std::list<std::string> subfolders;
  subfolders.push_back("prefix1");
  subfolders.push_back("prefix2");
  set_ament_prefix_path(subfolders);
  std::list<std::string> search_paths = ament_index_cpp::get_search_paths();
  EXPECT_EQ(search_paths.size(), 2UL);
}

TEST(AmentIndexCpp, not_existing_search_paths) {
  std::list<std::string> subfolders;
  subfolders.push_back("prefix1");
  subfolders.push_back("not_existing_prefix");
  set_ament_prefix_path(subfolders);
  std::list<std::string> search_paths = ament_index_cpp::get_search_paths();
  EXPECT_EQ(search_paths.size(), 1UL);
}

TEST(AmentIndexCpp, get_empty_resources) {
  EXPECT_THROW(ament_index_cpp::get_resources(""), std::runtime_error);
}

TEST(AmentIndexCpp, get_unknown_resources) {
  std::list<std::string> subfolders;
  subfolders.push_back("prefix1");
  set_ament_prefix_path(subfolders);
  std::map<std::string, std::string> resources =
    ament_index_cpp::get_resources("unknown_resource_type");
  EXPECT_EQ(resources.size(), 0UL);
}

TEST(AmentIndexCpp, get_resources) {
  // Ensure that if multiple resources of a particular type exist, all are found
  std::list<std::string> subfolders;
  subfolders.push_back("prefix1");
  set_ament_prefix_path(subfolders);
  // There are two resources of this type
  std::map<std::string, std::string> resources = ament_index_cpp::get_resources("resource_type1");
  EXPECT_EQ(resources.size(), 2UL);
  for (auto it : resources) {
    EXPECT_EQ(it.second, generate_subfolder_path("prefix1"));
    EXPECT_TRUE(it.first == "foo" || it.first == "bar");
  }
}

TEST(AmentIndexCpp, get_resources_overlay) {
  // Ensure that a resource type found in two prefixes returns two paths
  std::list<std::string> subfolders;
  subfolders.push_back("prefix1");
  subfolders.push_back("prefix2");
  set_ament_prefix_path(subfolders);
  // This resource type is found in prefix1 and prefix2
  std::map<std::string, std::string> resources = ament_index_cpp::get_resources("resource_type2");
  EXPECT_EQ(resources.size(), 2UL);
  for (auto it : resources) {
    EXPECT_TRUE(it.first == "foo" || it.first == "bar");
  }
}

TEST(AmentIndexCpp, get_resources_underlay) {
  // Ensure that a resource only found in one prefix only returns one path
  std::list<std::string> subfolders;
  subfolders.push_back("prefix1");
  subfolders.push_back("prefix2");
  set_ament_prefix_path(subfolders);
  // This resource type is only found in prefix2
  std::map<std::string, std::string> resources = ament_index_cpp::get_resources("resource_type3");
  EXPECT_EQ(resources.size(), 1UL);
  for (auto it : resources) {
    EXPECT_EQ(it.second, generate_subfolder_path("prefix2"));
    EXPECT_EQ(it.first, "bar");
  }
}

TEST(AmentIndexCpp, get_empty_resource) {
  std::string content;
  EXPECT_THROW(ament_index_cpp::get_resource("", "", content), std::runtime_error);
}

TEST(AmentIndexCpp, get_unknown_resource) {
  std::list<std::string> subfolders;
  subfolders.push_back("prefix1");
  set_ament_prefix_path(subfolders);
  std::string content;
  bool success = ament_index_cpp::get_resource("resource_type4", "bar", content);
  EXPECT_FALSE(success);
}

TEST(AmentIndexCpp, get_resource) {
  std::list<std::string> subfolders;
  subfolders.push_back("prefix1");
  set_ament_prefix_path(subfolders);
  std::string content;
  bool success = ament_index_cpp::get_resource("resource_type4", "foo", content);
  EXPECT_TRUE(success);
  EXPECT_EQ(content, "foo");
}

TEST(AmentIndexCpp, get_resource_underlay) {
  // Ensure that a resource can be found in the underlay if it doesn't exist in the overlay
  std::list<std::string> subfolders;
  subfolders.push_back("prefix1");
  subfolders.push_back("prefix2");
  set_ament_prefix_path(subfolders);
  std::string content;
  // This resource is only found in the underlay
  bool success = ament_index_cpp::get_resource("resource_type2", "bar", content);
  EXPECT_TRUE(success);
  EXPECT_EQ(content, "");
}

TEST(AmentIndexCpp, get_resource_overlay) {
  // Ensure that the resource in the overlay is found before the one in the underlay
  std::list<std::string> subfolders;
  subfolders.push_back("prefix1");
  subfolders.push_back("prefix2");
  set_ament_prefix_path(subfolders);
  std::string content;
  // This resource is in both the overlay and the underlay
  bool success = ament_index_cpp::get_resource("resource_type5", "foo", content);
  EXPECT_TRUE(success);
  EXPECT_EQ(content, "foo1");
}

TEST(AmentIndexCpp, get_resource_overlay_base_path) {
  // Ensure that a resource found in the overlay returns the correct path
  std::list<std::string> subfolders;
  subfolders.push_back("prefix1");
  subfolders.push_back("prefix2");
  set_ament_prefix_path(subfolders);
  std::string content;
  std::string base_path;
  // This resource is only found in the overlay
  bool success = ament_index_cpp::get_resource("resource_type2", "foo", content, &base_path);
  EXPECT_TRUE(success);
  EXPECT_EQ(base_path, generate_subfolder_path("prefix1"));
}

TEST(AmentIndexCpp, get_resource_underlay_base_path) {
  // Ensure that a resource found in the underlay returns the correct path
  std::list<std::string> subfolders;
  subfolders.push_back("prefix1");
  subfolders.push_back("prefix2");
  set_ament_prefix_path(subfolders);
  std::string content;
  std::string base_path;
  // This resource is only found in the underlay
  bool success = ament_index_cpp::get_resource("resource_type2", "bar", content, &base_path);
  EXPECT_TRUE(success);
  EXPECT_EQ(base_path, generate_subfolder_path("prefix2"));
}

TEST(AmentIndexCpp, get_package_prefix) {
  // Ensure that a known to exist package is found and that a known to not exist package is not.
  std::list<std::string> subfolders;
  subfolders.push_back("prefix1");  // only contains foo and bar packages
  subfolders.push_back("prefix2");  // only contains bar and baz packages
  set_ament_prefix_path(subfolders);
  // foo is found in prefix 1
  EXPECT_EQ(generate_subfolder_path("prefix1"), ament_index_cpp::get_package_prefix("foo"));
  // bar is in both, but prefix 1 takes precedence
  EXPECT_EQ(generate_subfolder_path("prefix1"), ament_index_cpp::get_package_prefix("bar"));
  // baz is found in prefix 2 only
  EXPECT_EQ(generate_subfolder_path("prefix2"), ament_index_cpp::get_package_prefix("baz"));
  // exception when package is not found
  EXPECT_THROW(
    ament_index_cpp::get_package_prefix("does_not_exist"),
    ament_index_cpp::PackageNotFoundError);
  // exception when the package name is empty
  EXPECT_THROW(
    ament_index_cpp::get_package_prefix(""),
    std::runtime_error);
}

TEST(AmentIndexCpp, get_package_share_directory) {
  // Ensure that a known to exist package is found and the share directory is correct.
  std::list<std::string> subfolders;
  subfolders.push_back("prefix1");  // only contains foo and bar packages
  subfolders.push_back("prefix2");  // only contains bar and baz packages
  set_ament_prefix_path(subfolders);
  // bar is in both, but prefix 1 takes precedence
  EXPECT_EQ(
    generate_subfolder_path("prefix1") + "/share/bar",
    ament_index_cpp::get_package_share_directory("bar"));
}

TEST(AmentIndexCpp, get_packages_with_prefixes) {
  // Ensure the list of packages and prefixes matches resource layout.
  std::list<std::string> subfolders;
  subfolders.push_back("prefix1");  // only contains foo and bar packages
  subfolders.push_back("prefix2");  // only contains bar and baz packages
  set_ament_prefix_path(subfolders);

  std::map<std::string, std::string> packages_with_prefixes =
    ament_index_cpp::get_packages_with_prefixes();
  // should be exactly three elements
  EXPECT_EQ(3UL, packages_with_prefixes.size());
  // foo is found in prefix 1
  EXPECT_EQ(generate_subfolder_path("prefix1"), packages_with_prefixes["foo"]);
  // bar is in both, but prefix 1 takes precedence
  EXPECT_EQ(generate_subfolder_path("prefix1"), packages_with_prefixes["bar"]);
  // baz is found in prefix 2 only
  EXPECT_EQ(generate_subfolder_path("prefix2"), packages_with_prefixes["baz"]);
}

TEST(AmentIndexCpp, has_empty_name) {
  EXPECT_THROW(
    ament_index_cpp::has_resource("type", ""),
    std::runtime_error);
}

TEST(AmentIndexCpp, has_empty_type) {
  EXPECT_THROW(
    ament_index_cpp::has_resource("", "name"),
    std::runtime_error);
}

TEST(AmentIndexCpp, has_unknown_resource) {
  bool success = ament_index_cpp::has_resource("resource_type4", "bar21");
  EXPECT_FALSE(success);
}

TEST(AmentIndexCpp, has_resource) {
  std::string result_path;

  EXPECT_TRUE(ament_index_cpp::has_resource("resource_type1", "foo", &result_path));
  EXPECT_EQ(result_path, generate_subfolder_path("prefix1"));

  EXPECT_TRUE(ament_index_cpp::has_resource("resource_type3", "bar"));

  EXPECT_TRUE(ament_index_cpp::has_resource("packages", "baz", &result_path));
  EXPECT_EQ(result_path, generate_subfolder_path("prefix2"));

  EXPECT_FALSE(ament_index_cpp::has_resource("resource_type1", "resource", &result_path));
}
