#ifndef CATS2_SETUP_MAP_HPP
#define CATS2_SETUP_MAP_HPP

#include <AgentState.hpp>

/*!
 * The simplified map of the experimental setup, consists of the external
 * polygon and several external polygons.
 */
class SetupMap
{
public:
    //! Constructor.
    SetupMap();

    //! Returns the polygon representing the setup.
    WorldPolygon polygon() const { return m_polygon; }

    //! Checks that the point is inside the setup.
    bool containsPoint(PositionMeters position) const;

    //! Returns the validity flag.
    bool isValid() const { return m_valid; }
    //! Returns the min x value of the polygon.
    double minX() const { return m_minX; }
    //! Returns the max x value of the polygon.
    double maxX() const { return m_maxX; }
    //! Returns the min y value of the polygon.
    double minY() const { return m_minY; }
    //! Returns the max y value of the polygon.
    double maxY() const { return m_maxY; }
    //! Reads the configuration space from the file.
    void init(QString setupFilePath);

private:
    //! A flag that says if the setup map was correctly initialized.
    bool m_valid;

    //! The experimental setup polygon.
    WorldPolygon m_polygon;
    //! The holes in the setup polygon.
    //! TODO : to implement support.
    QList<WorldPolygon> m_excludedPolygons;

    //! The min value of the 'x' coordinate.
    double m_minX;
    //! The min value of the 'y' coordinate.
    double m_minY;
    //! The max value of the 'x' coordinate.
    double m_maxX;
    //! The max value of the 'y' coordinate.
    double m_maxY;
};

#endif // CATS2_SETUP_MAP_HPP
