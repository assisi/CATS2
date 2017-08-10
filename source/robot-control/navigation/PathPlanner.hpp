#ifndef CATS2_PATH_PLANNER_HPP
#define CATS2_PATH_PLANNER_HPP

#include "DijkstraPathPlanner.hpp"

#include <AgentState.hpp>

#include <QtCore/QQueue>
#include <QtCore/QObject>

/*!
 * Runs the flight planner if necessary, if it's not needed than previous
 * resuls of flight planning are returnded. At the moment only Dijkstre path
 * planner is used, but in future it is planned to be extended with more of
 * them.
 */
class PathPlanner : public QObject
{
    Q_OBJECT
public:
    //! Constructor.
    explicit PathPlanner();

    //! Tells where to go.
    PositionMeters currentWaypoint(PositionMeters currentPosition,
                                   PositionMeters targetPosition);

public:
    //! Requests to sends the current trajectory.
    void requestTrajectory() { emit notifyTrajectoryChanged(m_subTargetsQueue); }
    //! Resets the trajectory.
    void clearTrajectory();
signals:
    //! Notifies on the trajectory changes.
    void notifyTrajectoryChanged(QQueue<PositionMeters>);

private:
    //! The path planner to the target position.
    DijkstraPathPlanner m_pathPlanner;
    //! The last recieved target position.
    PositionMeters m_lastReceivedTargetPosition;
    //! The queue of intermediate targets.
    QQueue<PositionMeters> m_subTargetsQueue;
    //! The position of the current target.
    PositionMeters m_currentSubTargetPosition;
};

#endif // CATS2_PATH_PLANNER_HPP
