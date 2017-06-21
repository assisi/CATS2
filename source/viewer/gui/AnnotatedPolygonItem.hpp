#ifndef CATS2_ANNOTATED_POLYGON_ITEM_HPP
#define CATS2_ANNOTATED_POLYGON_ITEM_HPP

#include "ColoredItem.hpp"

#include <QtWidgets/QGraphicsItem>

/*!
 * This class will show on the graphics scene a coloured polygon with
 * a label text inside.
 * NOTE : this item must be positioned at (0,0) to work correctly.
 */
class AnnotatedPolygonItem : public QGraphicsItem, public ColoredItem
{
public:
    enum { Type = UserType + 5 };

    //! Constructor.
    AnnotatedPolygonItem(QPolygonF polygon, QString title);

public:
    //! Return the area's label.
    QString title() const { return m_title; }
    //! Sets the status string.
    void setText(QString text);

public:
    //! Returns the type of the item.
    virtual int type() const override { return Type;}
    //! Paints the contents of an item in local coordinates.
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget) override ;
    //! Computes the bounding rectangle.
    virtual QRectF boundingRect() const override { return m_polygon.boundingRect(); }

private:
    //! The polygon to draw.
    QPolygonF m_polygon;
    //! The label to put on the polygon.
    QString m_title;
    //! The polygon text, provides some extra-information.
    QString m_text;
};

#endif // CATS2_ANNOTATED_POLYGON_ITEM_HPP
