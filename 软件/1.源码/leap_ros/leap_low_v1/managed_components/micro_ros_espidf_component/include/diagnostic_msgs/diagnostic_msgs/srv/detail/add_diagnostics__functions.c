// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from diagnostic_msgs:srv/AddDiagnostics.idl
// generated code does not contain a copyright notice
#include "diagnostic_msgs/srv/detail/add_diagnostics__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"

// Include directives for member types
// Member `load_namespace`
#include "rosidl_runtime_c/string_functions.h"

bool
diagnostic_msgs__srv__AddDiagnostics_Request__init(diagnostic_msgs__srv__AddDiagnostics_Request * msg)
{
  if (!msg) {
    return false;
  }
  // load_namespace
  if (!rosidl_runtime_c__String__init(&msg->load_namespace)) {
    diagnostic_msgs__srv__AddDiagnostics_Request__fini(msg);
    return false;
  }
  return true;
}

void
diagnostic_msgs__srv__AddDiagnostics_Request__fini(diagnostic_msgs__srv__AddDiagnostics_Request * msg)
{
  if (!msg) {
    return;
  }
  // load_namespace
  rosidl_runtime_c__String__fini(&msg->load_namespace);
}

bool
diagnostic_msgs__srv__AddDiagnostics_Request__are_equal(const diagnostic_msgs__srv__AddDiagnostics_Request * lhs, const diagnostic_msgs__srv__AddDiagnostics_Request * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // load_namespace
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->load_namespace), &(rhs->load_namespace)))
  {
    return false;
  }
  return true;
}

bool
diagnostic_msgs__srv__AddDiagnostics_Request__copy(
  const diagnostic_msgs__srv__AddDiagnostics_Request * input,
  diagnostic_msgs__srv__AddDiagnostics_Request * output)
{
  if (!input || !output) {
    return false;
  }
  // load_namespace
  if (!rosidl_runtime_c__String__copy(
      &(input->load_namespace), &(output->load_namespace)))
  {
    return false;
  }
  return true;
}

diagnostic_msgs__srv__AddDiagnostics_Request *
diagnostic_msgs__srv__AddDiagnostics_Request__create()
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  diagnostic_msgs__srv__AddDiagnostics_Request * msg = (diagnostic_msgs__srv__AddDiagnostics_Request *)allocator.allocate(sizeof(diagnostic_msgs__srv__AddDiagnostics_Request), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(diagnostic_msgs__srv__AddDiagnostics_Request));
  bool success = diagnostic_msgs__srv__AddDiagnostics_Request__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
diagnostic_msgs__srv__AddDiagnostics_Request__destroy(diagnostic_msgs__srv__AddDiagnostics_Request * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    diagnostic_msgs__srv__AddDiagnostics_Request__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
diagnostic_msgs__srv__AddDiagnostics_Request__Sequence__init(diagnostic_msgs__srv__AddDiagnostics_Request__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  diagnostic_msgs__srv__AddDiagnostics_Request * data = NULL;

  if (size) {
    data = (diagnostic_msgs__srv__AddDiagnostics_Request *)allocator.zero_allocate(size, sizeof(diagnostic_msgs__srv__AddDiagnostics_Request), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = diagnostic_msgs__srv__AddDiagnostics_Request__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        diagnostic_msgs__srv__AddDiagnostics_Request__fini(&data[i - 1]);
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
diagnostic_msgs__srv__AddDiagnostics_Request__Sequence__fini(diagnostic_msgs__srv__AddDiagnostics_Request__Sequence * array)
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
      diagnostic_msgs__srv__AddDiagnostics_Request__fini(&array->data[i]);
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

diagnostic_msgs__srv__AddDiagnostics_Request__Sequence *
diagnostic_msgs__srv__AddDiagnostics_Request__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  diagnostic_msgs__srv__AddDiagnostics_Request__Sequence * array = (diagnostic_msgs__srv__AddDiagnostics_Request__Sequence *)allocator.allocate(sizeof(diagnostic_msgs__srv__AddDiagnostics_Request__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = diagnostic_msgs__srv__AddDiagnostics_Request__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
diagnostic_msgs__srv__AddDiagnostics_Request__Sequence__destroy(diagnostic_msgs__srv__AddDiagnostics_Request__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    diagnostic_msgs__srv__AddDiagnostics_Request__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
diagnostic_msgs__srv__AddDiagnostics_Request__Sequence__are_equal(const diagnostic_msgs__srv__AddDiagnostics_Request__Sequence * lhs, const diagnostic_msgs__srv__AddDiagnostics_Request__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!diagnostic_msgs__srv__AddDiagnostics_Request__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
diagnostic_msgs__srv__AddDiagnostics_Request__Sequence__copy(
  const diagnostic_msgs__srv__AddDiagnostics_Request__Sequence * input,
  diagnostic_msgs__srv__AddDiagnostics_Request__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(diagnostic_msgs__srv__AddDiagnostics_Request);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    diagnostic_msgs__srv__AddDiagnostics_Request * data =
      (diagnostic_msgs__srv__AddDiagnostics_Request *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!diagnostic_msgs__srv__AddDiagnostics_Request__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          diagnostic_msgs__srv__AddDiagnostics_Request__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!diagnostic_msgs__srv__AddDiagnostics_Request__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}


// Include directives for member types
// Member `message`
// already included above
// #include "rosidl_runtime_c/string_functions.h"

bool
diagnostic_msgs__srv__AddDiagnostics_Response__init(diagnostic_msgs__srv__AddDiagnostics_Response * msg)
{
  if (!msg) {
    return false;
  }
  // success
  // message
  if (!rosidl_runtime_c__String__init(&msg->message)) {
    diagnostic_msgs__srv__AddDiagnostics_Response__fini(msg);
    return false;
  }
  return true;
}

void
diagnostic_msgs__srv__AddDiagnostics_Response__fini(diagnostic_msgs__srv__AddDiagnostics_Response * msg)
{
  if (!msg) {
    return;
  }
  // success
  // message
  rosidl_runtime_c__String__fini(&msg->message);
}

bool
diagnostic_msgs__srv__AddDiagnostics_Response__are_equal(const diagnostic_msgs__srv__AddDiagnostics_Response * lhs, const diagnostic_msgs__srv__AddDiagnostics_Response * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // success
  if (lhs->success != rhs->success) {
    return false;
  }
  // message
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->message), &(rhs->message)))
  {
    return false;
  }
  return true;
}

bool
diagnostic_msgs__srv__AddDiagnostics_Response__copy(
  const diagnostic_msgs__srv__AddDiagnostics_Response * input,
  diagnostic_msgs__srv__AddDiagnostics_Response * output)
{
  if (!input || !output) {
    return false;
  }
  // success
  output->success = input->success;
  // message
  if (!rosidl_runtime_c__String__copy(
      &(input->message), &(output->message)))
  {
    return false;
  }
  return true;
}

diagnostic_msgs__srv__AddDiagnostics_Response *
diagnostic_msgs__srv__AddDiagnostics_Response__create()
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  diagnostic_msgs__srv__AddDiagnostics_Response * msg = (diagnostic_msgs__srv__AddDiagnostics_Response *)allocator.allocate(sizeof(diagnostic_msgs__srv__AddDiagnostics_Response), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(diagnostic_msgs__srv__AddDiagnostics_Response));
  bool success = diagnostic_msgs__srv__AddDiagnostics_Response__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
diagnostic_msgs__srv__AddDiagnostics_Response__destroy(diagnostic_msgs__srv__AddDiagnostics_Response * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    diagnostic_msgs__srv__AddDiagnostics_Response__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
diagnostic_msgs__srv__AddDiagnostics_Response__Sequence__init(diagnostic_msgs__srv__AddDiagnostics_Response__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  diagnostic_msgs__srv__AddDiagnostics_Response * data = NULL;

  if (size) {
    data = (diagnostic_msgs__srv__AddDiagnostics_Response *)allocator.zero_allocate(size, sizeof(diagnostic_msgs__srv__AddDiagnostics_Response), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = diagnostic_msgs__srv__AddDiagnostics_Response__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        diagnostic_msgs__srv__AddDiagnostics_Response__fini(&data[i - 1]);
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
diagnostic_msgs__srv__AddDiagnostics_Response__Sequence__fini(diagnostic_msgs__srv__AddDiagnostics_Response__Sequence * array)
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
      diagnostic_msgs__srv__AddDiagnostics_Response__fini(&array->data[i]);
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

diagnostic_msgs__srv__AddDiagnostics_Response__Sequence *
diagnostic_msgs__srv__AddDiagnostics_Response__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  diagnostic_msgs__srv__AddDiagnostics_Response__Sequence * array = (diagnostic_msgs__srv__AddDiagnostics_Response__Sequence *)allocator.allocate(sizeof(diagnostic_msgs__srv__AddDiagnostics_Response__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = diagnostic_msgs__srv__AddDiagnostics_Response__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
diagnostic_msgs__srv__AddDiagnostics_Response__Sequence__destroy(diagnostic_msgs__srv__AddDiagnostics_Response__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    diagnostic_msgs__srv__AddDiagnostics_Response__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
diagnostic_msgs__srv__AddDiagnostics_Response__Sequence__are_equal(const diagnostic_msgs__srv__AddDiagnostics_Response__Sequence * lhs, const diagnostic_msgs__srv__AddDiagnostics_Response__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!diagnostic_msgs__srv__AddDiagnostics_Response__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
diagnostic_msgs__srv__AddDiagnostics_Response__Sequence__copy(
  const diagnostic_msgs__srv__AddDiagnostics_Response__Sequence * input,
  diagnostic_msgs__srv__AddDiagnostics_Response__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(diagnostic_msgs__srv__AddDiagnostics_Response);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    diagnostic_msgs__srv__AddDiagnostics_Response * data =
      (diagnostic_msgs__srv__AddDiagnostics_Response *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!diagnostic_msgs__srv__AddDiagnostics_Response__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          diagnostic_msgs__srv__AddDiagnostics_Response__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!diagnostic_msgs__srv__AddDiagnostics_Response__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
