cmake_minimum_required(VERSION 2.6)
project(m3server)

LIST(APPEND CMAKE_MODULE_PATH "..")

include_directories("../build/Common")
include_directories("..")
include_directories("~/served/src")
link_directories("~/served/lib")

add_executable(m3server
main.cpp
)


find_package(Boost COMPONENTS system REQUIRED)

target_link_libraries(m3server m3common m3core ${Boost_LIBRARIES} pthread fmt libserved.so)

add_custom_command(
        TARGET m3server POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_BINARY_DIR}/m3server
                ../../bin/m3server)
