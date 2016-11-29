#include "DijkstraPathPlanner.hpp"

#include <AgentState.hpp>

#include <settings/ReadSettingsHelper.hpp>

#include <QtCore/QQueue>

#include <limits>

bool operator<(QPoint const& p1, QPoint const& p2)
{
    return (p1.x() < p2.x() || ((p1.x() == p2.x()) && (p1.y() < p2.y())));
}

/*!
 * Constructor.
 */
DijkstraPathPlanner::DijkstraPathPlanner(QString setupFilePath) :
    m_minX(std::numeric_limits<float>::max()),
    m_minY(std::numeric_limits<float>::max())
{
    m_valid = init(setupFilePath);
    if (m_valid)
        qDebug() << Q_FUNC_INFO << "Successfully initialized the dijkstra path planner";
    else
        qDebug() << Q_FUNC_INFO << "Could not initialize the path planner";
}

/*!
 * Reads the configuration space from the file and builds the graph.
 */
bool DijkstraPathPlanner::init(QString controlMapFileName)
{
    bool successful = true;

    ReadSettingsHelper settings(controlMapFileName);

    // read the size of the grid square and the polygon
    settings.readVariable("gridSizeM", m_gridSizeM);
    successful = successful && (m_gridSizeM > 0);

    std::vector<cv::Point2f> polygon;
    settings.readVariable(QString("polygon"), polygon);
    successful = successful && (polygon.size() > 0);

    // build a graph
    if (successful) {
        // find min/max values
        float maxX = std::numeric_limits<float>::min();
        float maxY = std::numeric_limits<float>::min();
        for (cv::Point2f& point : polygon) {
            m_polygon.append(PositionMeters(point.x, point.y));
            m_minY = qMin(m_minY, point.y);
            m_minX = qMin(m_minX, point.x);
            maxY = qMax(maxY, point.y);
            maxX = qMax(maxX, point.x);
        }

        // add vertices
        int row = 0; // left to right
        int col = 0; // down to up
        while (m_minX + col * m_gridSizeM < maxX) {
            while (m_minY + row * m_gridSizeM < maxY) {
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
        double x = m_minX;
        double y = m_minY;
        while (x < maxX) {
            while (y < maxY) {
                // make the connections with four neighbours to the right, up,
                // and up and down diagonal
                PositionMeters currentPoint(x, y);
                addEdge(currentPoint, PositionMeters(x + m_gridSizeM, y));
                addEdge(currentPoint, PositionMeters(x, y + m_gridSizeM));
                addEdge(currentPoint, PositionMeters(x + m_gridSizeM, y + m_gridSizeM));
                addEdge(currentPoint, PositionMeters(x + m_gridSizeM, y - m_gridSizeM));
                row++;
                y += m_gridSizeM;
            }
            col++;
            x += m_gridSizeM;
            row = 0;
            y = m_minY;
        }
    }

    return successful;
}

/*!
 * Adds an edge between two points.
 */
void DijkstraPathPlanner::addEdge(PositionMeters firstPoint, PositionMeters secondPoint)
{
    if (m_polygon.containsPoint(firstPoint) && m_polygon.containsPoint(secondPoint)) {
        double distance = firstPoint.distance2DTo(secondPoint);
        Vertex firstVertex = m_gridNodeToVertexMap[positionToGridNode(firstPoint)];
        Vertex secondVertex = m_gridNodeToVertexMap[positionToGridNode(secondPoint)];
        boost::add_edge(firstVertex, secondVertex, distance, m_graph);
    }
}

/*! 
 * Computes the grid node point from the world position.
 */ 
QPoint DijkstraPathPlanner::positionToGridNode(PositionMeters position) const
{
    QPoint point;
    point.setX(floor((position.x() - m_minX) / m_gridSizeM + 0.5));
    point.setY(floor((position.y() - m_minY) / m_gridSizeM + 0.5));
    return point;
}

/*!
 * Computes the world position corresponding to the grid node point.
 */
PositionMeters DijkstraPathPlanner::gridNodeToPosition(QPoint gridNode) const
{
     PositionMeters position;
     position.setX(gridNode.x() * m_gridSizeM + m_minX);
     position.setY(gridNode.y() * m_gridSizeM + m_minY);
     return position;
}

/*!
 * Generates a path plan from the current to the target position.
 * Based on http://www.boost.org/doc/libs/1_46_1/libs/graph/example/dijkstra-example.cpp
 * and http://stackoverflow.com/questions/12675619/boost-dijkstra-shortest-path-how-can-you-get-the-shortest-path-and-not-just-th
 */
QQueue<PositionMeters> DijkstraPathPlanner::plan(PositionMeters startPoint, PositionMeters goalPoint)
{
    if (m_polygon.containsPoint(startPoint) && m_polygon.containsPoint(goalPoint)) {
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
        for (it = shortestPath.rbegin(); it != shortestPath.rend(); ++it)
        {
            point.setX(m_graph[*it].col);
            point.setY(m_graph[*it].row);
            path.enqueue(gridNodeToPosition(point));
        }
        return path;
    } else {
        qDebug() << Q_FUNC_INFO << "At least start or goal position are outside of the working space, path planning stopped";
        return QQueue<PositionMeters>();
    }
}
