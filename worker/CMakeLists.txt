cmake_minimum_required(VERSION 3.17)

project(
  CacheWorker
  VERSION 1.0
  LANGUAGES CXX)

set (CMAKE_CXX_STANDARD 20)

add_executable(worker worker.cpp)

add_library(CacheWorker cache.cpp cache.h)

target_link_libraries(worker PRIVATE Parser CacheWorker)
