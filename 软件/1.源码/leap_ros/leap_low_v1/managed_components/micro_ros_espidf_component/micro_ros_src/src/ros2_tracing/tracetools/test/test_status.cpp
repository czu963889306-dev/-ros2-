// Copyright 2020 Christophe Bedard
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <gtest/gtest.h>

#include "tracetools/config.h"
#include "tracetools/status.h"
#include "tracetools/tracetools.h"

TEST(TestStatus, test_status) {
  // No real need to check the output
  EXPECT_EQ(1, tracetools_status(false));
#ifndef TRACETOOLS_DISABLED
  EXPECT_EQ(0, tracetools_status(true));
  (void)ros_trace_compile_status();
#else
  EXPECT_EQ(1, tracetools_status(true));
#endif
}
