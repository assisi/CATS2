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
    // clear the timer
    m_changingDirectionTimer.clear();
    // call the CircularSetupController's start method to reset statistics
    CircularSetupController::start();
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
        bool directionChanged = updateTargetTurningDirection(m_fishGroupTurningDirection);
        // notify
        emit notifyControllerStatus(QString("fish: %1; robot follows")
                                    .arg(TurningDirection::toString(m_fishGroupTurningDirection)));

        // set the turning timer if the direction is changed
        if (directionChanged)
            m_changingDirectionTimer.reset();

        // set the control data
        if (m_changingDirectionTimer.isSet()) {
            // when the robot changes swimming direction to the oposite we
            // set the fish motion to make the robot turn sharper and give the
            // robot some time to turn
            controlData.controlMode = ControlModeType::GO_TO_POSITION;
            controlData.motionPattern = MotionPatternType::FISH_MOTION;
            // stop the timer once we consider that robot had enough time to turn
            if (m_changingDirectionTimer.isTimedOutSec(RobotChangingDirectionTimeout))
                m_changingDirectionTimer.clear();
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

