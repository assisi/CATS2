#include "CircularSetupLeaderController.hpp"

#include <QtCore/QDebug>

CircularSetupLeaderController::CircularSetupLeaderController(FishBot* robot,
                                                             ExperimentControllerSettingsPtr settings):
    CircularSetupController(robot,
                            ExperimentControllerType::CIRCULAR_SETUP_LEADER,
                            settings)
{

}

/*!
 * Returns the control values for given position.
 */
ExperimentController::ControlData CircularSetupLeaderController::step()
{
    ControlData controlData;
    // a check for the valid robot pointer
    if (m_robot) {
        // find out where the fish go
        computeFishTurningDirection();
        // notify
        emit notifyControllerStatus(QString("fish: %1; robot: %2")
                                    .arg(TurningDirection::toString(m_fishGroupTurningDirection))
                                    .arg(TurningDirection::toString(m_targetTurningDirection)));
        // set the control data
        controlData.controlMode = ControlModeType::GO_TO_POSITION;
        controlData.motionPattern = MotionPatternType::PID;
        controlData.data =
            QVariant::fromValue(computeTargetPosition());
    }
    return controlData;
}

/*!
 * Sets the direction to keep, expected values are "CW" or "CCW".
 */
void CircularSetupLeaderController::setTurningDirection(QString directionString)
{
    TurningDirection::Enum direction = TurningDirection::fromString(directionString);
    if (direction != TurningDirection::UNDEFINED) {
        updateTargetTurningDirection(direction);
    } else {
        qDebug() << QString("Trying to set an invalid turning direction %1")
                    .arg(directionString);
    }
}
