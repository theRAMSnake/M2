cmake_minimum_required(VERSION 2.6)

LIST(APPEND CMAKE_MODULE_PATH "FindZeroMQ.cmake")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic")

add_subdirectory(Common)
add_subdirectory(Central)
add_subdirectory(Inbox)
add_subdirectory(Shell)
add_subdirectory(Test)
