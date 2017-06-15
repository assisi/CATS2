#include "CircularSetupFollowerController.hpp"

CircularSetupFollowerController::CircularSetupFollowerController(FishBot* robot,
                                                                 ExperimentControllerSettingsPtr settings):
    CircularSetupController(robot, settings)
{

}

/*!
 * Called when the controller is activated. Used to reset parameters.
 */
void CircularSetupFollowerController::start()
{
    // start the timer
    m_fishTurningAngleUpdateTimer.reset();
}

/*!
 * Returns the control values for given position.
 */
ExperimentController::ControlData CircularSetupFollowerController::step()
{
    ControlData controlData;
    // a check for the valid robot pointer
    if (m_robot) {
        // find out where the fish go, updated regularly
        if (m_fishTurningAngleUpdateTimer.isTimedOutSec(FishTurningDirectionUpdateTimeout)) {
            computeFishTurningDirection();
            m_fishTurningAngleUpdateTimer.reset();
        }
        // and do the same
        updateTargetTurningDirection(m_fishGroupTurningDirection);
        // notify
        emit notifyControllerStatus(QString("fish: %1; robot follows")
                                    .arg(TurningDirection::toString(m_fishGroupTurningDirection)));
        // set the control data
        controlData.controlMode = ControlModeType::GO_TO_POSITION;
        controlData.motionPattern = MotionPatternType::PID;
        // target position is based on the turning direction
        controlData.data =
            QVariant::fromValue(computeTargetPosition());
    }
    return controlData;
}

