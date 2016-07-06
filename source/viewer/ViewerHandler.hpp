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
    //! Constructor.
    explicit ViewerHandler(TimestampedFrameQueuePtr inputQueue, ViewerWidget* viewerGui, QObject *parent = 0);
    //! Destructor.
    virtual ~ViewerHandler();

private:
    //! The data class.
    ViewerData _data;
    //! The GUI class. Owned by the main window, only the pointer is stored here.
    ViewerWidget* _viewerGui;
};

#endif // CATS2_VIEWER_HANDLER_HPP
