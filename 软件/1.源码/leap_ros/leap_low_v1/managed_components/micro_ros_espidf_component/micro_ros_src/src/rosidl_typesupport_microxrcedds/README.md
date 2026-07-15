# ROSIDL type support for Micro XRCE-DDS

[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)

## Overview

All packages contained in this repository are a part of the micro-ROS project stack.
For more information about the micro-ROS project click [here](https://micro.ros.org/).

## Packages

The repository contains the following packages:

### rmw_typesupport_microxrcedds_c

This package aims to give support to the rmw layer for ros messages in C language.

#### Includes

- Support for serialization / serialization code, generated during the build process, for each ROS 2 interface.
- Support for unbounded types for incoming messages using static memory.
- Support for building message support using ament extensions for finding not built interfaces.

<!-- #### Documentation

You can access the documentation online, which is hosted on [Read the Docs](). -->

## Purpose of the Project

This software is not ready for production use. It has neither been developed nor
tested for a specific use case. However, the license conditions of the
applicable Open Source licenses allow you to adapt the software to your needs.
Before using it in a safety relevant setting, make sure that the software
fulfills your requirements and adjust it according to any applicable safety
standards, e.g., ISO 26262.

## License

This repository is open-sourced under the Apache-2.0 license. See the [LICENSE](LICENSE) file for details.

For a list of other open-source components included in ROS 2 system_modes,
see the file [3rd-party-licenses.txt](3rd-party-licenses.txt).

## Known Issues/Limitations

There are no known limitations.
