#ifndef CATS2_MAIN_WINDOW_HPP
#define CATS2_MAIN_WINDOW_HPP

#include <ViewerPointerTypes.hpp>
#include <SetupType.hpp>
#include <TimestampedFrame.hpp>
#include <CommonPointerTypes.hpp>
#include <navigation/DijkstraPathPlanner.hpp>

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

private slots:
    //! Open control map.
    void openControlMap();
    //! Sets the start and goal positions.
    void onButtonClicked(Qt::MouseButton button,PositionMeters worldPosition);

private:
    //! The form.
    Ui::MainWindow* m_ui;

    //! The viewer handler.
    ViewerHandlerPtr m_viewerHandler;
    //! The control map.
    QSharedPointer<DijkstraPathPlanner> m_pathPlanner;

    //! Start position.
    PositionMeters m_startPosition;
    //! Goal position.
    PositionMeters m_goalPosition;
};

#endif // CATS2_MAIN_WINDOW_HPP
