cmake_minimum_required(VERSION 3.17)

project(
  RemoteCacheProfileClient
  VERSION 1.0
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

add_executable(profiler profile_client.cpp)

find_library(libs NAMES CppClient Parser)
target_link_libraries(profiler PRIVATE CppClient Parser)
