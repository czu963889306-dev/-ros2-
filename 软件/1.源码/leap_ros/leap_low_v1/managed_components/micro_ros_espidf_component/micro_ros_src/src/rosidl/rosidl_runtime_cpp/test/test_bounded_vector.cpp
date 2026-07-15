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

#include <gtest/gtest.h>
#include <utility>
#include <sstream>
#include <iterator>
#include <forward_list>

#include "rosidl_runtime_cpp/bounded_vector.hpp"


TEST(rosidl_generator_cpp, bounded_vector) {
  rosidl_runtime_cpp::BoundedVector<int, 2> v;
  ASSERT_EQ(v.size(), 0u);
  ASSERT_EQ(v.max_size(), 2u);
  v.push_back(1);
  v.push_back(2);
  ASSERT_THROW(v.push_back(3), std::length_error);
  ASSERT_THROW(v.resize(3), std::length_error);
  v.resize(1);
  ASSERT_EQ(v.size(), 1u);
  v = {1, 2};
  ASSERT_EQ(v.size(), 2u);
  auto l = {1, 2, 3};
  ASSERT_THROW(v = l, std::length_error);
}

TEST(rosidl_generator_cpp, bounded_vector_rvalue) {
  rosidl_runtime_cpp::BoundedVector<int, 2> v, vv;
  // emplace back
  ASSERT_EQ(v.size(), 0u);
  ASSERT_EQ(v.max_size(), 2u);
  v.emplace_back(1);
  v.emplace_back(2);
  ASSERT_THROW(v.emplace_back(3), std::length_error);
  ASSERT_EQ(v.size(), 2u);
  // move assignment
  vv = std::move(v);
  ASSERT_EQ(vv.size(), 2u);
  ASSERT_EQ(vv[0], 1);
  ASSERT_EQ(vv[1], 2);
}

TEST(rosidl_generator_cpp, bounded_vector_insert) {
  rosidl_runtime_cpp::BoundedVector<int, 6> v1;
  v1.push_back(1);
  auto it = v1.begin();
  rosidl_runtime_cpp::BoundedVector<int, 3> v2;
  v2.push_back(2);
  v2.push_back(3);
  v2.push_back(4);
  // insert v2 after the first position in reverse order
  ASSERT_THROW(v1.insert(it + 1, v2.end(), v2.begin()), std::length_error);
  // insert v2 after the 1st position
  ASSERT_NO_THROW(v1.insert(it + 1, v2.begin(), v2.end()));
  rosidl_runtime_cpp::BoundedVector<int, 6> vv1{1, 2, 3, 4};
  ASSERT_EQ(v1, vv1);
}

TEST(rosidl_generator_cpp, bounded_vector_comparisons) {
  rosidl_runtime_cpp::BoundedVector<int, 2> v, vv;
  ASSERT_EQ(v, vv);
  v.push_back(1);
  ASSERT_NE(v, vv);
  ASSERT_GT(v, vv);
  ASSERT_LT(vv, v);
}

TEST(rosidl_generator_cpp, bounded_vector_input_iterators) {
  rosidl_runtime_cpp::BoundedVector<int, 4> v, vv{1, 2, 3};
  std::istringstream ss("1 2 3");
  std::istream_iterator<int> ii(ss), end;
  v.assign(ii, end);
  ASSERT_EQ(v, vv);
  ss.clear();
  ss.str("10 11 12 13 14 15");
  ii = ss;
  ASSERT_THROW(v.assign(ii, end), std::length_error);
  ASSERT_EQ(v, vv);
  ss.clear();
  ss.str("0");
  ii = ss;
  v.insert(v.begin(), ii, end);
  vv.insert(vv.begin(), 0);
  ASSERT_EQ(v, vv);
  ss.clear();
  ss.str("10 11 12 13");
  ii = ss;
  v.pop_back();
  vv.pop_back();
  ASSERT_THROW(v.insert(v.begin() + 1, ii, end), std::length_error);
  ASSERT_EQ(v, vv);
}

TEST(rosidl_generator_cpp, bounded_vector_forward_iterators) {
  rosidl_runtime_cpp::BoundedVector<int, 4> v, vv{1, 2, 3};
  std::forward_list<int> l{1, 2, 3};
  v.assign(l.begin(), l.end());
  ASSERT_EQ(v, vv);
  l = {10, 11, 12, 13, 14, 15};
  ASSERT_THROW(v.assign(l.begin(), l.end()), std::length_error);
  ASSERT_EQ(v, vv);
  l = {0};
  v.insert(v.begin(), l.begin(), l.end());
  vv.insert(vv.begin(), 0);
  ASSERT_EQ(v, vv);
  l = {10, 11, 12, 13};
  v.pop_back();
  vv.pop_back();
  ASSERT_THROW(v.insert(v.begin() + 1, l.begin(), l.end()), std::length_error);
  ASSERT_EQ(v, vv);
}
