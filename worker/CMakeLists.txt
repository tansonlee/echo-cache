cmake_minimum_required(VERSION 3.17)

project(
  Worker
  VERSION 1.0
  LANGUAGES CXX)

set (CMAKE_CXX_STANDARD 20)
add_subdirectory(cache)

add_executable(worker worker.cpp)

find_library(libs NAMES Parser LocalCache SocketServer)

target_link_libraries(worker PRIVATE Parser LocalCache SocketServer)
