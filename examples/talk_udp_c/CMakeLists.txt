set(EXAMPLE_NAME talk_udp)

file(GLOB SOURCES ${CMAKE_SOURCE_DIR} "*.cpp" "*.c" "*.h")

add_executable(
  ${EXAMPLE_NAME}
  ${SOURCES}
  ${HEADER_FILE}
)
target_include_directories(${EXAMPLE_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include)

IF (WIN32)
  # Stuff for Windows
  target_link_libraries(${EXAMPLE_NAME} PRIVATE ws2_32.lib)
ELSE()
  # Stuff for Linux
ENDIF()
