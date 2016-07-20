#ifndef CATS2_VIEWER_HANDLER_HPP
#define CATS2_VIEWER_HANDLER_HPP

#include "ViewerData.hpp"
#include "ViewerWidget.hpp"

#include <QtCore/QObject>

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
    //! the viewer widget.
    explicit ViewerHandler(TimestampedFrameQueuePtr inputQueue, QWidget *parent = 0);
    //! Destructor.
    virtual ~ViewerHandler();

public:
    //! Returns the pointer to the viewer widget.
    ViewerWidget* widget() { return _viewerGui; }

private:
    //! The data class.
    ViewerData _data;
    //! The GUI class. Receives this class as a parent.
    ViewerWidget* _viewerGui;
};

#endif // CATS2_VIEWER_HANDLER_HPP
