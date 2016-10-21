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

/*!
 * Informs on what kind of control targets this control mode generates.
 */
QList<ControlTargetType> Idle::supportedTargets()
{
    return QList<ControlTargetType>({ControlTargetType::SPEED});
}
