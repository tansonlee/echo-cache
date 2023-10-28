add_library(LocalCache cache.cpp)

target_include_directories(LocalCache PUBLIC .)
find_library(libs NAMES Parser)
target_link_libraries(LocalCache PRIVATE Parser)
