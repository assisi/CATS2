#include "AgentItem.hpp"

#include <QtGui/QPainter>

/*!
 * Paints the contents of an item in local coordinates.
 */
void AgentItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);

    QPointF points[3] = {QPointF(0,0), QPointF(Size, -Size / 3), QPointF(Size + m_text.length() * 7, -Size / 3)};

    QPen pen(QBrush(Qt::blue, Qt::SolidPattern), 1, Qt::SolidLine, Qt::SquareCap, Qt::BevelJoin);
    painter->setPen(pen);
    painter->drawPolyline(points, 3);

    if (!m_text.isEmpty())
        painter->drawText(Size, -Size / 3 - 3, m_text);
}

QRectF AgentItem::boundingRect() const
{
    return QRectF(0, 0, Size + m_text.length() * 7, -2 * Size);
}
