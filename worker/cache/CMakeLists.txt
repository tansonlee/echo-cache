add_library(LocalCache cache.cpp hashtable.h hashtable.cpp lru_cache.cpp)

target_include_directories(LocalCache PUBLIC .)
find_library(libs NAMES Parser)
target_link_libraries(LocalCache PRIVATE Parser)
