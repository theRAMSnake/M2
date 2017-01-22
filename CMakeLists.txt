cmake_minimum_required(VERSION 2.6)
project(m2)

add_executable(m2 main.cpp)

install(TARGETS m2 RUNTIME DESTINATION bin)
