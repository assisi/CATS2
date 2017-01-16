#include "DijkstraPathPlanner.hpp"

#include "settings/RobotControlSettings.hpp"

#include <AgentState.hpp>

#include <QtCore/QQueue>
#include <QtCore/QDebug>

#include <limits>

bool operator<(QPoint const& p1, QPoint const& p2)
{
    return (p1.x() < p2.x() || ((p1.x() == p2.x()) && (p1.y() < p2.y())));
}

/*!
 * Constructor.
 */
DijkstraPathPlanner::DijkstraPathPlanner() :
    GridBasedMethod(RobotControlSettings::get().pathPlanningSettings().gridSizeMeters())
{
    m_valid = init();
    if (m_valid)
        qDebug() << Q_FUNC_INFO << "Successfully initialized the dijkstra path planner";
    else
        qDebug() << Q_FUNC_INFO << "Could not initialize the path planner";
}

/*!
 * Reads the configuration space from the file and builds the graph.
 */
bool DijkstraPathPlanner::init()
{
    bool successful = true;

    // read the size of the grid square and the polygon
    successful = (m_gridSizeMeters > 0) && m_setupMap.isValid();

    // build a graph
    if (successful) {
        // add vertices
        int row = 0; // left to right
        int col = 0; // down to up
        while (minX() + col * m_gridSizeMeters < m_setupMap.maxX()) {
            while (minY() + row * m_gridSizeMeters < m_setupMap.maxY()) {
                Vertex vertex = boost::add_vertex(m_graph);
                m_graph[boost::num_vertices(m_graph) - 1].row = row;
                m_graph[boost::num_vertices(m_graph) - 1].col = col;
                m_gridNodeToVertexMap.insert(QPoint(col, row), vertex);
                row++;
            }
            col++;
            row = 0;
        }

        // add edges
        row = 0; // left to right
        col = 0; // down to up
        double x = minX();
        double y = minY();
        while (x < m_setupMap.maxX()) {
            while (y < m_setupMap.maxY()) {
                // make the connections with four neighbours to the right, up,
                // and up and down diagonal
                PositionMeters currentPoint(x, y);
                addEdge(currentPoint, PositionMeters(x + m_gridSizeMeters, y));
                addEdge(currentPoint, PositionMeters(x, y + m_gridSizeMeters));
                addEdge(currentPoint, PositionMeters(x + m_gridSizeMeters, y + m_gridSizeMeters));
                addEdge(currentPoint, PositionMeters(x + m_gridSizeMeters, y - m_gridSizeMeters));
                // not needed but added as a test
//                addEdge(currentPoint, PositionMeters(x - m_gridSizeM, y));
//                addEdge(currentPoint, PositionMeters(x, y - m_gridSizeM));
//                addEdge(currentPoint, PositionMeters(x - m_gridSizeM, y + m_gridSizeM));
//                addEdge(currentPoint, PositionMeters(x - m_gridSizeM, y - m_gridSizeM));
                row++;
                y += m_gridSizeMeters;
            }
            col++;
            x += m_gridSizeMeters;
            row = 0;
            y = minY();
        }
    }

    return successful;
}

/*!
 * Adds an edge between two points.
 */
void DijkstraPathPlanner::addEdge(PositionMeters firstPoint, PositionMeters secondPoint)
{
    if (m_setupMap.containsPoint(firstPoint) && m_setupMap.containsPoint(secondPoint)) {
        double distance = firstPoint.distance2DTo(secondPoint);
        Vertex firstVertex = m_gridNodeToVertexMap[positionToGridNode(firstPoint)];
        Vertex secondVertex = m_gridNodeToVertexMap[positionToGridNode(secondPoint)];
        boost::add_edge(firstVertex, secondVertex, distance, m_graph);
    }
}

/*!
 * Generates a path plan from the current to the target position.
 * Based on http://www.boost.org/doc/libs/1_46_1/libs/graph/example/dijkstra-example.cpp
 * and http://stackoverflow.com/questions/12675619/boost-dijkstra-shortest-path-how-can-you-get-the-shortest-path-and-not-just-th
 */
QQueue<PositionMeters> DijkstraPathPlanner::plan(PositionMeters startPoint, PositionMeters goalPoint)
{
    if (m_setupMap.containsPoint(startPoint) && m_setupMap.containsPoint(goalPoint)) {
        Vertex startVertex = m_gridNodeToVertexMap[positionToGridNode(startPoint)];
        Vertex goalVertex = m_gridNodeToVertexMap[positionToGridNode(goalPoint)];

        // create vectors to store the predecessors (p) and the distances from the root (d)
        std::vector<Vertex> predecessors(num_vertices(m_graph));
        std::vector<float> distances(num_vertices(m_graph));

        // evaluate Dijkstra on graph g with source s, predecessor_map p and distance_map d
        boost::dijkstra_shortest_paths(m_graph, startVertex, boost::predecessor_map(&predecessors[0]).distance_map(&distances[0]));

        //reconstruct the shortest path based on the parent list
        std::vector<boost::graph_traits<UndirectedGraph>::vertex_descriptor > shortestPath;
        boost::graph_traits<UndirectedGraph>::vertex_descriptor currentVertex = goalVertex;
        while(currentVertex != startVertex)
        {
            shortestPath.push_back(currentVertex);
            currentVertex = predecessors[currentVertex];
        }
        shortestPath.push_back(startVertex);

        // get the path in world coordinates
        QQueue<PositionMeters> path;
        std::vector<boost::graph_traits<UndirectedGraph>::vertex_descriptor >::reverse_iterator it;
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
        qDebug() << Q_FUNC_INFO << QString("Shortest distance to the target is %1").arg(shortestDistance);

        // simplify the path
        simplifyPath(path);
        return path;
    } else {
        qDebug() << Q_FUNC_INFO << "At least start or goal position are outside of the working space, path planning stopped";
        return QQueue<PositionMeters>();
    }
}

/*!
 * Simplifies the resulted path by removing the points lying on the same line.
 */
void DijkstraPathPlanner::simplifyPath(QQueue<PositionMeters>& path)
{
    double previousDx, currentDx, previousDy, currentDy;
    QQueue<PositionMeters> reducedPath;

    // if the computed dijkstra path is not empty
    if (!path.empty()) {
        // add the starting point
        PositionMeters previousPosition = path.dequeue();
        reducedPath.enqueue(previousPosition);
        // if there are more than 2 nodes in the path
        if(path.size()>2) {
            PositionMeters currentPosition = path.dequeue();
            // compute the fisrt difference of position along x and y
            previousDx = currentPosition.x() - previousPosition.x();
            previousDy = currentPosition.y() - previousPosition.y();

            // for all the other points in the path
            while (path.size() > 1) {
                // update the positions
                previousPosition = currentPosition;
                currentPosition = path.dequeue();
                // compute the curent difference of position along x and y
                currentDx = currentPosition.x() - previousPosition.x();
                currentDy = currentPosition.y() - previousPosition.y();

                // if the slope is different
                if(!qFuzzyCompare(previousDx, currentDx) || !qFuzzyCompare(previousDy, currentDy)) {
                    // add the new point to the path
                    reducedPath.enqueue(previousPosition);
                }
                // update the current differences of positions along x and y
                previousDx = currentDx;
                previousDy = currentDy;
            }
        }
        // the last element
        reducedPath.enqueue(path.dequeue());
    }
    // return the reduced path
    path = reducedPath;
}
