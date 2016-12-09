#ifndef CATS2_TARGET_ITEM_HPP
#define CATS2_TARGET_ITEM_HPP

#include "ColoredItem.hpp"

#include <QtWidgets/QGraphicsItem>

class TargetItem : public QGraphicsItem, public ColoredItem
{
public:
    enum { Type = UserType + 8 };

    //! Constructor.
    TargetItem();

public:
    //! Returns the type of the item.
    virtual int type() const override { return Type;}
    //! Paints the contents of an item in local coordinates.
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget) override ;
    //! Computes the bounding rectangle.
    virtual QRectF boundingRect() const override;

private:
    //! The basic value used to draw the item.
    static constexpr int Size = 10;
};

#endif // CATS2_TARGET_ITEM_HPP
