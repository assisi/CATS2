#include "Idle.hpp"

/*!
 * Constructor.
 */
Idle::Idle(FishBot* robot) : ControlMode(robot, ControlModeType::IDLE)
{
}

/*!
 * The step of the control mode, generates the zero speed control target.
 */
ControlTargetPtr Idle::step()
{
    return ControlTargetPtr(new TargetSpeed(0.0, 0.0));
}
