^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package ament_cmake_core
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

1.3.14 (2026-02-23)
-------------------

1.3.13 (2026-01-13)
-------------------
* Create destination directory during symlink install (`#569 <https://github.com/ament/ament_cmake/issues/569>`_) (`#612 <https://github.com/ament/ament_cmake/issues/612>`_)
* Contributors: Joshua Whitley

1.3.12 (2025-06-27)
-------------------

1.3.11 (2024-11-25)
-------------------

1.3.10 (2024-07-26)
-------------------
* More specific prefix in some cmake_parse_argument calls (`#523 <https://github.com/ament/ament_cmake/issues/523>`_) (`#538 <https://github.com/ament/ament_cmake/issues/538>`_)
* Contributors: mergify[bot]

1.3.9 (2024-05-15)
------------------
* Add minimum required CMake for FindPython3 (`#518 <https://github.com/ament/ament_cmake/issues/518>`_)
* Contributors: Ryan

1.3.8 (2024-02-16)
------------------
* When finding Python3, use python3 executable as a hint (`#507 <https://github.com/ament/ament_cmake/issues/507>`_)
* Contributors: Scott K Logan

1.3.7 (2024-01-24)
------------------

1.3.6 (2023-11-13)
------------------

1.3.5 (2023-06-22)
------------------

1.3.4 (2023-04-25)
------------------

1.3.3 (2022-11-07)
------------------

1.3.2 (2022-05-17)
------------------

1.3.1 (2022-03-28)
------------------

1.3.0 (2022-02-17)
------------------
* Update forthcoming version in changelog
* Contributors: Audrow Nash

1.2.1 (2022-01-14)
------------------
* Resolve various ament_lint linter violations (`#360 <https://github.com/ament/ament_cmake/issues/360>`_)
  We can't add ament_lint linters in ament_cmake in the traditional way
  without creating a circular dependency between the repositories. Even
  though we can't automatically enforce linting, it's still a good idea to
  try to keep conformance where possible.
* Update maintainers to Michael Jeronimo and Michel Hidalgo (`#362 <https://github.com/ament/ament_cmake/issues/362>`_)
* Contributors: Audrow Nash, Scott K Logan

1.2.0 (2021-10-29)
------------------
* Use FindPython3 instead of FindPythonInterp (`#355 <https://github.com/ament/ament_cmake/issues/355>`_)
* Support commands with executable targets (`#352 <https://github.com/ament/ament_cmake/issues/352>`_)
* doc/resource_index: Indent list subitems correctly (`#342 <https://github.com/ament/ament_cmake/issues/342>`_)
* Update maintainers (`#336 <https://github.com/ament/ament_cmake/issues/336>`_)
* Contributors: Chris Lalancette, Michal Sojka, Shane Loretz

1.1.4 (2021-05-06)
------------------

1.1.3 (2021-03-09)
------------------

1.1.2 (2021-02-26 22:59)
------------------------

1.1.1 (2021-02-26 19:12)
------------------------

1.1.0 (2021-02-24)
------------------

1.0.4 (2021-01-25)
------------------

1.0.3 (2020-12-10)
------------------

1.0.2 (2020-12-07)
------------------
* Merge pull request `#287 <https://github.com/ament/ament_cmake/issues/287>`_ from ament/mjeronimo/add-condition-support
  * Check condition attr in package.xml dependencies
  The condition attribute was already parsed when reading the XML
  file. Just needed to check the condition when adding dependencies
  to the list for a particular key/target.
  Fixes `#266 <https://github.com/ament/ament_cmake/issues/266>`_
  * Address Dirk's code review feedback
* Address Dirk's code review feedback
* Check condition attr in package.xml dependencies
  The condition attribute was already parsed when reading the XML
  file. Just needed to check the condition when adding dependencies
  to the list for a particular key/target.
  Fixes `#266 <https://github.com/ament/ament_cmake/issues/266>`_
* Update package maintainers. (`#286 <https://github.com/ament/ament_cmake/issues/286>`_)
* Contributors: Michael Jeronimo, Michel Hidalgo

1.0.1 (2020-09-10)
------------------

1.0.0 (2020-07-22)
------------------

0.9.6 (2020-06-23)
------------------

0.9.5 (2020-06-02)
------------------

0.9.4 (2020-05-26)
------------------

0.9.3 (2020-05-19)
------------------

0.9.2 (2020-05-07)
------------------

0.9.1 (2020-04-24 15:45)
------------------------

0.9.0 (2020-04-24 12:25)
------------------------
* Make it possible to ignore a package deprecation warning
  Wrap the deprecation warning message in a conditional, letting callers set a variable to quiet the warning.
* Use DEPRECATION instead of WARNING for package deprecation messages
  This makes it possible to treat the warnings differently in downstream packages.
  Refer to the CMake documentation for more info: https://cmake.org/cmake/help/v3.0/command/message.html
* [Windows] Adding .lib into the symlink install file list (`#219 <https://github.com/ament/ament_cmake/issues/219>`_)
  * Adding .lib into the symlink install file list
  * rework.
  * conditionally guard by WIN32.
* fix escaping of regex (`#217 <https://github.com/ament/ament_cmake/issues/217>`_)
* Fix symlink install versioned shared library (`#216 <https://github.com/ament/ament_cmake/issues/216>`_)
  * Fix symlink install versioned shared library
  * Update ament_cmake_symlink_install.cmake.in
* Use regex for more clear string manipulation. (`#207 <https://github.com/ament/ament_cmake/issues/207>`_)
  I think this reads better. If you don't agree feel free to reject PR
* add .dsv env hooks to the local_setup.dsv (`#210 <https://github.com/ament/ament_cmake/issues/210>`_)
* Contributors: Dan Rose, Dirk Thomas, Jacob Perron, Jafar Abdi, Sean Yen

0.8.1 (2019-10-23)
------------------

0.8.0 (2019-10-04)
------------------
* generate a package.dsv file (`#202 <https://github.com/ament/ament_cmake/issues/202>`_)
* check existance of uninstall target before creating it (`#195 <https://github.com/ament/ament_cmake/issues/195>`_)
* ensure that PYTHON_INSTALL_DIR is initialized for generated .dsv file (`#190 <https://github.com/ament/ament_cmake/issues/190>`_)
  * ensure that PYTHON_INSTALL_DIR is initialized for generated .dsv file
  * use native path of PYTHON_INSTALL_DIR
* generate .dsv files beside known environment hooks which describe the intended environment change (`#187 <https://github.com/ament/ament_cmake/issues/187>`_)
* Rename uninstall target so it is unique per project (`#188 <https://github.com/ament/ament_cmake/issues/188>`_)
  * Rename uninstall target so it is unique per project
  Fixes `#127 <https://github.com/ament/ament_cmake/issues/127>`_
  * Revert whitespace change
  * add cumulative uninstall target
* Contributors: Alberto Soragna, Dan Rose, Dirk Thomas

0.7.3 (2019-05-29)
------------------

0.7.2 (2019-05-20)
------------------
* close file handle early (`#169 <https://github.com/ament/ament_cmake/issues/169>`_)
* Contributors: Dirk Thomas

0.7.1 (2019-05-07)
------------------

0.7.0 (2019-04-08)
------------------
* Add option to exclude packages in ament_execute_extensions: (`#165 <https://github.com/ament/ament_cmake/issues/165>`_)
  - This provides a mechanism for 'ament-auto' packages to have
  their own exclude options
* return prefix path in ament_index_has_resource (`#155 <https://github.com/ament/ament_cmake/issues/155>`_)
* Contributors: Dirk Thomas, jpsamper2009

0.6.0 (2018-11-13)
------------------
* only add existing directories to PATH (`#149 <https://github.com/ament/ament_cmake/issues/149>`_)
* Contributors: Dirk Thomas

0.5.1 (2018-07-17)
------------------
* fix wrong FOUND flag on repeated inclusion (`#146 <https://github.com/ament/ament_cmake/issues/146>`_)
  * fix wrong FOUND flag on repeated inclusion
  * avoid FATAL_ERROR, just set it to false
* simplify condition
* fix using uninitialized CMake variables (`#145 <https://github.com/ament/ament_cmake/issues/145>`_)
* add signature parameter to docblock (`#144 <https://github.com/ament/ament_cmake/issues/144>`_)
* Contributors: Dirk Thomas

0.5.0 (2018-06-13)
------------------
* change order of _CONFIG_EXTRAS_POST `#140 <https://github.com/ament/ament_cmake/issues/140>`_
* Fix ${PROJECT_NAME}_CONFIG_EXTRAS_POST (`#140 <https://github.com/ament/ament_cmake/issues/140>`_)
  * Fix `#139 <https://github.com/ament/ament_cmake/issues/139>`_.
  * project specific variable after the global populated by functions
* fix typos. (`#138 <https://github.com/ament/ament_cmake/issues/138>`_)
* Always write generated cmake as utf8 (`#136 <https://github.com/ament/ament_cmake/issues/136>`_)
  * Always write output as utf-8.
  CMake documentation suggests that we should be writing 7-bit ascii
  CMake source files or writing UTF-8 with a byte order mark. (Source:
  https://cmake.org/cmake/help/v3.5/manual/cmake-language.7.html#encoding).
  This doesn't actually do either of those things. It just cements our
  position of non-compliance (writing utf-8 without a byte order mark)
  so that builds don't crash if the system encoding is other than utf-8.
  Alternatively we could sanitize the generated CMake content so it is
  7-bit ascii and explicitly write it as such or consider adding the byte
  order mark.
  * Always read package.xml as utf-8.
  Cherry pick of https://github.com/ament/ament_cmake/commit/3d3c02b26948aa3708a3d2d0a924aa2c61a26cb5
* use catkin_pkg to parse manifests (`#137 <https://github.com/ament/ament_cmake/issues/137>`_)
* fix symlink install from subdirectories (`#134 <https://github.com/ament/ament_cmake/issues/134>`_)
* add CONFIG_EXTRAS_POST to ament_package() (`#123 <https://github.com/ament/ament_cmake/issues/123>`_)
* Contributors: Dirk Thomas, Steven! Ragnarök, csukuangfj

0.4.0 (2017-12-08)
------------------
* populate GROUP_DEPENDS and MEMBER_OF_GROUPS cmake variables (`#119 <https://github.com/ament/ament_cmake/issues/119>`_)
* Merge pull request `#112 <https://github.com/ament/ament_cmake/issues/112>`_ from ament/doc_available_env_hooks
  add doc about CMake variables for environment hooks
* add doc about CMake variables for environment hooks
* 0.0.3
* Merge pull request `#107 <https://github.com/ament/ament_cmake/issues/107>`_ from ament/flake8_plugins
  update style to satisfy new flake8 plugins
* update style to satisfy new flake8 plugins
* AMENT_INDEX_BINARY_DIR arg for register_resource_index (`#106 <https://github.com/ament/ament_cmake/issues/106>`_)
* make installing the markerfile optional (`#105 <https://github.com/ament/ament_cmake/issues/105>`_)
  * make installing the markerfile optional
  * correct check for unused args
* Merge pull request `#103 <https://github.com/ament/ament_cmake/issues/103>`_ from ament/resolve_some_todos
  Resolve some todos
* use file(GLOB LIST_DIRECTORIES
* remove obsolete todos
* add some more info to resource index doc (`#100 <https://github.com/ament/ament_cmake/issues/100>`_)
  * add some more info to resource index doc
  * typos
  * missing word
* 0.0.2
* fix spelling in docblock
* Merge pull request `#89 <https://github.com/ament/ament_cmake/issues/89>`_ from ament/symlink_install_targets_with_configs
  support symlink install for config specific targets
* support symlink install for config specific targets
* Merge pull request `#86 <https://github.com/ament/ament_cmake/issues/86>`_ from ament/remove_include
  remove unnecessary include
* remove unnecessary include
* Merge pull request `#84 <https://github.com/ament/ament_cmake/issues/84>`_ from ament/use_in_list
  use IN_LIST
* use IN_LIST
* remove __future_\_ imports
* Merge pull request `#77 <https://github.com/ament/ament_cmake/issues/77>`_ from ament/composition
  allow generator expression in resources
* allow generator expression in resources
* Merge pull request `#76 <https://github.com/ament/ament_cmake/issues/76>`_ from ament/parent_prefix_path_placeholder
  use {prefix} as a placeholder for the install prefix in the parent_prefix_path resource
* use {prefix} as a placeholder for the install prefix in the parent_prefix_path resource
* update schema url
* add schema to manifest files
* Merge pull request `#72 <https://github.com/ament/ament_cmake/issues/72>`_ from ament/cmake35
  require CMake 3.5
* remove trailing spaces from comparisons, obsolete quotes and explicit variable expansion
* remove obsolete policies
* require CMake 3.5
* fix comment
* Merge pull request `#68 <https://github.com/ament/ament_cmake/issues/68>`_ from ament/ctest_build_testing
  use CTest BUILD_TESTING
* use CTest BUILD_TESTING
* Ignore dot files and subdirectories in get_resources (`#67 <https://github.com/ament/ament_cmake/issues/67>`_)
  * Ignore directories, and files starting with a dot in find_resources
  * Copyedit
  * Specify behaviour of get_resources with directories and hidden files
* generate all ament index markers into <build>/ament_index_preinstall
  * use compliant layout for index resources in build space and allow using those
  * fix optional arguments of ament_index_register_package
  * allow to skip the AMENT_PREFIX_PATH and / or the folder in the binary dir
  * fix error handling error
  * allow overriding default prefix path for ament index CMake API
  * undo any ; -> \; substitution done to pass PATH lists on Windows
  * only replace : with ; when no on Windows
* Merge pull request `#63 <https://github.com/ament/ament_cmake/issues/63>`_ from ament/make_template_paths_relocatable
  defer evaluation of template paths to each package
* defer evaluation of template paths to each package
* Merge pull request `#51 <https://github.com/ament/ament_cmake/issues/51>`_ from ament/find_package_xml_in_sub_dir
  look for the package.xml in the project's source dir
* look for the package.xml in the project's source dir
* Merge pull request `#49 <https://github.com/ament/ament_cmake/issues/49>`_ from ament/delete_broken_symlinks
  also delete broken symlinks
* also delete broken symlinks
* Merge pull request `#45 <https://github.com/ament/ament_cmake/issues/45>`_ from ament/use_message_status
  avoid using message without STATUS
* avoid using message without STATUS
* Merge pull request `#42 <https://github.com/ament/ament_cmake/issues/42>`_ from ament/reuse_hook_from_ament_package
  reuse environment hook provided by ament_package
* reuse environment hook provided by ament_package
* Merge pull request `#41 <https://github.com/ament/ament_cmake/issues/41>`_ from ament/cleanup_windows_setup_files
  cleanup windows setup files
* clean up windows setup files
* Merge pull request `#40 <https://github.com/ament/ament_cmake/issues/40>`_ from ament/consistent_path_sep
  use consistent path separator
* use platform specific path separators
* Merge pull request `#37 <https://github.com/ament/ament_cmake/issues/37>`_ from ament/test_labels
  add labels to tests
* fix spelling
* Merge pull request `#29 <https://github.com/ament/ament_cmake/issues/29>`_ from ament/suppress_cmp0026
  set cmp0026 to OLD until we can migrate to use $<TARGET_FILE:...>
* update comment and set the policy in two other places
* set cmp0026 to OLD until we can migrate to use $<TARGET_FILE:...>
* Merge pull request `#26 <https://github.com/ament/ament_cmake/issues/26>`_ from ament/duplicate_resources
  never return duplicate resources
* never return duplicate resources
* Merge pull request `#23 <https://github.com/ament/ament_cmake/issues/23>`_ from ament/dump_export_to_cmake
  provide export tags to cmake
* provide export tags to cmake
* Merge pull request `#21 <https://github.com/ament/ament_cmake/issues/21>`_ from ament/load_config_extras_before_exported_information
  load CONFIG_EXTRAS before exported information
* load CONFIG_EXTRAS before exported information
* Merge pull request `#17 <https://github.com/ament/ament_cmake/issues/17>`_ from ament/per_package_parent_prefix_path
  generate per project parent_prefix_path files
* generate per project parent_prefix_path files
* add explicit build type
* Merge pull request `#14 <https://github.com/ament/ament_cmake/issues/14>`_ from ament/refactor_prefix_level_files
  disable generation of prefix level setup files by default
* disable generation of prefix level setup files by default
* Merge pull request `#13 <https://github.com/ament/ament_cmake/issues/13>`_ from ament/uninstall_target
  implement CMake uninstall target
* implement symlinked install(FILES .. RENAME ..)
* add CMake uninstall target
* fix up-to-date symlink detection, update comments
* Merge pull request `#12 <https://github.com/ament/ament_cmake/issues/12>`_ from ament/wjwwood_warnings_cleanup
  Fixing some CMake warnings
* use AMENT_ENABLE_TESTING to avoid warnings
* Set CMake policy 0042 to avoid warnings on OS X
* Merge pull request `#11 <https://github.com/ament/ament_cmake/issues/11>`_ from ament/typesupport_for_rmw_impl
  access content of resource index entries
* export type support for rmw implementation
* disable debug output
* Merge pull request `#9 <https://github.com/ament/ament_cmake/issues/9>`_ from ament/symlink_install_directory_pattern
  implement symlink install for DIRECTORY with PATTERN (EXCLUDE) (fix `#8 <https://github.com/ament/ament_cmake/issues/8>`_)
* fix exclude pattern
* implement symlink install for DIRECTORY with PATTERN (EXCLUDE) (fix `#8 <https://github.com/ament/ament_cmake/issues/8>`_)
* add missing copyright / license information, update format of existing license information
* Merge pull request `#3 <https://github.com/ament/ament_cmake/issues/3>`_ from ament/windows
  Windows Support
* Merge pull request `#5 <https://github.com/ament/ament_cmake/issues/5>`_ from ament/heterogeneous_destinations
  improve symlinked install of targets to support different destination types
* improve symlinked install of targets to support different destination types based on the file extension (fix `#4 <https://github.com/ament/ament_cmake/issues/4>`_)
* addressing review comments
* [windows] fix AMENT_PREFIX_PATH handling
* addressing review comments
* [windows] add back IS_WINDOWS in one place
* [windows] compact file extension logic
* simplify removal of backslashes from generated CMake
* [windows] use "arrays" to avoid large env vars
  the limit is 8192, but that the combined number
  of characters for all the concatenated env
  hook paths for each package.
  i think it could be further separated into
  one variable per env hook per package,
  but that seemed like overkill for now.
* [windows] add more .bat versions of env hooks
* [windows] convert \ in paths to / for CMake
  Otherwise CMake will interpret them as
  escape sequences or as line continuations.
* add has_resource function
* disable messages about install() invocations
* update cmake code style only
* update dependencies
* add marker file with run dependencies
* fix registering resources with content
* source environment hooks in alphanumeric order
* use project(.. NONE)
* refactor several low-level packages into ament_cmake_core (environment, environment_hooks, index, package_templates, symlink_install)
* fix comments
* refactored PYTHON_INSTALL_DIR computation
* deal with CMake double expansion
* add normalize_path function
* fix assert file exists message broken by code style change
* update cmake code style
* minor fixes
* code style only
* add ament_cmake_auto
* add ament_cmake_core
* Contributors: Dirk Thomas, Karsten Knese, Mikael Arguedas, William Woodall, dhood
