#include "AgentItem.hpp"

#include <QtGui/QPainter>

/*!
 * Constructor.
 */
AgentItem::AgentItem() :
    QGraphicsItem(),
    m_hasOrientation(true),
    m_highlighted(false)
{

}

/*!
 * Paints the contents of an item in local coordinates.
 */
void AgentItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);

    QPen pen(QBrush(m_color, Qt::SolidPattern), 2, Qt::SolidLine, Qt::SquareCap, Qt::BevelJoin);
    painter->setPen(pen);

    if (m_hasOrientation) {
        static const QPointF points[3] = {
            QPointF(-Size, Size),
            QPointF(-Size, -Size),
            QPointF(2 * Size, 0)
        };
        painter->drawPolygon(points, 3);

        if (m_highlighted) {
            static const QPointF externalPoints[3] = {
                QPointF(-Size - 3, Size + 3),
                QPointF(-Size - 3, -Size - 3),
                QPointF(2 * (Size + 3), 0)
            };
            painter->drawPolygon(externalPoints, 3);
        }
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

