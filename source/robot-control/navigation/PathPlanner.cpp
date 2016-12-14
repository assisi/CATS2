#include "PathPlanner.hpp"

#include "settings/RobotControlSettings.hpp"

#include <QtCore/QDebug>

/*!
 * Constructor.
 */
PathPlanner::PathPlanner() :
    m_pathPlanner(),
    m_lastReceivedTargetPosition(),
    m_subTargetsQueue(),
    m_currentSubTargetPosition()
{

}

/*!
 * Tells where to go.
 */
PositionMeters PathPlanner::currentWaypoint(PositionMeters currentPosition,
                                            PositionMeters targetPosition)
{
    // if the new position is set then we apply the path planning
    if (targetPosition != m_lastReceivedTargetPosition) {
        m_lastReceivedTargetPosition = targetPosition;
        // replan
        m_subTargetsQueue = m_pathPlanner.plan(currentPosition, targetPosition);
        // check that the planning worked
        if (m_subTargetsQueue.size() > 0) {
            // notify about changes
            emit notifyTrajectoryChanged(m_subTargetsQueue);
            // take the first sub-target
            m_currentSubTargetPosition = m_subTargetsQueue.dequeue();
        } else { // the planning failed
            clearTrajectory();
            return PositionMeters::invalidPosition();
        }
    }

    if (currentPosition.closeTo(m_currentSubTargetPosition)) {
        // in this case we need to set the next target
        if (! m_subTargetsQueue.isEmpty()) {
            m_currentSubTargetPosition = m_subTargetsQueue.dequeue();
        } else {
            qDebug() << Q_FUNC_INFO << "At destination";
            clearTrajectory();
        }
    }

    return m_currentSubTargetPosition;
}

/*!
 * Resets the trajectory.
 */
void PathPlanner::clearTrajectory()
{
    // invalidate the last received target to be sure that
    // we never fall to the same position next time
    m_lastReceivedTargetPosition.setValid(false);
    // clear the queue
    m_subTargetsQueue.clear();
    // notify that the queue is empty
    emit notifyTrajectoryChanged(m_subTargetsQueue);
}
