#ifndef CATS2_DIJKSTRA_PATH_PLANNER_HPP
#define CATS2_DIJKSTRA_PATH_PLANNER_HPP

#include <AgentState.hpp>

/*!
 * Runs the path planner to safety reach the target position by using the
 * Dijkstra path planner.
 */
class DijkstraPathPlanner
{
public:
    //! Constructor.
    DijkstraPathPlanner();

    //! Generates a path plan from the current to the target position.
    QQueue<PositionMeters> plan(PositionMeters start, PositionMeters goal);
};

#endif // CATS2_DIJKSTRA_PATH_PLANNER_HPP
