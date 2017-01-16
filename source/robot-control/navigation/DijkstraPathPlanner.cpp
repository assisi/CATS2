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
        while (m_setupMap.minX() + col * m_gridSizeMeters < m_setupMap.maxX()) {
            while (m_setupMap.minY() + row * m_gridSizeMeters < m_setupMap.maxY()) {
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
        double x = m_setupMap.minX();
        double y = m_setupMap.minY();
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
            y = m_setupMap.minY();
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
        return path;
    } else {
        qDebug() << Q_FUNC_INFO << "At least start or goal position are outside of the working space, path planning stopped";
        return QQueue<PositionMeters>();
    }
}
