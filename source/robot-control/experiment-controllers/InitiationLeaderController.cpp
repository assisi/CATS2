#include "InitiationLeaderController.hpp"

#include "FishBot.hpp"
#include "ControlArea.hpp"

#include <QtCore/QDebug>

/*!
 * Constructor.
 */
InitiationLeaderController::InitiationLeaderController(FishBot* robot,
                             ExperimentControllerSettingsPtr settings) :
    ExperimentController(robot, ExperimentControllerType::INITIATION_LEADER),
    m_settings(),
    m_state(UNDEFINED),
//    m_limitModelArea(false),
    m_inTargetRoom(false),
    m_targetAreaId(""),
    m_departureAreaId("")
{
    // NOTE : to get parameters specific for this controller we need to convert
    // the settings to the corresponding format
    InitiationLeaderControllerSettings* controllerSettings =
            dynamic_cast<InitiationLeaderControllerSettings*>(settings.data());
    if (controllerSettings != nullptr){
        // copy the parameters
        m_settings = controllerSettings->data();
    } else {
        qDebug() << "Could not set the controller's settings";
    }

    // load the control map
    // if there is no robot specifi control map then take form the default settings
    if (!readRobotControlMap())
        readControlMap(m_settings.defaultControlAreasFileName());
}

/*!
 * Returns the control values for given position.
 */
ExperimentController::ControlData InitiationLeaderController::step()
{
    ControlData controlData;
    // a check for the valid robot pointer
    if (m_robot) {
        // check where the robot and fish are
        updateAreasOccupation();

        if (needToChangeRoom()) {
            controlData = changeRoom();
        } else {
            // swimming with fish
            updateState(SWIMMING_WITH_FISH);
            controlData = stateControlData();
        }
    }
    return controlData;
}

/*!
 * Called when the controller is activated. Used to reset parameters.
 */
void InitiationLeaderController::start()
{
    updateState(SWIMMING_WITH_FISH);
    m_departureTimer.clear();
    m_fishFollowCheckTimer.clear();
    m_inTargetRoom = false;
    m_targetAreaId = "";
    m_departureAreaId = "";
}

/*!
 * Updates the current state.
 */
void InitiationLeaderController::updateState(State state)
{
    if (m_state != state) {
        qDebug() << QString("%1 state changed from %2 to %3")
                    .arg(m_robot->name())
                    .arg(stateToString(m_state))
                    .arg(stateToString(state));
        m_state = state;
        emit notifyControllerStatus(stateToString(m_state).toLower().replace("-", " "));
//        // a specific check for the case when the robot is switching to the
//        // model-based control mode
//        if (m_state == SWIMMING_WITH_FISH)
//            m_limitModelArea = true;
    }
}

/*!
 * Checks if the conditions are met to start the inititaion procedure.
 */
bool InitiationLeaderController::needToChangeRoom()
{
    // if the robot is changing the room right now
    if (changingRoom())
        return true; // continue

    // if the robot's position is a known room
    if (m_controlAreas.contains(m_robotAreaId) &&
            (m_controlAreas[m_robotAreaId]->type() == ControlAreaType::ROOM))
    {
        // if the majority of the fish is with the robot
        if (m_fishNumberByArea[m_robotAreaId] >= fishNumberInOtherRooms(m_robotAreaId)) {
            // if there is a preference for the room
            if (m_controlAreas.contains(m_preferedAreaId)) {
                // if the robot is in the prefered room
                if (m_robotAreaId == m_preferedAreaId) {
                    // the robot is already in the prefered room and most of fish
                    // are with it as well => do nothing
                    return false;
                } else {
                    // we are not in the prefered room => need to bring the fish
                    // there; wait until the departure timer's signal
                    return timeToDepart();
                }
            } else {
                // no preference for a room, so we can
                // wait until the departure timer's signal
                return timeToDepart();
            }
        } else {
            // so the robot is in the room where there is no or little fish
            // now if there is a preference for the room
            if (m_controlAreas.contains(m_preferedAreaId)) {
                // then let's go to catch up with the fish
                qDebug() << QString("%1 go to catch missing %2 fish, only %3 "
                                    "fish out of %4 are present")
                            .arg(m_robot->name())
                            .arg(fishNumberInOtherRooms(m_robotAreaId))
                            .arg(m_fishNumberByArea[m_robotAreaId])
                            .arg(RobotControlSettings::get().numberOfAnimals());
                return true;
//                // do nothing, we hope that the model will bring the robot to the fish
//                return false;
            } else {
                // when there is no preference for a room then do nothing,
                // wait until the fish arrive
                return false;
            }
        }
    } else {
        // wait until the robot is detected in a room
        return false;
    }
}

/*!
 * Runs the initiation state machine.
 */
ExperimentController::ControlData InitiationLeaderController::changeRoom()
{
    switch (m_state) {
    case SWIMMING_WITH_FISH:
        // find the room to go (target room) as another area of the type ROOM
        m_targetAreaId = "";
        m_departureAreaId = "";
        for (const auto& area : m_controlAreas.values()) {
            if ((area->id() != m_robotAreaId)
                    && (area->type() == ControlAreaType::ROOM)) {
                m_targetAreaId = area->id();
            }
        }
        // if managed to find where to go
        if (m_controlAreas.contains(m_targetAreaId)) {
            // save the original room
            m_departureAreaId = m_robotAreaId;
            // change the state to CHANGING_ROOM
            qDebug() << QString("%1 changes the room to %2 (%3)")
                        .arg(m_robot->name())
                        .arg(m_targetAreaId)
                        .arg(m_controlAreas[m_targetAreaId]->centroid().toString());
            updateState(CHANGING_ROOM);
            // we are not in the target room
            m_inTargetRoom = false;
            // if there is a preference for the room and we go to a non-prefered room
            // to pick up the fish there then there is no need to check if the fish follow
            if (m_controlAreas.contains(m_preferedAreaId) && (m_targetAreaId != m_preferedAreaId)) {
                m_fishFollowCheckTimer.clear();
            } else {
                // otherwise start the check-that-fish-follow timer
                m_fishFollowCheckTimer.reset();
            }
            // start the timer to switch from fish motion to PID
            m_fishToPIDTimer.reset();
        } else {
            // couldn't decide where to go, continue doing fish
        }
        break;
    case CHANGING_ROOM:
        // send the status on the timer
        if (m_fishFollowCheckTimer.isSet()) {
            QString stateString = stateToString(m_state).toLower().replace("-", " ");
            double timeLeftSec = m_settings.fishFollowCheckTimeOutSec() -
                    m_fishFollowCheckTimer.runTimeSec();
            emit notifyControllerStatus(QString("%1 (%2s)")
                                        .arg(stateString)
                                        .arg(timeLeftSec, 0, 'f', 1));
        } else {
            QString stateString = stateToString(m_state).toLower().replace("-", " ");
            emit notifyControllerStatus(QString("%1 (follow)").arg(stateString));
        }

        // let's check if we the initiation failed and we have to return
        // we check (1) if the timer to check that fish follow is ticking and
        // (2) if it expired
        if (m_fishFollowCheckTimer.isSet() &&
            m_fishFollowCheckTimer.isTimedOutSec(m_settings.fishFollowCheckTimeOutSec()))
        {
            if (fishFollow()) {
                // fish follow, continue transition
                m_fishFollowCheckTimer.clear();
                qDebug() << QString("Fish seem to follow %1")
                                .arg(m_robot->name());
            } else {
                // if fish not follow by some bad luck then the robot returns to
                // retry the initiation later
                qDebug() << QString("Fish don't follow %1, returning back to %2")
                                .arg(m_robot->name())
                                .arg(m_departureAreaId);
                updateState(GOING_BACK);
                // nothing more to do
                break;
            }
        }

        // now check if the robot arrived to the target room
        if (!m_inTargetRoom) {
            PositionMeters target = m_controlAreas[m_targetAreaId]->centroid();
            if (m_robot->state().position().isValid() &&
                m_robot->state().position().closeTo(target))
            {
                qDebug() << QString("%1 arrived to the room %2, wait the fish")
                            .arg(m_robot->name())
                            .arg(m_targetAreaId);
                // save the flag for now
                m_inTargetRoom = true;
            }
            // continue in CHANGING_ROOM state
        } else {
            // once in the target room the robot continues circling around the center
            // of the room until (1) it's sure that the fish follow it and (2) at
            // least one fish approaches it afterwards it passes in model based mode
            if (! m_fishFollowCheckTimer.isSet()) {
                // i.e. the fish were detected to follow, no check that someone
                // arrived close to the robot
                if (fishAroundRobot() > 0) {
                    // the robot switches to the swim-with-fish mode
                    qDebug() << QString("Fish in the proximity of %1, swim with them")
                                .arg(m_robot->name());
                    // switch to SWIMMING_WITH_FISH
                    updateState(SWIMMING_WITH_FISH);
                }
            }
        }
        break;
    case GOING_BACK:
        // if returned to the original room
        if (m_robot->state().position().closeTo(m_controlAreas[m_departureAreaId]->centroid()))
        {
            qDebug() << QString("%1 returned to the departure room %2")
                        .arg(m_robot->name())
                        .arg(m_departureAreaId);
            // switch to SWIMMING_WITH_FISH
            updateState(SWIMMING_WITH_FISH);
        }
        break;
    default:
        break;
    }

    return stateControlData();
}

/*!
 * Checks the departure time.
 */
bool InitiationLeaderController::timeToDepart()
{
    if (m_settings.departureOnTimeOut()) {
        // timer started
        if (m_departureTimer.isSet()) {
            // if time out?
            if (m_departureTimer.isTimedOutSec(m_settings.departureTimeOutSec())) {
                qDebug() << "Changing the room on timeout";
                // change the room
                m_departureTimer.clear();
                return true;
            } else {
                // need to wait more => do nothing
                return false;
            }
        } else {
            // start the timer
            m_departureTimer.reset();
            return false;
        }
    } else if (m_settings.departureWhenInGroup()) {
        PositionMeters robotPosition = m_robot->state().position();
        if (robotPosition.isValid()) {
            int fishCount = fishAroundRobot();
            // if enough fish are close to the robot
            if (fishCount >= m_settings.fishNumberAroundOnDeparture()) {
                qDebug() << QString("Detected %1 fish, changing the room")
                            .arg(fishCount);
                return true;
            } else {
//                qDebug() << QString("Detected %1 fish").arg(fishAroundRobot);
                return false;
            }
        } else
            return false;
    }
    return false;
}

/*!
 * Counts the number of fish around the robot.
 */
int InitiationLeaderController::fishAroundRobot()
{
    int fishCount = 0;
    PositionMeters robotPosition = m_robot->state().position();
    if (robotPosition.isValid()) {
        for (auto const& fishState : m_robot->fishStates()) {
            if (fishState.position().isValid() &&
                (fishState.position().distance2DTo(robotPosition) < m_settings.groupRadius()))
            {
                fishCount++;
            }
        }
    }
    return fishCount;
}

/*!
 * Checks that the fish follow.
 */
bool InitiationLeaderController::fishFollow()
{
    bool fishDoNotFollow = ((m_fishNumberByArea.contains(m_departureAreaId)) &&
                            (m_fishNumberByArea[m_departureAreaId] >
                             m_settings.maximalFishNumberAllowedToStay()));
    return !fishDoNotFollow;
}

/*!
 * Gets the control data that corresponds to the current state.
 */
ExperimentController::ControlData InitiationLeaderController::stateControlData()
{
    ControlData controlData;

    switch (m_state) {
    case SWIMMING_WITH_FISH:
        controlData.controlMode = ControlModeType::MODEL_BASED;
        controlData.motionPattern = MotionPatternType::FISH_MOTION;
//        // if we need to limit the model
//        if (m_limitModelArea &&
//                m_controlAreas.contains(m_robotAreaId) &&
//                m_controlAreas[m_robotAreaId]->type() == ControlAreaType::ROOM)
//        {
//            m_limitModelArea = false;
//            controlData.data =
//                QVariant::fromValue(m_controlAreas[m_robotAreaId]->annotatedPolygons());
//        }
        break;
    case CHANGING_ROOM:
        if (m_controlAreas.contains(m_targetAreaId)) {
            controlData.controlMode = ControlModeType::GO_TO_POSITION;

            // if the robot hasn't arrived to the target room
            if (!m_inTargetRoom) {
                // first we start in fish-motion and then switch to PID
                if (m_fishToPIDTimer.isSet()){
                    controlData.motionPattern = MotionPatternType::FISH_MOTION;
                    if (m_fishToPIDTimer.isTimedOutSec(1.)) // TODO : 1sec might be moved to settings
                        m_fishToPIDTimer.clear();
                } else
                    controlData.motionPattern = MotionPatternType::PID;
            } else {
                // upon arriving to the target room it switches to the fish-motion
                controlData.motionPattern = MotionPatternType::FISH_MOTION;
            }
            controlData.data =
                QVariant::fromValue(m_controlAreas[m_targetAreaId]->centroid());
        }
        break;
    case GOING_BACK:
        if (m_controlAreas.contains(m_departureAreaId)) {
            controlData.controlMode = ControlModeType::GO_TO_POSITION;
            controlData.motionPattern = MotionPatternType::PID;
            controlData.data =
                QVariant::fromValue(m_controlAreas[m_departureAreaId]->centroid());
        }
        break;
    default:
        break;
    }

    return controlData;
}

/*!
 * Converts the state to the string for the output.
 */
QString InitiationLeaderController::stateToString(State state)
{
    switch (state) {
    case SWIMMING_WITH_FISH:
        return "Swimming-with-fish";
        break;
    case CHANGING_ROOM:
        return "Changing-room";
        break;
    case GOING_BACK:
        return "Going-back";
        break;
    case UNDEFINED:
    default:
        return "Undefined";
        break;
    }
}
