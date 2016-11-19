# Add the qt-aseba interface files to the project, before tried with add_castom_command but it never worked
if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/dbusinterface.h)
    execute_process(COMMAND ln -s ${CMAKE_SOURCE_DIR}/dependencies/aseba/examples/clients/cpp-qt-dbus/dbusinterface.h dbusinterface.h
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endif()
if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/dbusinterface.cpp)
    execute_process(COMMAND ln -s ${CMAKE_SOURCE_DIR}/dependencies/aseba/examples/clients/cpp-qt-dbus/dbusinterface.cpp dbusinterface.cpp
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endif()

set(srcs
    settings/RobotControlSettings.cpp
    FishBot.cpp
    ControlLoop.cpp
    control-modes/Idle.cpp
    control-modes/GoStraight.cpp
    control-modes/GoToPosition.cpp
    control-modes/Manual.cpp
    control-modes/ControlMode.cpp
    ControlModeStateMachine.cpp
    dbusinterface.cpp
    gui/RobotControlWidget.cpp
    gui/RobotsWidget.cpp
    RobotsHandler.cpp
    Navigation.cpp
    control-maps/ControlArea.cpp
    control-maps/ControlMap.cpp
)

set(hdrs
    control-modes/ControlTarget.hpp
    dbusinterface.h
    RobotControlPointerTypes.hpp
    MotionPatternType.hpp
    control-modes/ControlModeType.hpp
)

qt5_wrap_ui(uis_hdrs gui/RobotControlWidget.ui gui/RobotsWidget.ui)
#qt5_add_resources(resources icons.qrc)

include_directories(${CMAKE_SOURCE_DIR}/source/common)
add_library(robot-control SHARED ${srcs} ${hdrs} ${uis_hdrs}) #${resources})
target_link_libraries(robot-control common Qt5::Core Qt5::Widgets Qt5::DBus)



