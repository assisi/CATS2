#include "GoStraight.hpp"
#include "ControlModeType.hpp"
#include "settings/RobotControlSettings.hpp"

#include <QtCore/QDebug>

/*!
 * Constructor.
 */
GoStraight::GoStraight(FishBot* robot) : ControlMode(robot, ControlModeType::GO_STRAIGHT)
{

}

/*!
 * Destructor.
 */
GoStraight::~GoStraight()
{
    qDebug() << Q_FUNC_INFO << "Destroying the object";
}

/*!
 * The step of the control mode, generates the zero speed control target.
 */
ControlTargetPtr GoStraight::step()
{
    int linearSpeed = RobotControlSettings::get().defaultLinearSpeedCmSec();
    return ControlTargetPtr(new TargetSpeed(linearSpeed, linearSpeed));
}

/*!
 * Informs on what kind of control targets this control mode generates.
 */
QList<ControlTargetType> GoStraight::supportedTargets()
{
    return QList<ControlTargetType>({ControlTargetType::SPEED});
}
