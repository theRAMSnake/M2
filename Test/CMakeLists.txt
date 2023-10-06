cmake_minimum_required(VERSION 2.6)
project(Test)

LIST(APPEND CMAKE_MODULE_PATH "..")

include_directories("/usr/include/python3.8")
include_directories("../build/Common")
include_directories("..")

add_executable(Test
EncDecTest.cpp
#RewardTest.cpp
#NewAPITest.cpp
#ExpressionsTest2.cpp
#ObjectTest.cpp
#CoreTest.cpp
DatabaseTest.cpp
#ContractsTest.cpp
#Strategy3Test.cpp
#Utils.cpp
#UserTest.cpp
#IdeasTest.cpp
#ConnectionsTest.cpp
)

find_package(Boost COMPONENTS system unit_test_framework filesystem REQUIRED)

link_directories("/usr/local/lib")
target_link_libraries(Test m3common m3core m4core crypto ${Boost_LIBRARIES} ${ZeroMQ_LIBRARIES} pthread profiler)

add_custom_command(
        TARGET Test POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_BINARY_DIR}/Test
                ../../bin/test)
