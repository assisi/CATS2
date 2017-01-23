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
    for (const auto& polygon : m_polygons) {
        WorldPolygon worldPolygon;
        for (const auto& point : polygon) {
            worldPolygon.append(PositionMeters(point.x(), point.y()));
        }
        polygons.polygons.append(worldPolygon);
    }
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
