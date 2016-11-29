#include "PathPlanner.hpp"

#include "settings/RobotControlSettings.hpp"

/*!
 * Constructor.
 */
PathPlanner::PathPlanner() :
    m_pathPlanner(RobotControlSettings::get().pathPlanningConfigPath()),
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
        m_subTargetsQueue = m_pathPlanner.plan(currentPosition, targetPosition);
        // take the first sub-target
        m_currentSubTargetPosition = m_subTargetsQueue.dequeue();
    }

    if (currentPosition.closeTo(m_currentSubTargetPosition)) {
        // in this case we need to set the next target
        if (! m_subTargetsQueue.isEmpty()) {
            m_currentSubTargetPosition = m_subTargetsQueue.dequeue();
        } else {
            // otherwise we arrived to the destination, so we keep the same
            // target
        }
    }

    return m_currentSubTargetPosition;
}