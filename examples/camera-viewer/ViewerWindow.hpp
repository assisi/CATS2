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
    Ui::ViewerWindow* _ui;

    // NOTE : all objects below receive this class as an owner, so no need in smart pointers.
    //! The video grabber.
    VideoGrabber* _grabber;
    //! The main camera viewer.
    ViewerWidget* _viewer;
    //! The viewer handler.
    ViewerHandler* _viewerHandler;
};

#endif // CATS2_EXAMPLE_VIEWER_WINDOW_HPP
