This document is a declaration of software quality for the `rmw` package, based on the guidelines in [REP-2004](https://reps.openrobotics.org/rep-2004/).

# `rmw` Quality Declaration

The package `rmw` claims to be in the **Quality Level 1** category.

Below are the rationales, notes, and caveats for this claim, organized by each requirement listed in the [Package Requirements for Quality Level 1 in REP-2004](https://reps.openrobotics.org/rep-2004/).

## Version Policy [1]

### Version Scheme [1.i]

`rmw` uses `semver` according to the recommendation for ROS Core packages in the [ROS 2 Developer Guide](https://docs.ros.org/en/rolling/Contributing/Developer-Guide.html#versioning).

### Version Stability [1.ii]

`rmw` is at a stable version, i.e. `>= 1.0.0`.
The current version can be found in its [package.xml](package.xml), and its change history can be found in its [CHANGELOG](CHANGELOG.rst).

### Public API Declaration [1.iii]

All symbols in the installed headers are considered part of the public API.

All installed headers are in the `include` directory of the package, headers in any other folders are not installed and considered private.

### API Stability Within a Released ROS Distribution [1.iv]/[1.vi]

`rmw` will not break public API within a released ROS distribution, i.e. no major releases once the ROS distribution is released.

### ABI Stability Within a Released ROS Distribution [1.v]/[1.vi]

`rmw` contains C code and therefore must be concerned with ABI stability, and will maintain ABI stability within a ROS distribution.

## Change Control Process [2]

`rmw` follows the recommended guidelines for ROS Core packages in the [ROS 2 Developer Guide](https://docs.ros.org/en/rolling/Contributing/Developer-Guide.html#quality-practices).

### Change Requests [2.i]

This package requires that all changes occur through a pull request.

### Contributor Origin [2.ii]

This package uses DCO as its confirmation of contributor origin policy. More information can be found in [CONTRIBUTING](../CONTRIBUTING.md).

### Peer Review Policy [2.iii]

Following the recommended guidelines for ROS Core packages, all pull requests must have at least 1 peer review.

### Continuous Integration [2.iv]

All pull request must pass CI on all [tier 1 platforms](https://reps.openrobotics.org/rep-2000/#support-tiers)

### Documentation Policy [2.v]

All pull requests must resolve related documentation changes before merging.

## Documentation [3]

### Feature Documentation [3.i]

`rmw` has a [feature list](http://docs.ros2.org/latest/api/rmw/) and each item in the list links to the corresponding feature documentation.
There is documentation for all of the features, and new features require documentation before being added.

### Public API Documentation [3.ii]

`rmw` has embedded API documentation and it is generated using doxygen and is [hosted](https://docs.ros2.org/latest/api/rmw/index.html) alongside the feature documentation.
There is documentation for all of the public API, and new additions to the public API require documentation before being added.

### License [3.iii]

The license for `rmw_implementation_cmake` is Apache 2.0, and a summary is in each source file, the type is declared in the [package.xml](package.xml) manifest file, and a full copy of the license is in the [LICENSE](../LICENSE) file.

There is an automated test which runs a linter that ensures each file has a license statement.

Most recent test results can be found [here](http://build.ros2.org/view/Rpr/job/Rpr__rmw__ubuntu_focal_amd64/lastCompletedBuild/testReport/rmw/).

### Copyright Statements [3.iv]

The copyright holders each provide a statement of copyright in each source code file in `rmw`.

There is an automated test which runs a linter that ensures each file has at least one copyright statement.

The results of those tests are available [here](http://build.ros2.org/view/Rpr/job/Rpr__rmw__ubuntu_focal_amd64/lastCompletedBuild/testReport/rmw/copyright/).

Results of the copyright test can be found [here](http://build.ros2.org/view/Rpr/job/Rpr__rmw__ubuntu_focal_amd64/lastCompletedBuild/testReport/rmw/copyright/).

## Testing [4]

### Feature Testing [4.i]

Some features in `rmw` have corresponding tests which simulate typical usage, and they are located in the `test` directory.
New features are required to have tests before being added.

### Public API Testing [4.ii]

Much of the public API has tests, and new additions or changes to the public API require tests before being added.
The tests aim to cover both typical usage and corner cases, but are quantified by contributing to code coverage.

### Coverage [4.iii]

`rmw` follows the recommendations for ROS Core packages in the [ROS 2 Developer Guide](https://docs.ros.org/en/rolling/Contributing/Developer-Guide.html#code-coverage), and opts to use line coverage instead of branch coverage.

This includes:

- tracking and reporting line coverage statistics
- achieving and maintaining a reasonable branch line coverage (90-100%)
- no lines are manually skipped in coverage calculations

Changes are required to make a best effort to keep or increase coverage before being accepted, but decreases are allowed if properly justified and accepted by reviewers.

This package has testing coverage of at least 95%.
Current coverage statistics can be viewed [here](https://ci.ros2.org/job/ci_linux_coverage/lastSuccessfulBuild/cobertura/).
A description of how coverage statistics are summarized from this page can be found in the [ROS 2 On-boarding Guide](https://docs.ros.org/en/rolling/Contributing/Developer-Guide.html#note-on-coverage-runs).

### Performance [4.iv]

`rmw` follows the recommendations for performance testing of C code in the [ROS 2 Developer Guide](https://docs.ros.org/en/rolling/Contributing/Developer-Guide.html#performance), and opts to do performance analysis on each release rather than each change.

System level performance benchmarks that cover features of `rmw` can be found at:
* [Benchmarks](http://build.ros2.org/view/Rci/job/Rci__benchmark_ubuntu_focal_amd64/BenchmarkTable/)
* [Performance](http://build.ros2.org/view/Rci/job/Rci__nightly-performance_ubuntu_focal_amd64/lastCompletedBuild/)

Changes that introduce regressions in performance must be adequately justified in order to be accepted and merged.

### Linters and Static Analysis [4.v]

`rmw` uses and passes all the standard linters and static analysis tools for a C++ package as described in the [ROS 2 Developer Guide](https://docs.ros.org/en/rolling/Contributing/Developer-Guide.html#linters-and-static-analysis).

Results of linter tests can be found [here](http://build.ros2.org/view/Rpr/job/Rpr__rmw__ubuntu_focal_amd64/lastCompletedBuild/testReport/rmw/).

## Dependencies [5]

Below are evaluations of each of `rmw`'s run-time and build-time dependencies that have been determined to influence the quality.

It has several "buildtool" dependencies, which do not affect the resulting quality of the package, because they do not contribute to the public library API.
It also has several test dependencies, which do not affect the resulting quality of the package, because they are only used to build and run the test code.

### Direct Runtime ROS Dependencies [5.i]/[5.ii]

#### `rcutils`

The `rcutils` package provides an API which contains common utilities and data structures needed when programming in C.

It is **Quality Level 1**, see its [Quality Declaration document](https://github.com/ros2/rcutils/blob/master/QUALITY_DECLARATION.md).

#### `rosidl_runtime_c`

The `rosidl_runtime_c` package provides runtime interfaces in C based on user defined ROS Messages and ROS Services, as well as associated support functions for those types.

It is **Quality Level 1**, see its [Quality Declaration document](https://github.com/ros2/rosidl/tree/master/rosidl_runtime_c/Quality_Declaration.md).

### Direct Runtime Non-ROS Dependencies [5.iii]

`rmw` does not have any runtime non-ROS dependencies.

## Platform Support [6]

`rmw` supports all of the tier 1 platforms as described in [REP-2000](https://reps.openrobotics.org/rep-2000/#support-tiers), and tests each change against all of them.

Currently nightly results can be seen here:
* [linux-aarch64_release](https://ci.ros2.org/view/nightly/job/nightly_linux-aarch64_release/lastBuild/testReport/rmw/)
* [linux_release](https://ci.ros2.org/view/nightly/job/nightly_linux_release/lastBuild/testReport/rmw/)
* [mac_osx_release](https://ci.ros2.org/view/nightly/job/nightly_osx_release/lastBuild/testReport/rmw/)
* [windows_release](https://ci.ros2.org/view/nightly/job/nightly_win_rel/lastBuild/testReport/rmw/)

## Security [7]

### Vulnerability Disclosure Policy [7.i]

This package conforms to the Vulnerability Disclosure Policy in [REP-2006](https://reps.openrobotics.org/rep-2006/).
