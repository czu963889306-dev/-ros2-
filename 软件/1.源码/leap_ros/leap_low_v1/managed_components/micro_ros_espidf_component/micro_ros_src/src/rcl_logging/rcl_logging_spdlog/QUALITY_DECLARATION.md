This document is a declaration of software quality for the `rcl_logging_spdlog` package, based on the guidelines in [REP-2004](https://github.com/ros-infrastructure/rep/blob/rep-2004/rep-2004.rst).

# rcl_logging_spdlog Quality Declaration

The package `rcl_logging_spdlog` claims to be in the **Quality Level 1** category.

Below are the rationales, notes, and caveats for this claim, organized by each requirement listed in the [Package Quality Categories in REP-2004](https://www.ros.org/reps/rep-2004.html) of the ROS2 developer guide.

## Version Policy [1]

### Version Scheme [1.i]

`rcl_logging_spdlog` uses `semver` according to the recommendation for ROS Core packages in the [ROS 2 Developer Guide](https://docs.ros.org/en/rolling/Contributing/Developer-Guide.html#versioning).

### Version Stability [1.ii]

Currently this package it is not at or above a stable version, i.e. `>= 1.0.0`.

### Public API Declaration [1.iii]

All symbols in the installed headers are considered part of the public API.

All installed headers are in the [include](./include/rcl_logging_spdlog) directory of the package, headers in any other folders are not installed and considered private.

### API Stability Policy [1.iv]

`rcl_logging_spdlog` will not break public API within a released ROS distribution, i.e. no major releases once the ROS distribution is released.

### ABI Stability Policy [1.v]

`rcl_logging_spdlog` contains C code and therefore must be concerned with ABI stability, and will maintain ABI stability within a ROS distribution.

### ABI and ABI Stability Within a Released ROS Distribution [1.vi]

`rcl_logging_spdlog` will not break API nor ABI within a released ROS distribution, i.e. no major releases once the ROS distribution is released.

## Change Control Process [2]

`rcl_logging_spdlog` follows the recommended guidelines for ROS Core packages in the [ROS 2 Developer Guide](https://docs.ros.org/en/rolling/Contributing/Developer-Guide.html#change-control-process).

### Change Requests [2.i]

All changes will occur through a pull request, check [ROS 2 Developer Guide](https://docs.ros.org/en/rolling/Contributing/Developer-Guide.html#change-control-process) for additional information.

### Contributor Origin [2.ii]

This package uses DCO as its confirmation of contributor origin policy. More information can be found in [CONTRIBUTING](../CONTRIBUTING.md)

### Peer Review Policy [2.iii]

Following the recommended guidelines in the [ROS 2 Developer Guide](https://docs.ros.org/en/rolling/Contributing/Developer-Guide.html#change-control-process) all pull requests must have at least 1 peer review.

### Continuous Integration [2.iv]

All pull requests must pass CI on all [tier 1 platforms](https://www.ros.org/reps/rep-2000.html#support-tiers)

Currently nightly results can be seen here:
* [linux-aarch64_release](https://ci.ros2.org/view/nightly/job/nightly_linux-aarch64_release/lastBuild/testReport/rcl_logging_spdlog/)
* [linux_release](https://ci.ros2.org/view/nightly/job/nightly_linux_release/lastBuild/testReport/rcl_logging_spdlog/)
* [mac_osx_release](https://ci.ros2.org/view/nightly/job/nightly_osx_release/lastBuild/testReport/rcl_logging_spdlog/)
* [windows_release](https://ci.ros2.org/view/nightly/job/nightly_win_rel/lastBuild/testReport/rcl_logging_spdlog/)

###  Documentation Policy [2.v]

All pull requests must resolve related documentation changes before merging.

## Documentation [3]

### Feature Documentation [3.i]

`rcl_logging_spdlog` has a documented feature list and it is hosted [here](http://docs.ros2.org/latest/api/rcl_logging_spdlog/index.html).

### Public API Documentation [3.ii]

`rcl_logging_spdlog` has documentation of its public API and it is hosted [here](http://docs.ros2.org/latest/api/rcl_logging_spdlog/index.html).

### License [3.iii]

The license for `rcl_logging_spdlog` is Apache 2.0, and a summary is in each source file, the type is declared in the [`package.xml`](./package.xml) manifest file, and a full copy of the license is in the [`LICENSE`](../LICENSE) file.

There is an automated test which runs a linter that ensures each file has a license statement. [Here](https://ci.ros2.org/view/nightly/job/nightly_linux_release/lastSuccessfulBuild/testReport/rcl_logging_spdlog/) can be found a list with the latest results of the various linters being run on the package.

### Copyright Statements [3.iv]

The copyright holders each provide a statement of copyright in each source code file in `rcl_logging_spdlog`.

There is an automated test which runs a linter that ensures each file has at least one copyright statement. Latest linter result report can be seen [here](https://ci.ros2.org/view/nightly/job/nightly_linux_release/lastSuccessfulBuild/testReport/rcl_logging_spdlog/copyright/).

## Testing [4]

### Feature Testing [4.i]

Each feature in `rcl_logging_spdlog` has corresponding tests which simulate typical usage, and they are located in the [`test`](https://github.com/ros2/rcl_logging/tree/master/rcl_logging_spdlog/test) directory.
New features are required to have tests before being added.

Currently nightly test results can be seen here:

* [linux-aarch64_release](https://ci.ros2.org/view/nightly/job/nightly_linux-aarch64_release/lastSuccessfulBuild/testReport/rcl_logging_spdlog/)
* [linux_release](https://ci.ros2.org/view/nightly/job/nightly_linux_release/lastBuild/lastSuccessfulBuild/testReport/rcl_logging_spdlog/)
* [mac_osx_release](https://ci.ros2.org/view/nightly/job/nightly_osx_release/lastBuild/lastSuccessfulBuild/testReport/rcl_logging_spdlog/)
* [windows_release](https://ci.ros2.org/view/nightly/job/nightly_win_rel/lastSuccessfulBuild/testReport/rcl_logging_spdlog/)

### Public API Testing [4.ii]

Each part of the public API has tests, and new additions or changes to the public API require tests before being added.
The tests aim to cover both typical usage and corner cases, but are quantified by contributing to code coverage.

### Coverage [4.iii]

`rcl_logging_spdlog` follows the recommendations for ROS Core packages in the [ROS 2 Developer Guide](https://docs.ros.org/en/rolling/Contributing/Developer-Guide.html#code-coverage), and opts to use line coverage instead of branch coverage.

This includes:

- tracking and reporting line coverage statistics
- achieving and maintaining a reasonable branch line coverage (90-100%)
- no lines are manually skipped in coverage calculations

Changes are required to make a best effort to keep or increase coverage before being accepted, but decreases are allowed if properly justified and accepted by reviewers.

Current coverage statistics can be viewed [here](https://ci.ros2.org/job/nightly_linux_coverage/lastSuccessfulBuild/cobertura/src_ros2_rcl_logging_rcl_logging_spdlog_src/). A description of how coverage statistics are calculated is summarized in this page ["ROS 2 Onboarding Guide"](https://docs.ros.org/en/rolling/Contributing/Developer-Guide.html#note-on-coverage-runs).

### Performance [4.iv]

`rcl_logging_spdlog` follows the recommendations for performance testing of C code in the [ROS 2 Developer Guide](https://docs.ros.org/en/rolling/Contributing/Developer-Guide.html#performance), and opts to do performance analysis on each release rather than each change.

Package and system level performance benchmarks that cover features of `rcl_logging_spdlog` can be found at:
* [Benchmarks](http://build.ros2.org/view/Rci/job/Rci__benchmark_ubuntu_focal_amd64/BenchmarkTable/)
* [Performance](http://build.ros2.org/view/Rci/job/Rci__nightly-performance_ubuntu_focal_amd64/lastCompletedBuild/)

Changes that introduce regressions in performance must be adequately justified in order to be accepted and merged.

### Linters and Static Analysis [4.v]

`rcl_logging_spdlog` uses and passes all the standard linters and static analysis tools for a C package as described in the [ROS 2 Developer Guide](https://docs.ros.org/en/rolling/Contributing/Developer-Guide.html#linters-and-static-analysis). Passing implies there are no linter/static errors when testing against CI of supported platforms.

Currently nightly results can be seen here:
* [linux-aarch64_release](https://ci.ros2.org/view/nightly/job/nightly_linux-aarch64_release/lastSuccessfulBuild/testReport/rcl_logging_spdlog/)
* [linux_release](https://ci.ros2.org/view/nightly/job/nightly_linux_release/lastBuild/lastSuccessfulBuild/testReport/rcl_logging_spdlog/)
* [mac_osx_release](https://ci.ros2.org/view/nightly/job/nightly_osx_release/lastBuild/lastSuccessfulBuild/testReport/rcl_logging_spdlog/)
* [windows_release](https://ci.ros2.org/view/nightly/job/nightly_win_rel/lastSuccessfulBuild/testReport/rcl_logging_spdlog/)

## Dependencies [5]

Below are evaluations of each of `rcl_logging_spdlog`'s run-time and build-time dependencies that have been determined to influence the quality.

`rcl_logging_spdlog` depends on the ROS packages `rcutils` and `spdlog_vendor`.

#### `rcutils`

The `rcutils` package provides an API which contains common utilities and data structures useful when programming in C.

It is **Quality Level 1**, see its [Quality Declaration document](https://github.com/ros2/rcutils/blob/master/QUALITY_DECLARATION.md).

#### `spdlog_vendor`

The `spdlog_vendor` package provides a CMake shim over the spdlog library.

It is **Quality Level 1**, see its [Quality Declaration document](https://github.com/ros2/spdlog_vendor/blob/master/QUALITY_DECLARATION.md).

### Optional Direct Runtime ROS Dependencies [5.ii]

`rcl_logging_spdlog` has no optional Direct Runtime ROS dependencies that need to be considered for this declaration.

### Direct Runtime non-ROS Dependency [5.iii]

`rcl_logging_spdlog` has a Direct Runtime non-ROS dependency on the `spdlog` library. It was declared to be Quality Level 1 [here](https://github.com/ros2/spdlog_vendor/blob/master/SPDLOG_QUALITY_DECLARATION.md).

## Platform Support [6]

`rcl_logging_spdlog` supports all of the tier 1 platforms as described in [REP-2000](https://www.ros.org/reps/rep-2000.html#support-tiers), and tests each change against all of them.

## Security [7]

### Vulnerability Disclosure Policy [7.i]

This package conforms to the Vulnerability Disclosure Policy in [REP-2006](https://www.ros.org/reps/rep-2006.html).

# Current status Summary

The chart below compares the requirements in the REP-2004 with the current state of the `rcl` package.

|Number|  Requirement| Current state |
|--|--|--|
|1| **Version policy** |---|
|1.i|Version Policy available | ✓ |
|1.ii|Stable version | ✓ |
|1.iii|Declared public API| ✓ |
|1.iv|API stability policy| ✓ |
|1.v|ABI stability policy| ✓ |
|1.vi_|API/ABI stable within ros distribution| ✓ |
|2| **Change control process** |---|
|2.i| All changes occur on change request | ✓ |
|2.ii| Contributor origin (DCO, CLA, etc) | ✓ |
|2.iii| Peer review policy | ✓ |
|2.iv| CI policy for change requests | ✓ |
|2.v| Documentation policy for change requests | ✓ |
|3| **Documentation** | --- |
|3.i| Per feature documentation | ✓ |
|3.ii| Per public API item documentation | ✓ |
|3.iii| Declared License(s) | ✓ |
|3.iv| Copyright in source files| ✓ |
|3.v.a| Quality declaration linked to README | ✓ |
|3.v.b| Centralized declaration available for peer review | ✓ |
|4| Testing | --- |
|4.i| Feature items tests | ✓ |
|4.ii| Public API tests | ✓ |
|4.iii.a| Using coverage | ✓ |
|4.iii.a| Coverage policy | ✓ |
|4.iv.a| Performance tests (if applicable) | ✓ |
|4.iv.b| Performance tests policy| ✓ |
|4.v.a| Code style enforcement (linters)| ✓ |
|4.v.b| Use of static analysis tools | ✓ |
|5| Dependencies | --- |
|5.i| Must not have ROS lower level dependencies | ✓ |
|5.ii| Optional ROS lower level dependencies| ✓ |
|5.iii| Justifies quality use of non-ROS dependencies |✓|
|6| Platform support | --- |
|6.i| Support targets Tier1 ROS platforms| ✓ |
|7| Security | --- |
|7.i| Vulnerability Disclosure Policy | ✓ |
