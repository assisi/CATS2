#include "TrajectoryItem.hpp"

#include <QtGui/QPainter>
#include <QtCore/QDebug>

/*!
 * Constructor.
 */
TrajectoryItem::TrajectoryItem(QPolygonF polygon) :
    m_polygon(polygon),
    m_boundingRectangle()
{
    updateBoundingRectangle();
}

/*!
 * Paints the polygon in local coordinates.
 */
void TrajectoryItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);

    QPen pen(QBrush(m_color, Qt::SolidPattern), 1, Qt::SolidLine, Qt::SquareCap, Qt::BevelJoin);
    painter->setPen(pen);

    painter->drawPolyline(m_polygon);
    // draw little points
    for (auto point : m_polygon) {
        painter->drawEllipse(point, 1, 1);
    }
}

/*!
 * Computes the bounding rectangle.
 */
QRectF TrajectoryItem::boundingRect() const
{
    return m_boundingRectangle;
}

/*!
 * Sets the polygon.
 */
void TrajectoryItem::setTrajectory(QPolygonF polygon)
{
    m_polygon = polygon;
    prepareGeometryChange();
    updateBoundingRectangle();
}

/*!
 * Recomputes the bounding rectangle.
 */
void TrajectoryItem::updateBoundingRectangle()
{
    if (m_polygon.size() > 0) {
        QPointF topLeft = m_polygon.first();
        QPointF bottomRight = m_polygon.first();
        for (const auto& point : m_polygon) {
            if (point.x() > bottomRight.x())
                bottomRight.setX(point.x());
            if (point.y() > bottomRight.y())
                bottomRight.setY(point.y());
            if (point.x() < topLeft.x())
                topLeft.setX(point.x());
            if (point.y() < topLeft.y())
                topLeft.setY(point.y());
        }
        m_boundingRectangle = QRectF(topLeft, bottomRight);
    } else
        m_boundingRectangle = QRectF();
}
