cmake_minimum_required(VERSION 2.6)
project(m2ctrl)

include_directories("../build/Common")
include_directories("..")

add_executable(m2ctrl 
   main.cpp
   ../WebApp/materia/ZmqPbChannel.cpp
)

LIST(APPEND CMAKE_MODULE_PATH "..")


find_package(Boost COMPONENTS system REQUIRED)

target_link_libraries(m2ctrl m2common ${Boost_LIBRARIES} ${ZeroMQ_LIBRARIES} pthread)

add_custom_command(
        TARGET m2ctrl POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_BINARY_DIR}/m2ctrl
                ../../bin/m2ctrl)
