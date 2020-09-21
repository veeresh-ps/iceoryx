# Build performance tests
cmake_minimum_required(VERSION 3.5)
project(perftest-udp)


find_package(iceoryx_utils REQUIRED)
add_library(perftest-udp udp.cpp)
target_link_libraries(perftest-udp
    PRIVATE
    iceoryx_utils::iceoryx_utils
)

