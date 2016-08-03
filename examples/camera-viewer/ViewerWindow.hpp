/*
	Class of the applications' main window
*/

#ifndef CATS2_EXAMPLE_VIEWER_WINDOW_HPP
#define CATS2_EXAMPLE_VIEWER_WINDOW_HPP

#include <TimestampedFrame.hpp>
#include <GrabberPointerTypes.hpp>
#include <ViewerPointerTypes.hpp>
#include <CommonPointerTypes.hpp>

#include <QtCore>
#include <QtGui>
#include <QtWidgets/QMainWindow>

class ViewerWidget;

namespace Ui
{
class ViewerWindow;
}

class ViewerWindow : public QMainWindow
{
    Q_OBJECT
public:
    //! Constructor. Gets the income frames queue and the optional coordinates conversion class.
    explicit ViewerWindow (TimestampedFrameQueuePtr queuePtr,
                           CoordinatesConversionPtr coordinatesConversion = CoordinatesConversionPtr(),
                           QWidget *parent = 0);
    //! Destructor.
    virtual ~ViewerWindow() override;

private:
    //! The form.
    Ui::ViewerWindow* m_ui;

    //! The viewer handler.
    ViewerHandlerPtr m_viewerHandler;
};

#endif // CATS2_EXAMPLE_VIEWER_WINDOW_HPP
