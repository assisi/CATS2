#include "ControlArea.hpp"

/*!
 * Constructor.
 */
ControlArea::ControlArea(QString id, ControlAreaType::Enum type = ControlAreaType::UNDEFINED) :
    m_id(id),
    m_type(type)
{

}

/*!
 * Checks if the point is inside this area.
 */
bool ControlArea::contains(QPointF point) const
{
    foreach (const QPolygonF& polygon, m_polygons) {
        if (polygon.containsPoint(point, Qt::OddEvenFill))
            return true;
    }
    return false;
}

/*!
 * Adds a polygon included in this area.
 */
void ControlArea::addPolygon(std::vector<cv::Point2f> cvPolygon)
{
    QPolygonF polygon;

    for (const auto& point : cvPolygon)
        polygon.append(QPointF(point.x, point.y));

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