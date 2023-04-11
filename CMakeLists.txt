cmake_minimum_required(VERSION 3.16)

# Kims MultiPlatform Socket
project(kmps)

# Put all the examples in ./build/output
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)

set(HEADER_FILE ${PROJECT_SOURCE_DIR}/include/kmps_socket.h)
add_subdirectory(examples/listen_udp_c)
add_subdirectory(examples/talk_udp_c)
add_subdirectory(examples/listen_mcast_cpp)
add_subdirectory(examples/talk_mcast_cpp)
