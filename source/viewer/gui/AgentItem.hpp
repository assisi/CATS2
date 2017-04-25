#ifndef CATS2_AGENT_ITEM_HPP
#define CATS2_AGENT_ITEM_HPP

#include "ColoredItem.hpp"

#include <QtWidgets/QGraphicsItem>

/*!
 * This class will show on the graphics scene a triangle defining the position
 * and orientation of the detected agent.
 */
class AgentItem : public QGraphicsItem, public ColoredItem
{
public:
    enum { Type = UserType + 17 };

    //! Constructor.
    AgentItem();

public:
    //! Set the orientation flag.
    void setHasOrientation(bool hasOrientation) { m_hasOrientation = hasOrientation; }
    //! Highlights the agent visually.
    void setHighlighted(bool highlighted) { m_highlighted = highlighted; }

    //! Returns the type of the item.
    virtual int type() const override { return Type;}
    //! Paints the contents of an item in local coordinates.
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget) override ;
    //! Computes the bounding rectangle.
    virtual QRectF boundingRect() const override;

private:
    //! The flag that defines if the orientation of the agent is known.
    bool m_hasOrientation;
    //! The flag that defines if the agent is highlighted.
    bool m_highlighted;
    //! The basic value used to draw the item.
    static constexpr int Size = 6;
};

#endif // CATS2_AGENT_ITEM_HPP
