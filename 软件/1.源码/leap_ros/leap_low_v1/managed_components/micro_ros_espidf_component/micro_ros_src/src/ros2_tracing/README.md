# ros2_tracing

[![pipeline status](https://gitlab.com/ros-tracing/ros2_tracing/badges/master/pipeline.svg)](https://gitlab.com/ros-tracing/ros2_tracing/commits/master)
[![codecov](https://codecov.io/gl/ros-tracing/ros2_tracing/branch/master/graph/badge.svg)](https://codecov.io/gl/ros-tracing/ros2_tracing)

Tracing tools for ROS 2.

## Overview

`ros2_tracing` provides [tracing instrumentation](#tracetools) for the core ROS 2 packages.
It also provides [tools to configure tracing](#tracing) through [a launch action](#launch-file-trace-action) and a [`ros2` CLI command](#trace-command).

`ros2_tracing` currently only supports the [LTTng](https://lttng.org/) tracer.
Consequently, it currently only supports Linux.

## Publications & presentations

[Read the `ros2_tracing` paper!](https://arxiv.org/abs/2201.00393)
If you use or refer to `ros2_tracing`, please cite:
* C. Bédard, I. Lütkebohle, and M. Dagenais, "ros2_tracing: Multipurpose Low-Overhead Framework for Real-Time Tracing of ROS 2," *IEEE Robotics and Automation Letters*, vol. 7, no. 3, pp. 6511–6518, 2022.

<details>
<summary>BibTeX</summary>

```bibtex
@article{bedard2022ros2tracing,
  title={ros2\_tracing: Multipurpose Low-Overhead Framework for Real-Time Tracing of ROS 2},
  author={B{\'e}dard, Christophe and L{\"u}tkebohle, Ingo and Dagenais, Michel},
  journal={IEEE Robotics and Automation Letters},
  year={2022},
  volume={7},
  number={3},
  pages={6511--6518},
  doi={10.1109/LRA.2022.3174346}
}
```
</details>

Also, check out the ROS World 2021 presentation titled "Tracing ROS 2 with ros2_tracing" ([video](https://vimeo.com/652633418), [slides](https://gitlab.com/ros-tracing/ros2_tracing/-/raw/master/doc/2021-10-20_ROS_World_2021_-_Tracing_ROS_2_with_ros2_tracing.pdf)).
Reference:
* C. Bédard, "Tracing ROS 2 with ros2_tracing," in *ROS World 2021*. Open Robotics, October 2021. [Online]. Available: https://vimeo.com/652633418, [(pdf)](https://gitlab.com/ros-tracing/ros2_tracing/-/raw/master/doc/2021-10-20_ROS_World_2021_-_Tracing_ROS_2_with_ros2_tracing.pdf)

## Tutorials & demos

* Real-Time Working Group documentation tutorial: [How to use `ros2_tracing` to trace and analyze an application](https://ros-realtime.github.io/Guides/ros2_tracing_trace_and_analyze.html)
* ROS World 2021 demo: [github.com/christophebedard/ros-world-2021-demo](https://github.com/christophebedard/ros-world-2021-demo)

## Building

As of Foxy, these instructions also apply to an installation from the Debian packages; it will not work out-of-the-box.

If LTTng is not found during build, or if the [`TRACETOOLS_DISABLED` option is enabled](#disabling-tracing), then this package will not do anything.

To enable tracing:

1. Install [LTTng](https://lttng.org/docs/v2.13/) (`>=2.11.1`) with the Python bindings to control tracing and read traces:
    ```
    $ sudo apt-get update
    $ sudo apt-get install lttng-tools liblttng-ust-dev
    $ sudo apt-get install python3-babeltrace python3-lttng
    ```
    * The above commands will only install the LTTng userspace tracer, LTTng-UST. You only need the userspace tracer to trace ROS 2.
    * To install the [LTTng kernel tracer](https://lttng.org/docs/v2.13/#doc-tracing-the-linux-kernel):
        ```
        $ sudo apt-get install lttng-modules-dkms
        ```
    * For more information about LTTng, [see its documentation](https://lttng.org/docs/v2.13/).
2. Build:
    *  If you've already built ROS 2 from source before installing LTTng, you will need to re-build at least up to `tracetools`:
        ```
        $ colcon build --packages-up-to tracetools --cmake-force-configure
        ```
    * If you rely on the ROS 2 binaries (Debian packages, release binaries, or prerelease binaries), you will need to clone this repo into your workspace and build at least up to `tracetools`:
        ```
        $ cd src/
        $ git clone https://gitlab.com/ros-tracing/ros2_tracing.git
        $ cd ../
        $ colcon build --packages-up-to tracetools
        ```
3. Source and check that tracing is enabled:
    ```
    $ source ./install/setup.bash
    $ ros2 run tracetools status
    Tracing enabled
    ```

### Disabling tracing

Alternatively, to build and disable tracing, use `TRACETOOLS_DISABLED`:

```
$ colcon build --cmake-args " -DTRACETOOLS_DISABLED=ON"
```

This will remove all instrumentation from the core ROS 2 packages, and thus they will not depend on or link against the shared library provided by the [`tracetools` package](#tracetools).

## Tracing

The steps above will not lead to trace data being generated, and thus they will have no impact on execution.
LTTng has to be configured for tracing.
The packages in this repo provide two options: a [command](#trace-command) and a [launch file action](#launch-file-trace-action).

The tracing directory can be configured using command/launch action parameters, or through environment variables with the following logic:

* Use `$ROS_TRACE_DIR` if `ROS_TRACE_DIR` is set and not empty.
* Otherwise, use `$ROS_HOME/tracing`, using `~/.ros` for `ROS_HOME` if not set or if empty.

Additionally, **if you're using kernel tracing with a non-root user, make sure that the [`tracing` group exists and that your user is added to it](https://lttng.org/docs/v2.13/#doc-tracing-group)**.

```
# Create group if it doesn't exist
$ sudo groupadd -r tracing
# Add user to the group
$ sudo usermod -aG tracing $USER
```

### Trace command

The first option is to use the `ros2 trace` command.

```
$ ros2 trace
```

By default, it will enable all ROS tracepoints and a few kernel tracepoints.
The trace will be written to `~/.ros/tracing/session-YYYYMMDDHHMMSS`.
Run the command with `-h` for more information.

You must [install the kernel tracer](#building) if you want to enable kernel events (using the `-k`/`--kernel-events` option).
If have installed the kernel tracer, use kernel tracing, and still encounter an error here, make sure to [add your user to the `tracing` group](#tracing).

### Launch file trace action

Another option is to use the `Trace` action in a Python, XML, or YAML launch file along with your `Node` action(s).
This way, tracing automatically starts when launching the launch file and ends when it exits or when terminated.

```
$ ros2 launch tracetools_launch example.launch.py
```

The `Trace` action will also set the `LD_PRELOAD` environment to preload [LTTng's userspace tracing helper(s)](https://lttng.org/docs/v2.13/#doc-prebuilt-ust-helpers) if the corresponding event(s) are enabled.
For more information, see [this example launch file](./tracetools_launch/launch/example.launch.py) and the [`Trace` action](./tracetools_launch/tracetools_launch/action.py).

You must [install the kernel tracer](#building) if you want to enable kernel events (`events_kernel` in Python, `events-kernel` in XML or YAML).
If have installed the kernel tracer, use kernel tracing, and still encounter an error here, make sure to [add your user to the `tracing` group](#tracing).

## Design

See the [design document](./doc/design_ros_2.md).

## Real-time

LTTng-UST, the current default userspace tracer used for [tracing ROS 2](#overview), was designed for real-time production applications.
It is a low-overhead tracer with many important real-time compatible features:

* userspace tracer completely implemented in userspace, independent from the kernel
* reentrant, thread-safe, signal-safe, non-blocking
* no system calls in the fast path
* no copies of the trace data

However, some settings need to be tuned for it to be fully real-time safe and for performance to be optimal for your use-case:

* timers[^rt-1]: use [read timer](https://lttng.org/docs/v2.13/#doc-channel-read-timer) to avoid a write(2) call
* sub-buffer[^rt-1] count and size:
    * see [documentation](https://lttng.org/docs/v2.13/#doc-channel-subbuf-size-vs-subbuf-count) for sub-buffer count and size tuning tips based on your use-case
    * minimize sub-buffer count to minimize sub-buffer switching overhead
* one-time memory allocation/lock/syscall per thread:
    * usually done the first time a tracepoint is executed within a thread for URCU thread registration, but registration can be manually performed to force it to be done during your application's initialization
    * see [this LTTng mailing list message](https://lists.lttng.org/pipermail/lttng-dev/2019-November/029409.html)

[^rt-1]: this setting cannot currently be set through the [`Trace` launch file action](#launch-file-trace-action) or the [`ros2 trace` command](#trace-command), see [!129](https://gitlab.com/ros-tracing/ros2_tracing/-/issues/129)

For further reading:

* [LTTng documentation](https://lttng.org/docs/v2.13/)
* [Combined Tracing of the Kernel and Applications with LTTng](http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.641.1965&rep=rep1&type=pdf#page=87): LTTng-UST architecture and design goals (section 3)
* [Survey and Analysis of Kernel and Userspace Tracers on Linux: Design, Implementation, and Overhead](https://dl.acm.org/doi/abs/10.1145/3158644): LTTng-UST overhead and design compared to other kernel and userspace tracers (table 6: average latency overhead per tracepoint of 158 ns)

The LTTng kernel tracer has a similar implementation, but is separate from the userspace tracer.

## Packages

### ros2trace

Package containing a `ros2cli` extension to enable tracing.

### tracetools

Library to support instrumenting ROS packages, including core packages.

This package claims to be in the **Quality Level 1** category, see the [Quality Declaration](./tracetools/QUALITY_DECLARATION.md) for more details.

See the [API documentation](https://ros-tracing.gitlab.io/ros2_tracing-api/).

### tracetools_launch

Package containing tools to enable tracing through launch files.

### tracetools_read

Package containing tools to read traces.

### tracetools_test

Package containing tools for tracing-related tests.

### tracetools_trace

Package containing tools to enable tracing.

### test_tracetools

Package containing unit and system tests for `tracetools`.

## Analysis

See [`tracetools_analysis`](https://gitlab.com/ros-tracing/tracetools_analysis).
