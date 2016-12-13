#include "GoToPosition.hpp"
#include "FishBot.hpp"

#include <QtCore/QDebug>

/*!
 * Constructor.
 */
GoToPosition::GoToPosition(FishBot* robot) :
    ControlMode(robot, ControlModeType::GO_TO_POSITION),
    m_targetPosition(0, 0, 0, false) // set the target invalid
{
}

/*!
 * The step of the control mode.
 */
ControlTargetPtr GoToPosition::step()
{
    if (m_targetPosition.isValid())
        return ControlTargetPtr(new TargetPosition(m_targetPosition));
    else
        return ControlTargetPtr();
}

/*!
 * Sets the target position.
 */
void GoToPosition::setTargetPosition(PositionMeters position)
{
    m_targetPosition = position;
    qDebug() << Q_FUNC_INFO << m_robot->name() << "got new target position" << position.toString();
}

/*!
 * Informs on what kind of control targets this control mode generates.
 */
QList<ControlTargetType> GoToPosition::supportedTargets()
{
    return QList<ControlTargetType>({ControlTargetType::SPEED, ControlTargetType::POSITION});
}
