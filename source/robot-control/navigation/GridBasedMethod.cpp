#include "GridBasedMethod.hpp"

#include "settings/RobotControlSettings.hpp"

#include <QtCore/QDebug>

/*!
 * Constructor.
 */
GridBasedMethod::GridBasedMethod(double gridSizeMeters) :
    m_setupMap(RobotControlSettings::get().setupMap()),
    m_gridSizeMeters(gridSizeMeters),
    m_setupGrid(),
    m_currentMaskId(),
    m_currentGrid()
{
    if (m_setupMap.isValid()) {
        // generate the grid based on the setup map
        m_setupGrid = generateGrid(QList<WorldPolygon>({m_setupMap.polygon()}),
                                   m_setupMap.excludedPolygons());
        // set the generated grid as the current
        m_setupGrid.copyTo(m_currentGrid);
    }
}

/*!
 * Destructor.
 */
GridBasedMethod::~GridBasedMethod()
{
    qDebug() << "Destroying the object";
}

/*!
 * Computes the grid node point from the world position.
 */
QPoint GridBasedMethod::positionToGridNode(PositionMeters position) const
{
    QPoint point;
    // FIXME : replace by std::round
    point.setX(floor((position.x() - minX()) / m_gridSizeMeters  + 0.5));
    point.setY(floor((position.y() - minY()) / m_gridSizeMeters  + 0.5));
    return point;
}

/*!
 * Computes the world position corresponding to the grid node point.
 */
PositionMeters GridBasedMethod::gridNodeToPosition(QPoint gridNode) const
{
     PositionMeters position;
     position.setX(gridNode.x() * m_gridSizeMeters + minX());
     position.setY(gridNode.y() * m_gridSizeMeters + minY());
     return position;
}

/*!
 * Returns the grid matrix corresponding to given polygons. The grid is
 * dimensioned by the minX/maxX/minY/maxY values defined above, and defines as
 * free all the nodes that are contained by at least one including polygon, and
 * are not contained by any of excluded polygons.
 */
cv::Mat GridBasedMethod::generateGrid(QList<WorldPolygon> includingPolygons,
                                      QList<WorldPolygon> excludedPolygons)
{
    // build the matrix, it covers the whole setup
    int cols = floor((maxX() - minX()) / m_gridSizeMeters + 0.5);
    int rows = floor((maxY() - minY()) / m_gridSizeMeters + 0.5);
    if ((cols > 0) && (rows > 0)) {
        // a matrix representing the setup
        cv::Mat arenaMatrix(rows, cols, CV_8U);
        // fill the matrix
        double y = minY();
        for (int row = 0; row < rows; ++row) { // rows go from min_y up to max_y
            double x = minX();
            for (int col = 0; col < cols; ++col) { // cols go from min_x right to max_x
                if (checkPointIncluded(PositionMeters(x, y),
                                       includingPolygons,
                                       excludedPolygons))
                {
                    arenaMatrix.at<uchar>(row, col) = static_cast<int>(FREE);
                } else {
                    arenaMatrix.at<uchar>(row, col) = static_cast<int>(OCCUPIED);
                }
                x += m_gridSizeMeters;
            }
            y += m_gridSizeMeters;
        }
        return arenaMatrix;
    }
    return cv::Mat();
}

/*!
 * Check if a point is contained by at least one including polygon, and is not
 * contained by any of excluded polygons.
 */
bool GridBasedMethod::checkPointIncluded(PositionMeters position,
                                         QList<WorldPolygon> includingPolygons,
                                         QList<WorldPolygon> excludedPolygons)
{
    // first we check if it's included to at least one including polygon
    bool foundIncludingPolygon = false;
    for (const auto& polygon : includingPolygons) {
        if (polygon.containsPoint(position)) {
            foundIncludingPolygon = true;
            break;
        }
    }
    if (foundIncludingPolygon) {
        // now check that it's not in any of the excluded polygons
        for (const auto& polygon : excludedPolygons) {
            if (polygon.containsPoint(position)) {
                // the point belongs to an excluded polygon, thus it's not
                // contained by the defined set of polygons
                return false;
            }
        }
        // if we reach here then the point is not inside of any of excluded
        // polygons
        return true;
    } else {
        return false;
    }
}

/*! Applies the mask on the arena matrix; used to limit the model output to a
 * specific area. Since the same mask can be requested to be applied several
 * times, we keep a map of used matrices; that's why a maskId is requested.
 */
void GridBasedMethod::setAreaMask(QString maskId, QList<WorldPolygon> maskPolygons)
{
    if (maskId != m_currentMaskId) {
        // if the mask is not yet known then generate the matrix and put it to the map
        if (! m_areaMasks.contains(maskId)) {
            m_areaMasks[maskId] = generateGrid(maskPolygons);
        }
        // apply the mask
        m_currentGrid = m_setupGrid & m_areaMasks[maskId];
        m_currentMaskId = maskId;
    }
}

/*!
 * Removes the mask from the arena matrix.
 */
void GridBasedMethod::clearAreaMask()
{
    m_setupGrid.copyTo(m_currentGrid);
    m_currentMaskId = "";
}

/*!
 * Checks that the point belongs to a setup. First checks for the current grid
 * (that is faster and takes into account the masks), if it's not yet generated
 * then we check in the original setup.
 */
bool GridBasedMethod::containsPoint(PositionMeters position) const
{
    // first check the grid
    if (!m_currentGrid.empty()) {
        QPoint node = positionToGridNode(position);
        if ((node.x() >= 0) && (node.x() < m_currentGrid.cols) &&
            (node.y() >= 0) && (node.y() < m_currentGrid.rows))
        {
            return (m_currentGrid.at<uchar>(node.y(), node.x()) == GridStatus::FREE);
        } else {
            // clearly outside of the setup
            return false;
        }
    } else {
        return m_setupMap.containsPoint(position);
    }
}

/*!
 * Checks that the node belongs to a setup.
 */
bool GridBasedMethod::containsNode(QPoint node) const
{
    return containsPoint(gridNodeToPosition(node));
}
