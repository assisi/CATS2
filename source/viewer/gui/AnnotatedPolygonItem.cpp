#include "AnnotatedPolygonItem.hpp"

#include <QtGui/QPainter>

/*!
 * Constructor.
 */
AnnotatedPolygonItem::AnnotatedPolygonItem(QPolygonF polygon, QColor color, QString label) :
    m_polygon(polygon),
    m_color(color),
    m_label(label)
{

}

/*!
 * Paints the polygon in local coordinates.
 */
void AnnotatedPolygonItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);

    QPen pen(QBrush(m_color, Qt::SolidPattern), 1, Qt::SolidLine, Qt::SquareCap, Qt::BevelJoin);
    painter->setPen(pen);

    painter->drawPolygon(m_polygon);
    painter->drawText(m_polygon.boundingRect().center(), m_label);
}

