// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from composition_interfaces:srv/ListNodes.idl
// generated code does not contain a copyright notice
#include "composition_interfaces/srv/detail/list_nodes__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"

bool
composition_interfaces__srv__ListNodes_Request__init(composition_interfaces__srv__ListNodes_Request * msg)
{
  if (!msg) {
    return false;
  }
  // structure_needs_at_least_one_member
  return true;
}

void
composition_interfaces__srv__ListNodes_Request__fini(composition_interfaces__srv__ListNodes_Request * msg)
{
  if (!msg) {
    return;
  }
  // structure_needs_at_least_one_member
}

bool
composition_interfaces__srv__ListNodes_Request__are_equal(const composition_interfaces__srv__ListNodes_Request * lhs, const composition_interfaces__srv__ListNodes_Request * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // structure_needs_at_least_one_member
  if (lhs->structure_needs_at_least_one_member != rhs->structure_needs_at_least_one_member) {
    return false;
  }
  return true;
}

bool
composition_interfaces__srv__ListNodes_Request__copy(
  const composition_interfaces__srv__ListNodes_Request * input,
  composition_interfaces__srv__ListNodes_Request * output)
{
  if (!input || !output) {
    return false;
  }
  // structure_needs_at_least_one_member
  output->structure_needs_at_least_one_member = input->structure_needs_at_least_one_member;
  return true;
}

composition_interfaces__srv__ListNodes_Request *
composition_interfaces__srv__ListNodes_Request__create()
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  composition_interfaces__srv__ListNodes_Request * msg = (composition_interfaces__srv__ListNodes_Request *)allocator.allocate(sizeof(composition_interfaces__srv__ListNodes_Request), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(composition_interfaces__srv__ListNodes_Request));
  bool success = composition_interfaces__srv__ListNodes_Request__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
composition_interfaces__srv__ListNodes_Request__destroy(composition_interfaces__srv__ListNodes_Request * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    composition_interfaces__srv__ListNodes_Request__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
composition_interfaces__srv__ListNodes_Request__Sequence__init(composition_interfaces__srv__ListNodes_Request__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  composition_interfaces__srv__ListNodes_Request * data = NULL;

  if (size) {
    data = (composition_interfaces__srv__ListNodes_Request *)allocator.zero_allocate(size, sizeof(composition_interfaces__srv__ListNodes_Request), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = composition_interfaces__srv__ListNodes_Request__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        composition_interfaces__srv__ListNodes_Request__fini(&data[i - 1]);
      }
      allocator.deallocate(data, allocator.state);
      return false;
    }
  }
  array->data = data;
  array->size = size;
  array->capacity = size;
  return true;
}

void
composition_interfaces__srv__ListNodes_Request__Sequence__fini(composition_interfaces__srv__ListNodes_Request__Sequence * array)
{
  if (!array) {
    return;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      composition_interfaces__srv__ListNodes_Request__fini(&array->data[i]);
    }
    allocator.deallocate(array->data, allocator.state);
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
  } else {
    // ensure that data, size, and capacity values are consistent
    assert(0 == array->size);
    assert(0 == array->capacity);
  }
}

composition_interfaces__srv__ListNodes_Request__Sequence *
composition_interfaces__srv__ListNodes_Request__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  composition_interfaces__srv__ListNodes_Request__Sequence * array = (composition_interfaces__srv__ListNodes_Request__Sequence *)allocator.allocate(sizeof(composition_interfaces__srv__ListNodes_Request__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = composition_interfaces__srv__ListNodes_Request__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
composition_interfaces__srv__ListNodes_Request__Sequence__destroy(composition_interfaces__srv__ListNodes_Request__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    composition_interfaces__srv__ListNodes_Request__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
composition_interfaces__srv__ListNodes_Request__Sequence__are_equal(const composition_interfaces__srv__ListNodes_Request__Sequence * lhs, const composition_interfaces__srv__ListNodes_Request__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!composition_interfaces__srv__ListNodes_Request__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
composition_interfaces__srv__ListNodes_Request__Sequence__copy(
  const composition_interfaces__srv__ListNodes_Request__Sequence * input,
  composition_interfaces__srv__ListNodes_Request__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(composition_interfaces__srv__ListNodes_Request);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    composition_interfaces__srv__ListNodes_Request * data =
      (composition_interfaces__srv__ListNodes_Request *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!composition_interfaces__srv__ListNodes_Request__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          composition_interfaces__srv__ListNodes_Request__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!composition_interfaces__srv__ListNodes_Request__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}


// Include directives for member types
// Member `full_node_names`
#include "rosidl_runtime_c/string_functions.h"
// Member `unique_ids`
#include "rosidl_runtime_c/primitives_sequence_functions.h"

bool
composition_interfaces__srv__ListNodes_Response__init(composition_interfaces__srv__ListNodes_Response * msg)
{
  if (!msg) {
    return false;
  }
  // full_node_names
  if (!rosidl_runtime_c__String__Sequence__init(&msg->full_node_names, 0)) {
    composition_interfaces__srv__ListNodes_Response__fini(msg);
    return false;
  }
  // unique_ids
  if (!rosidl_runtime_c__uint64__Sequence__init(&msg->unique_ids, 0)) {
    composition_interfaces__srv__ListNodes_Response__fini(msg);
    return false;
  }
  return true;
}

void
composition_interfaces__srv__ListNodes_Response__fini(composition_interfaces__srv__ListNodes_Response * msg)
{
  if (!msg) {
    return;
  }
  // full_node_names
  rosidl_runtime_c__String__Sequence__fini(&msg->full_node_names);
  // unique_ids
  rosidl_runtime_c__uint64__Sequence__fini(&msg->unique_ids);
}

bool
composition_interfaces__srv__ListNodes_Response__are_equal(const composition_interfaces__srv__ListNodes_Response * lhs, const composition_interfaces__srv__ListNodes_Response * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // full_node_names
  if (!rosidl_runtime_c__String__Sequence__are_equal(
      &(lhs->full_node_names), &(rhs->full_node_names)))
  {
    return false;
  }
  // unique_ids
  if (!rosidl_runtime_c__uint64__Sequence__are_equal(
      &(lhs->unique_ids), &(rhs->unique_ids)))
  {
    return false;
  }
  return true;
}

bool
composition_interfaces__srv__ListNodes_Response__copy(
  const composition_interfaces__srv__ListNodes_Response * input,
  composition_interfaces__srv__ListNodes_Response * output)
{
  if (!input || !output) {
    return false;
  }
  // full_node_names
  if (!rosidl_runtime_c__String__Sequence__copy(
      &(input->full_node_names), &(output->full_node_names)))
  {
    return false;
  }
  // unique_ids
  if (!rosidl_runtime_c__uint64__Sequence__copy(
      &(input->unique_ids), &(output->unique_ids)))
  {
    return false;
  }
  return true;
}

composition_interfaces__srv__ListNodes_Response *
composition_interfaces__srv__ListNodes_Response__create()
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  composition_interfaces__srv__ListNodes_Response * msg = (composition_interfaces__srv__ListNodes_Response *)allocator.allocate(sizeof(composition_interfaces__srv__ListNodes_Response), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(composition_interfaces__srv__ListNodes_Response));
  bool success = composition_interfaces__srv__ListNodes_Response__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
composition_interfaces__srv__ListNodes_Response__destroy(composition_interfaces__srv__ListNodes_Response * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    composition_interfaces__srv__ListNodes_Response__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
composition_interfaces__srv__ListNodes_Response__Sequence__init(composition_interfaces__srv__ListNodes_Response__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  composition_interfaces__srv__ListNodes_Response * data = NULL;

  if (size) {
    data = (composition_interfaces__srv__ListNodes_Response *)allocator.zero_allocate(size, sizeof(composition_interfaces__srv__ListNodes_Response), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = composition_interfaces__srv__ListNodes_Response__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        composition_interfaces__srv__ListNodes_Response__fini(&data[i - 1]);
      }
      allocator.deallocate(data, allocator.state);
      return false;
    }
  }
  array->data = data;
  array->size = size;
  array->capacity = size;
  return true;
}

void
composition_interfaces__srv__ListNodes_Response__Sequence__fini(composition_interfaces__srv__ListNodes_Response__Sequence * array)
{
  if (!array) {
    return;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      composition_interfaces__srv__ListNodes_Response__fini(&array->data[i]);
    }
    allocator.deallocate(array->data, allocator.state);
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
  } else {
    // ensure that data, size, and capacity values are consistent
    assert(0 == array->size);
    assert(0 == array->capacity);
  }
}

composition_interfaces__srv__ListNodes_Response__Sequence *
composition_interfaces__srv__ListNodes_Response__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  composition_interfaces__srv__ListNodes_Response__Sequence * array = (composition_interfaces__srv__ListNodes_Response__Sequence *)allocator.allocate(sizeof(composition_interfaces__srv__ListNodes_Response__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = composition_interfaces__srv__ListNodes_Response__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
composition_interfaces__srv__ListNodes_Response__Sequence__destroy(composition_interfaces__srv__ListNodes_Response__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    composition_interfaces__srv__ListNodes_Response__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
composition_interfaces__srv__ListNodes_Response__Sequence__are_equal(const composition_interfaces__srv__ListNodes_Response__Sequence * lhs, const composition_interfaces__srv__ListNodes_Response__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!composition_interfaces__srv__ListNodes_Response__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
composition_interfaces__srv__ListNodes_Response__Sequence__copy(
  const composition_interfaces__srv__ListNodes_Response__Sequence * input,
  composition_interfaces__srv__ListNodes_Response__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(composition_interfaces__srv__ListNodes_Response);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    composition_interfaces__srv__ListNodes_Response * data =
      (composition_interfaces__srv__ListNodes_Response *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!composition_interfaces__srv__ListNodes_Response__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          composition_interfaces__srv__ListNodes_Response__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!composition_interfaces__srv__ListNodes_Response__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
