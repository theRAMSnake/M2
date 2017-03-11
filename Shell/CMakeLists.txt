cmake_minimum_required(VERSION 2.6)
project(m2shell)

include_directories("../build/Common")
include_directories("..")

add_executable(m2shell 
main.cpp 
TShellMateriaServiceProvider.hpp
MessageFactory.cpp
)

LIST(APPEND CMAKE_MODULE_PATH "..")

find_package(ZeroMQ REQUIRED)
find_package(Boost COMPONENTS system REQUIRED)

target_link_libraries(m2shell m2common ${Boost_LIBRARIES} ${ZeroMQ_LIBRARIES} pthread)

add_custom_command(
        TARGET m2shell POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_BINARY_DIR}/m2shell
                ../../bin/m2shell)
