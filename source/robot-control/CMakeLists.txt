function(makeLink sourceFilePath destinationFilePath)
    if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${destinationFilePath})
        execute_process(COMMAND ln -s ${CMAKE_SOURCE_DIR}/${sourceFilePath} ${destinationFilePath}
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    endif()
endfunction()

# Add the qt-aseba interface files to the project, before tried with add_custom_command but it never worked
makeLink(dependencies/aseba/examples/clients/cpp-qt-dbus/dbusinterface.h dbusinterface.h)
makeLink(dependencies/aseba/examples/clients/cpp-qt-dbus/dbusinterface.cpp dbusinterface.cpp)

# Add fish-model files
if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/model)
    execute_process(COMMAND mkdir model WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endif()
makeLink(dependencies/fish-model/bm.cpp model/bm.cpp)
makeLink(dependencies/fish-model/bm.hpp model/bm.hpp)
makeLink(dependencies/fish-model/factory.cpp model/factory.cpp)
makeLink(dependencies/fish-model/factory.hpp model/factory.hpp)
makeLink(dependencies/fish-model/model.cpp model/model.cpp)
makeLink(dependencies/fish-model/model.hpp model/model.hpp)
makeLink(dependencies/fish-model/random.cpp model/random.cpp)
makeLink(dependencies/fish-model/random.h model/random.h)

set(srcs
    settings/RobotControlSettings.cpp
    FishBot.cpp
    ControlLoop.cpp
    control-modes/Idle.cpp
    control-modes/GoStraight.cpp
    control-modes/GoToPosition.cpp
    control-modes/ModelBased.cpp
    control-modes/Manual.cpp
    control-modes/ControlMode.cpp
    ControlModeStateMachine.cpp
    dbusinterface.cpp
    gui/RobotControlWidget.cpp
    gui/RobotsWidget.cpp
    gui/PotentialFieldWidget.cpp
    RobotsHandler.cpp
    experiment-controllers/ControlArea.cpp
    experiment-controllers/MapController.cpp
    experiment-controllers/ExperimentController.cpp
    experiment-controllers/ExperimentManager.cpp
    model/model.cpp
    model/factory.cpp
    model/bm.cpp
    model/random.cpp
    navigation/Navigation.cpp
    navigation/DijkstraPathPlanner.cpp
    navigation/PathPlanner.cpp
    navigation/ObstacleAvoidance.cpp
    navigation/PotentialField.cpp
    navigation/GridBasedMethod.cpp
    SetupMap.cpp
)

set(hdrs
    control-modes/ControlTarget.hpp
    dbusinterface.h
    RobotControlPointerTypes.hpp
    MotionPatternType.hpp
    control-modes/ControlModeType.hpp
    experiment-controllers/ExperimentControllerType.hpp
)

qt5_wrap_ui(uis_hdrs gui/RobotControlWidget.ui gui/RobotsWidget.ui gui/PotentialFieldWidget.ui)
#qt5_add_resources(resources icons.qrc)

include_directories(${CMAKE_SOURCE_DIR}/source/common)
add_library(robot-control SHARED ${srcs} ${hdrs} ${uis_hdrs}) #${resources})
target_link_libraries(robot-control common Qt5::Core Qt5::Widgets Qt5::DBus ${Boost_LIBRARIES})



