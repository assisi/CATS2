#include "CircularSetupLeaderController.hpp"

#include <QtCore/QDebug>

CircularSetupLeaderController::CircularSetupLeaderController(FishBot* robot,
                                                             ExperimentControllerSettingsPtr settings,
                                                             ExperimentControllerType::Enum type):
    CircularSetupController(robot, type, settings)
{
    // set the turning direction from the controller type
    TurningDirection::Enum direction = TurningDirection::UNDEFINED;
    if (type == ExperimentControllerType::CIRCULAR_SETUP_LEADER_CW)
        direction = TurningDirection::CLOCK_WISE;
    else if (type == ExperimentControllerType::CIRCULAR_SETUP_LEADER_CCW)
        direction = TurningDirection::COUNTER_CLOCK_WISE;
    updateTargetTurningDirection(direction);
}

/*!
 * Returns the control values for given position.
 */
ExperimentController::ControlData CircularSetupLeaderController::step()
{
    ControlData controlData;
    // a check for the valid robot pointer
    if (m_robot) {
        // find out where the fish go for the informative purposes
        computeFishTurningDirection();
        // notify
        emit notifyControllerStatus(QString("fish: %1; robot: %2")
                                    .arg(TurningDirection::toString(m_fishGroupTurningDirection))
                                    .arg(TurningDirection::toString(m_targetTurningDirection)));
        // set the control data
        controlData.controlMode = ControlModeType::GO_TO_POSITION;
        controlData.motionPattern = MotionPatternType::PID;
        // target position is based on the turning direction
        controlData.data =
            QVariant::fromValue(computeTargetPosition());
    }
    return controlData;
}

///*!
// * Sets the direction to keep, expected values are "CW" or "CCW".
// */
//void CircularSetupLeaderController::setTurningDirection(QString directionString)
//{
//    TurningDirection::Enum direction = TurningDirection::fromString(directionString);
//    if (direction != TurningDirection::UNDEFINED) {
//        updateTargetTurningDirection(direction);
//    } else {
//        qDebug() << QString("Trying to set an invalid turning direction %1")
//                    .arg(directionString);
//    }
//}
