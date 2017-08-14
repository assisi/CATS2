#ifndef CATS2_COLORED_ITEM_HPP
#define CATS2_COLORED_ITEM_HPP

#include <QtGui/QColor>

/*!
 * The common parent for all scene items.
 */
class ColoredItem
{
public:
    //! Constructor.
    explicit ColoredItem() :
        m_color(Qt::black)/*,
        m_active(true)*/
    {

    }

    //! Sets the color.
    void setColor(QColor color) { m_color = color; }

//    //! Sets the activity flag.
//    void setActive(bool active) { m_active = active; }
//    //! Returns the activity flag.
//    bool isActive() const { return m_active; }

protected:
    //! The color of the item.
    QColor m_color;
//    //! The activity flag. Means that the data is to be shown on the screen.
//    bool m_active;
};

#endif // CATS2_COLORED_ITEM_HPP
