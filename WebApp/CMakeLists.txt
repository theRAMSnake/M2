cmake_minimum_required(VERSION 2.6)
project(WebApp)

LIST(APPEND CMAKE_MODULE_PATH "..")

include_directories("../build/Common")
include_directories("..")

add_executable(WebApp 
main.cpp
WebApp.cpp
MainScreen.cpp
InboxView.cpp
CommonDialogManager.cpp
)

find_package(Boost COMPONENTS system REQUIRED)
find_package(ZeroMQ REQUIRED)
find_package(Wt REQUIRED)

target_link_libraries(WebApp m2common ${Boost_LIBRARIES} ${ZeroMQ_LIBRARIES} ${Wt_LIBRARIES} pthread)

add_custom_command(
        TARGET WebApp POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_BINARY_DIR}/WebApp
                ../../bin/WebApp)
