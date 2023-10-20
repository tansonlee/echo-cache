cmake_minimum_required(VERSION 3.17)

project(
  RemoteCacheCommandLine
  VERSION 1.0
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)


add_executable(main client.cpp)

find_library(libs NAMES CppClient)
target_link_libraries(main PRIVATE CppClient)
