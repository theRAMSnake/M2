cmake_minimum_required(VERSION 2.6)
project(Test)

LIST(APPEND CMAKE_MODULE_PATH "..")

include_directories("/usr/include/python3.8")
include_directories("../build/Common")
include_directories("..")

add_executable(Test 
#EncDecTest.cpp
#RewardTest.cpp
NewAPITest.cpp
#ExpressionsTest.cpp
#ObjectTest.cpp
#CoreTest.cpp
#DatabaseTest.cpp
ContractsTest.cpp
#Strategy3Test.cpp
Utils.cpp
#UserTest.cpp
)

find_package(Boost COMPONENTS system python unit_test_framework filesystem REQUIRED)

target_link_libraries(Test m2common m2core crypto python3.8 ${Boost_LIBRARIES} ${ZeroMQ_LIBRARIES} pthread)

add_custom_command(
        TARGET Test POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_BINARY_DIR}/Test
                ../../bin/test)
