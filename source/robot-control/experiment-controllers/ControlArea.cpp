#include "ControlArea.hpp"

/*!
 * Constructor.
 */
ControlArea::ControlArea(QString id, ControlAreaType::Enum type = ControlAreaType::UNDEFINED) :
    m_id(id),
    m_type(type),
    m_centroid()
{

}

/*!
 * Checks if the point is inside this area.
 */
bool ControlArea::contains(PositionMeters point) const
{
    foreach (const WorldPolygon& polygon, m_polygons) {
        if (polygon.containsPoint(point))
            return true;
    }
    return false;
}

/*!
 *  Returns the index of the area's polygon to which belongs the point, or
 * -1 of the point is not contained by any polygon.
 */
int ControlArea::polygonIndexOf(PositionMeters point) const
{
    for (int index = 0; index < m_polygons.size(); ++index) {
        if (m_polygons.at(index).contains(point))
            return index;
    }
    return -1;
}

/*!
 * Adds a polygon included in this area.
 */
void ControlArea::addPolygon(WorldPolygon polygon)
{
    m_polygons.append(polygon);
    // updates the area centroid
    updateCentroid();
}

/*!
 * Adds a polygon included in this area.
 */
void ControlArea::addPolygon(std::vector<cv::Point2f> cvPolygon)
{
    WorldPolygon polygon;

    for (const auto& point : cvPolygon)
        polygon.append(PositionMeters(point.x, point.y));

    addPolygon(polygon);
}

/*!
 * Returns the polygons to be used in gui.
 */
AnnotatedPolygons ControlArea::annotatedPolygons() const
{
    AnnotatedPolygons polygons;

    polygons.polygons = m_polygons;
    polygons.color = m_color;
    polygons.label = m_id;
    return polygons;
}

/*!
 * Computes the area centroid. For the areas composed of several polygons
 * only the first polygon is taken into account.
 */
void ControlArea::updateCentroid()
{
    if (m_polygons.size() > 0) {
        PositionMeters centroid;
        for (const auto& vertice : m_polygons.at(0)) {
            centroid += vertice;
        }
        centroid /= m_polygons.at(0).size();
        m_centroid = centroid;
    }
}
