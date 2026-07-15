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

#include "rcl/error_handling.h"
#include "rcl/lexer_lookahead.h"

struct rcl_lexer_lookahead2_impl_s
{
  // Text that is being analyzed for lexemes
  const char * text;
  // Where in the text analysis is being performed
  size_t text_idx;

  // first character of lexeme
  size_t start[2];
  // One past last character of lexeme
  size_t end[2];
  // Type of lexeme
  rcl_lexeme_t type[2];

  // Allocator to use if an error occurrs
  rcl_allocator_t allocator;
};

rcl_lexer_lookahead2_t
rcl_get_zero_initialized_lexer_lookahead2()
{
  static rcl_lexer_lookahead2_t zero_initialized = {
    .impl = NULL,
  };
  return zero_initialized;
}

rcl_ret_t
rcl_lexer_lookahead2_init(
  rcl_lexer_lookahead2_t * buffer,
  const char * text,
  rcl_allocator_t allocator)
{
  RCUTILS_CAN_SET_MSG_AND_RETURN_WITH_ERROR_OF(RCL_RET_INVALID_ARGUMENT);
  RCUTILS_CAN_SET_MSG_AND_RETURN_WITH_ERROR_OF(RCL_RET_BAD_ALLOC);

  RCL_CHECK_ALLOCATOR_WITH_MSG(&allocator, "invalid allocator", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(buffer, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(text, RCL_RET_INVALID_ARGUMENT);
  if (NULL != buffer->impl) {
    RCL_SET_ERROR_MSG("buffer must be zero initialized");
    return RCL_RET_INVALID_ARGUMENT;
  }

  buffer->impl = allocator.allocate(sizeof(rcl_lexer_lookahead2_impl_t), allocator.state);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    buffer->impl, "Failed to allocate lookahead impl", return RCL_RET_BAD_ALLOC);

  buffer->impl->text = text;
  buffer->impl->text_idx = 0u;
  buffer->impl->start[0] = 0u;
  buffer->impl->start[1] = 0u;
  buffer->impl->end[0] = 0u;
  buffer->impl->end[1] = 0u;
  buffer->impl->type[0] = RCL_LEXEME_NONE;
  buffer->impl->type[1] = RCL_LEXEME_NONE;
  buffer->impl->allocator = allocator;

  return RCL_RET_OK;
}

rcl_ret_t
rcl_lexer_lookahead2_fini(
  rcl_lexer_lookahead2_t * buffer)
{
  RCL_CHECK_ARGUMENT_FOR_NULL(buffer, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    buffer->impl, "buffer finalized twice", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ALLOCATOR_WITH_MSG(
    &(buffer->impl->allocator), "invalid allocator", return RCL_RET_INVALID_ARGUMENT);

  buffer->impl->allocator.deallocate(buffer->impl, buffer->impl->allocator.state);
  buffer->impl = NULL;
  return RCL_RET_OK;
}

rcl_ret_t
rcl_lexer_lookahead2_peek(
  rcl_lexer_lookahead2_t * buffer,
  rcl_lexeme_t * next_type)
{
  RCUTILS_CAN_SET_MSG_AND_RETURN_WITH_ERROR_OF(RCL_RET_INVALID_ARGUMENT);

  RCL_CHECK_ARGUMENT_FOR_NULL(buffer, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    buffer->impl, "buffer not initialized", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(next_type, RCL_RET_INVALID_ARGUMENT);

  rcl_ret_t ret;
  size_t length;

  if (buffer->impl->text_idx >= buffer->impl->end[0]) {
    // No buffered lexeme; get one
    ret = rcl_lexer_analyze(
      rcl_lexer_lookahead2_get_text(buffer),
      &(buffer->impl->type[0]),
      &length);

    if (RCL_RET_OK != ret) {
      return ret;
    }

    buffer->impl->start[0] = buffer->impl->text_idx;
    buffer->impl->end[0] = buffer->impl->start[0] + length;
  }

  *next_type = buffer->impl->type[0];
  return RCL_RET_OK;
}

rcl_ret_t
rcl_lexer_lookahead2_peek2(
  rcl_lexer_lookahead2_t * buffer,
  rcl_lexeme_t * next_type1,
  rcl_lexeme_t * next_type2)
{
  RCUTILS_CAN_SET_MSG_AND_RETURN_WITH_ERROR_OF(RCL_RET_INVALID_ARGUMENT);

  rcl_ret_t ret;
  // Peek 1 ahead first (reusing its error checking for buffer and next_type1)
  ret = rcl_lexer_lookahead2_peek(buffer, next_type1);
  if (RCL_RET_OK != ret) {
    return ret;
  }
  RCL_CHECK_ARGUMENT_FOR_NULL(next_type2, RCL_RET_INVALID_ARGUMENT);

  if (RCL_LEXEME_NONE == *next_type1 || RCL_LEXEME_EOF == *next_type1) {
    // No need to peek further
    *next_type2 = *next_type1;
    return ret;
  }

  size_t length;

  if (buffer->impl->text_idx >= buffer->impl->end[1]) {
    // No buffered lexeme; get one
    ret = rcl_lexer_analyze(
      &(buffer->impl->text[buffer->impl->end[0]]),
      &(buffer->impl->type[1]),
      &length);

    if (RCL_RET_OK != ret) {
      return ret;
    }

    buffer->impl->start[1] = buffer->impl->end[0];
    buffer->impl->end[1] = buffer->impl->start[1] + length;
  }

  *next_type2 = buffer->impl->type[1];
  return RCL_RET_OK;
}

rcl_ret_t
rcl_lexer_lookahead2_accept(
  rcl_lexer_lookahead2_t * buffer,
  const char ** lexeme_text,
  size_t * lexeme_text_length)
{
  RCUTILS_CAN_SET_MSG_AND_RETURN_WITH_ERROR_OF(RCL_RET_INVALID_ARGUMENT);
  RCUTILS_CAN_SET_MSG_AND_RETURN_WITH_ERROR_OF(RCL_RET_ERROR);

  RCL_CHECK_ARGUMENT_FOR_NULL(buffer, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    buffer->impl, "buffer not initialized", return RCL_RET_INVALID_ARGUMENT);
  if (
    (NULL == lexeme_text && NULL != lexeme_text_length) ||
    (NULL != lexeme_text && NULL == lexeme_text_length))
  {
    RCL_SET_ERROR_MSG("text and length must both be set or both be NULL");
    return RCL_RET_INVALID_ARGUMENT;
  }

  if (RCL_LEXEME_EOF == buffer->impl->type[0]) {
    // Reached EOF, nothing to accept
    if (NULL != lexeme_text && NULL != lexeme_text_length) {
      *lexeme_text = rcl_lexer_lookahead2_get_text(buffer);
      *lexeme_text_length = 0u;
    }
    return RCL_RET_OK;
  }

  if (buffer->impl->text_idx >= buffer->impl->end[0]) {
    RCL_SET_ERROR_MSG("no lexeme to accept");
    return RCL_RET_ERROR;
  }

  if (NULL != lexeme_text && NULL != lexeme_text_length) {
    *lexeme_text = &(buffer->impl->text[buffer->impl->start[0]]);
    *lexeme_text_length = buffer->impl->end[0] - buffer->impl->start[0];
  }

  // Advance lexer position
  buffer->impl->text_idx = buffer->impl->end[0];

  // Move second lexeme in buffer to first position
  buffer->impl->start[0] = buffer->impl->start[1];
  buffer->impl->end[0] = buffer->impl->end[1];
  buffer->impl->type[0] = buffer->impl->type[1];

  return RCL_RET_OK;
}

rcl_ret_t
rcl_lexer_lookahead2_expect(
  rcl_lexer_lookahead2_t * buffer,
  rcl_lexeme_t type,
  const char ** lexeme_text,
  size_t * lexeme_text_length)
{
  RCUTILS_CAN_SET_MSG_AND_RETURN_WITH_ERROR_OF(RCL_RET_WRONG_LEXEME);

  rcl_ret_t ret;
  rcl_lexeme_t lexeme;

  ret = rcl_lexer_lookahead2_peek(buffer, &lexeme);
  if (RCL_RET_OK != ret) {
    return ret;
  }
  if (type != lexeme) {
    if (RCL_LEXEME_NONE == lexeme || RCL_LEXEME_EOF == lexeme) {
      RCL_SET_ERROR_MSG_WITH_FORMAT_STRING(
        "Expected lexeme type (%d) not found, search ended at index %zu",
        type, buffer->impl->text_idx);
      return RCL_RET_WRONG_LEXEME;
    }
    RCL_SET_ERROR_MSG_WITH_FORMAT_STRING(
      "Expected lexeme type %d, got %d at index %zu", type, lexeme,
      buffer->impl->text_idx);
    return RCL_RET_WRONG_LEXEME;
  }
  return rcl_lexer_lookahead2_accept(buffer, lexeme_text, lexeme_text_length);
}

const char *
rcl_lexer_lookahead2_get_text(
  const rcl_lexer_lookahead2_t * buffer)
{
  return &(buffer->impl->text[buffer->impl->text_idx]);
}
