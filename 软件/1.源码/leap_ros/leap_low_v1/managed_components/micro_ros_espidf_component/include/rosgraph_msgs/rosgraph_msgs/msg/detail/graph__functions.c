// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from rosgraph_msgs:msg/Graph.idl
// generated code does not contain a copyright notice
#include "rosgraph_msgs/msg/detail/graph__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `nodes`
#include "rosgraph_msgs/msg/detail/node__functions.h"

bool
rosgraph_msgs__msg__Graph__init(rosgraph_msgs__msg__Graph * msg)
{
  if (!msg) {
    return false;
  }
  // nodes
  if (!rosgraph_msgs__msg__Node__Sequence__init(&msg->nodes, 0)) {
    rosgraph_msgs__msg__Graph__fini(msg);
    return false;
  }
  return true;
}

void
rosgraph_msgs__msg__Graph__fini(rosgraph_msgs__msg__Graph * msg)
{
  if (!msg) {
    return;
  }
  // nodes
  rosgraph_msgs__msg__Node__Sequence__fini(&msg->nodes);
}

bool
rosgraph_msgs__msg__Graph__are_equal(const rosgraph_msgs__msg__Graph * lhs, const rosgraph_msgs__msg__Graph * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // nodes
  if (!rosgraph_msgs__msg__Node__Sequence__are_equal(
      &(lhs->nodes), &(rhs->nodes)))
  {
    return false;
  }
  return true;
}

bool
rosgraph_msgs__msg__Graph__copy(
  const rosgraph_msgs__msg__Graph * input,
  rosgraph_msgs__msg__Graph * output)
{
  if (!input || !output) {
    return false;
  }
  // nodes
  if (!rosgraph_msgs__msg__Node__Sequence__copy(
      &(input->nodes), &(output->nodes)))
  {
    return false;
  }
  return true;
}

rosgraph_msgs__msg__Graph *
rosgraph_msgs__msg__Graph__create()
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  rosgraph_msgs__msg__Graph * msg = (rosgraph_msgs__msg__Graph *)allocator.allocate(sizeof(rosgraph_msgs__msg__Graph), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(rosgraph_msgs__msg__Graph));
  bool success = rosgraph_msgs__msg__Graph__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
rosgraph_msgs__msg__Graph__destroy(rosgraph_msgs__msg__Graph * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    rosgraph_msgs__msg__Graph__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
rosgraph_msgs__msg__Graph__Sequence__init(rosgraph_msgs__msg__Graph__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  rosgraph_msgs__msg__Graph * data = NULL;

  if (size) {
    data = (rosgraph_msgs__msg__Graph *)allocator.zero_allocate(size, sizeof(rosgraph_msgs__msg__Graph), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = rosgraph_msgs__msg__Graph__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        rosgraph_msgs__msg__Graph__fini(&data[i - 1]);
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
rosgraph_msgs__msg__Graph__Sequence__fini(rosgraph_msgs__msg__Graph__Sequence * array)
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
      rosgraph_msgs__msg__Graph__fini(&array->data[i]);
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

rosgraph_msgs__msg__Graph__Sequence *
rosgraph_msgs__msg__Graph__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  rosgraph_msgs__msg__Graph__Sequence * array = (rosgraph_msgs__msg__Graph__Sequence *)allocator.allocate(sizeof(rosgraph_msgs__msg__Graph__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = rosgraph_msgs__msg__Graph__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
rosgraph_msgs__msg__Graph__Sequence__destroy(rosgraph_msgs__msg__Graph__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    rosgraph_msgs__msg__Graph__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
rosgraph_msgs__msg__Graph__Sequence__are_equal(const rosgraph_msgs__msg__Graph__Sequence * lhs, const rosgraph_msgs__msg__Graph__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!rosgraph_msgs__msg__Graph__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
rosgraph_msgs__msg__Graph__Sequence__copy(
  const rosgraph_msgs__msg__Graph__Sequence * input,
  rosgraph_msgs__msg__Graph__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(rosgraph_msgs__msg__Graph);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    rosgraph_msgs__msg__Graph * data =
      (rosgraph_msgs__msg__Graph *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!rosgraph_msgs__msg__Graph__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          rosgraph_msgs__msg__Graph__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!rosgraph_msgs__msg__Graph__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
