cmake_minimum_required(VERSION 2.6)

project(all)

LIST(APPEND CMAKE_MODULE_PATH "FindZeroMQ.cmake")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -std=c++2a -O3")

add_subdirectory(Common)
add_subdirectory(Core)
add_subdirectory(Test)
add_subdirectory(Profile)
add_subdirectory(Ctrl)
add_subdirectory(Server)
add_subdirectory(tools4)
