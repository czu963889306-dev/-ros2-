This document is a declaration of software quality for the `rosidl_default_runtime` package, based on the guidelines in [REP-2004](https://www.ros.org/reps/rep-2004.html).

# `rosidl_default_runtime` Quality Declaration

The package `rosidl_default_runtime` claims to be in the **Quality Level 1** category as long as it is used with a **Quality Level 1** middleware.

Below are the rationales, notes, and caveats for this claim, organized by each requirement listed in the [Package Requirements for Quality Level 2 in REP-2004](https://www.ros.org/reps/rep-2004.html).

## Version Policy [1]

### Version Scheme [1.i]

`rosidl_default_runtime` uses `semver` according to the recommendation for ROS Core packages in the [ROS 2 Developer Guide](https://docs.ros.org/en/rolling/Contributing/Developer-Guide.html#versioning).

### Version Stability [1.ii]

`rosidl_default_runtime` is at a stable version i.e. `>= 1.0.0`.  The current version can be found in its [package.xml](./package.xml), and its change history can be found in its [CHANGELOG](./CHANGELOG.md).

### Public API Declaration [1.iii]

`rosidl_default_runtime` does not provide any public API of its own.

### API Stability Within a Released ROS Distribution [1.iv]/[1.vi]

`rosidl_default_runtime` does not provide its own API and therefore API stability will not be impacted by changes.

### ABI Stability Within a Released ROS Distribution [1.v]/[1.vi]

`rosidl_default_runtime` does not contain any C or C++ code, therefore changes will not affect ABI stability.

## Change Control Process [2]

`rosidl_default_runtime` follows the recommended guidelines for ROS Core packages in the [ROS 2 Developer Guide](https://docs.ros.org/en/rolling/Contributing/Developer-Guide.html#quality-practices).

### Change Requests [2.i]

All changes will occur through a pull request, check [ROS 2 Developer Guide](https://docs.ros.org/en/rolling/Contributing/Developer-Guide.html#change-control-process) for additional information.

### Contributor Origin [2.ii]

This package uses DCO as its confirmation of contributor origin policy. More information can be found in [CONTRIBUTING](../CONTRIBUTING.md).

### Peer Review Policy [2.iii]

Following the recommended guidelines for ROS Core packages, all pull requests must have at least 1 peer review.

### Continuous Integration [2.iv]

All pull request must pass CI on all [tier 1 platforms](https://www.ros.org/reps/rep-2000.html#support-tiers)

### Documentation Policy [2.v]

All pull requests must resolve related documentation changes before merging.

## Documentation [3]

### Feature Documentation [3.i]

`rosidl_default_runtime` does not currently have its own features.

### Public API Documentation [3.ii]

`rosidl_default_runtime` does not have a public API and therefore does not require API documentation.

### License [3.iii]

The license for `rosidl_default_runtime` is Apache 2.0, the type is declared in the [package.xml](package.xml) manifest file, and a full copy of the license is in the [LICENSE](../LICENSE) file.

There are no source files in this package and therefore linters do not check for a license statement.

### Copyright Statements [3.iv]

There are no source code files in this package and therefore no files are currently yet copyrighted.

## Testing [4]

`rosidl_default_runtime` is a package providing solely CMake files and therefore does not require tests and has no coverage or performance requirements.

### Linters and Static Analysis [4.v]

`rmw` uses and passes all the standard linters and static analysis tools for a CMake package as described in the [ROS 2 Developer Guide](https://docs.ros.org/en/rolling/Contributing/Developer-Guide.html#linters-and-static-analysis).

Results of linter tests can be found [here](https://ci.ros2.org/view/nightly/job/nightly_linux_release/lastBuild/testReport/rosidl_default_runtime/).

## Dependencies [5]

### Direct Runtime ROS Dependencies [5.i]/[5.ii]

`rosidl_default_runtime` has the following group dependencies all of which are at **Quality Level 1** for **Quality Level 1** middlewares:
* `rosidl_runtime_packages` At the time of writing, this group contains the following runtime packages:
  - `rosidl_runtime_c` [QUALITY DECLARATION](https://github.com/ros2/rosidl/blob/master/rosidl_runtime_c/QUALITY_DECLARATION.md)
  - `rosidl_runtime_cpp` [QUALITY DECLARATION](https://github.com/ros2/rosidl/blob/master/rosidl_runtime_cpp/QUALITY_DECLARATION.md)
  - `rosidl_typesupport_c` [QUALITY DECLARATION](https://github.com/ros2/rosidl_typesupport/blob/master/rosidl_typesupport_c/QUALITY_DECLARATION.md)
  - `rosidl_typesupport_cpp` [QUALITY DECLARATION](https://github.com/ros2/rosidl_typesupport/blob/master/rosidl_typesupport_cpp/QUALITY_DECLARATION.md)
* `rosidl_typesupport_c_packages` At the time of writing, this group does not contain middleware independent runtime dependencies. It will likely include `typesupport_c` packages for your chosen middleware, which need to be **Quality Level 1** for this package to be considered **Quality Level 1**.
* `rosidl_typesupport_cpp_packages` At the time of writing, this group does not contain middleware independent runtime dependencies. It will likely include `typesupport_cpp` packages for your chosen middleware, which need to be **Quality Level 1** for this package to be considered **Quality Level 1**.

Note that interface generation or interface typesupport dependencies used only by Python client libraries are not considered in this assessment.

### Direct Runtime Non-ROS Dependencies [5.iii]
`rosidl_default_runtime` does not have any runtime non-ROS dependencies.

## Platform Support [6]

`rosidl_default_runtime` supports all of the tier 1 platforms as described in [REP-2000](https://www.ros.org/reps/rep-2000.html#support-tiers), and tests each change against all of them.

Currently nightly results can be seen here:
* [linux-aarch64_release](https://ci.ros2.org/view/nightly/job/nightly_linux-aarch64_release/lastBuild/testReport/rosidl_default_runtime/)
* [linux_release](https://ci.ros2.org/view/nightly/job/nightly_linux_release/lastBuild/testReport/rosidl_default_runtime/)
* [mac_osx_release](https://ci.ros2.org/view/nightly/job/nightly_osx_release/lastBuild/testReport/rosidl_default_runtime/)
* [windows_release](https://ci.ros2.org/view/nightly/job/nightly_win_rel/lastBuild/testReport/rosidl_default_runtime/)

## Security [7]

### Vulnerability Disclosure Policy [7.i]

This package conforms to the Vulnerability Disclosure Policy in [REP-2006](https://www.ros.org/reps/rep-2006.html).
