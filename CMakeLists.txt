cmake_minimum_required(VERSION 2.6)

project(all)

LIST(APPEND CMAKE_MODULE_PATH "FindZeroMQ.cmake")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -std=c++17")

add_subdirectory(Common)
add_subdirectory(Calendar)
add_subdirectory(Central)
add_subdirectory(Inbox)
add_subdirectory(Actions)
add_subdirectory(Database)
add_subdirectory(Container)
add_subdirectory(Events)
add_subdirectory(Client)
add_subdirectory(Ctrl)
add_subdirectory(backuper)
add_subdirectory(restore)
add_subdirectory(Test)
#add_subdirectory(WebApp)
