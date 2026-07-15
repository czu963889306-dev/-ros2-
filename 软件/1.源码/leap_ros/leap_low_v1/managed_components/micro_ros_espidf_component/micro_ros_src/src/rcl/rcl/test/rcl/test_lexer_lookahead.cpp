// Copyright 2018 Open Source Robotics Foundation, Inc.
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

#include <string>

#include "osrf_testing_tools_cpp/scope_exit.hpp"

#include "rcl/error_handling.h"
#include "rcl/lexer_lookahead.h"

#ifdef RMW_IMPLEMENTATION
# define CLASSNAME_(NAME, SUFFIX) NAME ## __ ## SUFFIX
# define CLASSNAME(NAME, SUFFIX) CLASSNAME_(NAME, SUFFIX)
#else
# define CLASSNAME(NAME, SUFFIX) NAME
#endif

class CLASSNAME (TestLexerLookaheadFixture, RMW_IMPLEMENTATION) : public ::testing::Test
{
public:
  void SetUp()
  {
  }

  void TearDown()
  {
  }
};

#define SCOPE_LOOKAHEAD2(name, text) \
  { \
    name = rcl_get_zero_initialized_lexer_lookahead2(); \
    rcl_ret_t ret = rcl_lexer_lookahead2_init(&name, text, rcl_get_default_allocator()); \
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str; \
  } \
  auto __scope_lookahead2_ ## name = osrf_testing_tools_cpp::make_scope_exit( \
    [&name]() { \
      rcl_ret_t ret = rcl_lexer_lookahead2_fini(&buffer); \
      ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str; \
    })

TEST_F(CLASSNAME(TestLexerLookaheadFixture, RMW_IMPLEMENTATION), test_init_fini_twice)
{
  rcl_lexer_lookahead2_t buffer = rcl_get_zero_initialized_lexer_lookahead2();
  rcl_ret_t ret = rcl_lexer_lookahead2_init(&buffer, "foobar", rcl_get_default_allocator());
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  ret = rcl_lexer_lookahead2_fini(&buffer);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;

  ret = rcl_lexer_lookahead2_fini(&buffer);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret);
  rcl_reset_error();
}

TEST_F(CLASSNAME(TestLexerLookaheadFixture, RMW_IMPLEMENTATION), test_init_not_zero_initialized)
{
  rcl_lexer_lookahead2_t buffer;
  int not_zero = 1;
  buffer.impl = reinterpret_cast<struct rcl_lexer_lookahead2_impl_s *>(&not_zero);
  rcl_ret_t ret = rcl_lexer_lookahead2_init(&buffer, "foobar", rcl_get_default_allocator());
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret);
  rcl_reset_error();
}

TEST_F(CLASSNAME(TestLexerLookaheadFixture, RMW_IMPLEMENTATION), test_peek)
{
  rcl_ret_t ret;
  rcl_lexer_lookahead2_t buffer;
  SCOPE_LOOKAHEAD2(buffer, "foobar");

  rcl_lexeme_t lexeme = RCL_LEXEME_NONE;

  ret = rcl_lexer_lookahead2_peek(&buffer, &lexeme);
  EXPECT_EQ(RCL_RET_OK, ret);
  EXPECT_EQ(RCL_LEXEME_TOKEN, lexeme);

  // Test again to make sure peek isn't advancing the lexer
  lexeme = RCL_LEXEME_NONE;
  ret = rcl_lexer_lookahead2_peek(&buffer, &lexeme);
  EXPECT_EQ(RCL_RET_OK, ret);
  EXPECT_EQ(RCL_LEXEME_TOKEN, lexeme);
}

TEST_F(CLASSNAME(TestLexerLookaheadFixture, RMW_IMPLEMENTATION), test_peek2)
{
  rcl_ret_t ret;
  rcl_lexer_lookahead2_t buffer;
  SCOPE_LOOKAHEAD2(buffer, "foobar/");

  rcl_lexeme_t lexeme1 = RCL_LEXEME_NONE;
  rcl_lexeme_t lexeme2 = RCL_LEXEME_NONE;

  ret = rcl_lexer_lookahead2_peek2(&buffer, &lexeme1, &lexeme2);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  EXPECT_EQ(RCL_LEXEME_TOKEN, lexeme1);
  EXPECT_EQ(RCL_LEXEME_FORWARD_SLASH, lexeme2);

  // Test again to make sure peek2 isn't advancing the lexer
  lexeme1 = RCL_LEXEME_NONE;
  lexeme2 = RCL_LEXEME_NONE;
  ret = rcl_lexer_lookahead2_peek2(&buffer, &lexeme1, &lexeme2);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  EXPECT_EQ(RCL_LEXEME_TOKEN, lexeme1);
  EXPECT_EQ(RCL_LEXEME_FORWARD_SLASH, lexeme2);
}

TEST_F(CLASSNAME(TestLexerLookaheadFixture, RMW_IMPLEMENTATION), test_peek2_no_lexeme)
{
  rcl_ret_t ret;
  rcl_lexer_lookahead2_t buffer;
  SCOPE_LOOKAHEAD2(buffer, "~foo");

  rcl_lexeme_t lexeme1 = RCL_LEXEME_NONE;
  rcl_lexeme_t lexeme2 = RCL_LEXEME_NONE;

  ret = rcl_lexer_lookahead2_peek2(&buffer, &lexeme1, &lexeme2);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  EXPECT_EQ(RCL_LEXEME_NONE, lexeme1);
  EXPECT_EQ(RCL_LEXEME_NONE, lexeme2);
}

TEST_F(CLASSNAME(TestLexerLookaheadFixture, RMW_IMPLEMENTATION), test_peek2_no_lexeme_eof)
{
  rcl_ret_t ret;
  rcl_lexer_lookahead2_t buffer;
  SCOPE_LOOKAHEAD2(buffer, "~");

  rcl_lexeme_t lexeme1 = RCL_LEXEME_NONE;
  rcl_lexeme_t lexeme2 = RCL_LEXEME_NONE;

  ret = rcl_lexer_lookahead2_peek2(&buffer, &lexeme1, &lexeme2);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  EXPECT_EQ(RCL_LEXEME_NONE, lexeme1);
  EXPECT_EQ(RCL_LEXEME_NONE, lexeme2);
}

TEST_F(CLASSNAME(TestLexerLookaheadFixture, RMW_IMPLEMENTATION), test_peek2_eof)
{
  rcl_ret_t ret;
  rcl_lexer_lookahead2_t buffer;
  SCOPE_LOOKAHEAD2(buffer, "");

  rcl_lexeme_t lexeme1 = RCL_LEXEME_NONE;
  rcl_lexeme_t lexeme2 = RCL_LEXEME_NONE;

  ret = rcl_lexer_lookahead2_peek2(&buffer, &lexeme1, &lexeme2);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  EXPECT_EQ(RCL_LEXEME_EOF, lexeme1);
  EXPECT_EQ(RCL_LEXEME_EOF, lexeme2);
}

TEST_F(CLASSNAME(TestLexerLookaheadFixture, RMW_IMPLEMENTATION), test_eof)
{
  rcl_ret_t ret;
  rcl_lexer_lookahead2_t buffer;
  SCOPE_LOOKAHEAD2(buffer, "");

  {
    rcl_lexeme_t lexeme = RCL_LEXEME_NONE;
    ret = rcl_lexer_lookahead2_peek(&buffer, &lexeme);
    EXPECT_EQ(RCL_RET_OK, ret);
    EXPECT_EQ(RCL_LEXEME_EOF, lexeme);
  }
  {
    rcl_lexeme_t lexeme1 = RCL_LEXEME_NONE;
    rcl_lexeme_t lexeme2 = RCL_LEXEME_NONE;
    ret = rcl_lexer_lookahead2_peek2(&buffer, &lexeme1, &lexeme2);
    EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
    EXPECT_EQ(RCL_LEXEME_EOF, lexeme1);
    EXPECT_EQ(RCL_LEXEME_EOF, lexeme2);
  }
  // Accepting keeps the lexer at EOF
  {
    EXPECT_EQ(RCL_RET_OK, rcl_lexer_lookahead2_accept(&buffer, NULL, NULL));
    rcl_lexeme_t lexeme = RCL_LEXEME_NONE;
    ret = rcl_lexer_lookahead2_peek(&buffer, &lexeme);
    EXPECT_EQ(RCL_RET_OK, ret);
    EXPECT_EQ(RCL_LEXEME_EOF, lexeme);
  }
}


TEST_F(CLASSNAME(TestLexerLookaheadFixture, RMW_IMPLEMENTATION), test_accept)
{
  rcl_ret_t ret;
  rcl_lexer_lookahead2_t buffer;
  SCOPE_LOOKAHEAD2(buffer, "foobar/");

  rcl_lexeme_t lexeme = RCL_LEXEME_NONE;
  const char * lexeme_text;
  size_t lexeme_text_length;

  // Peek token
  ret = rcl_lexer_lookahead2_peek(&buffer, &lexeme);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  EXPECT_EQ(RCL_LEXEME_TOKEN, lexeme);

  // accept token
  ret = rcl_lexer_lookahead2_accept(&buffer, &lexeme_text, &lexeme_text_length);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  EXPECT_STREQ("foobar", std::string(lexeme_text, lexeme_text_length).c_str());

  // peek forward slash
  ret = rcl_lexer_lookahead2_peek(&buffer, &lexeme);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  EXPECT_EQ(RCL_LEXEME_FORWARD_SLASH, lexeme);

  // accept forward slash
  ret = rcl_lexer_lookahead2_accept(&buffer, &lexeme_text, &lexeme_text_length);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  EXPECT_STREQ("/", std::string(lexeme_text, lexeme_text_length).c_str());

  // peek eof
  ret = rcl_lexer_lookahead2_peek(&buffer, &lexeme);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  EXPECT_EQ(RCL_LEXEME_EOF, lexeme);

  // accept eof
  ret = rcl_lexer_lookahead2_accept(&buffer, &lexeme_text, &lexeme_text_length);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  EXPECT_STREQ("", std::string(lexeme_text, lexeme_text_length).c_str());

  // peek eof again
  ret = rcl_lexer_lookahead2_peek(&buffer, &lexeme);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  EXPECT_EQ(RCL_LEXEME_EOF, lexeme);
}

TEST_F(CLASSNAME(TestLexerLookaheadFixture, RMW_IMPLEMENTATION), test_accept_bad_arg)
{
  rcl_lexer_lookahead2_t buffer;
  rcl_lexer_lookahead2_t buffer_not_ini = rcl_get_zero_initialized_lexer_lookahead2();
  SCOPE_LOOKAHEAD2(buffer, "foobar/");

  rcl_lexeme_t lexeme = RCL_LEXEME_NONE;
  const char * lexeme_text;
  size_t lexeme_text_length = 0;

  // Can't accept without peek first
  rcl_ret_t ret = rcl_lexer_lookahead2_accept(&buffer, &lexeme_text, &lexeme_text_length);
  EXPECT_EQ(RCL_RET_ERROR, ret) << rcl_get_error_string().str;
  rcl_reset_error();

  // Expected usage
  ret = rcl_lexer_lookahead2_peek(&buffer, &lexeme);
  EXPECT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  EXPECT_EQ(RCL_LEXEME_TOKEN, lexeme);

  // Invalid nullptr parameter
  ret = rcl_lexer_lookahead2_accept(nullptr, &lexeme_text, &lexeme_text_length);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret) << rcl_get_error_string().str;
  rcl_reset_error();

  // Invalid not ini parameter
  ret = rcl_lexer_lookahead2_accept(&buffer_not_ini, &lexeme_text, &lexeme_text_length);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret) << rcl_get_error_string().str;
  rcl_reset_error();

  // Invalid nullptr as lexeme_text_length
  ret = rcl_lexer_lookahead2_accept(&buffer, &lexeme_text, nullptr);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret) << rcl_get_error_string().str;
  rcl_reset_error();

  // Invalid nullptr as lexeme_text
  ret = rcl_lexer_lookahead2_accept(&buffer, nullptr, &lexeme_text_length);
  EXPECT_EQ(RCL_RET_INVALID_ARGUMENT, ret) << rcl_get_error_string().str;
  rcl_reset_error();
}

TEST_F(CLASSNAME(TestLexerLookaheadFixture, RMW_IMPLEMENTATION), test_expect)
{
  rcl_ret_t ret;
  rcl_lexer_lookahead2_t buffer;
  SCOPE_LOOKAHEAD2(buffer, "node_name:__node:=new_1");
  const char * lexeme_text;
  size_t lexeme_text_length;

  ret = rcl_lexer_lookahead2_expect(&buffer, RCL_LEXEME_TOKEN, &lexeme_text, &lexeme_text_length);
  ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str;
  EXPECT_STREQ("node_name", std::string(lexeme_text, lexeme_text_length).c_str());

  ret = rcl_lexer_lookahead2_expect(
    &buffer, RCL_LEXEME_FORWARD_SLASH, &lexeme_text, &lexeme_text_length);
  EXPECT_EQ(RCL_RET_WRONG_LEXEME, ret) << rcl_get_error_string().str;
}

#define EXPECT_LOOKAHEAD(expected_lexeme, expected_text, buffer) \
  do { \
    const char * lexeme_text; \
    size_t lexeme_text_length; \
    rcl_lexeme_t lexeme; \
    ret = rcl_lexer_lookahead2_peek(&buffer, &lexeme); \
    EXPECT_EQ(expected_lexeme, lexeme); \
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str; \
    ret = rcl_lexer_lookahead2_accept(&buffer, &lexeme_text, &lexeme_text_length); \
    ASSERT_EQ(RCL_RET_OK, ret) << rcl_get_error_string().str; \
    EXPECT_STREQ(expected_text, std::string(lexeme_text, lexeme_text_length).c_str()); \
  } while (false)

TEST_F(CLASSNAME(TestLexerLookaheadFixture, RMW_IMPLEMENTATION), test_lex_long_string)
{
  rcl_ret_t ret;
  rcl_lexer_lookahead2_t buffer;
  SCOPE_LOOKAHEAD2(buffer, ":\\1rostopic://\\2rosservice://~/\\8:=**:*foobar");

  EXPECT_LOOKAHEAD(RCL_LEXEME_COLON, ":", buffer);
  EXPECT_LOOKAHEAD(RCL_LEXEME_BR1, "\\1", buffer);
  EXPECT_LOOKAHEAD(RCL_LEXEME_URL_TOPIC, "rostopic://", buffer);
  EXPECT_LOOKAHEAD(RCL_LEXEME_BR2, "\\2", buffer);
  EXPECT_LOOKAHEAD(RCL_LEXEME_URL_SERVICE, "rosservice://", buffer);
  EXPECT_LOOKAHEAD(RCL_LEXEME_TILDE_SLASH, "~/", buffer);
  EXPECT_LOOKAHEAD(RCL_LEXEME_BR8, "\\8", buffer);
  EXPECT_LOOKAHEAD(RCL_LEXEME_SEPARATOR, ":=", buffer);
  EXPECT_LOOKAHEAD(RCL_LEXEME_WILD_MULTI, "**", buffer);
  EXPECT_LOOKAHEAD(RCL_LEXEME_COLON, ":", buffer);
  EXPECT_LOOKAHEAD(RCL_LEXEME_WILD_ONE, "*", buffer);
  EXPECT_LOOKAHEAD(RCL_LEXEME_TOKEN, "foobar", buffer);
  EXPECT_LOOKAHEAD(RCL_LEXEME_EOF, "", buffer);
}

TEST_F(CLASSNAME(TestLexerLookaheadFixture, RMW_IMPLEMENTATION), test_lex_remap_rules)
{
  rcl_ret_t ret;
  rcl_lexer_lookahead2_t buffer;
  {
    SCOPE_LOOKAHEAD2(buffer, "foo:=bar");
    EXPECT_LOOKAHEAD(RCL_LEXEME_TOKEN, "foo", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_SEPARATOR, ":=", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_TOKEN, "bar", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_EOF, "", buffer);
  }
  {
    SCOPE_LOOKAHEAD2(buffer, "/foo/bar:=fiz/buzz");
    EXPECT_LOOKAHEAD(RCL_LEXEME_FORWARD_SLASH, "/", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_TOKEN, "foo", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_FORWARD_SLASH, "/", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_TOKEN, "bar", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_SEPARATOR, ":=", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_TOKEN, "fiz", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_FORWARD_SLASH, "/", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_TOKEN, "buzz", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_EOF, "", buffer);
  }
  {
    // Nodename prefix
    SCOPE_LOOKAHEAD2(buffer, "nodename:~/foo:=foo");
    EXPECT_LOOKAHEAD(RCL_LEXEME_TOKEN, "nodename", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_COLON, ":", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_TILDE_SLASH, "~/", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_TOKEN, "foo", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_SEPARATOR, ":=", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_TOKEN, "foo", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_EOF, "", buffer);
  }
  {
    // Partial namespace replacement
    SCOPE_LOOKAHEAD2(buffer, "/foo/**:=/fizz/\\1");
    EXPECT_LOOKAHEAD(RCL_LEXEME_FORWARD_SLASH, "/", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_TOKEN, "foo", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_FORWARD_SLASH, "/", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_WILD_MULTI, "**", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_SEPARATOR, ":=", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_FORWARD_SLASH, "/", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_TOKEN, "fizz", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_FORWARD_SLASH, "/", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_BR1, "\\1", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_EOF, "", buffer);
  }
  {
    // Full namespace replacement
    SCOPE_LOOKAHEAD2(buffer, "/foo/bar/*:=/bar/foo/\\1");
    EXPECT_LOOKAHEAD(RCL_LEXEME_FORWARD_SLASH, "/", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_TOKEN, "foo", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_FORWARD_SLASH, "/", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_TOKEN, "bar", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_FORWARD_SLASH, "/", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_WILD_ONE, "*", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_SEPARATOR, ":=", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_FORWARD_SLASH, "/", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_TOKEN, "bar", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_FORWARD_SLASH, "/", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_TOKEN, "foo", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_FORWARD_SLASH, "/", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_BR1, "\\1", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_EOF, "", buffer);
  }
  {
    // Change a base name
    SCOPE_LOOKAHEAD2(buffer, "**/foo:=\\1/bar");
    EXPECT_LOOKAHEAD(RCL_LEXEME_WILD_MULTI, "**", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_FORWARD_SLASH, "/", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_TOKEN, "foo", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_SEPARATOR, ":=", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_BR1, "\\1", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_FORWARD_SLASH, "/", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_TOKEN, "bar", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_EOF, "", buffer);
  }
  {
    // Change namespace
    SCOPE_LOOKAHEAD2(buffer, "__ns:=/new/namespace");
    EXPECT_LOOKAHEAD(RCL_LEXEME_NS, "__ns", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_SEPARATOR, ":=", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_FORWARD_SLASH, "/", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_TOKEN, "new", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_FORWARD_SLASH, "/", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_TOKEN, "namespace", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_EOF, "", buffer);
  }
  {
    // Change node name
    SCOPE_LOOKAHEAD2(buffer, "__node:=left_camera_driver");
    EXPECT_LOOKAHEAD(RCL_LEXEME_NODE, "__node", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_SEPARATOR, ":=", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_TOKEN, "left_camera_driver", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_EOF, "", buffer);
  }
  {
    // Topic only remap
    SCOPE_LOOKAHEAD2(buffer, "rostopic://foo/bar:=bar/foo");
    EXPECT_LOOKAHEAD(RCL_LEXEME_URL_TOPIC, "rostopic://", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_TOKEN, "foo", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_FORWARD_SLASH, "/", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_TOKEN, "bar", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_SEPARATOR, ":=", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_TOKEN, "bar", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_FORWARD_SLASH, "/", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_TOKEN, "foo", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_EOF, "", buffer);
  }
  {
    // Service only remap
    SCOPE_LOOKAHEAD2(buffer, "rosservice:///foo/bar:=/bar/foo");
    EXPECT_LOOKAHEAD(RCL_LEXEME_URL_SERVICE, "rosservice://", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_FORWARD_SLASH, "/", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_TOKEN, "foo", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_FORWARD_SLASH, "/", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_TOKEN, "bar", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_SEPARATOR, ":=", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_FORWARD_SLASH, "/", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_TOKEN, "bar", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_FORWARD_SLASH, "/", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_TOKEN, "foo", buffer);
    EXPECT_LOOKAHEAD(RCL_LEXEME_EOF, "", buffer);
  }
}
