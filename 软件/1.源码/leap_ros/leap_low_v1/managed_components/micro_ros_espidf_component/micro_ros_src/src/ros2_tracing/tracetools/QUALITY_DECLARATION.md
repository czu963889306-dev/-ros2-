This document is a declaration of software quality for the `tracetools` package, based on the guidelines in [REP-2004](https://www.ros.org/reps/rep-2004.html).

# `tracetools` Quality Declaration

The `tracetools` package claims to be in the **Quality Level 1** category.

Below are the rationales, notes, and caveats for this claim, organized by each requirement listed in the [Package Requirements for Quality Level 1 in REP-2004](https://www.ros.org/reps/rep-2004.html).

## Version Policy [1]

### Version Scheme [1.i]

`tracetools` uses `semver` according to the recommendation for ROS Core packages in the [ROS 2 Developer Guide](https://docs.ros.org/en/rolling/Contributing/Developer-Guide.html#versioning).

### Version Stability [1.ii]

`tracetools` is at or above a stable version, i.e. `>= 1.0.0`.

### Public API Declaration [1.iii]

All symbols in the installed headers that are not part of a `::detail` namespace and that do not have a leading underscore are considered part of the public API.

All installed headers are in the `include` directory of the package, headers in any other folders are not installed and considered private.

### API Stability Within a Released ROS Distribution [1.iv]/[1.vi]

`tracetools` will not break public API within a released ROS distribution, i.e. no major releases once the ROS distribution is released.

### ABI Stability Within a Released ROS Distribution [1.v]/[1.vi]

`tracetools` will maintain ABI stability within a ROS distribution.

## Change Control Process [2]

`tracetools` follows the recommended guidelines for ROS Core packages in the [ROS 2 Developer Guide](https://docs.ros.org/en/rolling/Contributing/Developer-Guide.html#change-control-process).

### Change Requests [2.i]

All changes occur through a merge request.

### Contributor Origin [2.ii]

`tracetools` uses DCO as its confirmation of contributor origin policy. More information can be found in [CONTRIBUTING](../CONTRIBUTING.md).
There is an automated DCO check for change requests.

### Peer Review Policy [2.iii]

All merge requests must have at least one peer review.

### Continuous Integration [2.iv]

All merge requests must pass CI on Ubuntu amd64.

Nightly results for all [tier 1 platforms as defined in REP-2000](https://www.ros.org/reps/rep-2000.html#support-tiers) can be found here (note that only the tagged release used in the [`ros2.repos`](https://github.com/ros2/ros2/blob/master/ros2.repos) file is tested nightly):
* [linux-aarch64_release](https://ci.ros2.org/view/nightly/job/nightly_linux-aarch64_release/lastBuild/testReport/tracetools/)
* [linux-amd64_release](https://ci.ros2.org/view/nightly/job/nightly_linux_release/lastBuild/testReport/tracetools/)
* [macos_release](https://ci.ros2.org/view/nightly/job/nightly_osx_release/lastBuild/testReport/tracetools/)
* [windows_release](https://ci.ros2.org/view/nightly/job/nightly_win_rel/lastBuild/testReport/tracetools/)

### Documentation Policy [2.v]

All merge requests must resolve related documentation changes before merging.

## Documentation [3]

### Feature Documentation [3.i]

`tracetools` has [documentation](../doc/design_ros_2.md) for all of its features, and new features require documentation before being added.

It does not currently have a features list with links to the corresponding feature documentation.

### Public API Documentation [3.ii]

`tracetools` has embedded API documentation which can be generated using doxygen. The latest version can be viewed [here](https://ros-tracing.gitlab.io/ros2_tracing-api/).

New additions to the public API require documentation before being added.

### License [3.iii]

The license for `tracetools` is Apache 2.0, and a summary is in each source file, the type is declared in the `package.xml` manifest file, and a full copy of the license is in the [`LICENSE`](./LICENSE) file.

There is an automated test which runs a linter that ensures each file has a license statement.

### Copyright Statement [3.iv]

The copyright holders each provide a statement of copyright in each source code file in `tracetools`.

There is an automated test which runs a linter that ensures each file has at least one copyright statement.

## Testing [4]

### Feature Testing [4.i]

Each feature in `tracetools` has corresponding system tests which simulate typical usage, and they are located in the `test_tracetools` package.
New features are required to have tests before being added.

### Public API Testing [4.ii]

Each part of the public API has tests, and new additions or changes to the public API require tests before being added.
The tests aim to cover both typical usage and corner cases, but are quantified by contributing to code coverage.

### Coverage [4.iii]

`tracetools` follows the recommendations for ROS Core packages in the [ROS 2 Developer Guide](https://docs.ros.org/en/rolling/Contributing/Developer-Guide.html#code-coverage).

This includes:

- tracking and reporting line coverage statistics
- achieving and maintaining a good branch line coverage (95%)
- no lines are manually skipped in coverage calculations

Changes are required to make a best effort to keep or increase coverage before being accepted, but decreases are allowed if properly justified and accepted by reviewers.

Current coverage statistics can be viewed [on codecov.io](https://codecov.io/gl/ros-tracing/ros2_tracing) or in the [results of the latest `coverage` CI job](https://gitlab.com/ros-tracing/ros2_tracing/pipelines/latest).

`tracetools` might not reach the 95% goal because it uses `#ifdef` in case LTTng is not installed, but, when using and enabling LTTng, the line coverage is above 95%.

### Performance [4.iv]

`tracetools` does not currently have any performance tests.
However, since it is simply an interface and does not contain much logic (or actual code), it does not arguably require performance tests.

### Linters and Static Analysis [4.v]

`tracetools` uses and passes all the standard linters and static analysis tools as described in the [ROS 2 Developer Guide](https://docs.ros.org/en/rolling/Contributing/Developer-Guide.html#linters-and-static-analysis).

## Dependencies [5]

`tracetools` has a few "buildtool" dependencies, which do not affect the resulting quality of the package, because they do not contribute to the public library API.
It also has a few test dependencies, which do not affect the resulting quality of the package, because they are only used to build and run the test code.

### Direct Runtime ROS Dependencies [5.i]

This package does not have any runtime ROS dependencies.

### Optional Direct Runtime ROS Dependencies [5.ii]

This package does not have any optional runtime ROS dependencies.

### Direct Runtime non-ROS Dependency [5.iii]

`tracetools` has a run-time dependency on [LTTng](https://lttng.org/docs/v2.11/).
LTTng provides tracing capabilities.

It is **Quality Level 1**, see its [Quality Declaration document](../LTTng_QUALITY_DECLARATION.md).

## Platform Support [6]

`tracetools` supports all of the tier 1 platforms as described in [REP-2000](https://www.ros.org/reps/rep-2000.html#support-tiers), and tests each change against all of them.
However, due to the nature of its features, they only work on Linux-based systems.

Nightly results can be found here:
* [linux-aarch64_release](https://ci.ros2.org/view/nightly/job/nightly_linux-aarch64_release/lastBuild/testReport/tracetools/)
* [linux-amd64_release](https://ci.ros2.org/view/nightly/job/nightly_linux_release/lastBuild/testReport/tracetools/)
* [macos_release](https://ci.ros2.org/view/nightly/job/nightly_osx_release/lastBuild/testReport/tracetools/)
* [windows_release](https://ci.ros2.org/view/nightly/job/nightly_win_rel/lastBuild/testReport/tracetools/)

## Security [7]

### Vulnerability Disclosure Policy [7.i]

`tracetools` conforms to the Vulnerability Disclosure Policy in [REP-2006](https://www.ros.org/reps/rep-2006.html).

# Current Status

The table below compares the requirements in REP-2004 with the current state of the `tracetools` package.

|Number| Requirement | Current state |
|--|--|--|
|1| **Version policy** ||
|1.i| Version policy | ✓ |
|1.ii| Stable version | ✓ |
|1.iii| Strictly declared public API | ✓ |
|1.iv| API stability policy | ✓ |
|1.v| ABI stability policy | ✓ |
|1.vi| API/ABI stablility policy within ROS distribution | ✓ |
|2| **Change control process** ||
|2.i| All changes occur through change request | ✓ |
|2.ii| Confirmation of contributor origin | ✓ * |
|2.iii| Peer review policy | ✓ * |
|2.iv| CI policy for change requests | ✓ |
|2.v| Documentation policy for change requests | ✓ |
|3| **Documentation** ||
|3.i| Per feature documentation | ✓ |
|3.ii| Public API documentation | ✓ |
|3.iii| Declared license(s) | ✓ |
|3.iv| Copyright in source files | ✓ |
|3.v.a| Quality declaration linked to from README | ✓ |
|3.v.b| Centralized declaration available for peer review |  |
|3.v.c| References any Level N lists the package belongs to | ✓ |
|4| **Testing** ||
|4.i| Feature items tests | ✓ |
|4.ii| Public API tests | ✓ |
|4.iii.a| Using coverage | ✓ |
|4.iii.b| Coverage policy | ✓ |
|4.iv.a| Performance tests | x |
|4.iv.b| Performance tests policy | ✓ |
|4.v.a| Code style enforcement (linters) | ✓ |
|4.v.b| Use of static analysis tools | ✓ |
|5| **Dependencies** ||
|5.i| Must not have lower level ROS dependencies | ✓ |
|5.ii| Optional ROS lower level dependencies | ✓ |
|5.iii| Justifies quality use of non-ROS dependencies | ✓ |
|6| **Platform Support** ||
|6.i| Support targets tier 1 ROS platforms | ✓ |
|7| **Security** ||
|7.i| Vulnerability Disclosure Policy | ✓ |

\* : going forward

Comparing this table to the [Quality Level Comparison Chart of REP-2004](https://www.ros.org/reps/rep-2004.html#quality-level-comparison-chart) led us to conclude that this package qualifies for Quality Level 1.
