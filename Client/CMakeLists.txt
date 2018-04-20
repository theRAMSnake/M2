cmake_minimum_required(VERSION 2.6)
project(m2client)

LIST(APPEND CMAKE_MODULE_PATH "..")
include_directories("../build/Common")
include_directories("..")

find_package(Boost COMPONENTS system REQUIRED)
find_package(Protobuf REQUIRED)

add_library(m2client STATIC 
   Id.cpp
   Inbox.cpp
   ZmqPbChannel.cpp
   MateriaClient.cpp
   Database.cpp
   Admin.cpp
   Journal.cpp
   Calendar.cpp
   Actions.cpp
   Container.cpp
   Events.cpp
   Strategy.cpp
)

target_link_libraries(m2client m2common)


