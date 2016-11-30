#include "TrajectoryItem.hpp"

#include <QtGui/QPainter>

/*!
 * Constructor.
 */
TrajectoryItem::TrajectoryItem(QPolygonF polygon, QColor color) :
    m_polygon(polygon),
    m_color(color)
{

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
}
