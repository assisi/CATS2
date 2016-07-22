/*
	Class of the applications' main window
*/

#ifndef CATS2_EXAMPLE_VIEWER_WINDOW_HPP
#define CATS2_EXAMPLE_VIEWER_WINDOW_HPP

#include <TimestampedFrame.hpp>

#include <QtCore>
#include <QtGui>
#include <QtWidgets/QMainWindow>

class VideoGrabber;
class ViewerWidget;
class ViewerHandler;

namespace Ui
{
class ViewerWindow;
}


class ViewerWindow : public QMainWindow
{
    Q_OBJECT
public:
    //! Constructor.
    explicit ViewerWindow (QWidget *parent = 0);
    //! Destructor.
    virtual ~ViewerWindow() override;

private:
    //! The form.
    Ui::ViewerWindow* m_ui;

    //! The video grabber.
    QSharedPointer<VideoGrabber> m_grabber;
    //! The viewer handler.
    QSharedPointer<ViewerHandler> m_viewerHandler;
};

#endif // CATS2_EXAMPLE_VIEWER_WINDOW_HPP
