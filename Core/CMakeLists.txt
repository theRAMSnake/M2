cmake_minimum_required(VERSION 3.6)
project(m3core)

find_package(PythonLibs REQUIRED)
LIST(APPEND CMAKE_MODULE_PATH "..")

include_directories("..")
include_directories(${PYTHON_INCLUDE_DIRS})

add_library(m3core
private/Core3.cpp
private/TypeSystem.cpp
private/CommandParser.cpp
private/Expressions2.cpp
private/Commands.cpp
private/ObjectManager.cpp
private/Impl.cpp
private/Database.cpp
private/Logger.cpp
private/JsonSerializer.cpp
private/JsonRestorationProvider.cpp
private/EmptyValueProvider.cpp
private/Object.cpp
private/types/SimpleList.cpp
private/types/Variable.cpp
private/Type.cpp
private/Connections.cpp
private/ScriptRunner.cpp
private/rng.cpp
private/subsystems/Finance.cpp
private/subsystems/User.cpp
private/subsystems/Common.cpp
private/subsystems/Journal.cpp
private/subsystems/Calendar.cpp
private/subsystems/Reward.cpp
private/subsystems/Strategy.cpp
private/subsystems/Ideas.cpp
)

find_package(Boost COMPONENTS system REQUIRED)

target_link_libraries(m3core m3common sqlite3 ${Boost_LIBRARIES} pthread fmt profiler ${PYTHON_LIBRARIES})
