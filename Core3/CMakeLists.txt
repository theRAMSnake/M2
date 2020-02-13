cmake_minimum_required(VERSION 2.6)
project(m3core)

LIST(APPEND CMAKE_MODULE_PATH "..")

include_directories("..")

add_library(m3core 
Database.cpp
)

find_package(Boost COMPONENTS system REQUIRED)

target_link_libraries(m3core sqlite3 ${Boost_LIBRARIES} pthread)
