add_library(CppClient client.cpp)

target_link_libraries(CppClient PRIVATE SocketClient)

target_include_directories(CppClient PUBLIC .)
