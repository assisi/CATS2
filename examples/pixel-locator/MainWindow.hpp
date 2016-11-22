#ifndef CATS2_MAIN_WINDOW_HPP
#define CATS2_MAIN_WINDOW_HPP

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
    explicit MainWindow (QWidget *parent = nullptr);
    //! Destructor.
    virtual ~MainWindow() override;

private slots:
    //! Open file.
    void openFile();
    //! Triggered when a new mouse position is received from the scene.
    void onMouseMoved(QPointF scenePosition);
    //! Triggered when a mouse click position is received from the scene.
    void onMouseClicked(QPointF scenePosition);
    //! Zoom on the video.
    void onZoomIn();
    //! Unzoom the video.
    void onZoomOut();
    //! Scale to the available area.
    void adjust();

private:
    //! The form.
    Ui::MainWindow* m_ui;

    //! The graphics scene to show the video stream and agents.
    FrameScene* m_scene;
    //! The item used to show the video stream.
    QGraphicsPixmapItem* m_videoFrame;
};

#endif // CATS2_MAIN_WINDOW_HPP
