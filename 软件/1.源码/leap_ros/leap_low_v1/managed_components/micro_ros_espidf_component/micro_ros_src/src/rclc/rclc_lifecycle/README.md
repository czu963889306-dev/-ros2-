# The rclc_lifecycle package

## Overview

The rclc_lifecycle package is a [ROS 2](http://www.ros2.org/) package and provides convenience functions to bundle a ROS Client Library (RCL) node with the ROS 2 Node Lifecycle state machine in the C programming language, similar to the [rclcpp Lifecycle Node](https://github.com/ros2/rclcpp/blob/master/rclcpp_lifecycle/include/rclcpp_lifecycle/lifecycle_node.hpp) for C++.

The quality declaration is available in [QUALITY_DECLARATION.md](QUALITY_DECLARATION.md).
## API

The API of the RCLC Lifecycle Node can be divided in several phases: Initialization, Running and Clean-Up.

### Initialization

Creation of a lifecycle node as a bundle of an rcl node and the rcl Node Lifecycle state machine:  

```C
#include "rclc_lifecycle/rclc_lifecycle.h"

rcl_allocator_t allocator = rcl_get_default_allocator();
rclc_support_t support;
rcl_ret_t rc;

// create rcl node
rc = rclc_support_init(&support, argc, argv, &allocator);
rcl_node_t my_node;
rc = rclc_node_init_default(&my_node, "lifecycle_node", "rclc", &support);

// rcl state machine
rcl_lifecycle_state_machine_t state_machine_ =
  rcl_lifecycle_get_zero_initialized_state_machine();
...

// create the lifecycle node
rclc_lifecycle_node_t lifecycle_node;
rcl_ret_t rc = rclc_make_node_a_lifecycle_node(
  &lifecycle_node,
  &my_node,
  &state_machine_,
  &allocator);
```

Register lifecycle services and optionally create callbacks for state changes. Executor needsto be equipped with 1 handle per node _and_ per service:  

```C
// Executor
rclc_executor_t executor = rclc_executor_get_zero_initialized_executor();
rclc_executor_init(
  &executor,
  &support.context,
  4,  // 1 for the node + 1 for each lifecycle service
  &allocator));
...

// Register lifecycle services
rclc_lifecycle_add_get_state_service(&lifecycle_node, &executor);
rclc_lifecycle_add_get_available_states_service(&lifecycle_node, &executor);
rclc_lifecycle_add_change_state_service(&lifecycle_node, &executor);

// Register lifecycle service callbacks
rclc_lifecycle_register_on_configure(&lifecycle_node, &my_on_configure);
rclc_lifecycle_register_on_activate(&lifecycle_node, &my_on_activate);
...
```

### Cleaning Up

To clean everything up, do:  

```C
rc += rcl_lifecycle_node_fini(&lifecycle_node, &allocator);
...
```

## Example

An example, how to use the RCLC Lifecycle Node is given in the file `lifecycle_node.c` in the [rclc_examples](../rclc_examples) package.

## Limitations

* Lifecycle services cannot yet be called via `ros2 lifecycle` CLI, e.g., `ros2 lifecycle set /node configure`. Instead use the `ros2 service` CLI, e.g., `ros2 service call /node/change_state lifecycle_msgs/ChangeState "{transition: {id: 1, label: configure}}"`.
