# RMW Micro XRCE-DDS implementation

[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)

## Overview

All packages contained in this repository are a part of the Micro-ROS project stack.
For more information about micro-ROS project click [here](https://micro.ros.org/).

## Packages

The repository contains the following packages:

### rmw_microxrcedds_c

This layer is the ROS 2 Middleware Abstraction Interface written in C.
This package provides a middleware implementation for XRCE-DDS (rmw layer).
The implementation wraps the latest code from eProsima's Micro XRCE-DDS client to communicate with the DDS world.
This library defines the interface used by upper layers in the ROS 2 stack, and that is implemented using XRCE-DDS middleware in the lower layers.

#### Library configuration

This RMW implementation can be configured via CMake arguments, its usual to configure them via `colcon.meta` file in a micro-ROS enviroment.

Most of these configuration are related to memory management because this RMW implementation tries to fully rely on static memory assignations. This leads to an upper bound in memory assignations, which is configured by the user before the build process.
By default, the package sets the values for all memory bounded. The upper bound is configurable by a file that sets the values during the build process.

More details about RMW Micro XRCE-DDS can be found [here](https://micro.ros.org/docs/tutorials/advanced/microxrcedds_rmw_configuration/).
All the configurable parameters are:

| Name                                      | Description                                                                                                                                                                                    | Default |
| ----------------------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ------- |
| RMW_UXRCE_TRANSPORT                       | Sets Micro XRCE-DDS transport to use. (udp, serial, custom)                                                                                                                                    | udp     |
| RMW_UXRCE_IPV                             | Sets Micro XRCE-DDS IP version to use. (ipv4, ipv6)                                                                                                                                            | ipv4    |
| RMW_UXRCE_CREATION_MODE                   | Sets creation mode in Micro XRCE-DDS. (bin, refs)                                                                                                                                              | bin     |
| RMW_UXRCE_MAX_HISTORY                     | This value sets the number of history slots available for RMW subscriptions, </br> requests and replies                                                                                        | 8       |
| RMW_UXRCE_MAX_SESSIONS                    | This value sets the maximum number of Micro XRCE-DDS sessions.                                                                                                                                 | 1       |
| RMW_UXRCE_MAX_NODES                       | This value sets the maximum number of nodes.                                                                                                                                                   | 4       |
| RMW_UXRCE_MAX_PUBLISHERS                  | This value sets the maximum number of topic publishers for an application.                                                                                                                           | 4       |
| RMW_UXRCE_MAX_SUBSCRIPTIONS               | This value sets the maximum number of topic subscriptions for an application.                                                                                                                        | 4       |
| RMW_UXRCE_MAX_SERVICES                    | This value sets the maximum number of service servers for an application.                                                                                                                             | 4       |
| RMW_UXRCE_MAX_CLIENTS                     | This value sets the maximum number of service clients for an application.                                                                                                                              | 4       |
| RMW_UXRCE_MAX_TOPICS                      | This value sets the maximum number of topics for an application. </br> If set to -1 RMW_UXRCE_MAX_TOPICS = RMW_UXRCE_MAX_PUBLISHERS + </br> RMW_UXRCE_MAX_SUBSCRIPTIONS + RMW_UXRCE_MAX_NODES. | -1      |
| RMW_UXRCE_MAX_WAIT_SETS                   | This value sets the maximum number of wait sets for an application.                                                                                                                            | 4       |
| RMW_UXRCE_MAX_GUARD_CONDITION             | This value sets the maximum number of guard conditions for an application.                                                                                                                     | 4       |
| RMW_UXRCE_NODE_NAME_MAX_LENGTH            | This value sets the maximum number of characters for a node name.                                                                                                                              | 60      |
| RMW_UXRCE_TOPIC_NAME_MAX_LENGTH           | This value sets the maximum number of characters for a topic name.                                                                                                                             | 60      |
| RMW_UXRCE_TYPE_NAME_MAX_LENGTH            | This value sets the maximum number of characters for a type name.                                                                                                                              | 100     |
| RMW_UXRCE_REF_BUFFER_LENGTH               | This value sets the maximum number of characters for a reference buffer.                                                                                                                       | 100     |
| RMW_UXRCE_ENTITY_CREATION_DESTROY_TIMEOUT | This value sets the default maximum time to wait for an XRCE entity creation </br> and destroy in milliseconds. If set to 0 best effort is used.                                               | 1000    |
| RMW_UXRCE_ENTITY_CREATION_TIMEOUT         | This value sets the maximum time to wait for an XRCE entity creation </br> in milliseconds. If set to 0 best effort is used.                                                                   | 1000    |
| RMW_UXRCE_ENTITY_DESTROY_TIMEOUT          | This value sets the maximum time to wait for an XRCE entity destroy </br> in milliseconds. If set to 0 best effort is used.                                                                    | 1000    |
| RMW_UXRCE_PUBLISH_RELIABLE_TIMEOUT        | This value sets the default time to wait for a publication in a </br> reliable mode in milliseconds.                                                                                           | 1000    |
| RMW_UXRCE_STREAM_HISTORY                  | This value sets the number of MTUs to buffer, both input and output. Must be a power-of-two.                                                                                                   | 4       |
| RMW_UXRCE_STREAM_HISTORY_INPUT            | This value sets the number of MTUs to input buffer. </br> It will be ignored if RMW_UXRCE_STREAM_HISTORY_OUTPUT is blank. If set, must be a power-of-two.                                      | -       |
| RMW_UXRCE_STREAM_HISTORY_OUTPUT           | This value sets the number of MTUs to output buffer. </br> It will be ignored if RMW_UXRCE_STREAM_HISTORY_INPUT is blank. If set, must be a power-of-two.                                      | -       |
| RMW_UXRCE_GRAPH                           | Allows to perform graph-related operations to the user                                                                                                                                         | OFF     |
| RMW_UXRCE_ALLOW_DYNAMIC_ALLOCATIONS       | Enables increasing static pools with dynamic allocation when needed.                                                                                                                           | OFF     |


## Purpose of the Project

This software is not ready for production use. It has neither been developed nor
tested for a specific use case. However, the license conditions of the
applicable Open Source licenses allow you to adapt the software to your needs.
Before using it in a safety relevant setting, make sure that the software
fulfills your requirements and adjust it according to any applicable safety
standards, e.g., ISO 26262.

## License

This repository is open-sourced under the Apache-2.0 license. See the [LICENSE](LICENSE) file for details.

For a list of other open-source components included in this repository,
see the file [3rd-party-licenses.txt](3rd-party-licenses.txt).

## Known Issues/Limitations

There are no known limitations.
