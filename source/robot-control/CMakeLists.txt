function(makeLink sourceFilePath destinationFilePath)
    if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${destinationFilePath})
        execute_process(COMMAND ln -s ${CMAKE_SOURCE_DIR}/${sourceFilePath} ${destinationFilePath}
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    endif()
endfunction()

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
makeLink(dependencies/fish-model/zones.hpp model/zones.hpp)
makeLink(dependencies/fish-model/zones.cpp model/zones.cpp)

set(srcs
    settings/RobotControlSettings.cpp
    settings/ExperimentControllerSettings.cpp
    settings/MapControllerSettings.cpp
    settings/InitiationControllerSettings.cpp
    FishBot.cpp
    ControlLoop.cpp
    control-modes/Idle.cpp
    control-modes/GoStraight.cpp
    control-modes/GoToPosition.cpp
    control-modes/ModelBased.cpp
    control-modes/Manual.cpp
    control-modes/ControlMode.cpp
    control-modes/Trajectory.cpp
    ControlModeStateMachine.cpp
    interfaces/DBusInterface.cpp
    interfaces/DashelInterface.cpp
    gui/RobotControlWidget.cpp
    gui/RobotsWidget.cpp
    gui/PotentialFieldWidget.cpp
    RobotsHandler.cpp
    experiment-controllers/ControlArea.cpp
    experiment-controllers/MapController.cpp
    experiment-controllers/ExperimentController.cpp
    experiment-controllers/ExperimentManager.cpp
    experiment-controllers/InitiationController.cpp
    model/model.cpp
    model/factory.cpp
    model/bm.cpp
    model/random.cpp
    model/zones.cpp
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
    RobotControlPointerTypes.hpp
    MotionPatternType.hpp
    ConnectionStatusType.hpp
    interfaces/Values.hpp
    control-modes/ControlModeType.hpp
    experiment-controllers/ExperimentControllerType.hpp
    experiment-controllers/ExperimentControllerFactory.hpp
)

qt5_wrap_ui(uis_hdrs gui/RobotControlWidget.ui gui/RobotsWidget.ui gui/PotentialFieldWidget.ui)
qt5_add_resources(resources icons.qrc)

include_directories(${CMAKE_SOURCE_DIR}/source/common)
add_library(robot-control SHARED ${srcs} ${hdrs} ${uis_hdrs}) #${resources})
target_link_libraries(robot-control common Qt5::Core Qt5::Widgets Qt5::DBus
                      Qt5::Xml ${Boost_LIBRARIES}
                      ${dashel_LIBRARIES} ${Aseba_LIBRARIES})

#install(TARGETS robot-control DESTINATION .)
#install(FILES
#         DESTINATION include/robot-control)
