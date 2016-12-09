#include "TargetItem.hpp"

#include <QtGui/QPainter>
#include <QtCore/QPointF>

/*!
 * Constructor.
 */
TargetItem::TargetItem()  :
    QGraphicsItem()
{

}

/*!
 * Paints the contents of an item in local coordinates.
 */
void TargetItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);

    QPen pen(QBrush(m_color, Qt::SolidPattern), 1, Qt::SolidLine, Qt::SquareCap, Qt::BevelJoin);
    painter->setPen(pen);

    painter->drawLine(QPointF(-Size, Size), QPointF(Size, -Size));
    painter->drawLine(QPointF(Size, Size), QPointF(-Size, -Size));
}

/*!
 * Computes the bounding rectangle.
 */
QRectF TargetItem::boundingRect() const
{
    return QRectF(-Size, -Size, 2 * Size, 2 * Size);
}


