#include "InitiationLureController.hpp"

#include "FishBot.hpp"
#include "ControlArea.hpp"
#include "control-modes/ModelBased.hpp"

#include <QtCore/QDebug>

/*!
 * Constructor.
 */
InitiationLureController::InitiationLureController(FishBot* robot,
                             ExperimentControllerSettingsPtr settings) :
    ExperimentController(robot, ExperimentControllerType::INITIATION_LURE),
    m_settings(),
    m_state(UNDEFINED),
    m_limitModelArea(false),
    m_inTargetRoom(false),
    m_targetAreaId(""),
    m_departureAreaId("")
{
    // NOTE : to get parameters specific for this controller we need to convert
    // the settings to the corresponding format
    InitiationLureControllerSettings* controllerSettings =
            dynamic_cast<InitiationLureControllerSettings*>(settings.data());
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
 * Returns the control values for given position.
 */
ExperimentController::ControlData InitiationLureController::step()
{
    ControlData controlData;

    // a check for the valid robot pointer
    if (m_robot) {
        // check where the robot and fish are
        updateAreasOccupation();

        if (needToChangeRoom()) {
            controlData = goToPreferredRoom();
        } else {
            // swimming with fish
            updateState(SWIMMING_IN_ROOM);
            controlData = stateControlData();
        }
    }

    return controlData;
}

/*!
 * Called when the controller is activated. Used to reset parameters.
 */
void InitiationLureController::start()
{
    updateState(SWIMMING_IN_ROOM);
    m_inTargetRoom = false;
    m_targetAreaId = "";
    m_departureAreaId = "";
}

/*!
 * Updates the current state.
 */
void InitiationLureController::updateState(State state)
{
    if (m_state != state) {
        qDebug() << QString("%1 state changed from %2 to %3")
                    .arg(m_robot->name())
                    .arg(stateToString(m_state))
                    .arg(stateToString(state));
        m_state = state;
        emit notifyControllerStatus(stateToString(m_state).toLower().replace("-", " "));
        // a specific check for the case when the robot is switching to the
        // model-based control mode
        if (m_state == SWIMMING_IN_ROOM)
            m_limitModelArea = true;
    }
}

/*!
 * Checks if the conditions are met to start the inititaion procedure.
 */
bool InitiationLureController::needToChangeRoom()
{
    // if the robot is changing the room right now
    if (changingRoom())
        return true; // continue

    // if the robot's position is a known room
    if (m_controlAreas.contains(m_robotAreaId))
    {
        // if there is a preference for the room
        if (m_controlAreas.contains(m_preferedAreaId)) {
            // if the robot is in the prefered room
            if (m_robotAreaId == m_preferedAreaId) {
                // the robot is already in the prefered room => do nothing
                return false;
            } else {
                // we are not in the prefered room => need to go there
                return true;
            }
        } else {
            // no preference for a room, so we stay in the current room
            return false;
        }
    } else {
        // wait until the robot is detected in a room
        return false;
    }
}

/*!
 * Brings the robot to the preferred room.
 */
ExperimentController::ControlData InitiationLureController::goToPreferredRoom()
{
    switch (m_state) {
    case SWIMMING_IN_ROOM:
        // find the room to go (target room) as another area of the type ROOM
        m_targetAreaId = m_preferedAreaId;
        m_departureAreaId = m_robotAreaId;
        // if knows where to go
        if (m_controlAreas.contains(m_targetAreaId)) {
            // change the state to CHANGING_ROOM
            qDebug() << QString("%1 changes the room to %2 (%3)")
                        .arg(m_robot->name())
                        .arg(m_targetAreaId)
                        .arg(m_controlAreas[m_targetAreaId]->centroid().toString());
            updateState(CHANGING_ROOM);
            // we are not in the target room
            m_inTargetRoom = false;
        } else {
            // couldn't decide where to go, continue doing fish
        }
        break;
    case CHANGING_ROOM:
        // if we just arrived to the target room then we swith to the model-based mode
        if (!m_inTargetRoom &&
                m_robot->state().position().isValid() &&
                m_robot->state().position()
                .closeTo(m_controlAreas[m_targetAreaId]->centroid()))
        {
            m_inTargetRoom = true;
            qDebug() << QString("%1 arrived to the target room").arg(m_robot->name());
            // switch to SWIMMING_WITH_FISH
            updateState(SWIMMING_IN_ROOM);
        }
        break;
    default:
        break;
    }
    return stateControlData();
}

/*!
 * Gets the control data that corresponds to the current state.
 */
ExperimentController::ControlData InitiationLureController::stateControlData()
{
    ControlData controlData;

    switch (m_state) {
    case SWIMMING_IN_ROOM:
        controlData.controlMode = ControlModeType::MODEL_BASED;
        controlData.motionPattern = MotionPatternType::FISH_MOTION;
        // if we need to limit the model
        if (m_limitModelArea &&
                m_controlAreas.contains(m_robotAreaId) &&
                m_controlAreas[m_robotAreaId]->type() == ControlAreaType::ROOM)
        {
            // ensure that it's sent once
            m_limitModelArea = false;
            controlData.data =
                QVariant::fromValue(m_controlAreas[m_robotAreaId]->annotatedPolygons());
        } else {
            ModelParameters parameters;
            parameters.ignoreFish = true;
            controlData.data = QVariant::fromValue(parameters);
        }
        break;
    case CHANGING_ROOM:
        if (m_controlAreas.contains(m_targetAreaId)) {
            controlData.controlMode = ControlModeType::GO_TO_POSITION;
            controlData.motionPattern = MotionPatternType::PID;
            controlData.data =
                QVariant::fromValue(m_controlAreas[m_targetAreaId]->centroid());
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
QString InitiationLureController::stateToString(State state)
{
    switch (state) {
    case SWIMMING_IN_ROOM:
        return "Swimming-in-room";
        break;
    case CHANGING_ROOM:
        return "Changing-room";
        break;
    case UNDEFINED:
    default:
        return "Undefined";
        break;
    }
}
