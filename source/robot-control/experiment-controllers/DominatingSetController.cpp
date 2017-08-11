#include "DominatingSetController.hpp"

#include "FishBot.hpp"
#include "ControlArea.hpp"

#include <QtCore/QDebug>

DominatingSetController::DominatingSetController(FishBot* robot,
                                                ExperimentControllerSettingsPtr settings) :
    ExperimentController(robot, ExperimentControllerType::DOMINATING_SET),
    m_settings(),
    m_robotAreaDefined(false),
    m_robotArea(nullptr),
    m_fishRoomsOccupationCounter({0,0}),
    m_robotRoomsOccupationCounter({0,0}),
    m_fishRoomId(-1),
    m_robotRoomId(-1)
{
    // NOTE : to get parameters specific for this controller we need to convert
    // the settings to the corresponding format
    DominatingSetControllerSettings* controllerSettings =
            dynamic_cast<DominatingSetControllerSettings*>(settings.data());
    if (controllerSettings != nullptr){
        // copy the parameters
        m_settings = controllerSettings->data();
    } else {
        qDebug() << "Could not set the controller's settings";
    }

    // load the control map
    readControlMap(m_settings.controlAreasFileName());
}

/*!
 * Called when the controller is activated. Used to reset parameters.
 */
void DominatingSetController::start()
{
    m_robotAreaDefined = false;
    // prepare the timer
    m_roomsStatisticsUpdateTimer.clear();
    // activates the path planner
    m_robot->setUsePathPlanning(true);
}

/*!
 * Returns the control values for given position.
 */
ExperimentController::ControlData DominatingSetController::step()
{
    ControlData controlData;
    // a check for the valid robot pointer
    if (m_robot) {
        updateAreasOccupation();
        if (!m_robotAreaDefined) {
            // if the robot's zone is defined then set it in the model
            // NOTE : here we assume that all the robots are placed in separated
            // areas and are detectable there
            if (m_controlAreas.contains(m_robotAreaId)) {
                m_robotAreaDefined = true;
                m_robotArea = m_controlAreas[m_robotAreaId];
                controlData.controlMode = ControlModeType::MODEL_BASED;
                controlData.motionPattern = MotionPatternType::PID;
                controlData.data = QVariant::fromValue(m_controlAreas[m_robotAreaId]->annotatedPolygons());
                // start the timer
                m_roomsStatisticsUpdateTimer.reset();
            } else {
                // stop until it's detected
                controlData.controlMode = ControlModeType::IDLE;
            }
        } else {
            // do model motion
            controlData.controlMode = ControlModeType::MODEL_BASED;
            controlData.motionPattern = MotionPatternType::PID;
//            ModelParameters parameters;
//            parameters.ignoreRobot = true;
//            controlData.data = QVariant::fromValue(parameters);
            // compute the experiment specific statistics
            updateRoomsStatistics();
        }
    }
    return controlData;
}

/*!
 * Called when the controller is disactivated.
 */
void DominatingSetController::finish()
{
    // dis-activates the path planner
    m_robot->setUsePathPlanning(false);
}

/*!
 * Computes the occupation of the rooms (left/right, up/down) by fish and
 * robots. Emits a signal notifing about these statistics values.
 */
void DominatingSetController::updateRoomsStatistics()
{
    if (m_robotArea.isNull())
        return;
    
    if (m_roomsStatisticsUpdateTimer.isSet() &&
            m_roomsStatisticsUpdateTimer.isTimedOutSec(RoomsStatisticsUpdateTimeout)) 
    {
        // stores the amount of fish in the left and right room of the section
        int fishByRoom[2] = {0,0};
        // get the fish states
        QList<StateWorld> fishStates = m_robot->fishStates();
        if (fishStates.size() > 0) {
            // for every fish check if it's in the robot's are and if it's the 
            // case then in which room it is
            for (const auto& fishState : fishStates) {
                int fishRoomIndex = m_robotArea->polygonIndexOf(fishState.position());
                if ((fishRoomIndex >= 0) && (fishRoomIndex < 2)) {
                    fishByRoom[fishRoomIndex]++;
                }
            }        
        }
        // update the fish rooms occupation statistics
        if (fishByRoom[0] > fishByRoom[1]) {
            m_fishRoomId = 0;
            m_fishRoomsOccupationCounter[0]++;
        }
        else if (fishByRoom[0] < fishByRoom[1]) {
            m_fishRoomId = 1;
            m_fishRoomsOccupationCounter[1]++;
        }
        // updates the robot room occupation statistics
        int robotRoomIndex = m_robotArea->polygonIndexOf(m_robot->state().position());
        if ((robotRoomIndex >= 0) && (robotRoomIndex < 2)) {
            m_robotRoomId = robotRoomIndex;
            m_robotRoomsOccupationCounter[robotRoomIndex] ++;
        }

        // notify
        emit notifyControllerStatus(QString("fish: %1; robot: %2")
                                    .arg(roomIdToString(m_fishRoomId))
                                    .arg(roomIdToString(m_robotRoomId)));
        if ((m_robotRoomId >=0) && (m_fishRoomId >= 0)) {
            emit notifyRoomsOccupation(m_robotArea->id(),
                                       roomIdToString(m_fishRoomId),
                                       roomIdToString(m_robotRoomId));
//            qDebug() << QString("Room occupation for the area %1 is: fish are "
//                                "on the %2, robot is on the %3")
//                        .arg(m_robotArea->id())
//                        .arg(roomIdToString(m_fishRoomId))
//                        .arg(roomIdToString(m_robotRoomId));
        } else {
            if (m_robotRoomId < 0)
                qDebug() << QString("Robot room is undefined on the area %1")
                            .arg(m_robotArea->id());
            if (m_fishRoomId < 0)
                qDebug() << QString("Fish room is undefined on the area %1")
                            .arg(m_robotArea->id());
        }
        // restart the timer
        m_roomsStatisticsUpdateTimer.reset();
    }
}

/*!
 * Converts the room id to the string: 0 -> lelf, 1 -> right, otherwise
 * undefined.
 */
QString DominatingSetController::roomIdToString(int id) const
{
    QString result = "undefined";
    switch (id) {
    case 0:
        result = "left";
        break;
    case 1:
        result = "right";
        break;
    default:
        break;
    }
    return result;
}
