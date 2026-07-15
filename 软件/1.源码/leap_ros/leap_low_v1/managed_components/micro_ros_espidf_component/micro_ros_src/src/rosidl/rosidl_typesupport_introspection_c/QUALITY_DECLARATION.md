This document is a declaration of software quality for the `rosidl_typesupport_introspection_c` package, based on the guidelines in [REP-2004](https://www.ros.org/reps/rep-2004.html).

# rosidl_typesupport_introspection_c Quality Declaration

The package `rosidl_typesupport_introspection_c` claims to be in the **Quality Level 1** category.

Below are the rationales, notes, and caveats for this claim, organized by each requirement listed in the [Package Requirements for Quality Level 1 in REP-2004](https://www.ros.org/reps/rep-2004.html).

## Version Policy [1]

### Version Scheme [1.i]

`rosidl_typesupport_introspection_c` uses `semver` according to the recommendation for ROS Core packages in the [ROS 2 Developer Guide](https://docs.ros.org/en/rolling/Contributing/Developer-Guide.html#versioning).

### Version Stability [1.ii]

`rosidl_typesupport_introspection_c` is at a stable version, i.e. >= 1.0.0. The current version can be found in its [package.xml](./package.xml), and its change history can be found in its [CHANGELOG](./CHANGELOG.rst).

### Public API Declaration [1.iii]

All symbols in the installed headers are considered part of the public API.

All installed headers are in the `include` directory of the package, headers in any other folders are not installed and considered private.

### API Stability Within a Released ROS Distribution [1.iv]/[1.vi]

`rosidl_typesupport_introspection_c` will not break public API within a released ROS distribution, i.e. no major releases once the ROS distribution is released.

### ABI Stability Within a Released ROS Distribution [1.v]/[1.vi]

`rosidl_typesupport_introspection_c` contains C++ code and therefore must be concerned with ABI stability, and will maintain ABI stability within a ROS distribution.

## Change Control Process [2]

`rosidl_typesupport_introspection_c` follows the recommended guidelines for ROS Core packages in the [ROS 2 Developer Guide](https://docs.ros.org/en/rolling/Contributing/Developer-Guide.html#quality-practices).

### Change Requests [2.i]

This package requires that all changes occur through a pull request.

### Contributor Origin [2.ii]

This package uses DCO as its confirmation of contributor origin policy.
More information can be found in [CONTRIBUTING](../CONTRIBUTING.md).

### Peer Review Policy [2.iii]

Following the recommended guidelines for ROS Core packages, all pull requests must have at least 1 peer review.

### Continuous Integration [2.iv]

All pull requests must pass CI on all [tier 1 platforms](https://www.ros.org/reps/rep-2000.html#support-tiers).

Currently nightly results can be seen here:
* [linux-aarch64_release](https://ci.ros2.org/view/nightly/job/nightly_linux-aarch64_release/lastBuild/testReport/rosidl_typesupport_introspection_c/)
* [linux_release](https://ci.ros2.org/view/nightly/job/nightly_linux_release/lastBuild/testReport/rosidl_typesupport_introspection_c/)
* [mac_osx_release](https://ci.ros2.org/view/nightly/job/nightly_osx_release/lastBuild/testReport/rosidl_typesupport_introspection_c/)
* [windows_release](https://ci.ros2.org/view/nightly/job/nightly_win_rel/lastBuild/testReport/rosidl_typesupport_introspection_c/)

### Documentation Policy [2.v]

All pull requests must resolve related documentation changes before merging.

## Documentation [3]

### Feature Documentation [3.i]

`rosidl_typesupport_introspection_c` has feature documentation and it is publicly [hosted](README.md).

### Public API Documentation [3.ii]

`rosidl_typesupport_introspection_c` has no API documentation.

### License [3.iii]

The license for `rosidl_typesupport_introspection_c` is Apache 2.0, and a summary is in each source file, the type is declared in the [package.xml](package.xml) manifest file, and a full copy of the license is in the [LICENSE](../LICENSE) file.

There is an automated test which runs a linter that ensures each file has a license statement.

### Copyright Statements [3.iv]

The copyright holders each provide a statement of copyright in each source code file in `rosidl_typesupport_introspection_c`.

There is an automated test which runs a linter that ensures each file has at least one copyright statement.

Most recent test results can be found [here](https://ci.ros2.org/job/nightly_linux_release/lastBuild/testReport/rosidl_typesupport_introspection_c/copyright)

## Testing [4]

### Note about tests design

`rosidl_typesupport_introspection_c` tests validate generated source code rather than the code generator itself. That is, tests verify that both API and ABI of the generated source code enable the expected, relevant use cases. These tests rely on a set of input interface definition files, and a subset of the `rosidl` toolchain to generate source code for each of these files.

### Feature Testing [4.i]

Each feature provided by `rosidl_typesupport_introspection_c` and the source code it generates has corresponding tests according to the rationale outlined in the "Note about tests design" at the beginning of section [4]. These can be found in the [`rosidl_typesupport_introspection_tests`](https://github.com/ros2/rosidl/tree/master/rosidl_typesupport_introspection_tests) package.

New features are required to have tests before being added.

Currently nightly test results can be seen here:

* [linux-aarch64_release](https://ci.ros2.org/view/nightly/job/nightly_linux-aarch64_release/lastBuild/testReport/rosidl_typesupport_introspection_tests/)
* [linux_release](https://ci.ros2.org/view/nightly/job/nightly_linux_release/lastBuild/testReport/rosidl_typesupport_introspection_tests/)
* [mac_osx_release](https://ci.ros2.org/view/nightly/job/nightly_osx_release/lastBuild/testReport/rosidl_typesupport_introspection_tests/)
* [windows_release](https://ci.ros2.org/view/nightly/job/nightly_win_rel/lastBuild/testReport/rosidl_typesupport_introspection_tests/)

### Public API Testing [4.ii]

The `rosidl_typesupport_introspection_c` package does not have tests for its own API. This is a design decision, as detailed in "Note about tests design" at the beginning of section [4]. The [`rosidl_typesupport_introspection_tests`](https://github.com/ros2/rosidl/tree/master/rosidl_typesupport_introspection_tests) packages implements such tests.

### Coverage [4.iii]

`rosidl_typesupport_introspection_c` follows the recommendations for ROS Core packages in the [ROS 2 Developer Guide](https://docs.ros.org/en/rolling/Contributing/Developer-Guide.html#code-coverage), and opts to use line coverage instead of branch coverage.

This includes:

- tracking and reporting line coverage statistics
- achieving and maintaining a reasonable branch line coverage (90-100%)
- no lines are manually skipped in coverage calculations

Current coverage statistics can be viewed [here](https://ci.ros2.org/job/nightly_linux_coverage/lastSuccessfulBuild/cobertura/). Given the different approach used for testing (detailed in "Note about tests design" at the beginning of section [4]) the pattern to search for `rosidl_typesupport_introspection_c` is:
`build.rosidl_typesupport_introspection_tests.rosidl_typesupport_introspection_c.rosidl_typesupport_introspection_tests.*`.

### Performance [4.iv]

APIs in `rosidl_typesupport_introspection_c` generated source code provide a thin layer for type-erased access to `rosidl` interfaces in C. As such, no useful benchmarks can be devised at the package scope, but rather in the context of a system-wide test (e.g. by benchmarking communication using a middleware implementation that relies on these APIs and it is thus affected by the code structure these induce).

Therefore, this package does not include any performance tests. Refer to the [`system_tests`](https://github.com/ros2/system_tests) repository for system-wide testing.

### Linters and Static Analysis [4.v]

`rosidl_typesupport_introspection_c` uses and passes all the standard linters and static analysis tools for a C package as described in the [ROS 2 Developer Guide](https://docs.ros.org/en/rolling/Contributing/Developer-Guide.html#linters-and-static-analysis).

Results of the linting tests can be found [here](https://ci.ros2.org/job/nightly_linux_release/lastBuild/testReport/rosidl_typesupport_introspection_c/).

## Dependencies [5]

### Direct Runtime ROS Dependencies [5.i/5.ii]

`rosidl_typesupport_introspection_c` does not have any runtime ROS dependencies.

It has "buildtool" dependencies, which do not affect the resulting quality of the package, because they do not contribute to the public library API.
It also has several test dependencies, which do not affect the resulting quality of the package, because they are only used to build and run the test code.

### Direct Runtime Non-ROS Dependencies [5.iii]

`rosidl_typesupport_introspection_c` does not have any runtime non-ROS dependencies.

## Platform Support [6]

`rosidl_typesupport_introspection_c` supports all of the tier 1 platforms as described in [REP-2000](https://www.ros.org/reps/rep-2000.html#support-tiers).

Currently nightly results can be seen here:
* [linux-aarch64_release](https://ci.ros2.org/view/nightly/job/nightly_linux-aarch64_release/lastBuild/testReport/rosidl_typesupport_introspection_c/)
* [linux_release](https://ci.ros2.org/view/nightly/job/nightly_linux_release/lastBuild/testReport/rosidl_typesupport_introspection_c/)
* [mac_osx_release](https://ci.ros2.org/view/nightly/job/nightly_osx_release/lastBuild/testReport/rosidl_typesupport_introspection_c/)
* [windows_release](https://ci.ros2.org/view/nightly/job/nightly_win_rel/lastBuild/testReport/rosidl_typesupport_introspection_c/)

# Security [7]

## Vulnerability Disclosure Policy [7.i]

This package conforms to the Vulnerability Disclosure Policy in [REP-2006](https://www.ros.org/reps/rep-2006.html).
