^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package tracetools_test
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

4.1.1 (2022-11-07)
------------------
* Merge branch 'clalancette/release-4.1.0' into 'master'
* Contributors: Christophe Bedard

4.1.0 (2022-03-29)
------------------
* Allow providing additional actions for TraceTestCase
* Contributors: Christophe Bedard

4.0.0 (2022-01-20)
------------------
* Remove default value for 'package' kwarg for TraceTestCase
* Move actual tests out of tracetools_test to new test_tracetools package
* Contributors: Christophe Bedard

3.1.0 (2021-08-11)
------------------
* Add tests for rmw init/pub, take, and executor instrumentation
* Add field type assertion utilities to TraceTestCase
* Fixing deprecated subscriber callback warnings
* Contributors: Abrar Rahman Protyasha, Christophe Bedard

2.3.0 (2021-03-31)
------------------
* Update after namespacing C++ tracetools functions and macros
* Contributors: Christophe Bedard

2.2.0 (2021-03-29)
------------------
* Add tests for rcl_publish and rclcpp_publish tracepoints
* Allow asserting order of list of events
* Contributors: Christophe Bedard

2.1.0 (2021-01-13)
------------------
* Allow skipping test trace cleanup by setting an environment variable
* Add test for timer-node linking instrumentation
* Increased code coverage > 94% as part of QL1
* Contributors: Christophe Bedard, Ingo Lütkebohle, Alejandro Hernández Cordero

2.0.0 (2020-10-12)
------------------
* Add lifecycle node state transition instrumentation test
* Contributors: Christophe Bedard, Ingo Lütkebohle

1.0.0 (2020-04-24)
------------------
* Add test_depend on python3-pytest-cov for tracetools_test
* Call ament_add_pytest_test only once for tracetools_test
* Move test_utils from tracetools to tracetools_test
* Contributors: Christophe Bedard

0.3.0 (2020-03-04)
------------------
* Merge and update service test to cover callback registration
* Fix default value for events_kernel in TraceTestCase
* Contributors: Christophe Bedard

0.2.12 (2019-12-09)
-------------------
* Use imperative mood in constructor docstring.
* Contributors: Christophe Bedard, Steven! Ragnarök

0.2.11 (2019-12-09)
-------------------
* Fix working directory for tracetools_test Python tests
* Fix version regex to support multi-digit numbers
* Contributors: Christophe Bedard

0.2.10 (2019-11-17)
-------------------
* Update tests after new intra-process communications
* Contributors: Christophe Bedard

0.2.6 (2019-08-16)
------------------
* Fix "do_more" check for test_ping/test_pong
* Contributors: Christophe Bedard

0.2.3 (2019-08-05)
------------------
* Fix Windows warnings
* Contributors: Christophe Bedard, Ingo Lütkebohle

0.2.0 (2019-07-30)
------------------
* Enable tracing by default if LTTng is available
* Contributors: Christophe Bedard, Tobias Blass

0.1.1 (2019-07-16)
------------------
* Disable tracing-related tests by default
* Contributors: Christophe Bedard

0.1.0 (2019-07-11)
------------------
* Add tracetools_test package with utilities
* Contributors: Christophe Bedard, Ingo Lütkebohle
