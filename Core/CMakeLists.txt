cmake_minimum_required(VERSION 2.6)
project(m2core)

LIST(APPEND CMAKE_MODULE_PATH "..")

include_directories("..")

add_library(m2core 
private/Core.cpp
private/Impl.cpp
private/Id.cpp
private/Database.cpp
private/Calendar.cpp
private/Inbox.cpp
private/Journal.cpp
private/Strategy.cpp
private/Goal.cpp
private/Measurement.cpp
private/Objective.cpp
)

find_package(Boost COMPONENTS system REQUIRED)

target_link_libraries(m2core ${Boost_LIBRARIES} pthread)
