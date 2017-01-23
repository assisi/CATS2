#ifndef CATS2_MAIN_WINDOW_HPP
#define CATS2_MAIN_WINDOW_HPP

#include <ViewerPointerTypes.hpp>
#include <SetupType.hpp>
#include <TimestampedFrame.hpp>
#include <CommonPointerTypes.hpp>
#include <experiment-controllers/MapController.hpp>

#include <QtCore>
#include <QtGui>
#include <QtWidgets/QMainWindow>

class FrameScene;
class QGraphicsPixmapItem;

namespace Ui
{
class MainWindow;
}

/*!
 * The main window of the application.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    //! Constructor.
    explicit MainWindow(SetupType::Enum setupType,
                        TimestampedFrameQueuePtr queuePtr,
                        CoordinatesConversionPtr coordinatesConversion = CoordinatesConversionPtr(),
                        QWidget *parent = nullptr);
    //! Destructor.
    virtual ~MainWindow() override;

private:
    //! The form.
    Ui::MainWindow* m_ui;

    //! The viewer handler.
    ViewerHandlerPtr m_viewerHandler;
    //! The control map.
    QSharedPointer<MapController> m_mapController;
};

#endif // CATS2_MAIN_WINDOW_HPP
