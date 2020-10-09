cmake_minimum_required(VERSION 2.6)
project(m2core)

LIST(APPEND CMAKE_MODULE_PATH "..")

include_directories("..")
include_directories("/usr/include/python3.8")

add_library(m2core 
private/Core3.cpp
private/TypeSystem.cpp
private/CommandParser.cpp
private/Expressions.cpp
private/Commands.cpp
private/ObjectManager.cpp
private/Impl.cpp
private/Database.cpp
private/Logger.cpp
private/JsonSerializer.cpp
private/JsonRestorationProvider.cpp
private/EmptyValueProvider.cpp
private/Object.cpp
private/TypeHandler.cpp
private/types/SimpleList.cpp
private/types/Variable.cpp
private/Type.cpp
private/subsystems/Finance.cpp
private/subsystems/User.cpp
private/subsystems/Common.cpp
private/subsystems/Journal.cpp
private/subsystems/Calendar.cpp
private/subsystems/Reward.cpp
private/subsystems/Strategy.cpp
)

find_package(Boost COMPONENTS system REQUIRED)

target_link_libraries(m2core m2common sqlite3 ${Boost_LIBRARIES} pthread fmt)
