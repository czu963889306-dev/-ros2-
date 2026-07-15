^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package tracetools
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

4.1.1 (2022-11-07)
------------------
* Merge branch 'clalancette/release-4.1.0' into 'master'
* Contributors: Christophe Bedard

4.1.0 (2022-03-29)
------------------
* Install headers to include/${PROJECT_NAME}
* Contributors: Christophe Bedard, Shane Loretz

4.0.0 (2022-01-20)
------------------
* Merge branch 'update-mentions-of-tracetools-test' into 'master'
  Update applicable mentions of tracetools_test to test_tracetools
  See merge request `ros-tracing/ros2_tracing!259 <https://gitlab.com/ros-tracing/ros2_tracing/-/merge_requests/259>`_
* Update applicable mentions of tracetools_test to test_tracetools
* Merge branch 'version-3-1-0' into 'master'
  Version 3.1.0
  See merge request `ros-tracing/ros2_tracing!256 <https://gitlab.com/ros-tracing/ros2_tracing/-/merge_requests/256>`_
* Contributors: Christophe Bedard

3.1.0 (2021-08-11)
------------------
* Correctly handle calls to TRACEPOINT() macro with no tracepoint args
* Move publisher handle tracepoint argument from rclcpp_publish to rcl_publish
* Add support for rmw init/pub, take, and executor instrumentation
* Contributors: Christophe Bedard

3.0.0 (2021-07-26)
------------------
* Export target on Windows and export an interface if TRACETOOLS_DISABLED
* Remove deprecated utility functions
* Contributors: Christophe Bedard, Ivan Santiago Paunovic

2.3.0 (2021-03-31)
------------------
* Update QD to be more specific about public API
* Namespace tracetools C++ functions and macros and deprecate current ones
* Contributors: Christophe Bedard

2.2.0 (2021-03-29)
------------------
* Add support for rcl_publish and rclcpp_publish tracepoints
* Contributors: Christophe Bedard

2.1.0 (2021-01-13)
------------------
* Add instrumentation support for linking a timer to a node
* Bring tracetools up to quality level 1
* Contributors: Christophe Bedard

2.0.0 (2020-10-12)
------------------
* Add lifecycle node state transition instrumentation
* Do not export tracetools if empty
* Allow disabling tracetools status app
* Contributors: Christophe Bedard, Ingo Lütkebohle, José Antonio Moral

1.0.0 (2020-04-24)
------------------
* Export -rdynamic using ament_export_link_flags and modern CMake
* Contributors: Christophe Bedard, Dirk Thomas

0.3.0 (2020-03-04)
------------------
* Various improvements to the build setup and CMakeLists.txt
* Contributors: Christophe Bedard, Stephen Brawner

0.2.10 (2019-11-17)
-------------------
* Add new rclcpp_subscription_init tracepoint to support new intra-process comms 
* Contributors: Christophe Bedard

0.2.9 (2019-10-18)
------------------
* Set symbols visibility to public for util functions
* Contributors: Christophe Bedard, Ingo Lütkebohle

0.2.8 (2019-10-14)
------------------
* Add overload of get_symbols as a fallback
* Contributors: Christophe Bedard, Ingo Lütkebohle

0.2.4 (2019-08-14)
------------------
* Ignore unused tracepoint parameters on Windows
* Contributors: Christophe Bedard

0.2.3 (2019-08-05)
------------------
* Fix Windows warnings
* Contributors: Christophe Bedard, Ingo Lütkebohle

0.2.2 (2019-08-01)
------------------
* Fix Windows linking issues
* Contributors: Christophe Bedard, Ingo Lütkebohle

0.2.0 (2019-07-30)
------------------
* Add option to compile out LTTng entirely
* Fix ament_target_dependencies() for tracetools status executable
* Remove bash scripts
* Enable tracing by default if LTTng is available
* Fix test_utils never getting built
* Contributors: Christophe Bedard, Ingo Lütkebohle, Tobias Blass

0.1.1 (2019-07-16)
------------------
* Disable tracing-related tests by default
* Contributors: Christophe Bedard

0.1.0 (2019-07-11)
------------------
* Add symbol resolution utils
* Add tracepoint definitions and wrapper macro for tracepoint functions
* Contributors: Christophe Bedard, Ingo Lütkebohle
