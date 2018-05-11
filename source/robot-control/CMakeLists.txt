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
makeLink(dependencies/fish-model/zones.cpp model/zones.cpp)
makeLink(dependencies/fish-model/zones.hpp model/zones.hpp)
makeLink(dependencies/fish-model/random.cpp model/random.cpp)
makeLink(dependencies/fish-model/random.h model/random.h)
makeLink(dependencies/fish-model/zones.hpp model/zones.hpp)
makeLink(dependencies/fish-model/zones.cpp model/zones.cpp)
makeLink(dependencies/fish-model/bmWithWalls.hpp model/bmWithWalls.hpp)
makeLink(dependencies/fish-model/bmWithWalls.cpp model/bmWithWalls.cpp)

set(srcs
    settings/RobotControlSettings.cpp
    settings/ExperimentControllerSettings.cpp
    settings/MapControllerSettings.cpp
    settings/InitiationLeaderControllerSettings.cpp
    settings/InitiationLureControllerSettings.cpp
    settings/DominatingSetControllerSettings.cpp
    settings/CircularSetupControllerSettings.cpp
    FishBot.cpp
    ControlLoop.cpp
    control-modes/Idle.cpp
    control-modes/GoStraight.cpp
    control-modes/GoToPosition.cpp
    control-modes/GenericFishModel.cpp
    control-modes/ModelBased.cpp
    control-modes/FishModelWithWalls.cpp
    control-modes/ZoneBasedFishModel.cpp
    control-modes/Manual.cpp
    control-modes/ControlMode.cpp
    control-modes/Trajectory.cpp
    control-modes/FollowGroup.cpp
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
    experiment-controllers/InitiationLeaderController.cpp
    experiment-controllers/InitiationLureController.cpp
    experiment-controllers/DominatingSetController.cpp
    experiment-controllers/CircularSetupController.cpp
    experiment-controllers/CircularSetupFollowerController.cpp
	experiment-controllers/CircularSetupFollowerWithModelController.cpp
    experiment-controllers/CircularSetupLeaderController.cpp
	experiment-controllers/CircularSetupLeaderWithModelController.cpp
    model/model.cpp
    model/zones.cpp
    model/factory.cpp
    model/bm.cpp
    model/random.cpp
    model/zones.cpp
    model/bmWithWalls.cpp
    navigation/Navigation.cpp
    navigation/DijkstraPathPlanner.cpp
    navigation/PathPlanner.cpp
    navigation/ObstacleAvoidance.cpp
    navigation/PotentialField.cpp
    navigation/GridBasedMethod.cpp
    SetupMap.cpp
    statistics/StatisticsSubscriber.cpp
    statistics/StatisticsPublisher.cpp
)

set(hdrs
    control-modes/ControlTarget.hpp
    RobotControlPointerTypes.hpp
    MotionPatternType.hpp
    ConnectionStatusType.hpp
    interfaces/Values.hpp
    control-modes/ControlModeType.hpp
    control-modes/ModelParameters.hpp
    experiment-controllers/ExperimentControllerType.hpp
    experiment-controllers/ExperimentControllerFactory.hpp
)

qt5_wrap_ui(uis_hdrs gui/RobotControlWidget.ui gui/RobotsWidget.ui gui/PotentialFieldWidget.ui)
qt5_add_resources(resources icons.qrc)

include_directories(${CMAKE_SOURCE_DIR}/source/common)
include_directories(${CMAKE_SOURCE_DIR}/source/inter-species)
add_library(robot-control SHARED ${srcs} ${hdrs} ${uis_hdrs}) #${resources})
target_link_libraries(robot-control common inter-species Qt5::Core Qt5::Widgets Qt5::DBus
                      Qt5::Xml ${Boost_LIBRARIES}
                      ${dashel_LIBRARIES} ${Aseba_LIBRARIES})
                      #-fsanitize=address)

#install(TARGETS robot-control DESTINATION .)
#install(FILES
#         DESTINATION include/robot-control)
