cmake_minimum_required(VERSION 2.6)
project(m3common)

LIST(APPEND CMAKE_MODULE_PATH "..")
include_directories("../build/Common")

find_package(Boost COMPONENTS system REQUIRED)

add_library(m3common STATIC 
   Utils.cpp
   Id.cpp
   Base64.cpp
   Codec.cpp
   Password.cpp
)

target_link_libraries(m3common crypto)
