cmake_minimum_required(VERSION 2.6)
project(m2Strategy)

include_directories("../build/Common")
include_directories("..")

add_executable(m2Strategy 
main.cpp 
)

LIST(APPEND CMAKE_MODULE_PATH "..")

find_package(ZeroMQ REQUIRED)
find_package(Boost COMPONENTS system filesystem REQUIRED)

target_link_libraries(m2Strategy m2common m2client ${Boost_LIBRARIES} ${ZeroMQ_LIBRARIES} pthread)

add_custom_command(
        TARGET m2Strategy POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_BINARY_DIR}/m2Strategy
                ../../bin/m2Strategy)
