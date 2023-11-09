add_library(CppClient echo_cache_client.cpp)

find_library(libs NAMES SocketClient Parser)
target_link_libraries(CppClient PRIVATE SocketClient Parser)

target_include_directories(CppClient PUBLIC .)
