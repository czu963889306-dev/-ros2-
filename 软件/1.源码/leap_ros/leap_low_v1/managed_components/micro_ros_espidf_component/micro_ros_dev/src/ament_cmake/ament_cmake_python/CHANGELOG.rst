^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package ament_cmake_python
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

1.3.14 (2026-02-23)
-------------------
* Add missing dependency (`#617 <https://github.com/ament/ament_cmake/issues/617>`_) (`#620 <https://github.com/ament/ament_cmake/issues/620>`_)
* Contributors: mergify[bot]

1.3.13 (2026-01-13)
-------------------

1.3.12 (2025-06-27)
-------------------

1.3.11 (2024-11-25)
-------------------

1.3.10 (2024-07-26)
-------------------

1.3.9 (2024-05-15)
------------------

1.3.8 (2024-02-16)
------------------

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
* Document ament_cmake_python (`#387 <https://github.com/ament/ament_cmake/issues/387>`_) (`#388 <https://github.com/ament/ament_cmake/issues/388>`_)
* Contributors: mergify[bot]

1.3.1 (2022-03-28)
------------------
* Use sysconfig directly to determine python lib dir (`#378 <https://github.com/ament/ament_cmake/issues/378>`_)
* Contributors: Scott K Logan

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
* Make ament_cmake_python symlink for symlink installs only (`#357 <https://github.com/ament/ament_cmake/issues/357>`_)
* Use FindPython3 instead of FindPythonInterp (`#355 <https://github.com/ament/ament_cmake/issues/355>`_)
* Make ament_python_install_package() match setuptools' egg names. (`#338 <https://github.com/ament/ament_cmake/issues/338>`_)
* Drop ament_cmake_python outdated tests. (`#340 <https://github.com/ament/ament_cmake/issues/340>`_)
* Update maintainers (`#336 <https://github.com/ament/ament_cmake/issues/336>`_)
* Make ament_python_install_package() install console_scripts (`#328 <https://github.com/ament/ament_cmake/issues/328>`_)
* Contributors: Chris Lalancette, Michel Hidalgo, Shane Loretz

1.1.4 (2021-05-06)
------------------

1.1.3 (2021-03-09)
------------------
* Symlink setup.cfg and sources before building Python egg-info (`#327 <https://github.com/ament/ament_cmake/issues/327>`_)
* Simplify ament_python_install_package() macro. (`#326 <https://github.com/ament/ament_cmake/issues/326>`_)
  Do not delegate to setuptools, install egg-info manually.
* Contributors: Michel Hidalgo

1.1.2 (2021-02-26 22:59)
------------------------
* Escape $ENV{DESTDIR} everywhere in ament_python_install_package() (`#324 <https://github.com/ament/ament_cmake/issues/324>`_)
  Follow up after f80071e2216e766f7bf1b0792493a5f6523e9226
* Contributors: Michel Hidalgo

1.1.1 (2021-02-26 19:12)
------------------------
* Use DESTDIR on ament_python_install_package() (`#323 <https://github.com/ament/ament_cmake/issues/323>`_)
  * Use DESTDIR on ament_python_install_package()
* Contributors: Michel Hidalgo

1.1.0 (2021-02-24)
------------------
* Make ament_python_install_package() install a flat Python egg (`#316 <https://github.com/ament/ament_cmake/issues/316>`_)
* Contributors: Michel Hidalgo

1.0.4 (2021-01-25)
------------------

1.0.3 (2020-12-10)
------------------
* [ament_cmake_python] ament_cmake_python_get_python_install_dir public (`#300 <https://github.com/ament/ament_cmake/issues/300>`_)
  * [ament_cmake_python] make the ament_cmake_python_get_python_install_dir a public interface.
* Contributors: Naveau

1.0.2 (2020-12-07)
------------------
* Update package maintainers. (`#286 <https://github.com/ament/ament_cmake/issues/286>`_)
* Contributors: Michel Hidalgo

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

0.8.1 (2019-10-23)
------------------

0.8.0 (2019-10-04)
------------------
* ensure that PYTHON_INSTALL_DIR is initialized for generated .dsv file (`#190 <https://github.com/ament/ament_cmake/issues/190>`_)
  * ensure that PYTHON_INSTALL_DIR is initialized for generated .dsv file
  * use native path of PYTHON_INSTALL_DIR
* Contributors: Dirk Thomas

0.7.3 (2019-05-29)
------------------

0.7.2 (2019-05-20)
------------------

0.7.1 (2019-05-07)
------------------

0.7.0 (2019-04-08)
------------------

0.6.0 (2018-11-13)
------------------

0.5.1 (2018-07-17)
------------------

0.5.0 (2018-06-13)
------------------

0.4.0 (2017-12-08)
------------------
* install file and not absolute path (`#110 <https://github.com/ament/ament_cmake/issues/110>`_)
* 0.0.3
* Merge pull request `#103 <https://github.com/ament/ament_cmake/issues/103>`_ from ament/resolve_some_todos
  Resolve some todos
* compile installed Python modules and packages by default, add option to skip compilation
* 0.0.2
* Merge pull request `#84 <https://github.com/ament/ament_cmake/issues/84>`_ from ament/use_in_list
  use IN_LIST
* use IN_LIST
* update schema url
* add schema to manifest files
* Merge pull request `#72 <https://github.com/ament/ament_cmake/issues/72>`_ from ament/cmake35
  require CMake 3.5
* require CMake 3.5
* Merge pull request `#58 <https://github.com/ament/ament_cmake/issues/58>`_ from ament/destination_suffix
  change DESTINATION argument name of ament_python_install_module()
* change DESTINATION argument name of ament_python_install_module()
* Merge pull request `#57 <https://github.com/ament/ament_cmake/issues/57>`_ from ament/only-install-python
  Added DESTINATION argument
* Added DESTINATION argument
* Merge pull request `#40 <https://github.com/ament/ament_cmake/issues/40>`_ from ament/consistent_path_sep
  use consistent path separator
* use platform specific path separators
* add explicit build type
* label todo with author
* disable debug output
* add missing copyright / license information, update format of existing license information
* Merge pull request `#3 <https://github.com/ament/ament_cmake/issues/3>`_ from ament/windows
  Windows Support
* addressing review comments
* [windows] convert \ in paths to / for CMake
  Otherwise CMake will interpret them as
  escape sequences or as line continuations.
* exclude .pyc files and __pycache_\_ folders from installation
* update cmake code style only
* fix Python install dir
* use project(.. NONE)
* refactor several low-level packages into ament_cmake_core (environment, environment_hooks, index, package_templates, symlink_install)
* invert dependency between ament_cmake_environment and ament_cmake_environment_hooks, add dependency on ament_cmake_environment
* refactor to use templates provided by ament_package
* refactored PYTHON_INSTALL_DIR computation
* update cmake code style
* minor fixes
* add ament_cmake_environment_hooks
* minor
* add ament_cmake_python
* Contributors: Dirk Thomas, Esteve Fernandez, Mikael Arguedas, William Woodall
