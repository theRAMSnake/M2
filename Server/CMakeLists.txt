cmake_minimum_required(VERSION 2.6)
project(m3server)

LIST(APPEND CMAKE_MODULE_PATH "..")

include_directories("../build/Common")
include_directories("..")

add_executable(m3server
main.cpp
)


find_package(Boost COMPONENTS system REQUIRED)

target_link_libraries(m3server m3common m3core ${Boost_LIBRARIES} ${ZeroMQ_LIBRARIES} pthread fmt zmq)

add_custom_command(
        TARGET m3server POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_BINARY_DIR}/m3server
                ../../bin/m3server)
