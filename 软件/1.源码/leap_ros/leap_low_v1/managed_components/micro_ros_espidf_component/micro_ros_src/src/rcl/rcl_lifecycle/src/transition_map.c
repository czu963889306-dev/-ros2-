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

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rcl/error_handling.h"
#include "rcl/macros.h"
#include "rcutils/format_string.h"

#include "rcl_lifecycle/transition_map.h"

rcl_lifecycle_transition_map_t
rcl_lifecycle_get_zero_initialized_transition_map()
{
  static rcl_lifecycle_transition_map_t transition_map;
  transition_map.states = NULL;
  transition_map.states_size = 0;
  transition_map.transitions = NULL;
  transition_map.transitions_size = 0;

  return transition_map;
}

rcl_ret_t
rcl_lifecycle_transition_map_is_initialized(const rcl_lifecycle_transition_map_t * transition_map)
{
  rcl_ret_t is_initialized = RCL_RET_OK;
  RCL_CHECK_FOR_NULL_WITH_MSG(
    transition_map, "transition_map pointer is null\n", return RCL_RET_INVALID_ARGUMENT);
  if (!transition_map->states && !transition_map->transitions) {
    is_initialized = RCL_RET_ERROR;
  }
  return is_initialized;
}

rcl_ret_t
rcl_lifecycle_transition_map_fini(
  rcl_lifecycle_transition_map_t * transition_map,
  const rcutils_allocator_t * allocator)
{
  RCUTILS_CAN_RETURN_WITH_ERROR_OF(RCL_RET_ERROR);
  RCUTILS_CAN_RETURN_WITH_ERROR_OF(RCL_RET_INVALID_ARGUMENT);

  RCL_CHECK_FOR_NULL_WITH_MSG(
    transition_map, "transition_map pointer is null\n", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ALLOCATOR_WITH_MSG(
    allocator, "can't free transition map, no allocator given\n", return RCL_RET_INVALID_ARGUMENT);

  rcl_ret_t fcn_ret = RCL_RET_OK;

  // free valid transitions for all states
  for (unsigned int i = 0; i < transition_map->states_size; ++i) {
    if (transition_map->states[i].valid_transitions != NULL) {
      allocator->deallocate(transition_map->states[i].valid_transitions, allocator->state);
      transition_map->states[i].valid_transitions = NULL;
    }
  }
  // free the primary states
  allocator->deallocate(transition_map->states, allocator->state);
  transition_map->states = NULL;
  transition_map->states_size = 0;
  // free the tansitions
  allocator->deallocate(transition_map->transitions, allocator->state);
  transition_map->transitions = NULL;
  transition_map->transitions_size = 0;

  return fcn_ret;
}

rcl_ret_t
rcl_lifecycle_register_state(
  rcl_lifecycle_transition_map_t * transition_map,
  rcl_lifecycle_state_t state,
  const rcutils_allocator_t * allocator)
{
  RCUTILS_CAN_RETURN_WITH_ERROR_OF(RCL_RET_INVALID_ARGUMENT);
  RCUTILS_CAN_RETURN_WITH_ERROR_OF(RCL_RET_LIFECYCLE_STATE_REGISTERED);
  RCUTILS_CAN_RETURN_WITH_ERROR_OF(RCL_RET_BAD_ALLOC);

  RCL_CHECK_FOR_NULL_WITH_MSG(
    transition_map, "transition_map pointer is null\n", return RCL_RET_INVALID_ARGUMENT);
  if (rcl_lifecycle_get_state(transition_map, state.id) != NULL) {
    RCL_SET_ERROR_MSG_WITH_FORMAT_STRING("state %u is already registered\n", state.id);
    return RCL_RET_LIFECYCLE_STATE_REGISTERED;
  }

  RCL_CHECK_ALLOCATOR_WITH_MSG(
    allocator, "invalid allocator", return RCL_RET_INVALID_ARGUMENT)
  // add new primary state memory
  unsigned int new_states_size = transition_map->states_size + 1;
  rcl_lifecycle_state_t * new_states = allocator->reallocate(
    transition_map->states,
    new_states_size * sizeof(rcl_lifecycle_state_t),
    allocator->state);
  RCL_CHECK_FOR_NULL_WITH_MSG(
    new_states, "failed to reallocate memory for new states\n", return RCL_RET_BAD_ALLOC);
  transition_map->states_size = new_states_size;
  transition_map->states = new_states;
  transition_map->states[transition_map->states_size - 1] = state;

  return RCL_RET_OK;
}

rcl_ret_t
rcl_lifecycle_register_transition(
  rcl_lifecycle_transition_map_t * transition_map,
  rcl_lifecycle_transition_t transition,
  const rcutils_allocator_t * allocator)
{
  RCUTILS_CAN_RETURN_WITH_ERROR_OF(RCL_RET_ERROR);
  RCUTILS_CAN_RETURN_WITH_ERROR_OF(RCL_RET_BAD_ALLOC);
  RCUTILS_CAN_RETURN_WITH_ERROR_OF(RCL_RET_INVALID_ARGUMENT);
  RCUTILS_CAN_RETURN_WITH_ERROR_OF(RCL_RET_LIFECYCLE_STATE_NOT_REGISTERED);

  RCL_CHECK_FOR_NULL_WITH_MSG(
    transition_map, "transition_map pointer is null\n", return RCL_RET_INVALID_ARGUMENT);
  RCL_CHECK_ALLOCATOR_WITH_MSG(
    allocator, "invalid allocator", return RCL_RET_INVALID_ARGUMENT);

  rcl_lifecycle_state_t * state = rcl_lifecycle_get_state(transition_map, transition.start->id);
  if (!state) {
    RCL_SET_ERROR_MSG_WITH_FORMAT_STRING("state %u is not registered\n", transition.start->id);
    return RCL_RET_LIFECYCLE_STATE_NOT_REGISTERED;
  }

  rcl_lifecycle_state_t * goal = rcl_lifecycle_get_state(transition_map, transition.goal->id);
  if (!goal) {
    RCL_SET_ERROR_MSG_WITH_FORMAT_STRING("state %u is not registered\n", transition.goal->id);
    return RCL_RET_LIFECYCLE_STATE_NOT_REGISTERED;
  }
  // Attempt to add new transition, don't update map if it fails
  unsigned int new_transitions_size = transition_map->transitions_size + 1;
  rcl_lifecycle_transition_t * new_transitions = allocator->reallocate(
    transition_map->transitions,
    new_transitions_size * sizeof(rcl_lifecycle_transition_t),
    allocator->state);
  if (!new_transitions) {
    RCL_SET_ERROR_MSG("failed to reallocate memory for new transitions");
    return RCL_RET_BAD_ALLOC;
  }
  transition_map->transitions_size = new_transitions_size;
  transition_map->transitions = new_transitions;
  // finally set the new transition to the end of the array
  transition_map->transitions[transition_map->transitions_size - 1] = transition;

  // we have to copy the transitons here once more to the actual state
  // as we can't assign only the pointer. This pointer gets invalidated whenever
  // we add a new transition and re-shuffle/re-allocate new memory for it.
  unsigned int new_valid_transitions_size = state->valid_transition_size + 1;
  rcl_lifecycle_transition_t * new_valid_transitions = allocator->reallocate(
    state->valid_transitions,
    new_valid_transitions_size * sizeof(rcl_lifecycle_transition_t),
    allocator->state);
  if (!new_valid_transitions) {
    RCL_SET_ERROR_MSG("failed to reallocate memory for new transitions on state");
    return RCL_RET_BAD_ALLOC;
  }
  state->valid_transition_size = new_valid_transitions_size;
  state->valid_transitions = new_valid_transitions;

  state->valid_transitions[state->valid_transition_size - 1] = transition;

  return RCL_RET_OK;
}

rcl_lifecycle_state_t *
rcl_lifecycle_get_state(
  rcl_lifecycle_transition_map_t * transition_map,
  unsigned int state_id)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    transition_map, "transition_map pointer is null\n", return NULL);
  for (unsigned int i = 0; i < transition_map->states_size; ++i) {
    if (transition_map->states[i].id == state_id) {
      return &transition_map->states[i];
    }
  }
  return NULL;
}

rcl_lifecycle_transition_t *
rcl_lifecycle_get_transitions(
  rcl_lifecycle_transition_map_t * transition_map,
  unsigned int transition_id)
{
  RCL_CHECK_FOR_NULL_WITH_MSG(
    transition_map, "transition_map pointer is null\n", return NULL);
  for (unsigned int i = 0; i < transition_map->transitions_size; ++i) {
    if (transition_map->transitions[i].id == transition_id) {
      return &transition_map->transitions[i];
    }
  }
  return NULL;
}

#ifdef __cplusplus
}
#endif
