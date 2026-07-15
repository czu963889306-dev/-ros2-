cmake_minimum_required(VERSION 3.5)

project(gmock_vendor NONE)

install(
  FILES ${PROJECT_NAME}Config.cmake
  DESTINATION share/${PROJECT_NAME}/cmake
)
install(
  FILES package.xml
  DESTINATION share/${PROJECT_NAME}
)
install(
  FILES CMakeLists.txt.install
  DESTINATION src/${PROJECT_NAME}
  RENAME CMakeLists.txt
)
install(
  DIRECTORY include/gmock
  DESTINATION src/${PROJECT_NAME}/include
)
install(
  DIRECTORY src
  DESTINATION src/${PROJECT_NAME}
)
