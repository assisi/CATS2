#ifndef CATS2_AGENT_TEXT_ITEM_HPP
#define CATS2_AGENT_TEXT_ITEM_HPP

#include <QtWidgets/QGraphicsItem>
#include <QtCore/QString>

/*!
 * This class with show on the graphics scene a label corresponding to the
 * detected agent.
 */
class AgentTextItem : public QGraphicsItem
{
public:
    enum { Type = UserType + 16 };

    //! Constructor.
    AgentTextItem(QString text) { setLabel(text); }

public:
    //! Set label.
    void setLabel(QString text) { m_text = text; }
    //! Returns the type of the item.
    virtual int type() const override { return Type;}
    //! Paints the contents of an item in local coordinates.
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget) override ;
    //! Computes the bounding rectangle.
    virtual QRectF boundingRect() const override;

private:
    //! The text that is shown on the item.
    QString m_text;

    //! The basic value used to draw the item.
    static constexpr int Size = 30;
};

#endif // CATS2_AGENT_TEXT_ITEM_HPP
