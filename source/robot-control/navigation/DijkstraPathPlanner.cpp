#include "DijkstraPathPlanner.hpp"

#include "settings/RobotControlSettings.hpp"

#include <AgentState.hpp>
#include <Timer.hpp>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/connected_components.hpp>

#include <QtCore/QQueue>
#include <QtCore/QDebug>
#include <QtGui/QVector2D>

#include <limits>

bool operator<(QPoint const& p1, QPoint const& p2)
{
    return (p1.x() < p2.x() || ((p1.x() == p2.x()) && (p1.y() < p2.y())));
}

/*!
 * Constructor.
 */
DijkstraPathPlanner::DijkstraPathPlanner() :
    GridBasedMethod(RobotControlSettings::get().pathPlanningSettings().gridSizeMeters()),
    m_gotErrorOnPreviousStep(false)
{
    m_valid = init();
    if (m_valid)
        qDebug() << "Successfully initialized the dijkstra path planner";
    else
        qDebug() << "Could not initialize the path planner";

//    cv::namedWindow("DijkstraGrid", cv::WINDOW_NORMAL);
}

/*!
 * Destructor.
 */
DijkstraPathPlanner::~DijkstraPathPlanner()
{
//    cv::destroyWindow("DijkstraGrid");
    qDebug() << "Destroying the object";
}

/*!
 * Builds the graph.
 */
bool DijkstraPathPlanner::init()
{
    bool successful = (!m_currentGrid.empty());

    // build a graph
    if (successful) {
        // add vertices based on the grid
        for (int col = 0; col < m_currentGrid.cols; ++col)
            for (int row = 0; row < m_currentGrid.rows; ++row) {
                Vertex vertex = boost::add_vertex(m_graph);
                m_graph[boost::num_vertices(m_graph) - 1].row = row;
                m_graph[boost::num_vertices(m_graph) - 1].col = col;
                m_gridNodeToVertexMap.insert(QPoint(col, row), vertex);
            }

        // add edges
        for (int col = 0; col < m_currentGrid.cols; ++col)
            for (int row = 0; row < m_currentGrid.rows; ++row) {
                // make the connections with four neighbours to the right, up,
                // and up and down diagonal
                QPoint currentNode(col, row);
                addEdge(currentNode, currentNode + QPoint(1, 0));
                addEdge(currentNode, currentNode + QPoint(0, 1));
                addEdge(currentNode, currentNode + QPoint(1, 1));
                addEdge(currentNode, currentNode + QPoint(1, -1));
            }

        // find connected components
        m_componentByVertex.resize(boost::num_vertices(m_graph));
        int num = boost::connected_components(m_graph, &m_componentByVertex[0]);
        qDebug() << QString("The graph contains %1 connected components")
                    .arg(num);

//        cv::imshow( "DijkstraGrid", m_currentGrid);
    }

    return successful;
}

/*!
 * Adds an edge between two points.
 */
void DijkstraPathPlanner::addEdge(QPoint firstNode, QPoint secondNode)
{
    if (containsNode(firstNode) && containsNode(secondNode)) {
        // distance between two grid nodes
        double distance = m_gridSizeMeters *
                QVector2D(secondNode - firstNode).length();
        Vertex firstVertex = m_gridNodeToVertexMap[firstNode];
        Vertex secondVertex = m_gridNodeToVertexMap[secondNode];
        boost::add_edge(firstVertex, secondVertex, distance, m_graph);
    }
}

/*!
 * Generates a path plan from the current to the target position.
 * Based on http://www.boost.org/doc/libs/1_46_1/libs/graph/example/dijkstra-example.cpp
 * and http://stackoverflow.com/questions/12675619/boost-dijkstra-shortest-path-
 * how-can-you-get-the-shortest-path-and-not-just-th
 */
QQueue<PositionMeters> DijkstraPathPlanner::plan(PositionMeters startPoint,
                                                 PositionMeters goalPoint)
{
    // the backup path to be suggested when we can't generate a good one
    QQueue<PositionMeters> backupPath;
    backupPath.enqueue(goalPoint);

    // the grid nodes corresponding to the start and goal positions
    QPoint startGridNode = positionToGridNode(startPoint);
    QPoint goalGridNode = positionToGridNode(goalPoint);

    // sanity checks: we verify that both grid nodes are inside the setup and
    // thus might be connected; if the path planning can not be run they return
    // the path consisting from a goal position
    if (! containsNode(startGridNode)) {
        if (! m_gotErrorOnPreviousStep) {
            qDebug() << QString("Start grid node position is outside of the "
                                "working space: %1, path planning stopped")
                        .arg(gridNodeToPosition(startGridNode).toString())
                     << startGridNode;
            m_gotErrorOnPreviousStep = true;
        }
        return backupPath;
    }
    if (! containsNode(goalGridNode)) {
        if (! m_gotErrorOnPreviousStep) {
            qDebug() << QString("Goal grid node position is outside of the "
                                "working space: %1, path planning stopped")
                        .arg(gridNodeToPosition(goalGridNode).toString())
                     << goalGridNode;
            m_gotErrorOnPreviousStep = true;
        }
        return backupPath;
    }

    m_gotErrorOnPreviousStep = false;

    Vertex startVertex = m_gridNodeToVertexMap[startGridNode];
    Vertex goalVertex = m_gridNodeToVertexMap[goalGridNode];

    // first we check that both vertices belong to the same component and thus
    // can be connected
    if (m_componentByVertex[startVertex] != m_componentByVertex[goalVertex]) {
        qDebug() << "Start and goal vertices belong to different graph "
                    "components and can not be connected";
        return backupPath;
    }

    // create vectors to store the predecessors (p) and the distances from the root (d)
    std::vector<Vertex> predecessors(num_vertices(m_graph));
    std::vector<float> distances(num_vertices(m_graph));

    // evaluate Dijkstra on graph g with source s, predecessor_map p and distance_map d
    boost::dijkstra_shortest_paths(m_graph, startVertex,
                                   boost::predecessor_map(&predecessors[0]).distance_map(&distances[0]));

    //reconstruct the shortest path based on the parent list
    std::vector<Vertex> shortestPath;
    Vertex currentVertex = goalVertex;
    // this loop theoretically might run forever, for the security a timer is
    // added that execute an "emergency" exit
    Timer timeOutBreakTimer;
    timeOutBreakTimer.reset();
    while(currentVertex != startVertex)
    {
        shortestPath.push_back(currentVertex);
        currentVertex = predecessors[currentVertex];

        if (timeOutBreakTimer.isTimedOutSec(1.)) {
            qDebug() << "Path planner was interrupted by a time-out; normally "
                        "this should never happen";
            return backupPath;
        }
    }
    shortestPath.push_back(startVertex);

    // the resulted path
    QQueue<PositionMeters> path;
    // get the path in world coordinates
    std::vector<Vertex>::reverse_iterator it;
    QPoint point;
    double shortestDistance = 0;
    for (it = shortestPath.rbegin(); it != shortestPath.rend(); ++it)
    {
        point.setX(m_graph[*it].col);
        point.setY(m_graph[*it].row);
        PositionMeters position = gridNodeToPosition(point);
        if (path.size() > 0)
            shortestDistance += path.last().distance2DTo(position);
        path.enqueue(position);
    }
//    qDebug() << QString("Shortest distance to the target is %1").arg(shortestDistance);

    // simplify the path
    simplifyPath(path);
    return path;
}

/*!
 * Simplifies the resulted path by removing the points lying on the same line.
 */
void DijkstraPathPlanner::simplifyPath(QQueue<PositionMeters>& path)
{
    QQueue<PositionMeters> reducedPath;

    // if the computed dijkstra path is not empty
    if (!path.empty()) {
        // add the starting point
        PositionMeters previousPosition = path.dequeue();
        reducedPath.enqueue(previousPosition);
        // if there were more than 2 nodes in the path
        if (path.size() > 1) {
            double previousDx, previousDy;
            PositionMeters currentPosition = path.dequeue();
            // compute the fisrt difference of position along x and y
            previousDx = currentPosition.x() - previousPosition.x();
            previousDy = currentPosition.y() - previousPosition.y();

            // the distance between two consequitive points
            double accumulatedDistance = qSqrt(previousDx * previousDx +
                                               previousDy * previousDy);
            // for all the other points in the path
            while (path.size() > 0) {
                // update the positions
                previousPosition = currentPosition;
                currentPosition = path.dequeue();
                // compute the curent difference of position along x and y
                double currentDx = currentPosition.x() - previousPosition.x();
                double currentDy = currentPosition.y() - previousPosition.y();
                // update the accumulated distance
                accumulatedDistance += qSqrt(currentDx * currentDx +
                                             currentDy * currentDy);
                // if the slope is different or if the distance between two
                // points becomes too long
                if(!qFuzzyCompare(previousDx, currentDx) ||
                        !qFuzzyCompare(previousDy, currentDy) ||
                        (accumulatedDistance > MaximalDistanceBetweenTwoPathPoints))
                {
                    // add the new point to the path
                    reducedPath.enqueue(previousPosition);
                    accumulatedDistance = 0;
                }
                // update the current differences of positions along x and y
                previousDx = currentDx;
                previousDy = currentDy;
            }
            // add the last point
            reducedPath.enqueue(currentPosition);
        } else {
            // all the last element
            if (path.size() > 0)
                reducedPath.enqueue(path.dequeue());
        }
    }
    // return the reduced path
    path = reducedPath;
}
