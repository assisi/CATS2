#include "GoStraight.hpp"
#include "ControlModeType.hpp"
#include "settings/RobotControlSettings.hpp"

#include <QtCore/QDebug>

/*!
 * Constructor.
 */
GoStraight::GoStraight(FishBot* robot) :
    ControlMode(robot, ControlModeType::GO_STRAIGHT),
    m_linearSpeed(RobotControlSettings::get().defaultLinearSpeedCmSec())
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
    return ControlTargetPtr(new TargetSpeed(m_linearSpeed, m_linearSpeed));
}

/*!
 * Informs on what kind of control targets this control mode generates.
 */
QList<ControlTargetType> GoStraight::supportedTargets()
{
    return QList<ControlTargetType>({ControlTargetType::SPEED});
}
