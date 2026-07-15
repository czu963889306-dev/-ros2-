This document is a declaration of software quality for the `rosidl_typesupport_interface` package, based on the guidelines in [REP-2004](https://www.ros.org/reps/rep-2004.html).

# rosidl_typesupport_interface Quality Declaration

The package `rosidl_typesupport_interface` claims to be in the **Quality Level 1** category.

Below are the rationales, notes, and caveats for this claim, organized by each requirement listed in the [Package Requirements for Quality Level 1 in REP-2004](https://www.ros.org/reps/rep-2004.html).

## Version Policy [1]

### Version Scheme [1.i]

`rosidl_typesupport_interface` uses `semver` according to the recommendation for ROS Core packages in the [ROS 2 Developer Guide](https://docs.ros.org/en/rolling/Contributing/Developer-Guide.html#versioning).

### Version Stability [1.ii]

`rosidl_typesupport_interface` is at a stable version, i.e. >= 1.0.0. The current version can be found in its [package.xml](./package.xml), and its change history can be found in its [CHANGELOG](./CHANGELOG.xml).

### Public API Declaration [1.iii]

All symbols in the installed headers are considered part of the public API.

All installed headers are in the `include` directory of the package, headers in any other folders are not installed and considered private.

### API Stability Within a Released ROS Distribution [1.iv]/[1.vi]

`rosidl_typesupport_interface` will not break public API within a released ROS distribution, i.e. no major releases once the ROS distribution is released.

### ABI Stability Within a Released ROS Distribution [1.v]/[1.vi]

`rosidl_typesupport_interface` contains C code and therefore must be concerned with ABI stability, and will maintain ABI stability within a ROS distribution.

## Change Control Process [2]

`rosidl_typesupport_interface` follows the recommended guidelines for ROS Core packages in the [ROS 2 Developer Guide](https://docs.ros.org/en/rolling/Contributing/Developer-Guide.html#quality-practices).

### Change Requests [2.i]

This package requires that all changes occur through a pull request.

### Contributor Origin [2.ii]

This package uses DCO as its confirmation of contributor origin policy.
More information can be found in [CONTRIBUTING](../CONTRIBUTING.md).

### Peer Review Policy [2.iii]

Following the recommended guidelines for ROS Core packages, all pull request have at least 1 peer review.

### Continuous Integration [2.iv]

All pull requests must pass CI on all [tier 1 platforms](https://www.ros.org/reps/rep-2000.html#support-tiers).

Currently nightly results can be seen here:
* [linux-aarch64_release](https://ci.ros2.org/view/nightly/job/nightly_linux-aarch64_release/lastBuild/testReport/rosidl_typesupport_interface/)
* [linux_release](https://ci.ros2.org/view/nightly/job/nightly_linux_release/lastBuild/testReport/rosidl_typesupport_interface/)
* [mac_osx_release](https://ci.ros2.org/view/nightly/job/nightly_osx_release/lastBuild/testReport/rosidl_typesupport_interface/)
* [windows_release](https://ci.ros2.org/view/nightly/job/nightly_win_rel/lastBuild/testReport/rosidl_typesupport_interface/)

### Documentation Policy [2.v]

All pull requests must resolve related documentation changes before merging.

## Documentation [3]

### Feature Documentation [3.i]

`rosidl_typesupport_interface` has feature documentation and it is publicly [hosted](README.md).

### Public API Documentation [3.ii]

`rosidl_typesupport_interface` has API documentation, hosted [here](http://docs.ros2.org/foxy/api/rosidl_typesupport_interface/index.html).

### License [3.iii]

The license for `rosidl_typesupport_interface` is Apache 2.0, and a summary is in each source file, the type is declared in the [package.xml](package.xml) manifest file, and a full copy of the license is in the [LICENSE](../LICENSE) file.

There is an automated test which runs a linter that ensures each file has a license statement.

### Copyright Statements [3.iv]

The copyright holders each provide a statement of copyright in each source code file in `rosidl_typesupport_interface`.

There is an automated test which runs a linter that ensures each file has at least one copyright statement.

Most recent test results can be found [here](https://ci.ros2.org/job/nightly_linux_release/lastBuild/testReport/rosidl_typesupport_interface/copyright)

## Testing [4]

### Feature Testing [4.i]

The features of `rosidl_typesupport_interface` are tested, and their tests are located in the test directory.

Most recent test results can be found [here](https://ci.ros2.org/job/nightly_linux_release/lastBuild/testReport/rosidl_typesupport_interface).

### Public API Testing [4.ii]

The public API of `rosidl_typesupport_interface` is tested, and the tests are located in the test directory.

Most recent test results can be found [here](https://ci.ros2.org/job/nightly_linux_release/lastBuild/testReport/rosidl_typesupport_interface).

### Coverage [4.iii]

`rosidl_typesupport_interface` follows the recommendations for ROS Core packages in the [ROS 2 Developer Guide](https://docs.ros.org/en/rolling/Contributing/Developer-Guide.html#code-coverage), and opts to use line coverage instead of branch coverage.

This includes:

- tracking and reporting line coverage statistics
- achieving and maintaining a reasonable branch line coverage (90-100%)
- no lines are manually skipped in coverage calculations

Changes are required to make a best effort to keep or increase coverage before being accepted, but decreases are allowed if properly justified and accepted by reviewers.

Because this package is a header only library with just C macro definitions, its coverage is not able to be reported.
The unit test of all its macros can be found in the test directory.

### Performance [4.iv]

`rosidl_typesupport_interface` does not have performance tests because this package contains simple interface macros that define the naming convention for typesupport symbols.
These macros do not introduce any runtime overhead when used.
Therefore it has been decided that this package does not need performance tests to qualify for Quality Level 1.

### Linters and Static Analysis [4.v]

`rosidl_typesupport_interface` uses and passes all the standard linters and static analysis tools for a C package as described in the [ROS 2 Developer Guide](https://docs.ros.org/en/rolling/Contributing/Developer-Guide.html#linters-and-static-analysis).

Results of the linting tests can be found [here](https://ci.ros2.org/job/nightly_linux_release/lastBuild/testReport/rosidl_typesupport_interface/).

## Dependencies [5]

### Direct Runtime ROS Dependencies [5.i/5.ii]

`rosidl_typesupport_interface` does not have any runtime ROS dependencies.

It has "buildtool" dependencies, which do not affect the resulting quality of the package, because they do not contribute to the public library API.
It also has several test dependencies, which do not affect the resulting quality of the package, because they are only used to build and run the test code.

### Direct Runtime Non-ROS Dependencies [5.iii]

`rosidl_typesupport_interface` does not have any runtime non-ROS dependencies.

## Platform Support [6]

`rosidl_typesupport_interface` supports all of the tier 1 platforms as described in [REP-2000](https://www.ros.org/reps/rep-2000.html#support-tiers), and tests each change against all of them.

Currently nightly results can be seen here:
* [linux-aarch64_release](https://ci.ros2.org/view/nightly/job/nightly_linux-aarch64_release/lastBuild/testReport/rosidl_typesupport_interface/)
* [linux_release](https://ci.ros2.org/view/nightly/job/nightly_linux_release/lastBuild/testReport/rosidl_typesupport_interface/)
* [mac_osx_release](https://ci.ros2.org/view/nightly/job/nightly_osx_release/lastBuild/testReport/rosidl_typesupport_interface/)
* [windows_release](https://ci.ros2.org/view/nightly/job/nightly_win_rel/lastBuild/testReport/rosidl_typesupport_interface/)

# Security [7]

## Vulnerability Disclosure Policy [7.i]

This package conforms to the Vulnerability Disclosure Policy in [REP-2006](https://www.ros.org/reps/rep-2006.html).
