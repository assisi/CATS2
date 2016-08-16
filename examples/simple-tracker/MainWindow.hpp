/*
	Class of the applications' main window
*/

#ifndef CATS2_EXAMPLE_MAIN_WINDOW_HPP
#define CATS2_EXAMPLE_MAIN_WINDOW_HPP

#include <TrackerPointerTypes.hpp>
#include <SetupType.hpp>

#include <QtCore>
#include <QtGui>
#include <QtWidgets/QMainWindow>

class VideoGrabber;
class ViewerWidget;
class ViewerHandler;

namespace Ui
{
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    //! Constructor.
    explicit MainWindow (SetupType::Enum setupType = SetupType::MAIN_CAMERA, QWidget *parent = 0);
    //! Destructor.
    virtual ~MainWindow() override;

private:
    //! The form.
    Ui::MainWindow* m_ui;

    //! The tracking setup for the main camera.
    TrackingSetupPtr m_cameraSetup;
};

#endif // CATS2_EXAMPLE_MAIN_WINDOW_HPP
