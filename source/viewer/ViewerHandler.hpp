#ifndef CATS2_VIEWER_HANDLER_HPP
#define CATS2_VIEWER_HANDLER_HPP

#include "ViewerWidget.hpp"

#include <TimestampedFrame.hpp>

#include <QtCore/QObject>

class ViewerData;

/*!
* \brief This class manages the visualization of the video stream.
* Binds together the data and GUI.
*
*/
class ViewerHandler : public QObject
{
    Q_OBJECT
public:
    //! Constructor. It receives a qwidget as a parent to pass it further as a parent of
    //! the viewer widget. It's set by a reference to prevent getting null pointers.
    explicit ViewerHandler(TimestampedFrameQueuePtr inputQueue, QWidget& parent);
    //! Forbids passing a temporary object as a parent to the constructor.
    explicit ViewerHandler(TimestampedFrameQueuePtr inputQueue, QWidget&& parent) = delete;
    //! Destructor.
    virtual ~ViewerHandler();

public:
    //! Returns the pointer to the viewer widget.
    ViewerWidget* widget() { return _viewerWidget; }

private:
    //! The data class.
    QSharedPointer<ViewerData> _data;
    //! The GUI class.
    ViewerWidget* _viewerWidget;
};

#endif // CATS2_VIEWER_HANDLER_HPP
