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
    // if robot's position is invalid or the target position is invald then
    // don't move
    if ((!m_robot->state().position().isValid()) || (!m_targetPosition.isValid())) {
        qDebug() << Q_FUNC_INFO << m_robot->name() << "Invalid robot's and/or target position, not moving";
        // send a command to stop
        return ControlTargetPtr(new TargetSpeed(0.0, 0.0));
    }
    // otherwise we decide if to go to the target or if we are already there
    if (m_robot->state().position().distanceTo(m_targetPosition) < TargetReachedDistanceThresholdM) {
        qDebug() << Q_FUNC_INFO << m_robot->name() << "Arrived to the target, stopping";
        // send a stop command
        return ControlTargetPtr(new TargetSpeed(0.0, 0.0));
    } else {
        // otherwise continue advancing
        return ControlTargetPtr(new TargetPosition(m_targetPosition));
    }
}

/*!
 * Sets the target position.
 */
void GoToPosition::setTargetPosition(PositionMeters position)
{
    m_targetPosition = position;
    qDebug() << Q_FUNC_INFO << m_robot->name() << "got new target position" << position.toString();
}
