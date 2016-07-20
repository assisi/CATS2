#include "ui_ViewerWidget.h"
#include "ViewerWidget.hpp"

#include <QtWidgets/QGraphicsPixmapItem>
#include <QtCore/QDir>
#include <QtCore/QStandardPaths>
#include <QtCore/QDateTime>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>

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

/*!
 * Save current frame.
 */
void ViewerWidget::saveCurrentFrameToFile()
{
    QString defaultName = QString("%1%2%3_%4.png").arg(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation))
                                                  .arg(QDir::separator())
                                                  .arg(QApplication::applicationName())
                                                  .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh-mm-ss"));

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save image"), defaultName, tr("Images (*.png *.bmp *.jpg)"));
    if (!fileName.isEmpty()) {
        if (!_videoFrame->pixmap().save(fileName)) {
            QMessageBox::information(this, tr("Saving current frame..."), tr("Can't save image"));;
        }
    }
}
