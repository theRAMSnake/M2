cmake_minimum_required(VERSION 2.6)
project(m2migrate)

include_directories("../build/Common")
include_directories("..")

add_executable(m2migrate 
main.cpp 
)

LIST(APPEND CMAKE_MODULE_PATH "..")

find_package(ZeroMQ REQUIRED)
find_package(Boost COMPONENTS system date_time filesystem REQUIRED)

target_link_libraries(m2migrate m2common ${Boost_LIBRARIES} ${ZeroMQ_LIBRARIES} pthread dl)

add_custom_command(
        TARGET m2migrate POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_BINARY_DIR}/m2migrate
                ../../bin/m2migrate)
