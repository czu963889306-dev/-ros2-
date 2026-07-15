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

#include "rcl/lexer.h"

#ifdef RMW_IMPLEMENTATION
# define CLASSNAME_(NAME, SUFFIX) NAME ## __ ## SUFFIX
# define CLASSNAME(NAME, SUFFIX) CLASSNAME_(NAME, SUFFIX)
#else
# define CLASSNAME(NAME, SUFFIX) NAME
#endif

class CLASSNAME (TestLexerFixture, RMW_IMPLEMENTATION) : public ::testing::Test
{
public:
  void SetUp()
  {
  }

  void TearDown()
  {
  }
};

// Not using a function so gtest failure output shows the line number where the macro is used
#define EXPECT_LEX(expected_lexeme, expected_text, text) \
  do { \
    rcl_lexeme_t actual_lexeme; \
    size_t length; \
    rcl_ret_t ret = rcl_lexer_analyze(text, &actual_lexeme, &length); \
    ASSERT_EQ(RCL_RET_OK, ret); \
    EXPECT_EQ(expected_lexeme, actual_lexeme); \
    std::string actual_text(text, 0u, length); \
    EXPECT_EQ(length, actual_text.size()); \
    EXPECT_STREQ(expected_text, actual_text.c_str()); \
  } while (false)

TEST_F(CLASSNAME(TestLexerFixture, RMW_IMPLEMENTATION), test_token_different_endings)
{
  // Things get recognized as tokens whether input ends or non token characters come after them
  EXPECT_LEX(RCL_LEXEME_TOKEN, "foo", "foo");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "foo", "foo:");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "foo_", "foo_");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "foo_", "foo_:");
}

TEST_F(CLASSNAME(TestLexerFixture, RMW_IMPLEMENTATION), test_token_start_char)
{
  // Check full range for starting character
  EXPECT_LEX(RCL_LEXEME_TOKEN, "a", "a");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "b", "b");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "c", "c");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "d", "d");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "e", "e");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "f", "f");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "g", "g");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "h", "h");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "i", "i");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "j", "j");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "k", "k");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "l", "l");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "m", "m");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "n", "n");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "o", "o");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "p", "p");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "q", "q");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "r", "r");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "s", "s");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "t", "t");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "u", "u");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "v", "v");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "w", "w");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "x", "x");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "y", "y");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "z", "z");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "A", "A");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "B", "B");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "C", "C");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "D", "D");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "E", "E");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "F", "F");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "G", "G");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "H", "H");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "I", "I");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "J", "J");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "K", "K");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "L", "L");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "M", "M");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "N", "N");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "O", "O");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "P", "P");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "Q", "Q");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "R", "R");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "S", "S");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "T", "T");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "U", "U");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "V", "V");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "W", "W");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "X", "X");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "Y", "Y");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "Z", "Z");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "_", "_");
}

TEST_F(CLASSNAME(TestLexerFixture, RMW_IMPLEMENTATION), test_token_adjacent_ascii)
{
  // Check banned characters adjacent to allowed ones in ASCII
  EXPECT_LEX(RCL_LEXEME_NONE, "@", "@");
  EXPECT_LEX(RCL_LEXEME_NONE, "[", "[");
  EXPECT_LEX(RCL_LEXEME_NONE, "`", "`");
  EXPECT_LEX(RCL_LEXEME_NONE, "{", "{");
}

TEST_F(CLASSNAME(TestLexerFixture, RMW_IMPLEMENTATION), test_token_cannot_start_with_digits)
{
  // Tokens cannot start with digits
  EXPECT_LEX(RCL_LEXEME_NONE, "0", "0");
  EXPECT_LEX(RCL_LEXEME_NONE, "1", "1");
  EXPECT_LEX(RCL_LEXEME_NONE, "2", "2");
  EXPECT_LEX(RCL_LEXEME_NONE, "3", "3");
  EXPECT_LEX(RCL_LEXEME_NONE, "4", "4");
  EXPECT_LEX(RCL_LEXEME_NONE, "5", "5");
  EXPECT_LEX(RCL_LEXEME_NONE, "6", "6");
  EXPECT_LEX(RCL_LEXEME_NONE, "7", "7");
  EXPECT_LEX(RCL_LEXEME_NONE, "8", "8");
  EXPECT_LEX(RCL_LEXEME_NONE, "9", "9");
}

TEST_F(CLASSNAME(TestLexerFixture, RMW_IMPLEMENTATION), test_token_underscores)
{
  // Tokens may contain underscores
  EXPECT_LEX(RCL_LEXEME_TOKEN, "_abcd", "_abcd");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "abcd_", "abcd_");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "ab_cd", "ab_cd");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "_a_b_c_d_", "_a_b_c_d_");

  // Tokens cannot contain double underscores
  EXPECT_LEX(RCL_LEXEME_TOKEN, "_a_", "_a__bcd");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "a_", "a__bcd");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "A_", "A__bcd");
  EXPECT_LEX(RCL_LEXEME_NONE, "__a", "__a");
  EXPECT_LEX(RCL_LEXEME_NONE, "__A", "__A");
}

TEST_F(CLASSNAME(TestLexerFixture, RMW_IMPLEMENTATION), test_token_contain_digits)
{
  // Tokens may contain digits
  EXPECT_LEX(RCL_LEXEME_TOKEN, "_0_", "_0_");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "_1_", "_1_");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "_2_", "_2_");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "_3_", "_3_");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "_4_", "_4_");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "_5_", "_5_");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "_6_", "_6_");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "_7_", "_7_");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "_8_", "_8_");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "_9_", "_9_");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "a0a", "a0a");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "a1a", "a1a");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "a2a", "a2a");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "a3a", "a3a");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "a4a", "a4a");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "a5a", "a5a");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "a6a", "a6a");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "a7a", "a7a");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "a8a", "a8a");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "a9a", "a9a");
}

TEST_F(CLASSNAME(TestLexerFixture, RMW_IMPLEMENTATION), test_token_end_with_digits)
{
  // Tokens may end with digits
  EXPECT_LEX(RCL_LEXEME_TOKEN, "_0", "_0");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "_1", "_1");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "_2", "_2");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "_3", "_3");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "_4", "_4");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "_5", "_5");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "_6", "_6");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "_7", "_7");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "_8", "_8");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "_9", "_9");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "a0", "a0");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "a1", "a1");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "a2", "a2");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "a3", "a3");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "a4", "a4");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "a5", "a5");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "a6", "a6");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "a7", "a7");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "a8", "a8");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "a9", "a9");
}

TEST_F(CLASSNAME(TestLexerFixture, RMW_IMPLEMENTATION), test_token_close_to_url_scheme)
{
  // Things that almost look like a url scheme but are actually tokens
  EXPECT_LEX(RCL_LEXEME_TOKEN, "ro", "ro");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "ros", "ros");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "ross", "ross");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "rosse", "rosse");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "rosser", "rosser");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "rosserv", "rosserv");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "rosservi", "rosservi");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "rosservic", "rosservic");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "rosservice", "rosservice");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "rosservice", "rosservice:");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "rosservice", "rosservice:=");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "rosservice", "rosservice:/");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "rosservice", "rosservice:/a");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "rost", "rost");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "rosto", "rosto");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "rostop", "rostop");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "rostopi", "rostopi");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "rostopic", "rostopic");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "rostopic", "rostopic:");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "rostopic", "rostopic:=");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "rostopic", "rostopic:/");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "rostopic", "rostopic:/a");
}

TEST_F(CLASSNAME(TestLexerFixture, RMW_IMPLEMENTATION), test_token_upper_case)
{
  // Tokens may contain uppercase characters
  EXPECT_LEX(RCL_LEXEME_TOKEN, "ABC", "ABC");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "_DEF", "_DEF");
  EXPECT_LEX(RCL_LEXEME_TOKEN, "_GHI_", "_GHI_");
}

TEST_F(CLASSNAME(TestLexerFixture, RMW_IMPLEMENTATION), test_url_scheme)
{
  // No text after scheme
  EXPECT_LEX(RCL_LEXEME_URL_SERVICE, "rosservice://", "rosservice://");
  EXPECT_LEX(RCL_LEXEME_URL_TOPIC, "rostopic://", "rostopic://");

  // Some text after scheme
  EXPECT_LEX(RCL_LEXEME_URL_SERVICE, "rosservice://", "rosservice://abcd");
  EXPECT_LEX(RCL_LEXEME_URL_SERVICE, "rosservice://", "rosservice:///");
  EXPECT_LEX(RCL_LEXEME_URL_TOPIC, "rostopic://", "rostopic://abcd");
  EXPECT_LEX(RCL_LEXEME_URL_TOPIC, "rostopic://", "rostopic:///");
}

TEST_F(CLASSNAME(TestLexerFixture, RMW_IMPLEMENTATION), test_backreferences)
{
  // No text after backreference
  EXPECT_LEX(RCL_LEXEME_BR1, "\\1", "\\1");
  EXPECT_LEX(RCL_LEXEME_BR2, "\\2", "\\2");
  EXPECT_LEX(RCL_LEXEME_BR3, "\\3", "\\3");
  EXPECT_LEX(RCL_LEXEME_BR4, "\\4", "\\4");
  EXPECT_LEX(RCL_LEXEME_BR5, "\\5", "\\5");
  EXPECT_LEX(RCL_LEXEME_BR6, "\\6", "\\6");
  EXPECT_LEX(RCL_LEXEME_BR7, "\\7", "\\7");
  EXPECT_LEX(RCL_LEXEME_BR8, "\\8", "\\8");
  EXPECT_LEX(RCL_LEXEME_BR9, "\\9", "\\9");

  // Some text after backreference
  EXPECT_LEX(RCL_LEXEME_BR1, "\\1", "\\1a");
  EXPECT_LEX(RCL_LEXEME_BR2, "\\2", "\\2a");
  EXPECT_LEX(RCL_LEXEME_BR3, "\\3", "\\3a");
  EXPECT_LEX(RCL_LEXEME_BR4, "\\4", "\\4a");
  EXPECT_LEX(RCL_LEXEME_BR5, "\\5", "\\5a");
  EXPECT_LEX(RCL_LEXEME_BR6, "\\6", "\\6a");
  EXPECT_LEX(RCL_LEXEME_BR7, "\\7", "\\7a");
  EXPECT_LEX(RCL_LEXEME_BR8, "\\8", "\\8a");
  EXPECT_LEX(RCL_LEXEME_BR9, "\\9", "\\9a");

  // Not valid backreferences
  EXPECT_LEX(RCL_LEXEME_NONE, "\\0", "\\0");
  EXPECT_LEX(RCL_LEXEME_NONE, "\\a", "\\a");
  EXPECT_LEX(RCL_LEXEME_NONE, "\\Z", "\\Z");
  EXPECT_LEX(RCL_LEXEME_NONE, "\\_", "\\_");
}

TEST_F(CLASSNAME(TestLexerFixture, RMW_IMPLEMENTATION), test_forward_slash)
{
  EXPECT_LEX(RCL_LEXEME_FORWARD_SLASH, "/", "/");
  EXPECT_LEX(RCL_LEXEME_FORWARD_SLASH, "/", "//");
  EXPECT_LEX(RCL_LEXEME_FORWARD_SLASH, "/", "/_");
}

TEST_F(CLASSNAME(TestLexerFixture, RMW_IMPLEMENTATION), test_wildcards)
{
  EXPECT_LEX(RCL_LEXEME_WILD_ONE, "*", "*");
  EXPECT_LEX(RCL_LEXEME_WILD_ONE, "*", "*/");
  EXPECT_LEX(RCL_LEXEME_WILD_MULTI, "**", "**");
  EXPECT_LEX(RCL_LEXEME_WILD_MULTI, "**", "**/");
}

TEST_F(CLASSNAME(TestLexerFixture, RMW_IMPLEMENTATION), test_colon)
{
  EXPECT_LEX(RCL_LEXEME_COLON, ":", ":");
  EXPECT_LEX(RCL_LEXEME_COLON, ":", ":r");
}

TEST_F(CLASSNAME(TestLexerFixture, RMW_IMPLEMENTATION), test_separator)
{
  EXPECT_LEX(RCL_LEXEME_SEPARATOR, ":=", ":=");
  EXPECT_LEX(RCL_LEXEME_SEPARATOR, ":=", ":=0");
}

TEST_F(CLASSNAME(TestLexerFixture, RMW_IMPLEMENTATION), test_ns)
{
  // Has __ns
  EXPECT_LEX(RCL_LEXEME_NS, "__ns", "__ns");
  EXPECT_LEX(RCL_LEXEME_NS, "__ns", "__nsssss");

  // Things that are almost __ns
  EXPECT_LEX(RCL_LEXEME_NONE, "__", "__");
  EXPECT_LEX(RCL_LEXEME_NONE, "__n", "__n");
  EXPECT_LEX(RCL_LEXEME_NONE, "__n!", "__n!");
}

TEST_F(CLASSNAME(TestLexerFixture, RMW_IMPLEMENTATION), test_name)
{
  // Has __name
  EXPECT_LEX(RCL_LEXEME_NODE, "__name", "__name");
  EXPECT_LEX(RCL_LEXEME_NODE, "__name", "__namessss");

  // Things that are almost __name
  EXPECT_LEX(RCL_LEXEME_NONE, "__na", "__na");
  EXPECT_LEX(RCL_LEXEME_NONE, "__naa", "__naa");
  EXPECT_LEX(RCL_LEXEME_NONE, "__nam", "__nam");
  EXPECT_LEX(RCL_LEXEME_NONE, "__nama", "__nama");
}

TEST_F(CLASSNAME(TestLexerFixture, RMW_IMPLEMENTATION), test_node)
{
  // Has __node
  EXPECT_LEX(RCL_LEXEME_NODE, "__node", "__node");
  EXPECT_LEX(RCL_LEXEME_NODE, "__node", "__nodessss");

  // Things that are almost __node
  EXPECT_LEX(RCL_LEXEME_NONE, "__", "__");
  EXPECT_LEX(RCL_LEXEME_NONE, "__n", "__n");
  EXPECT_LEX(RCL_LEXEME_NONE, "__ne", "__ne");
  EXPECT_LEX(RCL_LEXEME_NONE, "__no", "__no");
  EXPECT_LEX(RCL_LEXEME_NONE, "__noa", "__noa");
  EXPECT_LEX(RCL_LEXEME_NONE, "__nod", "__nod");
  EXPECT_LEX(RCL_LEXEME_NONE, "__noda", "__noda");
}

TEST_F(CLASSNAME(TestLexerFixture, RMW_IMPLEMENTATION), test_tilde_slash)
{
  EXPECT_LEX(RCL_LEXEME_TILDE_SLASH, "~/", "~/");
  EXPECT_LEX(RCL_LEXEME_TILDE_SLASH, "~/", "~//");
  EXPECT_LEX(RCL_LEXEME_NONE, "~", "~");
  EXPECT_LEX(RCL_LEXEME_NONE, "~!", "~!");
}

TEST_F(CLASSNAME(TestLexerFixture, RMW_IMPLEMENTATION), test_eof)
{
  EXPECT_LEX(RCL_LEXEME_EOF, "", "");
}
