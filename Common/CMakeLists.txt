cmake_minimum_required(VERSION 3.6)
project(m3common)

LIST(APPEND CMAKE_MODULE_PATH "..")

find_package(Boost COMPONENTS system REQUIRED)
find_package(OpenSSL)

include_directories(${OPENSSL_INCLUDE_DIR})

add_library(m3common STATIC 
   Utils.cpp
   Id.cpp
   Base64.cpp
   Codec.cpp
   Password.cpp
)

target_link_libraries(m3common crypto)
