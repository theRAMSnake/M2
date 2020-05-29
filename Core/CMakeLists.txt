cmake_minimum_required(VERSION 2.6)
project(m2core)

LIST(APPEND CMAKE_MODULE_PATH "..")

include_directories("..")
include_directories("/usr/include/python3.8")

add_library(m2core 
private/Core.cpp
private/Core3.cpp
private/TraitSystem.cpp
private/CommandParser.cpp
private/Expressions.cpp
private/Commands.cpp
private/ObjectManager.cpp
private/Impl.cpp
private/Database.cpp
private/Calendar.cpp
private/Inbox.cpp
private/Journal.cpp
private/Strategy_v2.cpp
private/Logger.cpp
private/JsonSerializer.cpp
private/Backuper.cpp
private/StrategyGraph.cpp
private/NodeAttributes.cpp
private/FreeData.cpp
private/FreeDataInterpreter.cpp
private/Finance.cpp
private/Reward.cpp
private/Challenge.cpp
private/ScriptRunner.cpp
)

find_package(Boost COMPONENTS system python REQUIRED)

target_link_libraries(m2core sqlite3 ${Boost_LIBRARIES} pthread python3.8 fmt)
