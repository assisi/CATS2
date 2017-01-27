#ifndef CATS2_GRID_BASED_METHOD_HPP
#define CATS2_GRID_BASED_METHOD_HPP

#include "SetupMap.hpp"

#include <opencv2/video.hpp>
#include <opencv2/video/background_segm.hpp>

#include <QtCore/QMap>

/*!
 * Stores the setup map, and the grid step.
 */
class GridBasedMethod
{
public:
    //! Constructor.
    GridBasedMethod(double gridSizeMeters);

public:
    //! Returns the polygon representing the setup.
    WorldPolygon polygon() const { return m_setupMap.polygon(); }

protected:
    //! The status of a node of a grid map.
    enum GridStatus {
        OCCUPIED = 0,
        FREE = 255 // to be able to visualize
    };

public:
    //! Applies the mask on the arena matrix; used to limit the model output to
    //! a specific area. Since the same mask can be requested to be applied
    //! several times, we keep a map of used matrices; that's why a maskId is
    //! requested.
    void setAreaMask(QString maskId, QList<WorldPolygon> maskPolygons);
    //! Removes the mask from the arena matrix.
    void clearAreaMask();

protected:
    //! The margin to guarantee that all the walls are included to the grid.
    static constexpr double MarginGridNodesNumber = 5 / 2; // i.e. 2.5 nodes outside of the border

    //! Returns the grid's minimal value of the x coordinate. It provides a
    //! margin outside from the setup's border to guarantee that all the walls
    //! are inside of the grid matrix.
    inline double minX() const
    {
        return m_setupMap.minX() - MarginGridNodesNumber * m_gridSizeMeters;
    }
    //! Returns the grid's minimal value of the y coordinate.
    inline double minY() const
    {
        return m_setupMap.minY() - MarginGridNodesNumber * m_gridSizeMeters;
    }
    //! Returns the graph's maximal value of the x coordinate.
    inline double maxX() const
    {
        return m_setupMap.maxX() + MarginGridNodesNumber * m_gridSizeMeters;
    }
    //! Returns the graph's maximal value of the y coordinate.
    inline double maxY() const
    {
        return m_setupMap.maxY() + MarginGridNodesNumber * m_gridSizeMeters;
    }

protected:
    //! Computes the grid node point from the world position.
    QPoint positionToGridNode(PositionMeters position) const;
    //! Computes the world position corresponding to the grid node point.
    PositionMeters gridNodeToPosition(QPoint gridNode) const;

    //! Returns the grid matrix corresponding to given polygons. The grid is
    //! dimensioned by the minX/maxX/minY/maxY values defined above, and defines
    //! as free all the nodes that are contained by at least one including
    //! polygon, and are not contained by any of excluded polygons.
    cv::Mat generateGrid(QList<WorldPolygon> includingPolygons,
                         QList<WorldPolygon> excludedPolygons = QList<WorldPolygon>());
    //! Check if a point is contained by at least one including polygon, and is
    //! not contained by any of excluded polygons.
    bool checkPointIncluded(PositionMeters position,
                            QList<WorldPolygon> includingPolygons,
                            QList<WorldPolygon> excludedPolygons);
    //! Checks that the point belongs to a setup. First checks for the current
    //! grid (that is faster and takes into account the masks), if it's not yet
    //! generated then we check in the original setup.
    bool containsPoint(PositionMeters position) const;
    //! Checks that the node belongs to a setup.
    bool containsNode(QPoint node) const;

protected:
    //! The setup map.
    SetupMap m_setupMap;
    //! The size of the grid square.
    double m_gridSizeMeters;

    //! The rectangular grid covering the whole setup.
    cv::Mat m_setupGrid;
    //! The masks that might be applied on the setup grid. They are ordered by
    //! ids.
    QMap<QString, cv::Mat> m_areaMasks;
    //! The current grid used by this method, it's the setup grid that might be
    //! limited by a mask.
    cv::Mat m_currentGrid;
};

#endif // CATS2_GRID_BASED_METHOD_HPP
