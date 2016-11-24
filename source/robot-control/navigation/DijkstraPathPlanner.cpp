#include "DijkstraPathPlanner.hpp"

#include <QtCore/QQueue>

/*!
 * Constructor.
 */
DijkstraPathPlanner::DijkstraPathPlanner()
{

}

/*!
 * Generates a path plan from the current to the target position.
 */
QQueue<PositionMeters> DijkstraPathPlanner::plan(PositionMeters start, PositionMeters goal)
{
    // TODO : to implement
    QQueue<PositionMeters> path;
    path.enqueue(goal);

    return path;
}
