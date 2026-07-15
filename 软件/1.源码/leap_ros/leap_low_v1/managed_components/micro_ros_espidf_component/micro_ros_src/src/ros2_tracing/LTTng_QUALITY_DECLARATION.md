This document is a declaration of software quality for LTTng (*Linux Trace Toolkit: next generation*), an external dependency, based on the guidelines in [REP-2004](https://www.ros.org/reps/rep-2004.html).

# LTTng Quality Declaration

This quality declaration claims that LTTng is in the **Quality Level 1** category.

Below are the rationales, notes, and caveats for this claim, organized by each requirement listed in the [Package Requirements for Quality Level 1 in REP-2004](https://www.ros.org/reps/rep-2004.html).

Note that LTTng itself is split into multiple repositories.
Each repository may provide more than one (Debian or other) package.

* [`lttng-tools`](https://github.com/lttng/lttng-tools): components to control LTTng tracing
* [`lttng-ust`](https://github.com/lttng/lttng-ust): userspace instrumentation and tracing
* [`lttng-modules`](https://github.com/lttng/lttng-modules): Linux kernel instrumentation and tracing

In general, LTTng follows the strict guidelines of the Linux kernel.

Currently, LTTng is used by downstream (ROS) packages through Ubuntu packages.
This means that the LTTng version is fixed for a given Ubuntu distro, and thus it is fixed for the corresponding ROS distro(s).
If any issue regarding the following requirements arises, it should be possible to fork the LTTng packages, apply a fix, and create and use vendor packages.

## Version Policy [1]

The mitigation strategy mentioned in the first section of this document applies here.

### Version Scheme [1.i]

LTTng does not declare any versioning scheme, but seems to follow `semver`.
Upstream [issue #1269](https://bugs.lttng.org/issues/1269) tracks the declaration of a formal version scheme.

### Version Stability [1.ii]

All LTTng packages are at or above a stable version, i.e. `>= 1.0.0`.

### Public API Declaration [1.iii]

LTTng packages clearly define their public APIs, which is their installed headers (`include/` directories).

### API Stability Within a Released ROS Distribution [1.iv]/[1.vi]

API stability within a released ROS distribution is achieved through Ubuntu package restrictions for each distribution.

### ABI Stability Within a Released ROS Distribution [1.v]/[1.vi]

ABI stability within a released ROS distribution is achieved through Ubuntu package restrictions for each distribution.

## Change Control Process [2]

### Change Requests [2.i]

All changes occur through the [LTTng mailing list](https://lists.lttng.org/cgi-bin/mailman/listinfo/lttng-dev) or through their [code review platform](https://review.lttng.org/).

### Contributor Origin [2.ii]

LTTng uses DCO as its confirmation of contributor origin policy.
For more information, see the [documentation on their Gerrit instance](https://review.lttng.org/Documentation/user-signedoffby.html).

All commits are [signed-off by the author(s)](https://lttng.org/community/#create-and-send-a-patch) and the reviewer(s).

* https://github.com/lttng/lttng-tools/commits/master
* https://github.com/lttng/lttng-ust/commits/master
* https://github.com/lttng/lttng-modules/commits/master

### Peer Review Policy [2.iii]

All changes have at least one peer review.

### Continuous Integration [2.iv]

Most changes are tested on CI before merging.
Release branches and `master` are periodically tested on their [Jenkins instance](https://ci.lttng.org/).

### Documentation Policy [2.v]

LTTng does not have an explicit documentation policy.

All features which are currently utilized by the dependent packages are documented.
If they depend on features which are undocumented, it will be necessary for them to provide their own documentation or contribute it upstream.

## Documentation [3]

### Feature Documentation [3.i]

Features are listed and well documented on the [LTTng website](https://lttng.org/docs/).

### Public API Documentation [3.ii]

LTTng packages have embedded API documentation.
It can be viewed on their man pages:

* [`lttng-ust`](https://lttng.org/man/3/lttng-ust/v2.11/)

### License [3.iii]

All repositories have a `LICENSE` file.
All relevant files have a license identifier.

* `lttng-tools` is licensed under LGPLv2.1 and GPLv2, see [`LICENSE` file](https://github.com/lttng/lttng-tools/blob/master/LICENSE)
* `lttng-ust` is licensed under LGPLv2.1, the MIT license and GPLv2, see [`LICENSE` file](https://github.com/lttng/lttng-ust/blob/master/LICENSE)
* `lttng-modules` is licensed under LGPLv2.1, GPLv2 and the MIT license, see [`LICENSE` file](https://github.com/lttng/lttng-modules/blob/master/LICENSE)

### Copyright Statement [3.iv]

All relevant files have a copyright statement and include a list of copyright holders.

## Testing [4]

### Feature Testing [4.i]

LTTng packages have tests in their `test/` directories which simulate typical usage.

### Public API Testing [4.ii]

LTTng packages have tests in their `test/` directories which test their public APIs.

### Coverage [4.iii]

LTTng does not have an explicit coverage policy and does not appear to track code coverage.
However, that does not impact the quality of dependent packages, because it mostly serves as a metric to show that it is properly tested.
Looking at the tests that LTTng has, it does seem to be properly tested.
In any case, the mitigation strategy mentioned in the first section of this document applies here if an issue is found.

### Performance [4.iv]

LTTng does not have an explicit performance regression policy.
Some relevant packages have performance tests in the form of benchmarks:

* [`lttng-ust`](https://ci.lttng.org/view/Benchmarks/job/lttng-ust-benchmarks_master_build/lastSuccessfulBuild/artifact/src/lttng-ust-benchmarks/benchmarks.json)
* [general benchmarks](https://ci.lttng.org/view/Benchmarks/job/baremetal_benchmarks_kmaster_lmaster/)

### Linters and Static Analysis [4.v]

Coverity, `cppcheck` and `scan-build` are regularly run against LTTng packages:

* [lttng-tools_master_cppcheck](https://ci.lttng.org/view/Code%20quality/job/lttng-tools_master_cppcheck/cppcheckResult)
* [lttng-tools_master_scan-build](https://ci.lttng.org/view/Code%20quality/job/lttng-tools_master_scan-build/HTML_20Report/)
* [lttng-tools_master_coverity](https://ci.lttng.org/view/Code%20quality/job/lttng-tools_master_coverity/)
* [lttng-ust_master_cppcheck](https://ci.lttng.org/view/Code%20quality/job/lttng-ust_master_cppcheck/cppcheckResult)
* [lttng-ust_master_scan-build](https://ci.lttng.org/view/Code%20quality/job/lttng-ust_master_scan-build/HTML_20Report/)
* [lttng-ust_master_coverity](https://ci.lttng.org/view/Code%20quality/job/lttng-ust_master_coverity/)
* [lttng-modules_master_cppcheck](https://ci.lttng.org/view/Code%20quality/job/lttng-modules_master_cppcheck/cppcheckResult)
* [lttng-modules_master_scan-build](https://ci.lttng.org/view/Code%20quality/job/lttng-tools_master_scan-build/HTML_20Report/)

## Dependencies [5]

### Direct Runtime non-ROS Dependency [5.iii]

LTTng packages have some direct runtime dependencies, including:

* [`liburcu`](http://liburcu.org/): userspace RCU (read-copy-update) library

## Platform Support [6]

As its name suggests, LTTng only supports Linux-based systems.

## Security [7]

### Vulnerability Disclosure Policy [7.i]

LTTng does not have an explicit vulnerability disclosure policy.
Upstream [issue #1268](https://bugs.lttng.org/issues/1268) tracks the declaration of a formal VDP.

The mitigation strategy mentioned in the first section of this document applies here.

# Current Status

The table below compares the requirements in REP-2004 with the current state of the LTTng package.

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
|2.ii| Confirmation of contributor origin | ✓ |
|2.iii| Peer review policy | ✓ |
|2.iv| CI policy for change requests | ✓ |
|2.v| Documentation policy for change requests | ✓ * |
|3| **Documentation** ||
|3.i| Per feature documentation | ✓ |
|3.ii| Public API documentation | ✓ |
|3.iii| Declared license(s) | ✓ |
|3.iv| Copyright in source files | ✓ |
|3.v.a| Quality declaration linked to from README | ✓ |
|3.v.b| Centralized declaration available for peer review | ✓ |
|3.v.c| References any Level N lists the package belongs to | ✓ |
|4| **Testing** ||
|4.i| Feature items tests | ✓ |
|4.ii| Public API tests | ✓ |
|4.iii.a| Using coverage | ✓ * |
|4.iii.b| Coverage policy | ✓ * |
|4.iv.a| Performance tests | ✓ |
|4.iv.b| Performance tests policy | ✓ * |
|4.v.a| Code style enforcement (linters) | ✓ |
|4.v.b| Use of static analysis tools | ✓ |
|5| **Dependencies** ||
|5.i| Must not have lower level ROS dependencies | ✓ |
|5.ii| Optional ROS lower level dependencies | ✓ |
|5.iii| Justifies quality use of non-ROS dependencies | ✓ * |
|6| **Platform Support** ||
|6.i| Support targets tier 1 ROS platforms | ✓ |
|7| **Security** ||
|7.i| Vulnerability Disclosure Policy | ✓ * |

\* : mitigated/does not apply

Comparing this table to the [Quality Level Comparison Chart of REP-2004](https://www.ros.org/reps/rep-2004.html#quality-level-comparison-chart) led us to conclude that this package qualifies for Quality Level 1.
