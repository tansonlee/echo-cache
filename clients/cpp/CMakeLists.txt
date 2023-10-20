add_library(CppClient cpp_client.cpp)

find_library(libs NAMES SocketClient)
target_link_libraries(CppClient PRIVATE SocketClient)

target_include_directories(CppClient PUBLIC .)
