cmake_minimum_required(VERSION 2.6)
project(m2client)

LIST(APPEND CMAKE_MODULE_PATH "..")
include_directories("../build/Common")
include_directories("..")

find_package(Boost COMPONENTS system REQUIRED)
find_package(Protobuf REQUIRED)

add_library(m2client STATIC 
   private/Id.cpp
   private/Inbox.cpp
   private/ZmqPbChannel.cpp
   private/MateriaClient.cpp
   private/Database.cpp
   private/Admin.cpp
   private/Journal.cpp
   private/Calendar.cpp
   private/Actions.cpp
   private/Container.cpp
   private/Events.cpp
   private/Strategy.cpp
   private/ProtoConverter.cpp
)

target_link_libraries(m2client m2common)


