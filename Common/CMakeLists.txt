cmake_minimum_required(VERSION 2.6)
project(m2common)

LIST(APPEND CMAKE_MODULE_PATH "..")
include_directories("../build/Common")

find_package(Boost COMPONENTS system REQUIRED)
find_package(Protobuf REQUIRED)

add_library(m2common STATIC 
   Utils.cpp
   Id.cpp
   Codec.cpp
)

add_subdirectory(messages)

target_link_libraries(m2common m2commonMessages)
