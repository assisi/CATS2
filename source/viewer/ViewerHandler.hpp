#ifndef CATS2_VIEWER_HANDLER_HPP
#define CATS2_VIEWER_HANDLER_HPP

#include <CommonPointerTypes.hpp>
#include <ViewerPointerTypes.hpp>

#include <QtCore/QObject>

class ViewerData;
class ViewerWidget;

/*!
* \brief This class manages the visualization of the video stream.
* Binds together the data and GUI.
 * NOTE : handler classes should managed through smart pointers without using the Qt's mechanism
 * of ownership; thus we set the parent to nullptr in the constructor.
*/
class ViewerHandler : public QObject
{
    Q_OBJECT
public:
    //! Constructor. It receives a qwidget to pass as a parent to the viewer widget.
    explicit ViewerHandler(TimestampedFrameQueuePtr inputQueue, QWidget* parentWidget);
    //! Destructor.
    virtual ~ViewerHandler();

public:
    //! Returns the pointer to the viewer widget.
    ViewerWidget* widget() { return m_widget; }

private:
    //! The data class.
    QSharedPointer<ViewerData> m_data;
    //! The GUI class.
    ViewerWidget* m_widget;
};

#endif // CATS2_VIEWER_HANDLER_HPP
