cmake_minimum_required(VERSION 3.15)
project(depender)

find_package(exlib REQUIRED)

add_executable(depender
  src/depender.c)
target_link_libraries(depender exlib)
install(TARGETS depender
  RUNTIME DESTINATION bin)
