cmake_minimum_required(VERSION 2.6)
project(m2central)

LIST(APPEND CMAKE_MODULE_PATH "..")

include_directories("../build/Common")
include_directories("..")

add_executable(m2central 
main.cpp
ComponentGateway.cpp
AdminServiceImpl.cpp
)

find_package(ZeroMQ REQUIRED)
find_package(Boost COMPONENTS system REQUIRED)

target_link_libraries(m2central m2common ${Boost_LIBRARIES} ${ZeroMQ_LIBRARIES} pthread)

add_custom_command(
        TARGET m2central POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_BINARY_DIR}/m2central
                ../../bin/m2central)
