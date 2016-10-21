#include "GoStraight.hpp"
#include "ControlModeType.hpp"

/*!
 * Constructor.
 */
GoStraight::GoStraight(FishBot* robot) : ControlMode(robot, ControlModeType::GO_STRAIGHT)
{

}

/*!
 * The step of the control mode, generates the zero speed control target.
 */
ControlTargetPtr GoStraight::step()
{
    return ControlTargetPtr(new TargetSpeed(Speed, Speed));
}

/*!
 * Informs on what kind of control targets this control mode generates.
 */
QList<ControlTargetType> GoStraight::supportedTargets()
{
    return QList<ControlTargetType>({ControlTargetType::SPEED});
}
