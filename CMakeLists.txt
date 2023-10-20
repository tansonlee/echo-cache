cmake_minimum_required(VERSION 3.17)

project(
  CacheProject
  VERSION 1.0
  LANGUAGES CXX)

set (CMAKE_CXX_STANDARD 20)

add_subdirectory(worker)
add_subdirectory(shared)
add_subdirectory(orchestrator)
add_subdirectory(clients)