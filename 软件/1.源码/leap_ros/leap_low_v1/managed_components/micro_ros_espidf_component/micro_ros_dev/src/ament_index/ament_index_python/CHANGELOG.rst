^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package ament_index_python
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

1.4.1 (2026-01-12)
------------------

1.4.0 (2022-03-01)
------------------

* Print warning when get_package_share_directory() does not exist (Fix `#74 <https://github.com/ament/ament_index/issues/74>`_) (`#77 <https://github.com/ament/ament_index/issues/77>`_)
* Fail lookups on invalid resource names (`#69 <https://github.com/ament/ament_index/issues/69>`_)
* Add get_package_share_path method (`#73 <https://github.com/ament/ament_index/issues/73>`_)
* Contributors: David V. Lu, rob-clarke

1.0.6 (2021-05-06)
------------------

1.0.5 (2021-04-14)
------------------
* Remove Claire as the maintainer. (`#71 <https://github.com/ament/ament_index/issues/71>`_)
* Contributors: Chris Lalancette

1.0.4 (2021-04-06)
------------------
* Change links from index.ros.org -> docs.ros.org (`#70 <https://github.com/ament/ament_index/issues/70>`_)
* Contributors: Chris Lalancette

1.0.3 (2021-03-18)
------------------
* Add Audrow as a maintainer (`#68 <https://github.com/ament/ament_index/issues/68>`_)
* Contributors: Audrow Nash

1.0.2 (2021-01-25)
------------------
* update maintainers (`#67 <https://github.com/ament/ament_index/issues/67>`_)
* Contributors: Claire Wang

1.0.1 (2020-10-07)
------------------
* add rational why ament_index pkgs don't have explicit performance tests (`#65 <https://github.com/ament/ament_index/issues/65>`_)
* Remove the Quality Level from the README.md. (`#62 <https://github.com/ament/ament_index/issues/62>`_)
* Fix document link (`#61 <https://github.com/ament/ament_index/issues/61>`_)
* [Quality Declaration] Update Version Stability to stable version (`#58 <https://github.com/ament/ament_index/issues/58>`_)
* Contributors: Alejandro Hernández Cordero, Chris Lalancette, Dirk Thomas, Matthijs van der Burgh

1.0.0 (2020-05-22)
------------------
* Added Doxygen and Sphinx (`#55 <https://github.com/ament/ament_index/issues/55>`_)
* Fixes Quality Declaration link + style (`#54 <https://github.com/ament/ament_index/issues/54>`_)
* Add quality declaration ament_index_python (`#50 <https://github.com/ament/ament_index/issues/50>`_)
* Contributors: Alejandro Hernández Cordero, Jorge Perez

0.8.0 (2020-04-17)
------------------
* more verbose test_flake8 error messages (same as `ros2/launch_ros#135 <https://github.com/ros2/launch_ros/issues/135>`_)
* Add copyright checker for ament_index_python files (`#44 <https://github.com/ament/ament_index/issues/44>`_)
* Code refactors to the ament_index_python test (`#43 <https://github.com/ament/ament_index/issues/43>`_)
* Increase test coverage ament_index_python (`#41 <https://github.com/ament/ament_index/issues/41>`_)
* Fix resource name autocompleter in ament_index CLI (`#42 <https://github.com/ament/ament_index/issues/42>`_)
* Contributors: Dirk Thomas, Jorge Perez

0.7.2 (2019-10-16)
------------------
* 0.7.2
* fix completion of first arg (`#38 <https://github.com/ament/ament_index/issues/38>`_)
* install resource marker file for package (`#37 <https://github.com/ament/ament_index/issues/37>`_)
* add ament_index CLI (`#35 <https://github.com/ament/ament_index/issues/35>`_)
* Contributors: Dirk Thomas

0.7.1 (2019-09-18)
------------------
* install package manifest (`#34 <https://github.com/ament/ament_index/issues/34>`_)
* Contributors: Dirk Thomas

0.7.0 (2019-04-11)
------------------

0.5.1 (2018-06-18 13:50)
------------------------
* level setup.py versions to 0.5.1
* Contributors: Mikael Arguedas

0.5.0 (2018-06-18 13:25)
------------------------
* add pytest markers to linter tests
* set zip_safe to avoid warning during installation (`#29 <https://github.com/ament/ament_index/issues/29>`_)
* Contributors: Dirk Thomas

0.4.0 (2017-12-08)
------------------
* remove test_suite, add pytest as test_requires
* 0.0.3
* update style to satisfy new flake8 plugins
* 0.0.2
* use flake8 instead of pep8 and pyflakes
* refactor get_package_prefix to be more efficient
* add function to get packages with prefixes as dict
* doc fixup
* test package related functions
* add package related functions
* refactor ament_index_python into separate files
* expose prefix path from get_resource, add C++ has_resource
* update schema url
* add schema to manifest files
* Check resource file is readable
* Ignore subdirectories and dotfiles in get_resources
* Ignore dot files in get_resources
* update unit tests to match `#10 <https://github.com/ament/ament_index/issues/10>`_
* fix finding resources in overlayed workspaces
* add pep257 check
* add tests for ament_index_python, fix behavior
* add Python API to read information from the ament index
* Contributors: Deanna Hood, Dirk Thomas, Tully Foote, William Woodall, dhood
