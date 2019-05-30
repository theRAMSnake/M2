cmake_minimum_required(VERSION 2.6)
project(m2server)

LIST(APPEND CMAKE_MODULE_PATH "..")

include_directories("../build/Common")
include_directories("..")

add_executable(m2server
main.cpp
ProtoConvertion.cpp
../Core/private/NodeAttributes.cpp #will be lib soon
)

find_package(ZeroMQ REQUIRED)
find_package(Boost COMPONENTS system REQUIRED)

target_link_libraries(m2server m2common m2core ${Boost_LIBRARIES} ${ZeroMQ_LIBRARIES} pthread)

add_custom_command(
        TARGET m2server POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_BINARY_DIR}/m2server
                ../../bin/m2server)
