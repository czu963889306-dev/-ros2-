This document is a declaration of software quality for the `rclc_lifecycle` package, based on the guidelines in [REP-2004](https://reps.openrobotics.org/rep-2004/).

# `rclc_lifecycle` Quality Declaration

The package `rclc_lifecycle` claims to be in the **Quality Level 2** category when it is used with a **Quality Level 2** middleware.

Below are the rationales, notes, and caveats for this claim, organized by each requirement listed in the [Package Quality Categories in REP-2004](https://reps.openrobotics.org/rep-2004/).

## Version Policy [1]

### Version Scheme [1.i]

`rclc_lifecycle` uses `semver` according to the recommendation for ROS Core packages in the [ROS 2 Developer Guide](https://docs.ros.org/en/rolling/Contributing/Developer-Guide.html#versioning).

### Version Stability [1.ii]

`rclc_lifecycle` is at a stable version, i.e. `>= 1.0.0`.
The current version can be found in its [package.xml](package.xml), and its change history can be found in its [CHANGELOG](CHANGELOG.rst).

### Public API Declaration [1.iii]

All symbols in the installed headers are considered part of the public API.

All installed headers are in the [`include`](./include/rclc_lifecycle) directory of the package, headers in any other folders are not installed and considered private.

### API Stability Policy [1.iv]

`rclc_lifecycle` will not break public API within a released ROS distribution, i.e. no major releases once the ROS distribution is released.

### ABI Stability Policy [1.v]

`rclc_lifecycle` contains C code and therefore must be concerned with ABI stability and will maintain ABI stability within a ROS distribution. Because an ABI-break is considered as major version change, there will be no major releases once the ROS distribution is released.

### API and ABI Stability Within a Released ROS Distribution [1.vi]

`rclc_lifecycle` will not break API nor ABI within a released ROS distribution, i.e. no major releases once the ROS distribution is released.

## Change Control Process [2]

The stability of `rclc_lifecycle` is ensured through reviews, CI and tests.
The change control process can be found in [CONTRIBUTING](../CONTRIBUTING.md).

All changes to `rclc_lifecycle` occur through pull requests that are required to pass all CI tests.
In case of failure, only maintainers can merge the pull request, and only when there is enough evidence that the failure is unrelated to the change.
Additionally, all pull requests must have at least one positive review from another contributor that did not author the pull request.

### Change Requests [2.i]

All changes will occur through a pull request.

### Contributor Origin [2.ii]

This package uses [Developer Certificate of Origin (DCO)](https://developercertificate.org/) as its confirmation of contributor origin policy since version 1.0.0. More information can be found in [CONTRIBUTING](../CONTRIBUTING.md).

### Peer Review Policy [2.iii]

All pull requests will be peer-reviewed by at least one other contributor who did not author the pull request. Approval is required before merging.

### Continuous Integration [2.iv]

All pull requests must pass CI to be considered for merging, unless maintainers consider that there is enough evidence that the failure is unrelated to the changes.
CI testing is automatically triggered by incoming pull requests.
Current results can be seen [../README.md](../README.md).

###  Documentation Policy [2.v]

All pull requests must resolve related documentation changes before merging.

## Documentation [3]

### Feature Documentation [3.i]

`rclc_lifecycle` features are documented in the package [README.md](README.md) and in the header files.

### Public API Documentation [3.ii]

`rclc_lifecycle` has embedded API documentation.

New additions to the public API require documentation before being added.

### License [3.iii]

The license for `rclc_lifecycle` is Apache 2.0, and a summary can be found in each source file.
A full copy of the license can be found [here](../LICENSE).

### Copyright Statements [3.iv]

The copyright holders each provide a statement of copyright in each source code file in `rclc_lifecycle`.

## Testing [4]

### Feature Testing [4.i]

`rclc_lifecycle` provides tests which simulate typical usage, and they are located in the [`test` directory](test).
New features are required to have tests before being added as stated in [CONTRIBUTING](CONTRIBUTING.md).
Current results can be seen [../README.md](../README.md).

Most features in `rclc_lifecycle` have corresponding tests which simulate typical usage, and they are located in the [`test`](./test) directory.
New features are required to have tests before being added.

### Public API Testing [4.ii]

Each part of the public API has tests, and new additions or changes to the public API require tests before being added. The tests aim to cover both typical usage and corner cases, but are quantified by contributing to code coverage.

### Coverage [4.iii]

The coverage report is available in the [../README.md](../README.md).

`rclc_lifecycle` checks the coverage of every commit. Last coverage assessment can be seen in [Codecov](https://app.codecov.io/gh/ros2/rclc_lifecycle/commits).

### Coverage [4.iv]

Performance tests for `rclc_lifecycle` have not been implemented.

### Linters and Static Analysis [4.v]

`rclc_lifecycle` code style is enforced using [uncrustify](https://github.com/uncrustify/uncrustify).
Among the CI tests, there are tests that ensure that every pull request is compliant with the code style.
The latest CI results can be seen [../README.md](../README.md).

`rclc_lifecycle` uses and passes all the standard linters and static analysis tools for a C99 package as described in the [ROS 2 Developer Guide](https://docs.ros.org/en/rolling/Contributing/Developer-Guide.html#linters-and-static-analysis).

Results of the latest linter tests can be found in the [../README.md](../README.md)

## Dependencies [5]

Below are evaluations of each of `rclc_lifecycle`'s run-time and build-time dependencies that have been determined to influence the quality.

It has several "buildtool" dependencies, which do not affect the resulting quality of the package, because they do not contribute to the public library API.
It also has several test dependencies, which do not affect the resulting quality of the package, because they are only used to build and run the test code.

### Direct Runtime ROS Dependencies [5.i]

#### `rclc`

The `rcl` package provides the API of the C programming language based on rcl.

It is **Quality Level 2**, see its [Quality Declaration document](https://github.com/ros2/rclc/blob/master/rclc/QUALITY_DECLARATION.md).

#### `rcl_lifecycle`

The `rcl_lifecycle` package provides functionality for ROS 2 lifecycle nodes in C.

It is **Quality Level 1**, see its [Quality Declaration document](https://github.com/ros2/rcl/blob/master/rcl_lifecycle/QUALITY_DECLARATION.md).

#### `lifecycle_msgs`

The `lifecycle_msgs` contains message and service definitions for managing lifecycle nodes. These messages and services form a standardized interface for transitioning these managed nodes through a known state-machine.

It is **Quality Level 1**, see its [Quality Declaration document](https://github.com/ros2/rcl_interfaces/blob/master/lifecycle_msgs/QUALITY_DECLARATION.md).



### Optional Direct Runtime ROS Dependencies [5.ii]

`rclc_lifecycle` has no optional Direct Runtime ROS dependencies that need to be considered for this declaration.

### Direct Runtime non-ROS Dependency [5.iii]

`rclc_lifecycle` has no Direct Runtime non-ROS dependencies that need to be considered for this declaration.

## Platform Support [6]

`rclc_lifecycle` supports all of the tier 1 platforms as described in [REP-2000](https://reps.openrobotics.org/rep-2000/#support-tiers) for Galactic Release (i.e.: Linux amd64, Linux arm64 and Windows 10), and tests each change against all of them.

## Security [7]

### Vulnerability Disclosure Policy [7.i]

This package conforms to the Vulnerability Disclosure Policy in [REP-2006](https://reps.openrobotics.org/rep-2006/).

# Current status Summary

The chart below compares the requirements in the REP-2004 with the current state of the `rclc_lifecycle` package.

|Number| Requirement| Current state |
|--|--|--|
|1| **Version policy** |---|
|1.i|Version Policy available | ✓ |
|1.ii|Stable version |✓|
|1.iii|Declared public API|✓|
|1.iv|API stability policy|✓|
|1.v|ABI stability policy|✓|
|1.vi|API/ABI stable within ros distribution|✓|
|2| **Change control process** |---|
|2.i| All changes occur on change request | ✓|
|2.ii| Contributor origin (DCO, CLA, etc) | ✓|
|2.iii| Peer review policy | ✓ |
|2.iv| CI policy for change requests | ✓ |
|2.v| Documentation policy for change requests | ✓ |
|3| **Documentation** | --- |
|3.i| Per feature documentation | ✓ |
|3.ii| Per public API item documentation | ✓ |
|3.iii| Declared License(s) | ✓ |
|3.iv| Copyright in source files| ✓ |
|3.v.a| Quality declaration linked to README | ✓ |
|3.v.b| Centralized declaration available for peer review |✓|
|4| Testing | --- |
|4.i| Feature items tests | ✓ |
|4.ii| Public API tests | ✓ |
|4.iii.a| Using coverage | ✓ |
|4.iii.a| Coverage policy | ✓ |
|4.iv.a| Performance tests (if applicable) | x |
|4.iv.b| Performance tests policy| x |
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
