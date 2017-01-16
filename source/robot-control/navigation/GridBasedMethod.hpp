#ifndef CATS2_GRID_BASED_METHOD_HPP
#define CATS2_GRID_BASED_METHOD_HPP

#include "SetupMap.hpp"

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
    //! The status of a node of a grid map. A try to make something usable and
    //! compatible with the fish model.
    enum GridStatus {
        OCCUPIED = 0,
        FREE = 1
    };

protected:
    //! Computes the grid node point from the world position.
    QPoint positionToGridNode(PositionMeters position) const;
    //! Computes the world position corresponding to the grid node point.
    PositionMeters gridNodeToPosition(QPoint gridNode) const;
    //! Returns the graph's minimal value of the x coordinate. It's shifted by a
    //! half-grid size from the border of the setup to insure that all grid
    //! nodes are inside of the setup.
    inline double minX() const { return m_setupMap.minX() + m_gridSizeMeters / 2;}
    //! Returns the graph's minimal value of the y coordinate. It's shifted by a
    //! half-grid size from the border of the setup to insure that all grid
    //! nodes are inside of the setup.
    inline double minY() const { return m_setupMap.minY() + m_gridSizeMeters / 2;}
    //! Returns the grid matrix corresponding to the setup map.
    cv::Mat generateGrid();

protected:
    //! The setup map.
    SetupMap m_setupMap;
    //! The size of the grid square.
    double m_gridSizeMeters;
};

#endif // CATS2_GRID_BASED_METHOD_HPP
