cmake_minimum_required(VERSION 3.6)

project(all)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -std=c++2a -g")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Boost COMPONENTS system REQUIRED)
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )
link_directories("/usr/local/opt/openssl/lib")
link_directories("/usr/local/lib")

add_subdirectory(Common)
add_subdirectory(Core)
add_subdirectory(Test)
add_subdirectory(Profile)
add_subdirectory(Ctrl)
#add_subdirectory(Server)
add_subdirectory(tools4)
