cmake_minimum_required(VERSION 2.6)
project(m2legacy)

include_directories("../build/Common")
include_directories("~/oldm/evolution/src")
include_directories("..")

add_executable(m2legacy 
main.cpp 
~/oldm/evolution/src/materia/Environment.cpp
~/oldm/evolution/src/materia/EntityTree.cpp
~/oldm/evolution/src/materia/components/EntityTreeComponent.cpp
~/oldm/evolution/src/materia/components/TaskListComponent.cpp
~/oldm/evolution/src/materia/components/ProjectManagerComponent.cpp
~/oldm/evolution/src/materia/components/KnowledgeComponent.cpp
~/oldm/evolution/src/materia/components/ViewComponent.cpp
~/oldm/evolution/src/materia/MateriaDb.cpp
~/oldm/evolution/src/materia/EntityReference.cpp
~/oldm/evolution/src/materia/Entity.cpp
~/oldm/evolution/src/materia/EntityProperty.cpp
~/oldm/evolution/src/materia/Value.cpp
~/oldm/evolution/src/materia/ValueCast.cpp
~/oldm/evolution/src/materia/Container.cpp
~/oldm/evolution/src/utf8/String.cpp
~/oldm/evolution/src/sqlite/sqlite3.c
~/oldm/evolution/src/utf8/Exception.cpp
)

LIST(APPEND CMAKE_MODULE_PATH "..")

find_package(ZeroMQ REQUIRED)
find_package(Boost COMPONENTS system date_time REQUIRED)

target_link_libraries(m2legacy m2common ${Boost_LIBRARIES} ${ZeroMQ_LIBRARIES} pthread dl)

add_custom_command(
        TARGET m2legacy POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_BINARY_DIR}/m2legacy
                ../../bin/m2legacy)
