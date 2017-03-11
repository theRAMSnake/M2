cmake_minimum_required(VERSION 2.6)
project(m2InboxService)

include_directories("../build/Common")
include_directories("..")

add_executable(m2InboxService 
main.cpp 
)

LIST(APPEND CMAKE_MODULE_PATH "..")

find_package(ZeroMQ REQUIRED)
find_package(Boost COMPONENTS system REQUIRED)

target_link_libraries(m2InboxService m2common ${Boost_LIBRARIES} ${ZeroMQ_LIBRARIES} pthread)

add_custom_command(
        TARGET m2InboxService POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_BINARY_DIR}/m2InboxService
                ../../bin/m2InboxService)
