#include "CircularSetupFollowerController.hpp"

CircularSetupFollowerController::CircularSetupFollowerController(FishBot* robot,
                                                                 ExperimentControllerSettingsPtr settings):
    CircularSetupController(robot,
                            ExperimentControllerType::CIRCULAR_SETUP_FOLLOWER,
                            settings)
{

}

/*!
 * Called when the controller is activated. Used to reset parameters.
 */
void CircularSetupFollowerController::start()
{

}

/*!
 * Returns the control values for given position.
 */
ExperimentController::ControlData CircularSetupFollowerController::step()
{
    ControlData controlData;
    // a check for the valid robot pointer
    if (m_robot) {
        // find out where the fish go
        computeFishTurningDirection();
        // and do the same
        m_targetTurningDirection = m_fishGroupTurningDirection;
        // notify
        emit notifyControllerStatus(QString("fish: %1; robot follows")
                                    .arg(TurningDirection::toString(m_fishGroupTurningDirection)));
        // set the control data
        controlData.controlMode = ControlModeType::GO_TO_POSITION;
        controlData.motionPattern = MotionPatternType::PID;
        controlData.data =
            QVariant::fromValue(computeTargetPosition());
    }
    return controlData;
}

