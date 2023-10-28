cmake_minimum_required(VERSION 3.17)

project(
  CacheOrchestrator
  VERSION 1.0
  LANGUAGES CXX)

set (CMAKE_CXX_STANDARD 20)

add_executable(orchestrator orchestrator.cpp helpers.cpp)
find_library(libs NAMES Parser SocketClient SocketServer)

target_link_libraries(orchestrator PRIVATE Parser SocketClient SocketServer)
