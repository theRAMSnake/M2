cmake_minimum_required(VERSION 2.6)
project(m2DatabaseService)

include_directories("../build/Common")
include_directories("..")

add_executable(m2DatabaseService 
main.cpp 
)

LIST(APPEND CMAKE_MODULE_PATH "..")

find_package(ZeroMQ REQUIRED)
find_package(LIBBSONCXX REQUIRED)
find_package(LIBMONGOCXX REQUIRED)
find_package(Boost COMPONENTS system filesystem REQUIRED)

include_directories(${LIBMONGOCXX_INCLUDE_DIRS})
include_directories(${LIBBSONCXX_INCLUDE_DIRS})

target_link_libraries(m2DatabaseService m2common ${Boost_LIBRARIES} ${ZeroMQ_LIBRARIES} ${LIBMONGOCXX_LIBRARIES} ${LIBBSONCXX_LIBRARIES} pthread)

add_custom_command(
        TARGET m2DatabaseService POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_BINARY_DIR}/m2DatabaseService
                ../../bin/m2DatabaseService)
