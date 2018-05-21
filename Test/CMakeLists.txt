cmake_minimum_required(VERSION 2.6)
project(Test)

LIST(APPEND CMAKE_MODULE_PATH "..")

include_directories("../build/Common")
include_directories("..")

add_executable(Test 
AdminServiceHighLevelTest.cpp
InboxServiceHighLevelTest.cpp
ActionServiceHighLevelTest.cpp
CalendarServiceHighLevelTest.cpp
TestHelpers.cpp
ContainerTest.cpp
EventsTest.cpp
StrategyTest.cpp
#JournalServiceTest.cpp
)

find_package(Boost COMPONENTS unit_test_framework filesystem REQUIRED)
find_package(ZeroMQ REQUIRED)

target_link_libraries(Test m2common m2client sqlite3 ${Boost_LIBRARIES} ${ZeroMQ_LIBRARIES} pthread)

add_custom_command(
        TARGET Test POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_BINARY_DIR}/Test
                ../../bin/test)
