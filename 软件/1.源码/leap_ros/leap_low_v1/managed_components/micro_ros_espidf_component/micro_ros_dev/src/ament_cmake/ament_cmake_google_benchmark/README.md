# ament\_cmake\_google\_benchmark

This package contains logic for invoking [Google Benchmark](https://github.com/google/benchmark#readme) tests and is
comprised of three main parts:

1. The `ament_add_google_benchmark*` CMake macros.\
   These macros are used in ament packages to compile Google Benchmark executables, run Google Benchmark tests with
   CTest, or both.
2. The `run_and_convert.py` test wrapper.\
   This script is used to convert the Google Benchmark JSON results to a custom format which can be consumed by the
   [Jenkins Benchmark plugin](https://plugins.jenkins.io/benchmark/). The script also combines the generated results
   with elements from an overlay file, which may contain additional context for the benchmarks, such as descriptions
   or value thresholds.
3. The `benchmark_schema.json` output schema.\
   This schema describes the output format for the `run_and_convert.py` test wrapper, and should be used by Jenkins
   in parsing the results for aggregation and threshold checks. It is also the schema used by the overlay file.

### Examples

The source file format is well-described in the Google Benchmark README. Here is a simple example:
```c++
#include <benchmark/benchmark.h>

static void increment_perf(benchmark::State & state)
{
  size_t i = 0;
  for (auto _ : state) {
    i++;
  }
}
BENCHMARK(increment_perf);
```

To compile and run this benchmark, the following is added to the `CMakeLists.txt`:
```cmake
if(BUILD_TESTING)
  find_package(ament_cmake_google_benchmark REQUIRED)
  ament_add_google_benchmark(simple_benchmark test/simple_benchmark.cpp)
endif()
```

Jenkins can also trigger a build warning when the metrics exceed predefined thresholds. Here is an example overlay
file to sets a threshold of 0.6% deviation above or below the average metric from all previous Jenkins job builds which
include that metric:
```json
{
  "package_name.simple_benchmark": {
    "increment_perf": {
      "real_time": {
        "thresholds": [
          {
            "method": "percentageaverage",
            "percentage": 0.6
          }
        ]
      }
    }
  }
}
```

This overlay file can be generated at build time or checked in as a static file. The location of the file is
communicated to this package using the `AMENT_CMAKE_GOOGLE_BENCHMARK_OVERLAY` CMake variable. For example:
```cmake
if(BUILD_TESTING)
  set(AMENT_CMAKE_GOOGLE_BENCHMARK_OVERLAY test/benchmark_thresholds.json)
  ...
endif()
```
