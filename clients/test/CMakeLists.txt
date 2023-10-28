cmake_minimum_required(VERSION 3.17)

project(
  RemoteCacheTest
  VERSION 1.0
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

add_executable(test test.m.cpp)

find_library(libs NAMES CppClient Parser)
target_link_libraries(test PRIVATE CppClient Parser)
