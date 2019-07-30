cmake_minimum_required(VERSION 2.6)
project(Test)

LIST(APPEND CMAKE_MODULE_PATH "..")

include_directories("../build/Common")
include_directories("..")

add_executable(Test 
#InboxTest.cpp
CalendarTest.cpp
#JournalTest.cpp
#StrategyGraphTest.cpp
#Strategy2Test.cpp
#FreeDataTest.cpp
EncDecTest.cpp
)

find_package(Boost COMPONENTS unit_test_framework filesystem REQUIRED)
find_package(ZeroMQ REQUIRED)

target_link_libraries(Test m2common m2core crypto ${Boost_LIBRARIES} ${ZeroMQ_LIBRARIES} pthread)

add_custom_command(
        TARGET Test POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_BINARY_DIR}/Test
                ../../bin/test)
