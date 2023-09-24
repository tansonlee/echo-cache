cmake_minimum_required(VERSION 3.17)

project(
  CacheOrchestrator
  VERSION 1.0
  LANGUAGES CXX)

set (CMAKE_CXX_STANDARD 20)

add_library(Parser ../shared/parser.cpp ../shared/parser.h)

add_executable(orchestrator orchestrator.cpp)

target_link_libraries(orchestrator PRIVATE Parser)
