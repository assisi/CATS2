#include "ui_ViewerWidget.h"
#include "ViewerWidget.hpp"

#include <QtWidgets/QGraphicsPixmapItem>

/*!
 * Constructor.
 */
ViewerWidget::ViewerWidget(QWidget *parent) :
    QWidget(parent),
    _uiViewer(new Ui::ViewerWidget)
{
    _uiViewer->setupUi(this);

    // create the scene
    _scene = new QGraphicsScene(this);
    _uiViewer->view->setScene(_scene);

    // create the video frame item
    _videoFrame = new QGraphicsPixmapItem();
    _videoFrame->setZValue(0);
    _videoFrame->setPos(0,0);
    _scene->addItem(_videoFrame);
}

/*!
 * Destructor.
 */
ViewerWidget::~ViewerWidget()
{
    delete _uiViewer;
}

/*!
 * Zoom on the video.
 */
void ViewerWidget::onZoomIn()
{
    _uiViewer->view->scale(1.2, 1.2);
}

/*!
 * Unzoom the video.
 */
void ViewerWidget::onZoomOut()
{
    _uiViewer->view->scale(1/1.2, 1/1.2);
}

void ViewerWidget::onNewFrame(QSharedPointer<QImage> frame)
{
    _videoFrame->setPixmap(QPixmap::fromImage(*frame.data()));
}

