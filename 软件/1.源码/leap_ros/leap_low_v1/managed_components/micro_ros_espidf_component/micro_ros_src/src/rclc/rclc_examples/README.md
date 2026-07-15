General information about this repository, including legal information, build instructions and known issues/limitations, are given in [README.md](../README.md) in the repository root.

# The rclc_examples package

The rclc_examples package provides examples for using the RCLC-Exector and convenience functions for creating RCL objects like subscriptions and timers. 

## Table of contents

- [Minimal publisher-subscriber](#minimal-publisher-subscriber)
- [Minimal publisher-subscriber only with RCL-API](#minimal-publisher-subscriber-only-with-rcl-api)
- [RCLC-Executor with trigger function](#rclc-executor-with-trigger-function)
- [Service and client node](#service-and-client-node)
- [Action server and client](#action-server-and-client)
- [Lifecycle node](#lifecycle-node)
- [Parameter server](#parameter-server)
- [Subscription callback with C++ class method](#subscription-callback-with-c++-class-method)
- [Subscription with context](#subscription-with-context)
- [Real-time concurrency with slow timer and long subscription](#real-time-concurrency-with-slow-timer-and-long-subscription)

## Minimal publisher-subscriber
The example [example_executor.c](src/example_executor.c) demonstrates basic features of the rclc package and the rclc-Executor to setup a publisher and a subscriber. This example uses also the convenience functions to configure rcl objects, like subscriptions, timers, etc. This saves in this case about 24% of lines of code compared the the same application with direct rcl-API, as described in the setup [Minimal publisher-subscriber only with RCL-API](#minimal-publisher-subscriber-only-with-rcl-api).

**Step 1** Setup ROS 2 Workspace

Open a terminal with ROS 2 workspace. Assuming that the ROS 2 installation resides in `/opt/ros/ROSDISTRO`, setup the ROS2 environment by:
```C
~$ source /opt/ros/ROSDISTRO/setup.bash
```

**Step 2** Build the package
Download and build the rclc repository in a workspace (for example `ros2_ws`). Then source the workspace:
```C
~/ros2_ws/$ colcon build --packages-up-to rclc_examples
~/ros2_ws/$ source ./install/local_setup.bash
```

**Step 3** Run the example executor demo.

The binary of the example is `example_executor`.

```C
~/ros2_ws/$ ros2 run rclc_examples example_executor
```

Example output:

```C
Created timer with timeout 1000 ms.
Created subscriber topic_0:
Debug: number of DDS handles: 2
Published message Hello World!
Callback: I heard: Hello World!
Published message Hello World!
Callback: I heard: Hello World!
Published message Hello World!
Callback: I heard: Hello World!
Published message Hello World!
Callback: I heard: Hello World!
Published message Hello World!
Callback: I heard: Hello World!
```

## Minimal publisher-subscriber only with RCL-API

**Step 1** Setup ROS 2 Workspace

Open a terminal with ROS 2 workspace. Assuming that the ROS 2 installation resides in `/opt/ros/ROSDISTRO`, setup the ROS2 environment by:
```C
~$ source /opt/ros/ROSDISTRO/setup.bash
```

**Step 2** Build the package
Download the rclc repository in a workspace (for example `ros2_ws`). Then source the workspace:
```C
~/ros2_ws/$ colcon build --packages-up-to rclc_examples
~/ros2_ws/$ source ./install/local_setup.bash
```
It should build these packages:
- rcl_yaml_param_parser
- rcl
- rclc
- rclc_examples


**Step 3** Run the example executor.

The binary of the example is `example_executor_only_rcl`.

```C
~/ros2_ws/$ ros2 run rclc_examples example_executor_only_rcl
```
The publisher publishes the message `Hello World!`in `topic_0` at a rate of 1Hz and the subscriber prints out in the callback `Callback: I heard: Hello World!`.

You should see the following output:

```C
Created timer with timeout 1000 ms.
Created subscriber topic_0:
Debug: number of DDS handles: 2
Published message Hello World!
Callback: I heard: Hello World!
Published message Hello World!
Callback: I heard: Hello World!
Published message Hello World!
Callback: I heard: Hello World!
Published message Hello World!
Callback: I heard: Hello World!
Published message Hello World!
Callback: I heard: Hello World!
```

## RCLC-Executor with trigger function
[example_executor_trigger.c](src/example_executor_trigger.c) demonstrates the rclc-Executor with a trigger function.

**Step 1, Step 2**
To setup ROS2 workspace and build the package refer to Step 1 and Step 2 in the [Minimal publisher-subscriber](#minimal-publisher-subscriber).

**Step 3**
This example implements two RCLC Executors, one for publishing `executor_pub`and one for subscribing messages `executor_sub`.

The Executor `executor_pub` publishes string `topic_0` every 100ms (using a timer with 100ms) and an integer `topic_1` every 1000ms (using a timer with 1000ms).

With the trigger condition `rclc_executor_trigger_any` this executor publishes whenenver any timer is ready.

Executor `executor_sub` has two subscriptions, `my_string_sub` and `my_int_sub` subscribing to `topic_0` and `topic_1`, respectivly.

With the trigger condition `rclc_executor_trigger_all` this executor starts evaluating the callbacks only when both messages have arrived. To make this clearly visible, we set the quality of service parameter of the length of the DDS-queue to 0 for subscription `my_string_sub`, which subscribes to `topic_0` with the higher rate.

```C
my_subscription_options.qos.depth = 0
rc = rcl_subscription_init(
  &my_string_sub,
  &my_node,
  my_type_support,
  topic_name,
  &my_subscription_options);
```

Consequently, the messages "in between" are lost.

The binary of the example is `example_executor_trigger`. You run the example with:
```C
~/ros2_ws/$ ros2 run rclc_examples  example_executor_trigger
```
 Then you should see the following output:

```C
Created timer 'my_string_timer' with timeout 100 ms.
Created 'my_int_timer' with timeout 1000 ms.
Created subscriber topic_0:
Created subscriber topic_1:
Executor_pub: number of DDS handles: 2
Executor_sub: number of DDS handles: 2
Published: Hello World! 0
Published: Hello World! 1
Published: Hello World! 2
Published: Hello World! 3
Published: Hello World! 4
Published: Hello World! 5
Published: Hello World! 6
Published: Hello World! 7
Published: Hello World! 8
Published: Hello World! 9
Published: 0
Callback 1: Hello World! 9  <---
Callback 2: 0               <---
Published: Hello World! 10
Published: Hello World! 11
Published: Hello World! 12
Published: Hello World! 13
Published: Hello World! 14
Published: Hello World! 15
Published: Hello World! 16
Published: Hello World! 17
Published: Hello World! 18
Published: Hello World! 19
Published: 1
Callback 1: Hello World! 19 <---
Callback 2: 1               <---
```
The results show, that the callbacks are triggered together, only when the integer message `topic_1` was published and received. At that moment the current string message of the `topic_0` is processed as well.

## Service and client node
The two files [example_service_node.c](src/example_service_node.c) and [example_client_node.c](src/example_client_node.c) demonstrate service/client functionality in micro-ROS.

**Step 1, Step 2**
To setup ROS2 workspace and build the package refer to Step 1 and Step 2 in the [Minimal publisher-subscriber](#minimal-publisher-subscriber).

**Step 3**
Open two Terminal windows and source the ROS 2 distribution/install/setup.bash and rclc repository/install/local_setup.bash.

window 1: start service node
```C
$ ros2 run rclc_examples example_service_node
INFO: rcl_wait timeout 10 ms
Service request value: 24 + 42. Seq 1
Received service response 24 + 42 = 66. Seq 1
```

window 2: start client node
```C
~$ ros2 run rclc_examples example_client_node
Send service request 24 + 42. Seq 1
INFO: rcl_wait timeout 10 ms
```

A request message is sent from the client node to the service node and answered.


## Action server and client
The files [example_action_client.c](src/example_action_client.c) and [example_action_server.c](src/example_action_server.c) demonstrate the action client and action server functionality in micro-ROS.

## Lifecycle node
The file [example_lifecycle_node.c](src/example_lifecycle_node.c)  demonstrates the lifecycle node functionality in micro-ROS.

## Parameter server
The file [example_parameter_server.c](src/example_parameter_server.c)  demonstrates the parameter server functionality in micro-ROS.

## Subscription callback with C++ class method
The files [example_pingpong.cpp](src/example_pingpong.cpp), [example_pingpong_helper.h](src/example_pingpong_helper.h), [example_pingpong_helper.c](src/example_pingpong_helper.c) implement a ping-pong demo using a method of a C++ class as subscription callback.

## Subscription with context
The file [example_sub_context.c](src/example_sub_context.c) shows, how to use a subscription with a context. This allows the subscription to access some other data structure additionally to the message data.

## Real-time concurrency with slow timer and long subscription
The example [example_short_timer_long_subscription.c](src/example_short_timer_long_subscription.c) demonstrates what happens, if a high frequency timer (every 100ms) and
a subscription with a long processing time is managed by one executor. This demo shows, that the timer events are dropped during the long processing time of the subscription and are also not caught-up when there would be sufficient time.