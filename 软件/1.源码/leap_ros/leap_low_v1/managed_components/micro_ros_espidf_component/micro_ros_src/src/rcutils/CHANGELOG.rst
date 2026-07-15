^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package rcutils
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

5.1.7 (2025-08-22)
------------------
* Add rcutils_raw_steady_time_now method for slew-free clock (backport `#507 <https://github.com/ros2/rcutils/issues/507>`_) (`#515 <https://github.com/ros2/rcutils/issues/515>`_)
* feat: Add human readable date to logging formats. (`#510 <https://github.com/ros2/rcutils/issues/510>`_)
* Contributors: Tomoya Fujita, mergify[bot]

5.1.6 (2024-05-15)
------------------
* update cast to modern style (`#418 <https://github.com/ros2/rcutils/issues/418>`_) (`#462 <https://github.com/ros2/rcutils/issues/462>`_)
* Contributors: mergify[bot]

5.1.5 (2024-02-16)
------------------
* Fix if(TARGET ...) condition for test (`#447 <https://github.com/ros2/rcutils/issues/447>`_) (`#448 <https://github.com/ros2/rcutils/issues/448>`_)
* Contributors: mergify[bot]

5.1.4 (2023-11-13)
------------------
* memmove for overlaping memory (`#434 <https://github.com/ros2/rcutils/issues/434>`_) (`#437 <https://github.com/ros2/rcutils/issues/437>`_)
* Contributors: mergify[bot]

5.1.3 (2023-04-25)
------------------
* avoid unnecessary copy for rcutils_char_array_vsprintf. (`#412 <https://github.com/ros2/rcutils/issues/412>`_) (`#413 <https://github.com/ros2/rcutils/issues/413>`_)
* Contributors: mergify[bot]

5.1.2 (2022-11-07)
------------------
* Change syntax __VAR_ARGS_\_ to __VA_ARGS_\_ (`#376 <https://github.com/ros2/rcutils/issues/376>`_) (`#377 <https://github.com/ros2/rcutils/issues/377>`_)
* Clarify duration arg description in logging macros (`#359 <https://github.com/ros2/rcutils/issues/359>`_) (`#360 <https://github.com/ros2/rcutils/issues/360>`_)
* Contributors: mergify[bot]

5.1.1 (2022-03-31)
------------------
* Update launch test for change related to enviroment variables in launch (`#354 <https://github.com/ros2/rcutils/issues/354>`_)
* Contributors: Jacob Perron

5.1.0 (2022-03-01)
------------------
* Remove dst_size from strlen usage (`#353 <https://github.com/ros2/rcutils/issues/353>`_)
* Install headers to include\${PROJECT_NAME} (`#351 <https://github.com/ros2/rcutils/issues/351>`_)
* Contributors: Jorge Perez, Shane Loretz

5.0.1 (2022-01-14)
------------------
* Use static_cast instead of C-style cast (`#349 <https://github.com/ros2/rcutils/issues/349>`_)
* Contributors: Jacob Perron

5.0.0 (2021-11-01)
------------------
* Fixing up documentation build when using rosdoc2 (`#344 <https://github.com/ros2/rcutils/issues/344>`_)
* Stop double-defining structs. (`#333 <https://github.com/ros2/rcutils/issues/333>`_)
* Use FindPython3 explicitly instead of FindPythonInterp implicitly (`#345 <https://github.com/ros2/rcutils/issues/345>`_)
* Fix build on Android (`#342 <https://github.com/ros2/rcutils/issues/342>`_)
* Deprecate get_env.h and move content to env.{h,c} (`#340 <https://github.com/ros2/rcutils/issues/340>`_)
* Contributors: Chris Lalancette, Christophe Bedard, Ivan Santiago Paunovic, Shane Loretz, William Woodall

4.0.2 (2021-04-12)
------------------
* Declare dependency on libatomic (`#338 <https://github.com/ros2/rcutils/issues/338>`_)
* Contributors: Scott K Logan

4.0.1 (2021-04-06)
------------------
* updating quality declaration links (re: `ros2/docs.ros2.org#52 <https://github.com/ros2/docs.ros2.org/issues/52>`_) (`#335 <https://github.com/ros2/rcutils/issues/335>`_)
* Contributors: shonigmann

4.0.0 (2021-03-18)
------------------
* Quiet down a warning in release mode. (`#334 <https://github.com/ros2/rcutils/issues/334>`_)
* Make the logging separate char an implementation detail. (`#332 <https://github.com/ros2/rcutils/issues/332>`_)
* Performance tests demo (`#288 <https://github.com/ros2/rcutils/issues/288>`_)
* Remove references of __xstat (`#330 <https://github.com/ros2/rcutils/issues/330>`_)
* Update the documentation to be more consistent. (`#331 <https://github.com/ros2/rcutils/issues/331>`_)
* Shorten some excessively long lines of CMake (`#328 <https://github.com/ros2/rcutils/issues/328>`_)
* qnx-support: include sys/link.h & avoid using dlinfo (`#327 <https://github.com/ros2/rcutils/issues/327>`_)
* QNX uses XSI-compliant (`#326 <https://github.com/ros2/rcutils/issues/326>`_)
* Contributors: Ahmed Sobhy, Chris Lalancette, Homalozoa X, Jorge Perez, Scott K Logan

3.1.0 (2021-01-25)
------------------
* Add an API for directory iteration (`#323 <https://github.com/ros2/rcutils/issues/323>`_)
* Fix a leak during error handling in dir size calculation (`#324 <https://github.com/ros2/rcutils/issues/324>`_)
* Fix rcutils_shared_library_t path on Windows. (`#322 <https://github.com/ros2/rcutils/issues/322>`_)
* Check linker flags instead of assuming compiler correlation. (`#321 <https://github.com/ros2/rcutils/issues/321>`_)
* Improve shared library relative paths handling (`#320 <https://github.com/ros2/rcutils/issues/320>`_)
* Contributors: Michel Hidalgo, Scott K Logan

3.0.0 (2020-12-02)
------------------
* Update rcutils_calculate_directory_size() to support recursion (`#306 <https://github.com/ros2/rcutils/issues/306>`_)
* Updating QD to QL 1 (`#317 <https://github.com/ros2/rcutils/issues/317>`_)
* Address unused return values found in scan-build (`#316 <https://github.com/ros2/rcutils/issues/316>`_)
* use one copy for continuous area instead of loop copy (`#312 <https://github.com/ros2/rcutils/issues/312>`_)
* use a better way to check whether string is empty (`#315 <https://github.com/ros2/rcutils/issues/315>`_)
* Use helper funciton to copy string (`#314 <https://github.com/ros2/rcutils/issues/314>`_)
* Disable a Windows platform warning. (`#311 <https://github.com/ros2/rcutils/issues/311>`_)
* Fix format of code description on document (`#313 <https://github.com/ros2/rcutils/issues/313>`_)
* Make sure to check the return values of rcutils APIs. (`#302 <https://github.com/ros2/rcutils/issues/302>`_)
* Contributors: Barry Xu, Chen Lihui, Chris Lalancette, Stephen Brawner

2.2.0 (2020-10-19)
------------------
* Add rcutils_expand_user() to expand user directory in path (`#298 <https://github.com/ros2/rcutils/issues/298>`_)
* Update the maintainers. (`#299 <https://github.com/ros2/rcutils/issues/299>`_)
* Remove the temporary variable in RCUTILS_LOGGING_AUTOINIT (`#290 <https://github.com/ros2/rcutils/issues/290>`_)
* Contributors: Chris Lalancette, Christophe Bedard, Felix Endres

2.1.0 (2020-10-02)
------------------
* Add RCUTILS_NO_FAULT_INJECTION() macro. (`#295 <https://github.com/ros2/rcutils/issues/295>`_)
* Inject faults on rcutils_get_env() and rcutils_set_env() call. (`#292 <https://github.com/ros2/rcutils/issues/292>`_)
* env.h and get_env.h docblock fixes (`#291 <https://github.com/ros2/rcutils/issues/291>`_)
* Introduce rcutils_strcasecmp, case insensitive string compare. (`#280 <https://github.com/ros2/rcutils/issues/280>`_)
* Stop using fprintf to avoid using file handles by changing as few lines of code as possible. (`#289 <https://github.com/ros2/rcutils/issues/289>`_)
* Defines QNX implementation for rcutils_get_platform_library_name (`#287 <https://github.com/ros2/rcutils/issues/287>`_)
* Contributors: Ahmed Sobhy, Ivan Santiago Paunovic, Michel Hidalgo, tomoya

2.0.0 (2020-08-28)
------------------
* Add RCUTILS_CAN_SET_ERROR_MSG_AND_RETURN_WITH_ERROR_OF() macro. (`#284 <https://github.com/ros2/rcutils/issues/284>`_)
  To fault inject error messages as well as return codes.
* Change rcutils_fault_injection_set_count to use int64_t (`#283 <https://github.com/ros2/rcutils/issues/283>`_)
* adds QNX support for rcutils_get_executable_name (`#282 <https://github.com/ros2/rcutils/issues/282>`_)
* Add fault injection hooks to default allocator (`#277 <https://github.com/ros2/rcutils/issues/277>`_)
* Fault injection macros and functionality (plus example) (`#264 <https://github.com/ros2/rcutils/issues/264>`_)
* ensure -fPIC is used when building a static lib (`#276 <https://github.com/ros2/rcutils/issues/276>`_)
* Drop vsnprintf mocks entirely. (`#275 <https://github.com/ros2/rcutils/issues/275>`_)
  Binary API is not portable across platforms and compilation config.
* Fix vsnprintf mocks for Release builds. (`#274 <https://github.com/ros2/rcutils/issues/274>`_)
* Improve test coverage mocking system calls (`#272 <https://github.com/ros2/rcutils/issues/272>`_)
* Use mimick/mimick.h header (`#273 <https://github.com/ros2/rcutils/issues/273>`_)
* Add mock test for rcutils/strerror (`#265 <https://github.com/ros2/rcutils/issues/265>`_)
* Add compiler option -Wconversion and add explicit casts for conversions that may alter the value or change the sign (`#263 <https://github.com/ros2/rcutils/issues/263>`_)
  See https://github.com/ros2/rcutils/pull/263#issuecomment-663252537.
* Removed doxygen warnings (`#266 <https://github.com/ros2/rcutils/issues/266>`_) (`#268 <https://github.com/ros2/rcutils/issues/268>`_)
* Removed doxygen warnings (`#266 <https://github.com/ros2/rcutils/issues/266>`_)
* Force _GNU_SOURCE if glibc is used. (`#267 <https://github.com/ros2/rcutils/issues/267>`_)
* Add parenthesis around the argument in time conversion macros defined in time.h (`#261 <https://github.com/ros2/rcutils/issues/261>`_)
* Contributors: Ahmed Sobhy, Alejandro Hernández Cordero, Dirk Thomas, Johannes Meyer, Jorge Perez, Michel Hidalgo, brawner

1.1.0 (2020-06-26)
------------------
* Add token join macros (`#262 <https://github.com/ros2/rcutils/issues/262>`_)
* Add rcutils_string_array_sort function (`#248 <https://github.com/ros2/rcutils/issues/248>`_)
* Add rcutils_string_array_resize function (`#247 <https://github.com/ros2/rcutils/issues/247>`_)
* Increase testing coverage of rcutils to 95% (`#258 <https://github.com/ros2/rcutils/issues/258>`_)
* Update QUALITY_DECLARATION to reflect QL 2 status (`#260 <https://github.com/ros2/rcutils/issues/260>`_)
* Update version stability section of quality declaration for 1.0 (`#256 <https://github.com/ros2/rcutils/issues/256>`_)
* Contributors: Alejandro Hernández Cordero, Jorge Perez, Karsten Knese, Michel Hidalgo, Scott K Logan, Steven! Ragnarök, Stephen Brawner

1.0.1 (2020-06-03)
------------------
* Set appropriate size for buffered logging on Windows (logging.c) (`#259 <https://github.com/ros2/rcutils/issues/259>`_)
* Add Security Vulnerability Policy pointing to REP-2006
* Updates to QD to be more like other ones
* Contributors: Chris Lalancette, Stephen Brawner

1.0.0 (2020-05-26)
------------------
* Improved implementation and testing for empty ``rcutils_string_array_t`` (`#246 <https://github.com/ros2/rcutils/issues/246>`_)
* Contributors: Scott K Logan

0.9.2 (2020-05-22)
------------------
* Move likely/unlikely macros from logging.h to macros.h (`#253 <https://github.com/ros2/rcutils/issues/253>`_)
* Add rcutils_set_env function (`#250 <https://github.com/ros2/rcutils/issues/250>`_)
* Reset error state after testing expected errors (`#251 <https://github.com/ros2/rcutils/issues/251>`_)
* Fix a link to REP-2004 (`#245 <https://github.com/ros2/rcutils/issues/245>`_)
* Contributors: Ivan Santiago Paunovic, Scott K Logan, Shota Aoki

0.9.1 (2020-05-08)
------------------
* Blast545/fix qd missing section (`#243 <https://github.com/ros2/rcutils/issues/243>`_)
* update rcutils_get_env to always use getenv (`#237 <https://github.com/ros2/rcutils/issues/237>`_)
* Contributors: Jorge Perez, Suyash Behera

0.9.0 (2020-04-24)
------------------
* Improved documentation (`#225 <https://github.com/ros2/rcutils/issues/225>`_)
* Increased test coverage (`#224 <https://github.com/ros2/rcutils/issues/224>`_)
* Set errno to EINVAL when explicitly returning -1 (`#239 <https://github.com/ros2/rcutils/issues/239>`_)
* Don't assume errno is set to 0 on success on Windows (`#238 <https://github.com/ros2/rcutils/issues/238>`_)
* Make sure to initialize buffers for logging testing (`#233 <https://github.com/ros2/rcutils/issues/233>`_)
* Add deprecated with message macro (`#235 <https://github.com/ros2/rcutils/issues/235>`_)
* Don't check GetLastError() on success (`#236 <https://github.com/ros2/rcutils/issues/236>`_)
* Add a RCUTILS_DEPRECATED macro to enable platform specific deprecation (`#234 <https://github.com/ros2/rcutils/issues/234>`_)
* Don't leak memory on realloc failing (`#232 <https://github.com/ros2/rcutils/issues/232>`_)
* Assume WIN32 HINSTANCE is a void * (`#230 <https://github.com/ros2/rcutils/issues/230>`_)
* Use ament_export_targets() (`#228 <https://github.com/ros2/rcutils/issues/228>`_)
* Add freebsd support (`#223 <https://github.com/ros2/rcutils/issues/223>`_)
* Added debug version for library names (`#227 <https://github.com/ros2/rcutils/issues/227>`_)
* Fixed condition in rcutils_get_platform_library_name (`#226 <https://github.com/ros2/rcutils/issues/226>`_)
* Added rcutils_is_shared_library_loaded function (`#222 <https://github.com/ros2/rcutils/issues/222>`_)
* Export interfaces in a addition to include directories / libraries (`#221 <https://github.com/ros2/rcutils/issues/221>`_)
* Included utils to load, unload and get symbols from shared libraries (`#215 <https://github.com/ros2/rcutils/issues/215>`_)
* Check and link against libatomic (`#172 <https://github.com/ros2/rcutils/issues/172>`_) (`#178 <https://github.com/ros2/rcutils/issues/178>`_)
* Remove test for large allocation failure (`#214 <https://github.com/ros2/rcutils/issues/214>`_)
* Increase rcutils line testing coverage  (`#208 <https://github.com/ros2/rcutils/issues/208>`_)
* Don't both print with fprintf and RCUTILS_SET_ERROR_MSG. (`#213 <https://github.com/ros2/rcutils/issues/213>`_)
* All logging to the same stream (`#196 <https://github.com/ros2/rcutils/issues/196>`_)
* Style update to match uncrustify with explicit language (`#210 <https://github.com/ros2/rcutils/issues/210>`_)
* Add in a concurrent test to test_logging_output_format.py (`#209 <https://github.com/ros2/rcutils/issues/209>`_)
* Fix bug in split function (`#206 <https://github.com/ros2/rcutils/issues/206>`_)
* Fixes in comments (`#207 <https://github.com/ros2/rcutils/issues/207>`_)
* Code style only: wrap after open parenthesis if not in one line (`#203 <https://github.com/ros2/rcutils/issues/203>`_)
* Split visibility macro project independent logic (`#194 <https://github.com/ros2/rcutils/issues/194>`_)
* Increase max length of env var value on Windows to 32767 (`#201 <https://github.com/ros2/rcutils/issues/201>`_)
* Improve error message on Windows when rcutils_get_env fails (`#200 <https://github.com/ros2/rcutils/issues/200>`_)
* Fix filesystem tests to account for extra byte on Windows (`#199 <https://github.com/ros2/rcutils/issues/199>`_)
* Calculate file and directory size (`#197 <https://github.com/ros2/rcutils/issues/197>`_)
* Fix race in rcutils launch_tests (`#193 <https://github.com/ros2/rcutils/issues/193>`_)
* Changing default logging format to include timestamp (`#190 <https://github.com/ros2/rcutils/issues/190>`_)
* Contributors: Alejandro Hernández Cordero, Chris Lalancette, Dirk Thomas, Jorge Perez, Karsten Knese, Peter Baughman, Scott K Logan, Shane Loretz, Steven Macenski, Thomas Moulard, Tully Foote, Michael Dodson

0.8.4 (2019-11-18)
------------------
* fix type of logging feature keys (`#192 <https://github.com/ros2/rcutils/issues/192>`_)
* Contributors: Dirk Thomas

0.8.3 (2019-11-12)
------------------
* Fix uninitialized handle error (`#187 <https://github.com/ros2/rcutils/issues/187>`_)
* Use Win32 wrapper around 64 bit atomic operations (`#186 <https://github.com/ros2/rcutils/issues/186>`_)
* Contributors: Sean Kelly

0.8.2 (2019-10-23)
------------------
* Specify working directory for filesystem test (`#185 <https://github.com/ros2/rcutils/issues/185>`_)
* Make use of time source type for throttling logs (`#183 <https://github.com/ros2/rcutils/issues/183>`_)
* Remove ready_fn - will be replaced by ReadyToTest() (`#184 <https://github.com/ros2/rcutils/issues/184>`_)
* Contributors: Brian Marchi, Dan Rose, Peter Baughman

0.8.1 (2019-10-03)
------------------
* Implement rcutils_mkdir. (`#166 <https://github.com/ros2/rcutils/issues/166>`_)
* Contributors: Chris Lalancette

0.8.0 (2019-09-24)
------------------
* Make g_rcutils_log_severity_names public and immutable. (`#180 <https://github.com/ros2/rcutils/issues/180>`_)
* use _WIN32 instead of WIN32 (`#179 <https://github.com/ros2/rcutils/issues/179>`_)
* Revert "check and link against libatomic (`#172 <https://github.com/ros2/rcutils/issues/172>`_)" (`#177 <https://github.com/ros2/rcutils/issues/177>`_)
* check and link against libatomic (`#172 <https://github.com/ros2/rcutils/issues/172>`_)
* Rewrite test_logging_throttle tests: (`#167 <https://github.com/ros2/rcutils/issues/167>`_)
* Disable uncrustify indentation check for macros that use windows  `__pragma` (`#164 <https://github.com/ros2/rcutils/issues/164>`_)
* Fix armhf warning (`#163 <https://github.com/ros2/rcutils/issues/163>`_)
* Contributors: Christian Rauch, Dirk Thomas, Emerson Knapp, Michel Hidalgo, Shane Loretz, jpsamper2009

0.7.3 (2019-05-29)
------------------
* getprogname() is the correct API to use on Android. (`#162 <https://github.com/ros2/rcutils/issues/162>`_)
* Contributors: Chris Lalancette

0.7.1 (2019-05-08)
------------------
* Add function rcutils_string_array_cmp (`#144 <https://github.com/ros2/rcutils/issues/144>`_)
* Rename result variable for clarity. (`#157 <https://github.com/ros2/rcutils/issues/157>`_)
* Add in utilities needed for log location (`#155 <https://github.com/ros2/rcutils/issues/155>`_)
* remove macros from source file (`#156 <https://github.com/ros2/rcutils/issues/156>`_)
* Migrate launch tests to new launch_testing features & API (`#140 <https://github.com/ros2/rcutils/issues/140>`_)
* Use GCC extension for printf-like functions (`#154 <https://github.com/ros2/rcutils/issues/154>`_)
* Fix leak in test_logging.cpp (`#153 <https://github.com/ros2/rcutils/issues/153>`_)
* Fix leak in test_logging_macros.cpp (`#152 <https://github.com/ros2/rcutils/issues/152>`_)
* Fix remaining leaks in test_string_map.cpp (`#151 <https://github.com/ros2/rcutils/issues/151>`_)
* Fix a leak in test_array_list.cpp (`#149 <https://github.com/ros2/rcutils/issues/149>`_)
* Contributors: Chris Lalancette, Dirk Thomas, Jacob Perron, Michel Hidalgo, Steven! Ragnarök, Thomas Moulard

0.7.0 (2019-04-13)
------------------
* Fix ASAN failure in test_string_map.cpp (`#147 <https://github.com/ros2/rcutils/issues/147>`_)
* Add tests for stdatomic_helper.h and fix bugs (`#150 <https://github.com/ros2/rcutils/issues/150>`_)
* Windows messages when atomic type is unsupported (`#145 <https://github.com/ros2/rcutils/issues/145>`_)
* Use CMake property to determine when to use memory_tools. (`#139 <https://github.com/ros2/rcutils/issues/139>`_)
* Add section about DCO to CONTRIBUTING.md
* Use ament_target_dependencies where possible. (`#137 <https://github.com/ros2/rcutils/issues/137>`_)
* Fix doc typo in string_map.h. (`#138 <https://github.com/ros2/rcutils/issues/138>`_)
* Add launch along with launch_testing as test dependencies. (`#136 <https://github.com/ros2/rcutils/issues/136>`_)
* Drops legacy launch API usage. (`#134 <https://github.com/ros2/rcutils/issues/134>`_)
* Contributors: Dirk Thomas, Jacob Perron, Michel Hidalgo, Shane Loretz, Steven! Ragnarök, Thomas Moulard, ivanpauno

0.6.2 (2019-02-07)
------------------
* Adding an ArrayList and HashMap implementation to rcutils (`#131 <https://github.com/ros2/rcutils/issues/131>`_)
* Change uncrustify max line length to 0 (`#133 <https://github.com/ros2/rcutils/issues/133>`_)
* Contributors: Jacob Perron, Nick Burek

0.6.1 (2018-12-06)
------------------
* Logging (`#127 <https://github.com/ros2/rcutils/issues/127>`_)
* fixes to support including in c++ and fetch_add (`#129 <https://github.com/ros2/rcutils/issues/129>`_)
* reiterate over char array (`#130 <https://github.com/ros2/rcutils/issues/130>`_)
* add rcutils_unsigned_char_array_t (`#125 <https://github.com/ros2/rcutils/issues/125>`_)
* Contributors: Karsten Knese, Nick Burek, William Woodall

0.6.0 (2018-11-16)
------------------
* Added rcutils_to_native_path function (`#119 <https://github.com/ros2/rcutils/issues/119>`_)
* Moved stdatomic helper to rcutils (`#126 <https://github.com/ros2/rcutils/issues/126>`_)
* Fixed warning in release build due to assert (`#124 <https://github.com/ros2/rcutils/issues/124>`_)
* Updated to avoid dynamic memory allocation during error handling (`#121 <https://github.com/ros2/rcutils/issues/121>`_)
* Added macro semicolons (`#120 <https://github.com/ros2/rcutils/issues/120>`_)
* Added LL suffix to avoid c4307 (`#118 <https://github.com/ros2/rcutils/issues/118>`_)
* Updated to use the same allocator to free allocated message (`#115 <https://github.com/ros2/rcutils/issues/115>`_)
* Renamed rcutils_serialized_message -> rcutils_char_array (`#111 <https://github.com/ros2/rcutils/issues/111>`_)
* Moved serialized_message from rmw (`#110 <https://github.com/ros2/rcutils/issues/110>`_)
* Updated to verify that the requested allocation size does not overflow. (`#109 <https://github.com/ros2/rcutils/issues/109>`_)
* Contributors: Chris Lalancette, Jacob Perron, Karsten Knese, Mikael Arguedas, Ruffin, Shane Loretz, Todd Malsbary, William Woodall

0.5.1 (2018-06-28)
------------------

* Removed redundant stat() call (`#108 <https://github.com/ros2/rcutils/pull/108>`_)

0.5.0 (2018-06-20)
------------------
* Audited use of malloc/realloc/calloc/free to make sure it always goes through an ``rcutils_allocator_t`` (`#102 <https://github.com/ros2/rcutils/issues/102>`_)
* Added ability to include a timestamp when a console logging message happens (`#85 <https://github.com/ros2/rcutils/issues/85>`_)
* Updated to use new memory_tools from osrf_testing_tools_cpp (`#101 <https://github.com/ros2/rcutils/issues/101>`_)
* Fixed a possible bug by preventing the default logger's level from being unset (`#106 <https://github.com/ros2/rcutils/issues/106>`_)
* Updated to use launch.legacy instead of launch (now used for new launch system) (`#105 <https://github.com/ros2/rcutils/issues/105>`_)
* Fixed a memory check issue in ``split.c`` (`#104 <https://github.com/ros2/rcutils/issues/104>`_)
  * Signed-off-by: testkit <cathy.shen@intel.com>
* Added ``RCUTILS_CONSOLE_STDOUT_LINE_BUFFERED`` to control flusing of output from the default output handler of the logging macros. (`#98 <https://github.com/ros2/rcutils/issues/98>`_)
* Can now control shared/static linking via BUILD_SHARED_LIBS (`#94 <https://github.com/ros2/rcutils/issues/94>`_)
* Addressed some MISRA C compliance issues (`#91 <https://github.com/ros2/rcutils/issues/91>`_)
* Fixed a steady time overflow issue (`#87 <https://github.com/ros2/rcutils/issues/87>`_)
* Changed rcutils_time_point_value_t type from uint64_t to int64_t (`#84 <https://github.com/ros2/rcutils/issues/84>`_)
* Fixed out-of-bounds read issue (`#83 <https://github.com/ros2/rcutils/issues/83>`_)
  * Signed-off-by: Ethan Gao <ethan.gao@linux.intel.com>
* Contributors: Dirk Thomas, Ethan Gao, Michael Carroll, Mikael Arguedas, Sagnik Basu, Shane Loretz, William Woodall, cshen, dhood, serge-nikulin
