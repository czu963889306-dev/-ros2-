This document is a declaration of software quality for the `rmw_implementation` package, based on the guidelines in [REP-2004](https://www.ros.org/reps/rep-2004.html).

# `rmw_implementation` Quality Declaration

The package `rmw_implementation` claims to be in the **Quality Level 1** category when it is used with a **Quality Level 1** middleware.

Below are the rationales, notes, and caveats for this claim, organized by each requirement listed in the [Package Requirements for Quality Level 1 in REP-2004](https://www.ros.org/reps/rep-2004.html).

## Version Policy [1]

### Version Scheme [1.i]

`rmw_implementation` uses `semver` according to the recommendation for ROS Core packages in the [ROS 2 Developer Guide](https://docs.ros.org/en/rolling/Contributing/Developer-Guide.html#versioning).

### Version Stability [1.ii]

`rmw_implementation` is at a stable version, i.e. `>= 1.0.0`.
The current version can be found in its [package.xml](./package.xml), and its change history can be found in its [CHANGELOG](./CHANGELOG.rst).

### Public API Declaration [1.iii]

`rmw_implementation` does not expose a public API.

### API Stability Policy [1.iv]

`rmw_implementation` will not break public API within a released ROS distribution, i.e. no major releases once the ROS distribution is released.

### ABI Stability Policy [1.v]

`rmw_implementation` contains C code and therefore must be concerned with ABI stability, and will maintain ABI stability within a ROS distribution.

### ABI and ABI Stability Within a Released ROS Distribution [1.vi]

`rmw_implementation` will not break API nor ABI within a released ROS distribution, i.e. no major releases once the ROS distribution is released.

## Change Control Process [2]

`rmw_implementation` follows the recommended guidelines for ROS Core packages in the [ROS 2 Developer Guide](https://docs.ros.org/en/rolling/Contributing/Developer-Guide.html#change-control-process).

### Change Requests [2.i]

This package requires that all changes occur through a pull request.

### Contributor Origin [2.ii]

This package has a confirmation of contributor origin policy, which can be found in [CONTRIBUTING](../CONTRIBUTING.md).

### Peer Review Policy [2.iii]

All pull request will be peer-reviewed, check [ROS 2 Developer Guide](https://docs.ros.org/en/rolling/Contributing/Developer-Guide.html#change-control-process) for additional information.

### Continuous Integration [2.iv]

All pull request must pass CI on all [tier 1 platforms](https://www.ros.org/reps/rep-2000.html#support-tiers)

Currently nightly results can be seen here:
* [linux-aarch64_release](https://ci.ros2.org/view/nightly/job/nightly_linux-aarch64_release/lastBuild/testReport/rmw_implementation/)
* [linux_release](https://ci.ros2.org/view/nightly/job/nightly_linux_release/lastBuild/testReport/rmw_implementation/)
* [mac_osx_release](https://ci.ros2.org/view/nightly/job/nightly_osx_release/lastBuild/testReport/rmw_implementation/)
* [windows_release](https://ci.ros2.org/view/nightly/job/nightly_win_rel/lastBuild/testReport/rmw_implementation/)

### Documentation Policy [2.v]

`rmw_implementation` does not expose a public API.
There is no need to add documentation to new changes.
In the case that this package adds public API features the documentation of new functionality will be required.

## Documentation [3]

### Feature Documentation [3.i]

`rmw_implementation` features are documented in the repository level [README](./README.md).

### Public API Documentation [3.ii]

`rmw_implementation` does not expose a public API.

### License [3.iii]

The license for `rmw_implementation` is Apache 2.0, and a summary is in each source file, the type is declared in the [package.xml](./package.xml) manifest file, and a full copy of the license is in the [LICENSE](../LICENSE) file.

There is an automated test which runs a linter (ament_copyright) that ensures each file has a license statement.

### Copyright Statement [3.iv]

The copyright holders each provide a statement of copyright in each source code file in `rmw_implementation`.

There is an automated test which runs a linter (ament_copyright) that ensures each file has at least one copyright statement.

Most recent test results can be found [here](https://ci.ros2.org/view/nightly/job/nightly_linux_release/lastBuild/testReport/rcl/copyright/).

## Testing [4]

### Feature Testing [4.i]

`rmw_implementation` features enable ROS middleware configuration and dynamic loading.
Unit tests for these features are located in the test directory.
Most recent test results can be found [here](https://ci.ros2.org/job/nightly_linux_release/lastBuild/testReport/rmw_implementation).

Integration and system tests against available `rmw` implementations up the stack, such as those found in [`test_rclcpp`](https://github.com/ros2/system_tests/tree/rolling/test_rclcpp) and [`test_communication`](https://github.com/ros2/system_tests/tree/rolling/test_communication) packages, further extend coverage.

### Public API Testing [4.ii]

`rmw_implementation` does not expose a public API.

### Coverage [4.iii]

`rmw_implementation` follows the recommendations for ROS Core packages in the [ROS 2 Developer Guide](https://docs.ros.org/en/rolling/Contributing/Developer-Guide.html#code-coverage), and opts to use line coverage instead of branch coverage.

This includes:

- tracking and reporting line coverage statistics
- achieving and maintaining a reasonable branch line coverage (90-100%)
- no lines are manually skipped in coverage calculations

Changes are required to make a best effort to keep or increase coverage before being accepted, but decreases are allowed if properly justified and accepted by reviewers.

Current coverage statistics can be viewed [here](https://ci.ros2.org/job/ci_linux_coverage/lastSuccessfulBuild/cobertura/).
A description of how coverage statistics are summarized from this page, can be found in the [ROS 2 Onboarding Guide](https://docs.ros.org/en/rolling/Contributing/Developer-Guide.html#note-on-coverage-runs).

### Performance [4.iv]

The performance tests of `rmw_implementation` are located in the [test/benchmark directory](https://github.com/ros2/rmw_implementation/tree/rolling/rmw_implementation/test/benchmark).
The most recent test results can be found [here](http://build.ros2.org/view/Rci/job/Rci__benchmark_ubuntu_focal_amd64/BenchmarkTable/).

### Linters and Static Analysis [4.v]

`rmw_implementation` uses and passes all the standard linters and static analysis tools for a C++ package as described in the [ROS 2 Developer Guide](https://docs.ros.org/en/rolling/Contributing/Developer-Guide.html#linters-and-static-analysis).

Currently nightly results can be seen here:
* [linux-aarch64_release](https://ci.ros2.org/view/nightly/job/nightly_linux-aarch64_release/lastBuild/testReport/rmw_implementation/)
* [linux_release](https://ci.ros2.org/view/nightly/job/nightly_linux_release/lastBuild/testReport/rmw_implementation/)
* [mac_osx_release](https://ci.ros2.org/view/nightly/job/nightly_osx_release/lastBuild/testReport/rmw_implementation/)
* [windows_release](https://ci.ros2.org/view/nightly/job/nightly_win_rel/lastBuild/testReport/rmw_implementation/)

## Dependencies [5]

### Direct Runtime ROS Dependencies [5.i/5.ii]

`rmw_implementation` has run-time and build-time dependencies that are at **Quality Level 1**

 - rcpputils: [QUALITY DECLARATION](https://github.com/ros2/rcpputils/blob/rolling/QUALITY_DECLARATION.md)
 - rcutils: [QUALITY DECLARATION](https://github.com/ros2/rcutils/blob/rolling/QUALITY_DECLARATION.md)
 - rmw: [QUALITY DECLARATION](https://github.com/ros2/rmw/blob/rolling/rmw/QUALITY_DECLARATION.md)

### Direct Runtime non-ROS Dependency [5.iii]

`rmw_implementation` does not have any runtime non-ROS dependencies.

It has one "buildtool" dependency, which do not affect the resulting quality of the package, because they do not contribute to the public library API.
It also has several test dependencies, which do not affect the resulting quality of the package, because they are only used to build and run the test code.

## Platform Support [6]

`rmw_implementation` supports all of the tier 1 platforms as described in [REP-2000](https://www.ros.org/reps/rep-2000.html#support-tiers), and tests each change against all of them.

Currently nightly results can be seen here:
* [linux-aarch64_release](https://ci.ros2.org/view/nightly/job/nightly_linux-aarch64_release/lastBuild/testReport/rmw_implementation/)
* [linux_release](https://ci.ros2.org/view/nightly/job/nightly_linux_release/lastBuild/testReport/rmw_implementation/)
* [mac_osx_release](https://ci.ros2.org/view/nightly/job/nightly_osx_release/lastBuild/testReport/rmw_implementation/)
* [windows_release](https://ci.ros2.org/view/nightly/job/nightly_win_rel/lastBuild/testReport/rmw_implementation/)

## Security [7]

### Vulnerability Disclosure Policy [7.i]

This package conforms to the Vulnerability Disclosure Policy in [REP-2006](https://www.ros.org/reps/rep-2006.html).

# Current status Summary

The chart below compares the requirements in the REP-2004 with the current state of the rmw_implementation package.

|Number|  Requirement| Current state |
|--|--|--|
|1| **Version policy** |---|
|1.i|Version Policy available | ✓ |
|1.ii|Stable version | ✓ |
|1.iii|Declared public API|Not required|
|1.iv|API stability policy|✓|
|1.v|ABI stability policy|✓|
|1.vi_|API/ABI stable within ros distribution|✓|
|2| **Change control process** |---|
|2.i| All changes occur on change request |✓|
|2.ii| Contributor origin (DCO, CLA, etc) |✓|
|2.iii| Peer review policy |✓|
|2.iv| CI policy for change requests |✓|
|2.v| Documentation policy for change requests |✓|
|3| **Documentation** | --- |
|3.i| Per feature documentation | ✓ |
|3.ii| Per public API item documentation | Not required |
|3.iii| Declared License(s) | ✓ |
|3.iv| Copyright in source files| ✓ |
|3.v.a| Quality declaration linked to README | ✓ |
|3.v.b| Centralized declaration available for peer review |✓|
|4| **Testing** | --- |
|4.i| Feature items tests | ✓ |
|4.ii| Public API tests | None |
|4.iii.a| Using coverage | ✓ |
|4.iii.a| Coverage policy | ✓ |
|4.iv.a| Performance tests (if applicable) | ✓ |
|4.iv.b| Performance tests policy| None |
|4.v.a| Code style enforcement (linters)| ✓ |
|4.v.b| Use of static analysis tools | None |
|5| **Dependencies** | --- |
|5.i| Must not have ROS lower level dependencies | ✓ |
|5.ii| Optional ROS lower level dependencies| ✓ |
|5.iii| Justifies quality use of non-ROS dependencies |✓|
|6| Platform support | --- |
|6.i| Support targets Tier1 ROS platforms| ✓ |
|7| **Security** | --- |
|7.i| Vulnerability Disclosure Policy | ✓ |
