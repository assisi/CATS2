#ifndef CATS2_VIEWER_WIDGET_HPP
#define CATS2_VIEWER_WIDGET_HPP

#include <QtCore/QObject>
#include <QtWidgets/QWidget>

class QGraphicsScene;
class QGraphicsPixmapItem;

namespace Ui
{
class ViewerWidget;
}

class ViewerData;

/*!
 * \brief The user interface class that shows current video frame and the detected objects positions.
 */
class ViewerWidget : public QWidget
{
    Q_OBJECT
public:
    //! Constructor.
    explicit ViewerWidget(QSharedPointer<ViewerData> data, QWidget *parent);
    //! Destructor.
    virtual ~ViewerWidget() final;

public slots:
    //! A new frame arrived.
    void onNewFrame(QSharedPointer<QImage> frame);
    //! Zoom on the video.
    void onZoomIn();
    //! Unzoom the video.
    void onZoomOut();
    //! Save current frame.
    void saveCurrentFrameToFile();

protected:
    //! The data class that provides the frames and agent's positions to show.
    QSharedPointer<ViewerData> m_data;
    //! The form.
    Ui::ViewerWidget* m_uiViewer;
    //! The scene.
    QGraphicsScene* m_scene;
    //! The item used to show the video stream.
    QGraphicsPixmapItem* m_videoFrame;
    // TODO add the agents here
};

#endif // CATS2_VIEWER_WIDGET_HPP
