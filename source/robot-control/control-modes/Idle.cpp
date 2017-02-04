#include "Idle.hpp"

#include <QtCore/QDebug>

/*!
 * Constructor.
 */
Idle::Idle(FishBot* robot) : ControlMode(robot, ControlModeType::IDLE)
{

}

/*!
 * Destructor.
 */
Idle::~Idle()
{
    qDebug() << Q_FUNC_INFO << "Destroying the object";
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
