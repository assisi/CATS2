#ifndef CATS2_VIEWER_WIDGET_HPP
#define CATS2_VIEWER_WIDGET_HPP

#include "ViewerPointerTypes.hpp"

#include <AgentData.hpp>

#include <QtCore/QObject>
#include <QtWidgets/QWidget>

class FrameScene;
class QGraphicsPixmapItem;
class AgentItem;

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
    explicit ViewerWidget(ViewerDataPtr data, QSize frameSize, QWidget *parent = nullptr);
    //! Destructor.
    virtual ~ViewerWidget() final;

signals:
    //! Notifies that the mouse position has changed and sends it out in both
    //! image and world coordinates.
    void mousePosition(PositionPixels imagePosition, PositionMeters worldPosition);

public slots:
    //! Triggered on arrival of the new data.
    void showAgents(QList<AgentDataWorld> agentsData);

public slots:
    //! Zoom on the video.
    void onZoomIn();
    //! Unzoom the video.
    void onZoomOut();
    //! Save current frame.
    void saveCurrentFrameToFile();
    //! Scale to the available area.
    void adjust();

protected slots:
    //! A new frame arrived.
    void onNewFrame(QSharedPointer<QImage> frame);
    //! Triggered when a new mouse position is received from the scene.
    void onMouseMoved(QPointF scenePosition);

protected:
    //! The data object that provides the frames and agent's positions to show.
    ViewerDataPtr m_data;
    //! The frame size.
    QSize m_frameSize;
    //! The form.
    Ui::ViewerWidget* m_uiViewer;
    //! The graphics scene to show the video stream and agents.
    FrameScene* m_scene;
    //! The item used to show the video stream.
    QGraphicsPixmapItem* m_videoFrame;

    //! The agents maps, contains the agent's id's with corresponding
    //! graphics items.
    // FIXME : at the moment that agent's id is copied as it is without taking
    // into account that theoretically two agents coming from different sources
    // can have the same id.
    QMap<QString, AgentItem*> m_agents;
};

#endif // CATS2_VIEWER_WIDGET_HPP
