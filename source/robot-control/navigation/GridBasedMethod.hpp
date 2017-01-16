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
    //! Returns the grid matrix corresponding to the setup map.
    cv::Mat generateGrid();

protected:
    //! The setup map.
    SetupMap m_setupMap;
    //! The size of the grid square.
    double m_gridSizeMeters;
};

#endif // CATS2_GRID_BASED_METHOD_HPP
