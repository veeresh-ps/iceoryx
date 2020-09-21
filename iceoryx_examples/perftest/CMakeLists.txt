# Build performance tests
cmake_minimum_required(VERSION 3.5)
project(perftest)
set(PERF_CLIENT perf-client)
set(PERF_SERVER perf-server)

find_package(iceoryx_utils REQUIRED)

if(CMAKE_SYSTEM_NAME MATCHES QNX)
    set(ADDITIONAL_LIBS socket)
endif(CMAKE_SYSTEM_NAME MATCHES QNX)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_subdirectory(udp)
set(REQUIRED_LIBS iceoryx_utils::iceoryx_utils perftest-udp)



add_executable(${PERF_CLIENT} client.cpp base/base.cpp)
set_target_properties(${PERF_CLIENT} PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)

target_link_libraries(${PERF_CLIENT}
    PRIVATE
    ${REQUIRED_LIBS}
    ${ADDITIONAL_LIBS}
)

add_executable(${PERF_SERVER} server.cpp base/base.cpp)
set_target_properties(${PERF_SERVER} PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)

target_link_libraries(${PERF_SERVER}
    PRIVATE
    ${REQUIRED_LIBS}
    ${ADDITIONAL_LIBS}
)

install (TARGETS  ${PERF_CLIENT} ${PERF_SERVER} 
    RUNTIME DESTINATION bin
    )
