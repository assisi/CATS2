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
