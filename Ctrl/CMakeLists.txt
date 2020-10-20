cmake_minimum_required(VERSION 2.6)
project(m3ctrl)

include_directories("../build/Common")
include_directories("..")

add_executable(m3ctrl 
   main.cpp
)

LIST(APPEND CMAKE_MODULE_PATH "..")


find_package(Boost COMPONENTS system REQUIRED)

target_link_libraries(m3ctrl m3common ${Boost_LIBRARIES} ${ZeroMQ_LIBRARIES} pthread)

add_custom_command(
        TARGET m3ctrl POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_BINARY_DIR}/m3ctrl
                ../../bin/m3ctrl)
