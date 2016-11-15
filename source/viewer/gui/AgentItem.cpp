#include "AgentItem.hpp"

#include <QtGui/QPainter>

/*!
 * Constructor.
 */
AgentItem::AgentItem() :
    QGraphicsItem(),
    m_hasOrientation(false)
{

}

/*!
 * Paints the contents of an item in local coordinates.
 */
void AgentItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);

    QPen pen(QBrush(Qt::blue, Qt::SolidPattern), 2, Qt::SolidLine, Qt::SquareCap, Qt::BevelJoin);
    painter->setPen(pen);

    if (m_hasOrientation) {
        static const QPointF points[3] = {
            QPointF(-Size,-Size),
            QPointF(Size, -Size),
            QPointF(0, 2 * Size)
        };
        painter->drawPolygon(points, 3);
    } else {
        painter->drawEllipse(QPointF(0, 0), Size, Size);
    }
}

/*!
 * Computes the bounding rectangle.
 */
QRectF AgentItem::boundingRect() const
{
    return QRectF(-2 * Size, -2 * Size, 4 * Size, 4 * Size);
}

