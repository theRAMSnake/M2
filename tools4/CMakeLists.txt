cmake_minimum_required(VERSION 2.6)
project(m4tools)

include_directories("../build/Common")
include_directories("..")

add_executable(m4tools 
main.cpp
)

LIST(APPEND CMAKE_MODULE_PATH "..")

target_link_libraries(m4tools m3common pthread dl)

add_custom_command(
        TARGET m4tools POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_BINARY_DIR}/m4tools
                ../../bin/m4tools)
