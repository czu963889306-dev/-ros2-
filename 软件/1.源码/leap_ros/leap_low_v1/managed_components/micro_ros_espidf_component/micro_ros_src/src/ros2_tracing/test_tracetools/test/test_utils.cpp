// Copyright 2019 Robert Bosch GmbH
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

#include <functional>
#include <memory>
#include <string>

#include "tracetools/utils.hpp"

void function_shared(const std::shared_ptr<int> p)
{
  (void)p;
}

class SomeClassWithCallback
{
public:
  SomeClassWithCallback() {}

  void my_callback(int some_number, std::string some_string)
  {
    (void)some_number;
    (void)some_string;
  }
};

/*
   Testing symbol resolution for std::function object created from a function pointer.
 */
TEST(TestUtils, valid_symbol_funcptr) {
  std::function<void(std::shared_ptr<int>)> f = &function_shared;
  EXPECT_STREQ(tracetools::get_symbol(f), "function_shared(std::shared_ptr<int>)") <<
    "invalid symbol";
}

/*
   Testing get_symbol_funcptr from a null pointer.
 */
TEST(TestUtils, invalid_get_symbol_funcptr) {
  EXPECT_STREQ(tracetools::detail::get_symbol_funcptr(nullptr), TRACETOOLS_SYMBOL_UNKNOWN);
}

/*
   Testing symbol resolution for std::function object created from a lambda.
 */
TEST(TestUtils, valid_symbol_lambda) {
  std::function<int(int)> l = [](int num) {return num + 1;};
  EXPECT_STREQ(
    tracetools::get_symbol(l),
    "TestUtils_valid_symbol_lambda_Test::TestBody()::{lambda(int)#1}") <<
    "invalid symbol";
}

/*
   Testing symbol resolution lambdas with capture.
 */
TEST(TestUtils, valid_symbol_lambda_capture) {
  int num = 1;

  auto l = [ = ]() {return num + 1;};
  EXPECT_STREQ(
    tracetools::get_symbol(l),
    "TestUtils_valid_symbol_lambda_capture_Test::TestBody()::{lambda()#1}") <<
    "invalid symbol";

  auto m = [&](int other_num) {return num + other_num;};
  EXPECT_STREQ(
    tracetools::get_symbol(m),
    "TestUtils_valid_symbol_lambda_capture_Test::TestBody()::{lambda(int)#2}") <<
    "invalid symbol";
}

/*
   Testing symbol resolution for std::function object created from std::bind.
 */
TEST(TestUtils, valid_symbol_bind) {
  SomeClassWithCallback scwc;
  std::function<void(int, std::string)> fscwc = std::bind(
    &SomeClassWithCallback::my_callback,
    &scwc,
    std::placeholders::_1,
    std::placeholders::_2
  );
  EXPECT_STREQ(
    tracetools::get_symbol(
      fscwc),
    "std::_Bind<void (SomeClassWithCallback::*(SomeClassWithCallback*, "
    "std::_Placeholder<1>, std::_Placeholder<2>))(int, std::__cxx11::basic_string"
    "<char, std::char_traits<char>, std::allocator<char> >)>")
    <<
    "invalid symbol";
}
