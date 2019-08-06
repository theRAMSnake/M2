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
ActionsView.cpp
strategy/StrategyView.cpp
WtConverters.cpp
CalendarView.cpp
TemplateBuilder.cpp
MateriaClient.cpp
MainView.cpp
JournalView.cpp
FilesView.cpp
SharedFilesView.cpp
strategy/GoalsComboBox.cpp
strategy/GoalViewCtrl.hpp
strategy/GraphView.cpp
strategy/GraphEditDialog.cpp
strategy/NodeEditDialog.cpp
RewardsSmartPage.cpp
materia/ZmqPbChannel.cpp
materia/StrategyModel.cpp
materia/CalendarModel.cpp
materia/InboxModel.cpp
materia/JournalModel.cpp
materia/FreeDataModel.cpp
dialog/CommonDialogManager.cpp
dialog/Dialog.cpp
)

find_package(Boost COMPONENTS system regex filesystem REQUIRED)
find_package(ZeroMQ REQUIRED)
find_package(Wt REQUIRED)

link_directories(/usr/lib64)
target_link_libraries(WebApp m2common ${Wt_LIBRARIES}  ${Boost_LIBRARIES} ${ZeroMQ_LIBRARIES} pthread wt wthttp) #

add_custom_command(
        TARGET WebApp POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_BINARY_DIR}/WebApp
                ../../bin/WebApp)
