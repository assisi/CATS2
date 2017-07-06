#include "CircularSetupLeaderController.hpp"

#include <QtCore/QDebug>

CircularSetupLeaderController::CircularSetupLeaderController(FishBot* robot,
                                                             ExperimentControllerSettingsPtr settings,
                                                             TurningDirection::Enum turningDirection):
    CircularSetupController(robot, settings,
                            turningDirection == TurningDirection::CLOCK_WISE ?
                                ExperimentControllerType::CIRCULAR_SETUP_LEADER_CW :
                                ExperimentControllerType::CIRCULAR_SETUP_LEADER_CCW)
{
    updateTargetTurningDirection(turningDirection);
}

/*!
 * Returns the control values for given position.
 */
ExperimentController::ControlData CircularSetupLeaderController::step()
{
    ControlData controlData;
    // a check for the valid robot pointer
    if (m_robot) {
        // find out where the fish go for the informative purposes, updated regularly
        if (m_fishTurningAngleUpdateTimer.isTimedOutSec(FishTurningDirectionUpdateTimeout)) {
            computeFishTurningDirection();
            m_fishTurningAngleUpdateTimer.reset();
            // notify
            emit notifyControllerStatus(QString("fish: %1; robot: %2")
                                        .arg(TurningDirection::toString(m_fishGroupTurningDirection))
                                        .arg(TurningDirection::toString(m_targetTurningDirection)));
            emit notifyTurningDirections(TurningDirection::toString(m_fishGroupTurningDirection),
                                         TurningDirection::toString(m_targetTurningDirection));
        }

        // set the control data
        controlData.controlMode = ControlModeType::GO_TO_POSITION;
        controlData.motionPattern = MotionPatternType::PID;
        // target position is based on the turning direction
        controlData.data =
            QVariant::fromValue(computeTargetPosition());
    }
    return controlData;
}
