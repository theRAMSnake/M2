cmake_minimum_required(VERSION 2.6)

LIST(APPEND CMAKE_MODULE_PATH "FindZeroMQ.cmake")

add_subdirectory(Common)
add_subdirectory(Central)
add_subdirectory(Shell)
