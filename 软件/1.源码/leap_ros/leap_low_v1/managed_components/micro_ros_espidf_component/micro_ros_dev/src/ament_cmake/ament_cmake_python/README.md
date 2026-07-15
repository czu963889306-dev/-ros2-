# ament_cmake_python

This package adds functions for installing Python packages and modules in CMake.

## API

Calling `find_package(ament_cmake_python)` will make the following API available.

### ament_get_python_install_dir

The CMake function [`ament_get_python_install_dir`](cmake/ament_get_python_install_dir.cmake) gets the path Python packages will be installed to.
The path is always relative to `CMAKE_INSTALL_PREFIX`.

The path can be customized by setting `PYTHON_INSTALL_DIR` on the command line.
It must be a relative path.
For example, the cmake command bellow would cause Python code to be installed to `${CMAKE_INSTALL_PREFIX}/foobar/site-packages`.

```console
$ cmake ../path/to/package/using/ament_cmake_python -DPYTHON_INSTALL_DIR=foobar/site-packages
```

### ament_python_install_module

The  CMake macro [`ament_python_install_module`](cmake/ament_python_install_module.cmake) will install a single Python module to the Python install directory.

### ament_python_install_package

The CMake macro [`ament_python_install_package`](cmake/ament_python_install_package.cmake) will install a Python package and all subpackages to the Python install directory.
