#ifndef CATS2_TRAJECTORY_ITEM_HPP
#define CATS2_TRAJECTORY_ITEM_HPP

#include "ColoredItem.hpp"

#include <QtWidgets/QGraphicsItem>
#include <QtCore/QString>

/*!
* This class will show on the graphics scene a polyline.
* NOTE : this item must be positioned at (0,0) to work correctly.
*/
class TrajectoryItem : public QGraphicsItem, public ColoredItem
{
public:
    enum { Type = UserType + 26 };

    //! Constructor.
    TrajectoryItem(QPolygonF polygon);
    //! Sets the polygon.
    void setTrajectory(QPolygonF polygon);

public:
    //! Returns the type of the item.
    virtual int type() const override { return Type;}
    //! Paints the contents of an item in local coordinates.
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget) override ;
    //! Computes the bounding rectangle.
    virtual QRectF boundingRect() const override;

private:
    //! Recomputes the bounding rectangle.
    void updateBoundingRectangle();

private:
    //! The polygon to draw.
    QPolygonF m_polygon;
    //! The bounding rectangle.
    QRectF m_boundingRectangle;

};

#endif // CATS2_TRAJECTORY_ITEM_HPP
