cmake_minimum_required(VERSION 2.6)
project(Profile)

LIST(APPEND CMAKE_MODULE_PATH "..")

include_directories("..")
include_directories("../build/Common")

add_executable(Profile 
main.cpp
)

find_package(Boost COMPONENTS system filesystem REQUIRED)

target_link_libraries(Profile m2common m2core ${Boost_LIBRARIES} pthread)

add_custom_command(
        TARGET Profile POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_BINARY_DIR}/Profile
                ../../bin/Profile)
