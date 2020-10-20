cmake_minimum_required(VERSION 2.6)
project(m3tools)

include_directories("../build/Common")
include_directories("..")

add_executable(m3tools 
main.cpp
)

LIST(APPEND CMAKE_MODULE_PATH "..")


find_package(Boost COMPONENTS system date_time filesystem REQUIRED)

target_link_libraries(m3tools m3common ${Boost_LIBRARIES} ${ZeroMQ_LIBRARIES} pthread dl)

add_custom_command(
        TARGET m3tools POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_BINARY_DIR}/m3tools
                ../../bin/m3tools)
