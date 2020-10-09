cmake_minimum_required(VERSION 2.6)
project(m2common)

LIST(APPEND CMAKE_MODULE_PATH "..")
include_directories("../build/Common")

find_package(Boost COMPONENTS system REQUIRED)

add_library(m2common STATIC 
   Utils.cpp
   Id.cpp
   Codec.cpp
   Password.cpp
)

target_link_libraries(m2common crypto)
