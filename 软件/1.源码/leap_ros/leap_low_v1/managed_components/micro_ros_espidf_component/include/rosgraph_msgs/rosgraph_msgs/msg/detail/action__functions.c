// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from rosgraph_msgs:msg/Action.idl
// generated code does not contain a copyright notice
#include "rosgraph_msgs/msg/detail/action__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `name`
#include "rosidl_runtime_c/string_functions.h"
// Member `send_goal`
// Member `get_result`
// Member `cancel_goal`
#include "rosgraph_msgs/msg/detail/service__functions.h"
// Member `feedback`
// Member `status`
#include "rosgraph_msgs/msg/detail/topic__functions.h"

bool
rosgraph_msgs__msg__Action__init(rosgraph_msgs__msg__Action * msg)
{
  if (!msg) {
    return false;
  }
  // name
  if (!rosidl_runtime_c__String__init(&msg->name)) {
    rosgraph_msgs__msg__Action__fini(msg);
    return false;
  }
  // send_goal
  if (!rosgraph_msgs__msg__Service__init(&msg->send_goal)) {
    rosgraph_msgs__msg__Action__fini(msg);
    return false;
  }
  // get_result
  if (!rosgraph_msgs__msg__Service__init(&msg->get_result)) {
    rosgraph_msgs__msg__Action__fini(msg);
    return false;
  }
  // cancel_goal
  if (!rosgraph_msgs__msg__Service__init(&msg->cancel_goal)) {
    rosgraph_msgs__msg__Action__fini(msg);
    return false;
  }
  // feedback
  if (!rosgraph_msgs__msg__Topic__init(&msg->feedback)) {
    rosgraph_msgs__msg__Action__fini(msg);
    return false;
  }
  // status
  if (!rosgraph_msgs__msg__Topic__init(&msg->status)) {
    rosgraph_msgs__msg__Action__fini(msg);
    return false;
  }
  return true;
}

void
rosgraph_msgs__msg__Action__fini(rosgraph_msgs__msg__Action * msg)
{
  if (!msg) {
    return;
  }
  // name
  rosidl_runtime_c__String__fini(&msg->name);
  // send_goal
  rosgraph_msgs__msg__Service__fini(&msg->send_goal);
  // get_result
  rosgraph_msgs__msg__Service__fini(&msg->get_result);
  // cancel_goal
  rosgraph_msgs__msg__Service__fini(&msg->cancel_goal);
  // feedback
  rosgraph_msgs__msg__Topic__fini(&msg->feedback);
  // status
  rosgraph_msgs__msg__Topic__fini(&msg->status);
}

bool
rosgraph_msgs__msg__Action__are_equal(const rosgraph_msgs__msg__Action * lhs, const rosgraph_msgs__msg__Action * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // name
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->name), &(rhs->name)))
  {
    return false;
  }
  // send_goal
  if (!rosgraph_msgs__msg__Service__are_equal(
      &(lhs->send_goal), &(rhs->send_goal)))
  {
    return false;
  }
  // get_result
  if (!rosgraph_msgs__msg__Service__are_equal(
      &(lhs->get_result), &(rhs->get_result)))
  {
    return false;
  }
  // cancel_goal
  if (!rosgraph_msgs__msg__Service__are_equal(
      &(lhs->cancel_goal), &(rhs->cancel_goal)))
  {
    return false;
  }
  // feedback
  if (!rosgraph_msgs__msg__Topic__are_equal(
      &(lhs->feedback), &(rhs->feedback)))
  {
    return false;
  }
  // status
  if (!rosgraph_msgs__msg__Topic__are_equal(
      &(lhs->status), &(rhs->status)))
  {
    return false;
  }
  return true;
}

bool
rosgraph_msgs__msg__Action__copy(
  const rosgraph_msgs__msg__Action * input,
  rosgraph_msgs__msg__Action * output)
{
  if (!input || !output) {
    return false;
  }
  // name
  if (!rosidl_runtime_c__String__copy(
      &(input->name), &(output->name)))
  {
    return false;
  }
  // send_goal
  if (!rosgraph_msgs__msg__Service__copy(
      &(input->send_goal), &(output->send_goal)))
  {
    return false;
  }
  // get_result
  if (!rosgraph_msgs__msg__Service__copy(
      &(input->get_result), &(output->get_result)))
  {
    return false;
  }
  // cancel_goal
  if (!rosgraph_msgs__msg__Service__copy(
      &(input->cancel_goal), &(output->cancel_goal)))
  {
    return false;
  }
  // feedback
  if (!rosgraph_msgs__msg__Topic__copy(
      &(input->feedback), &(output->feedback)))
  {
    return false;
  }
  // status
  if (!rosgraph_msgs__msg__Topic__copy(
      &(input->status), &(output->status)))
  {
    return false;
  }
  return true;
}

rosgraph_msgs__msg__Action *
rosgraph_msgs__msg__Action__create()
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  rosgraph_msgs__msg__Action * msg = (rosgraph_msgs__msg__Action *)allocator.allocate(sizeof(rosgraph_msgs__msg__Action), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(rosgraph_msgs__msg__Action));
  bool success = rosgraph_msgs__msg__Action__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
rosgraph_msgs__msg__Action__destroy(rosgraph_msgs__msg__Action * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    rosgraph_msgs__msg__Action__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
rosgraph_msgs__msg__Action__Sequence__init(rosgraph_msgs__msg__Action__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  rosgraph_msgs__msg__Action * data = NULL;

  if (size) {
    data = (rosgraph_msgs__msg__Action *)allocator.zero_allocate(size, sizeof(rosgraph_msgs__msg__Action), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = rosgraph_msgs__msg__Action__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        rosgraph_msgs__msg__Action__fini(&data[i - 1]);
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
rosgraph_msgs__msg__Action__Sequence__fini(rosgraph_msgs__msg__Action__Sequence * array)
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
      rosgraph_msgs__msg__Action__fini(&array->data[i]);
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

rosgraph_msgs__msg__Action__Sequence *
rosgraph_msgs__msg__Action__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  rosgraph_msgs__msg__Action__Sequence * array = (rosgraph_msgs__msg__Action__Sequence *)allocator.allocate(sizeof(rosgraph_msgs__msg__Action__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = rosgraph_msgs__msg__Action__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
rosgraph_msgs__msg__Action__Sequence__destroy(rosgraph_msgs__msg__Action__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    rosgraph_msgs__msg__Action__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
rosgraph_msgs__msg__Action__Sequence__are_equal(const rosgraph_msgs__msg__Action__Sequence * lhs, const rosgraph_msgs__msg__Action__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!rosgraph_msgs__msg__Action__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
rosgraph_msgs__msg__Action__Sequence__copy(
  const rosgraph_msgs__msg__Action__Sequence * input,
  rosgraph_msgs__msg__Action__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(rosgraph_msgs__msg__Action);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    rosgraph_msgs__msg__Action * data =
      (rosgraph_msgs__msg__Action *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!rosgraph_msgs__msg__Action__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          rosgraph_msgs__msg__Action__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!rosgraph_msgs__msg__Action__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
