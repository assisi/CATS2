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

/*!
 * \brief The user interface class that shows current video frame and the detected objects positions.
 */
class ViewerWidget : public QWidget
{
    Q_OBJECT
public:
    //! Constructor.
    explicit ViewerWidget(QWidget *parent = 0);
    //! Destructor.
    virtual ~ViewerWidget();

public slots:
    //! A new frame arrived.
    void onNewFrame(QSharedPointer<QImage> frame);
    //! Zoom on the video.
    void onZoomIn();
    //! Unzoom the video.
    void onZoomOut();

protected:
    //! The form.
    Ui::ViewerWidget* _uiViewer;
    //! The scene.
    QGraphicsScene* _scene;
    //! The item used to show the video stream.
    QGraphicsPixmapItem* _videoFrame;
    // TODO add the agents here
};

#endif // CATS2_VIEWER_WIDGET_HPP
