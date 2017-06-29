#include "AnnotatedPolygonItem.hpp"

#include <QtGui/QPainter>

/*!
 * Constructor.
 */
AnnotatedPolygonItem::AnnotatedPolygonItem(QPolygonF polygon, QString title) :
    m_polygon(polygon),
    m_title(title),
    m_text()
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

    QFont font("Times", 9, QFont::Normal);
    painter->setFont(font);

    QPointF center = m_polygon.boundingRect().center();
    painter->drawPolygon(m_polygon);
    painter->drawText(center, m_title);
    if (!m_text.isEmpty()) {
        font.setPixelSize(9);
        painter->setFont(font);
        center.ry() += font.pixelSize();
        painter->drawText(center, m_text);
    }
}

/*!
 * Sets the status string.
 */
void AnnotatedPolygonItem::setText(QString text)
{
    m_text = text;
}
