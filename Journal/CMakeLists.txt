cmake_minimum_required(VERSION 2.6)
project(m2Journal)

include_directories("../build/Common")
include_directories("..")

add_executable(m2Journal 
main.cpp 
)

LIST(APPEND CMAKE_MODULE_PATH "..")

find_package(ZeroMQ REQUIRED)
find_package(Boost COMPONENTS system filesystem REQUIRED)

target_link_libraries(m2Journal m2common m2client ${Boost_LIBRARIES} ${ZeroMQ_LIBRARIES} pthread)

add_custom_command(
        TARGET m2Journal POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_BINARY_DIR}/m2Journal
                ../../bin/m2Journal)
