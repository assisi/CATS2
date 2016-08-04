#ifndef CATS2_VIEWER_WIDGET_HPP
#define CATS2_VIEWER_WIDGET_HPP

#include "ViewerPointerTypes.hpp"

#include <AgentData.hpp>

#include <QtCore/QObject>
#include <QtWidgets/QWidget>

class FrameScene;
class QGraphicsPixmapItem;

namespace Ui
{
class ViewerWidget;
}

/*!
 * \brief The user interface class that shows current video frame and the detected objects positions.
 */
class ViewerWidget : public QWidget
{
    Q_OBJECT
public:
    //! Constructor.
    explicit ViewerWidget(ViewerDataPtr data, QWidget *parent = nullptr);
    //! Destructor.
    virtual ~ViewerWidget() final;

signals:
    //! Notifies that the mouse position has changed and sends it out in both
    //! image and world coordinates.
    void mousePosition(PositionPixels imagePosition, PositionMeters worldPosition);

public slots:
    //! Zoom on the video.
    void onZoomIn();
    //! Unzoom the video.
    void onZoomOut();
    //! Save current frame.
    void saveCurrentFrameToFile();

protected slots:
    //! A new frame arrived.
    void onNewFrame(QSharedPointer<QImage> frame);
    //! Triggered when a new mouse position is received from the scene.
    void onMouseMoved(QPointF scenePosition);

protected:
    //! The data object that provides the frames and agent's positions to show.
    ViewerDataPtr m_data;
    //! The form.
    Ui::ViewerWidget* m_uiViewer;
    //! The graphics scene to show the video stream and agents.
    FrameScene* m_scene;
    //! The item used to show the video stream.
    QGraphicsPixmapItem* m_videoFrame;
    // TODO add the agents here
};

#endif // CATS2_VIEWER_WIDGET_HPP
