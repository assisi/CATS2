#include "CircularSetupFollowerController.hpp"

CircularSetupFollowerController::CircularSetupFollowerController(FishBot* robot,
                                                                 ExperimentControllerSettingsPtr settings):
    CircularSetupController(robot, settings)
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
        bool directionChanged = updateTargetTurningDirection(m_fishGroupTurningDirection);
        // notify
        emit notifyControllerStatus(QString("fish: %1; robot follows")
                                    .arg(TurningDirection::toString(m_fishGroupTurningDirection)));
        // set the control data
        if (directionChanged) {
            // when the robot changes swimming direction to the oposite we
            // set the fish motion to make the robot turn sharper
            controlData.controlMode = ControlModeType::GO_TO_POSITION;
            controlData.motionPattern = MotionPatternType::FISH_MOTION;
        } else {
            controlData.controlMode = ControlModeType::GO_TO_POSITION;
            controlData.motionPattern = MotionPatternType::PID;
        }
        // target position is based on the turning direction
        controlData.data =
            QVariant::fromValue(computeTargetPosition());
    }
    return controlData;
}

