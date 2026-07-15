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
#include "rcl/lexer.h"

/* The lexer tries to find a lexeme in a string.
 * It looks at one character at a time, and uses that character's value to decide how to transition
 * a state machine.
 * A transition is taken if a character's ASCII value falls within its range.
 * There is never more than one matching transition.
 *
 * If no transition matches then it uses a state's '<else,M>' transition.
 * Every state has exactly one '<else,M>' transition.
 * In the diagram below all states have an `<else,0>` to T_NONE unless otherwise specified.
 *
 * When a transition is taken it causes the lexer to move to another character in the string.
 * Normal transitions always move the lexer forwards one character.
 * '<else,M>' transitions may cause the lexer to move forwards 1, or backwards N.
 * The movement M is written as M = 1 + N so it can be stored in an unsigned integer.
 * For example, an `<else>` transition with M = 0 moves the lexer forwards 1 character, M = 1 keeps
 * the lexer at the current character, and M = 2 moves the lexer backwards one character.

digraph remapping_lexer {
  rankdir=LR;
  node [shape = box, fontsize = 7];
    T_TILDE_SLASH
    T_URL_SERVICE
    T_URL_TOPIC
    T_COLON
    T_NODE
    T_NS
    T_SEPARATOR
    T_BR1
    T_BR2
    T_BR3
    T_BR4
    T_BR5
    T_BR6
    T_BR7
    T_BR8
    T_BR9
    T_TOKEN
    T_FORWARD_SLASH
    T_WILD_ONE
    T_WILD_MULTI
    T_EOF
    T_NONE
    T_DOT
  node [shape = circle];
  S0 -> T_FORWARD_SLASH [ label = "/"];
  S0 -> T_DOT [ label = "."];
  S0 -> S1 [ label = "\\"];
  S0 -> S2 [ label = "~"];
  S0 -> S3 [ label = "_" ];
  S0 -> S9 [ label = "a-qs-zA-Z"];
  S0 -> S11 [ label = "r"];
  S0 -> S30 [ label = "*"];
  S0 -> S31 [ label = ":"];
  S1 -> T_BR1 [ label = "1"];
  S1 -> T_BR2 [ label = "2"];
  S1 -> T_BR3 [ label = "3"];
  S1 -> T_BR4 [ label = "4"];
  S1 -> T_BR5 [ label = "5"];
  S1 -> T_BR6 [ label = "6"];
  S1 -> T_BR7 [ label = "7"];
  S1 -> T_BR8 [ label = "8"];
  S1 -> T_BR9 [ label = "9"];
  S2 -> T_TILDE_SLASH [ label ="/" ];
  S3 -> S4 [ label = "_" ];
  S3 -> S10 [ label = "<else,1>", color = crimson, fontcolor = crimson];
  S4 -> S5 [ label = "n" ];
  S5 -> T_NS [ label = "s"];
  S5 -> S6 [ label = "o" ];
  S6 -> S8 [ label = "d" ];
  S5 -> S7 [ label = "a" ];
  S7 -> S8 [ label = "m" ];
  S8 -> T_NODE [ label = "e"];
  S9 -> T_TOKEN [ label = "<else,1>", color=crimson, fontcolor=crimson];
  S9 -> S9 [ label = "a-zA-Z0-9"];
  S9 -> S10 [ label = "_"];
  S10 -> T_TOKEN [ label = "<else,1>", color=crimson, fontcolor=crimson];
  S10 -> S9 [ label = "a-zA-Z0-9"];
  S11 -> S9 [ label = "<else,1>", color=crimson, fontcolor=crimson];
  S11 -> S12 [ label = "o"];
  S12 -> S9 [ label = "<else,1>", color=crimson, fontcolor=crimson];
  S12 -> S13 [ label = "s"];
  S13 -> S9 [ label = "<else,1>", color=crimson, fontcolor=crimson];
  S13 -> S14 [ label = "t"];
  S13 -> S21 [ label = "s"];
  S14 -> S9 [ label = "<else,1>", color=crimson, fontcolor=crimson];
  S14 -> S15 [ label = "o"];
  S15 -> S9 [ label = "<else,1>", color=crimson, fontcolor=crimson];
  S15 -> S16 [ label = "p"];
  S16 -> S9 [ label = "<else,1>", color=crimson, fontcolor=crimson];
  S16 -> S17 [ label = "i"];
  S17 -> S9 [ label = "<else,1>", color=crimson, fontcolor=crimson];
  S17 -> S18 [ label = "c"];
  S18 -> S9 [ label = "<else,1>", color=crimson, fontcolor=crimson];
  S18 -> S19 [ label = ":"];
  S19 -> S20 [ label = "/"];
  S19 -> S9 [ label = "<else,2>", color=crimson, fontcolor=crimson];
  S20 -> T_URL_TOPIC [ label = "/"];
  S20 -> S9 [ label = "<else,3>", color=crimson, fontcolor=crimson];
  S21 -> S9 [ label = "<else,1>", color=crimson, fontcolor=crimson];
  S21 -> S22 [ label = "e"];
  S22 -> S9 [ label = "<else,1>", color=crimson, fontcolor=crimson];
  S22 -> S23 [ label = "r"];
  S23 -> S9 [ label = "<else,1>", color=crimson, fontcolor=crimson];
  S23 -> S24 [ label = "v"];
  S24 -> S9 [ label = "<else,1>", color=crimson, fontcolor=crimson];
  S24 -> S25 [ label = "i"];
  S25 -> S9 [ label = "<else,1>", color=crimson, fontcolor=crimson];
  S25 -> S26 [ label = "c"];
  S26 -> S9 [ label = "<else,1>", color=crimson, fontcolor=crimson];
  S26 -> S27 [ label = "e"];
  S27 -> S28 [ label = ":"];
  S27 -> S9 [ label = "<else,1>", color=crimson, fontcolor=crimson];
  S28 -> S29 [ label = "/"];
  S28 -> S9 [ label = "<else,2>", color=crimson, fontcolor=crimson];
  S29 -> T_URL_SERVICE [ label = "/"];
  S29 -> S9 [ label = "<else,3>", color=crimson, fontcolor=crimson];
  S30 -> T_WILD_MULTI[ label = "*"];
  S30 -> T_WILD_ONE [ label = "<else,1>", color=crimson, fontcolor=crimson];
  S31 -> T_SEPARATOR [ label = "="];
  S31 -> T_COLON [ label = "<else,1>", color=crimson, fontcolor=crimson];
}
*/

/// Represents a transition from one state to another
/// \internal
typedef struct rcl_lexer_transition_s
{
  /// Index of a state to transition to
  const unsigned char to_state;
  /// Start of a range of chars (inclusive) which activates this transition
  const char range_start;
  /// End of a range of chars (inclusive) which activates this transition
  const char range_end;
} rcl_lexer_transition_t;

/// Represents a non-terminal state
/// \internal
typedef struct rcl_lexer_state_s
{
  /// Transition to this state if no other transition matches
  const unsigned char else_state;
  /// Movement associated with taking else state
  const unsigned char else_movement;
  /// Transitions in the state machine (NULL value at end of array)
  const rcl_lexer_transition_t transitions[12];
} rcl_lexer_state_t;

#define S0 0u
#define S1 1u
#define S2 2u
#define S3 3u
#define S4 4u
#define S5 5u
#define S6 6u
#define S7 7u
#define S8 8u
#define S9 9u
#define S10 10u
#define S11 11u
#define S12 12u
#define S13 13u
#define S14 14u
#define S15 15u
#define S16 16u
#define S17 17u
#define S18 18u
#define S19 19u
#define S20 20u
#define S21 21u
#define S22 22u
#define S23 23u
#define S24 24u
#define S25 25u
#define S26 26u
#define S27 27u
#define S28 28u
#define S29 29u
#define S30 30u
#define S31 31u
#define LAST_STATE S31

#define T_TILDE_SLASH 32u
#define T_URL_SERVICE 33u
#define T_URL_TOPIC 34u
#define T_COLON 35u
#define T_NODE 36u
#define T_NS 37u
#define T_SEPARATOR 38u
#define T_BR1 39u
#define T_BR2 40u
#define T_BR3 41u
#define T_BR4 42u
#define T_BR5 43u
#define T_BR6 44u
#define T_BR7 45u
#define T_BR8 46u
#define T_BR9 47u
#define T_TOKEN 48u
#define T_FORWARD_SLASH 49u
#define T_WILD_ONE 50u
#define T_WILD_MULTI 51u
#define T_EOF 52u
#define T_NONE 53u
#define T_DOT 54u

// used to figure out if a state is terminal or not
#define FIRST_TERMINAL T_TILDE_SLASH
#define LAST_TERMINAL T_NONE

// Used to mark where the last transition is in a state
#define END_TRANSITIONS {0, '\0', '\0'}

static const rcl_lexer_state_t g_states[LAST_STATE + 1] =
{
  // S0
  {
    T_NONE,
    0u,
    {
      {T_FORWARD_SLASH, '/', '/'},
      {T_DOT, '.', '.'},
      {S1, '\\', '\\'},
      {S2, '~', '~'},
      {S3, '_', '_'},
      {S9, 'a', 'q'},
      {S9, 's', 'z'},
      {S9, 'A', 'Z'},
      {S11, 'r', 'r'},
      {S30, '*', '*'},
      {S31, ':', ':'},
      END_TRANSITIONS
    }
  },
  // S1
  {
    T_NONE,
    0u,
    {
      {T_BR1, '1', '1'},
      {T_BR2, '2', '2'},
      {T_BR3, '3', '3'},
      {T_BR4, '4', '4'},
      {T_BR5, '5', '5'},
      {T_BR6, '6', '6'},
      {T_BR7, '7', '7'},
      {T_BR8, '8', '8'},
      {T_BR9, '9', '9'},
      END_TRANSITIONS
    }
  },
  // S2
  {
    T_NONE,
    0u,
    {
      {T_TILDE_SLASH, '/', '/'},
      END_TRANSITIONS
    }
  },
  // S3
  {
    S10,
    1u,
    {
      {S4, '_', '_'},
      END_TRANSITIONS
    }
  },
  // S4
  {
    T_NONE,
    0u,
    {
      {S5, 'n', 'n'},
      END_TRANSITIONS
    }
  },
  // S5
  {
    T_NONE,
    0u,
    {
      {T_NS, 's', 's'},
      {S6, 'o', 'o'},
      {S7, 'a', 'a'},
      END_TRANSITIONS
    }
  },
  // S6
  {
    T_NONE,
    0u,
    {
      {S8, 'd', 'd'},
      END_TRANSITIONS
    }
  },
  // S7
  {
    T_NONE,
    0u,
    {
      {S8, 'm', 'm'},
      END_TRANSITIONS
    }
  },
  // S8
  {
    T_NONE,
    0u,
    {
      {T_NODE, 'e', 'e'},
      END_TRANSITIONS
    }
  },
  // S9
  {
    T_TOKEN,
    1u,
    {
      {S9, 'a', 'z'},
      {S9, 'A', 'Z'},
      {S9, '0', '9'},
      {S10, '_', '_'},
      END_TRANSITIONS
    }
  },
  // S10
  {
    T_TOKEN,
    1u,
    {
      {S9, 'a', 'z'},
      {S9, 'A', 'Z'},
      {S9, '0', '9'},
      END_TRANSITIONS
    }
  },
  // S11
  {
    S9,
    1u,
    {
      {S12, 'o', 'o'},
      END_TRANSITIONS
    }
  },
  // S12
  {
    S9,
    1u,
    {
      {S13, 's', 's'},
      END_TRANSITIONS
    }
  },
  // S13
  {
    S9,
    1u,
    {
      {S14, 't', 't'},
      {S21, 's', 's'},
      END_TRANSITIONS
    }
  },
  // S14
  {
    S9,
    1u,
    {
      {S15, 'o', 'o'},
      END_TRANSITIONS
    }
  },
  // S15
  {
    S9,
    1u,
    {
      {S16, 'p', 'p'},
      END_TRANSITIONS
    }
  },
  // S16
  {
    S9,
    1u,
    {
      {S17, 'i', 'i'},
      END_TRANSITIONS
    }
  },
  // S17
  {
    S9,
    1u,
    {
      {S18, 'c', 'c'},
      END_TRANSITIONS
    }
  },
  // S18
  {
    S9,
    1u,
    {
      {S19, ':', ':'},
      END_TRANSITIONS
    }
  },
  // S19
  {
    S9,
    2u,
    {
      {S20, '/', '/'},
      END_TRANSITIONS
    }
  },
  // S20
  {
    S9,
    3u,
    {
      {T_URL_TOPIC, '/', '/'},
      END_TRANSITIONS
    }
  },
  // S21
  {
    S9,
    1u,
    {
      {S22, 'e', 'e'},
      END_TRANSITIONS
    }
  },
  // S21
  {
    S9,
    1u,
    {
      {S23, 'r', 'r'},
      END_TRANSITIONS
    }
  },
  // S23
  {
    S9,
    1u,
    {
      {S24, 'v', 'v'},
      END_TRANSITIONS
    }
  },
  // S24
  {
    S9,
    1u,
    {
      {S25, 'i', 'i'},
      END_TRANSITIONS
    }
  },
  // S25
  {
    S9,
    1u,
    {
      {S26, 'c', 'c'},
      END_TRANSITIONS
    }
  },
  // S26
  {
    S9,
    1u,
    {
      {S27, 'e', 'e'},
      END_TRANSITIONS
    }
  },
  // S27
  {
    S9,
    1u,
    {
      {S28, ':', ':'},
      END_TRANSITIONS
    }
  },
  // S28
  {
    S9,
    2u,
    {
      {S29, '/', '/'},
      END_TRANSITIONS
    }
  },
  // S29
  {
    S9,
    3u,
    {
      {T_URL_SERVICE, '/', '/'},
      END_TRANSITIONS
    }
  },
  // S30
  {
    T_WILD_ONE,
    1u,
    {
      {T_WILD_MULTI, '*', '*'},
      END_TRANSITIONS
    }
  },
  // S31
  {
    T_COLON,
    1u,
    {
      {T_SEPARATOR, '=', '='},
      END_TRANSITIONS
    }
  }
};

static const rcl_lexeme_t g_terminals[LAST_TERMINAL + 1] = {
  // 0
  RCL_LEXEME_TILDE_SLASH,
  // 1
  RCL_LEXEME_URL_SERVICE,
  // 2
  RCL_LEXEME_URL_TOPIC,
  // 3
  RCL_LEXEME_COLON,
  // 4
  RCL_LEXEME_NODE,
  // 5
  RCL_LEXEME_NS,
  // 6
  RCL_LEXEME_SEPARATOR,
  // 7
  RCL_LEXEME_BR1,
  // 8
  RCL_LEXEME_BR2,
  // 9
  RCL_LEXEME_BR3,
  // 10
  RCL_LEXEME_BR4,
  // 11
  RCL_LEXEME_BR5,
  // 12
  RCL_LEXEME_BR6,
  // 13
  RCL_LEXEME_BR7,
  // 14
  RCL_LEXEME_BR8,
  // 15
  RCL_LEXEME_BR9,
  // 16
  RCL_LEXEME_TOKEN,
  // 17
  RCL_LEXEME_FORWARD_SLASH,
  // 18
  RCL_LEXEME_WILD_ONE,
  // 19
  RCL_LEXEME_WILD_MULTI,
  // 20
  RCL_LEXEME_EOF,
  // 21
  RCL_LEXEME_NONE,
  // 22
  RCL_LEXEME_DOT
};

rcl_ret_t
rcl_lexer_analyze(
  const char * text,
  rcl_lexeme_t * lexeme,
  size_t * length)
{
  RCUTILS_CAN_SET_MSG_AND_RETURN_WITH_ERROR_OF(RCL_RET_INVALID_ARGUMENT);
  RCUTILS_CAN_SET_MSG_AND_RETURN_WITH_ERROR_OF(RCL_RET_ERROR);

  RCL_CHECK_ARGUMENT_FOR_NULL(text, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(lexeme, RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ARGUMENT_FOR_NULL(length, RCL_RET_INVALID_ARGUMENT);

  *length = 0u;

  if ('\0' == text[0u]) {
    // Early exit if string is empty
    *lexeme = RCL_LEXEME_EOF;
    return RCL_RET_OK;
  }

  const rcl_lexer_state_t * state;
  char current_char;
  size_t next_state = S0;
  size_t movement;

  // Analyze one character at a time until lexeme is found
  do {
    if (next_state > LAST_STATE) {
      // Should never happen
      RCL_SET_ERROR_MSG("Internal lexer bug: next state does not exist");
      return RCL_RET_ERROR;
    }
    state = &(g_states[next_state]);
    current_char = text[*length];
    next_state = 0u;
    movement = 0u;

    // Look for a transition that contains this character in its range
    size_t transition_idx = 0u;
    const rcl_lexer_transition_t * transition;
    do {
      transition = &(state->transitions[transition_idx]);
      if (transition->range_start <= current_char && transition->range_end >= current_char) {
        next_state = transition->to_state;
        break;
      }
      ++transition_idx;
    } while (0u != transition->to_state);

    // if no transition was found, take the else transition
    if (0u == next_state) {
      next_state = state->else_state;
      movement = state->else_movement;
    }

    if (0u == movement) {
      if ('\0' != current_char) {
        // Go forwards 1 char as long as the end hasn't been reached
        ++(*length);
      }
    } else {
      // Go backwards N chars
      if (movement - 1u > *length) {
        // Should never happen
        RCL_SET_ERROR_MSG("Internal lexer bug: movement would read before start of string");
        return RCL_RET_ERROR;
      }
      *length -= movement - 1u;
    }
  } while (next_state < FIRST_TERMINAL);

  if (FIRST_TERMINAL > next_state || next_state - FIRST_TERMINAL > LAST_TERMINAL) {
    // Should never happen
    RCL_SET_ERROR_MSG("Internal lexer bug: terminal state does not exist");
    return RCL_RET_ERROR;
  }
  *lexeme = g_terminals[next_state - FIRST_TERMINAL];
  return RCL_RET_OK;
}
