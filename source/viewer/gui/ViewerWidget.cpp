#include "ui_ViewerWidget.h"
#include "ViewerWidget.hpp"
#include "ViewerData.hpp"
#include "FrameScene.hpp"

#include <CoordinatesConversion.hpp>

#include <QtWidgets/QGraphicsPixmapItem>
#include <QtCore/QDir>
#include <QtCore/QDebug>
#include <QtCore/QStandardPaths>
#include <QtCore/QDateTime>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>

/*!
 * Constructor.
 */
ViewerWidget::ViewerWidget(ViewerDataPtr viewerData, QWidget *parent) :
    QWidget(parent),
    m_data(viewerData),
    m_uiViewer(new Ui::ViewerWidget)
{
    m_uiViewer->setupUi(this);
    // always receive mouse events
    m_uiViewer->view->setMouseTracking(true);

    // create the scene
    m_scene = new FrameScene(this);
    connect(m_scene, &FrameScene::mouseMoved, this, &ViewerWidget::onMouseMoved);
    m_uiViewer->view->setScene(m_scene);

    // create the video frame item
    m_videoFrame = new QGraphicsPixmapItem();
    m_videoFrame->setZValue(0);
    m_videoFrame->setPos(0,0);
    m_scene->addItem(m_videoFrame);

    // connect to the data class
    qRegisterMetaType<QSharedPointer<QImage>>("QSharedPointer<QImage>");
    connect(m_data.data(), &ViewerData::newFrame, this, &ViewerWidget::onNewFrame);
}

/*!
 * Destructor.
 */
ViewerWidget::~ViewerWidget()
{
    delete m_uiViewer;
}

/*!
 * Zoom on the video.
 */
void ViewerWidget::onZoomIn()
{
    m_uiViewer->view->scale(1.2, 1.2);
}

/*!
 * Unzoom the video.
 */
void ViewerWidget::onZoomOut()
{
    m_uiViewer->view->scale(1/1.2, 1/1.2);
}

void ViewerWidget::onNewFrame(QSharedPointer<QImage> frame)
{
    if (parent()) { // doesn't make sense to update the widget that belongs nowhere
        if (!frame.isNull())
            m_videoFrame->setPixmap(QPixmap::fromImage(*frame.data()));
    }
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
        if (!m_videoFrame->pixmap().save(fileName)) {
            QMessageBox::information(this, tr("Saving current frame..."), tr("Can't save image"));;
        }
    }
}

/*!
 * Triggered when a new mouse position is received from the scene.
 */
void ViewerWidget::onMouseMoved(QPointF scenePosition)
{
    if (!m_data.isNull()) {
        CoordinatesConversionPtr coordinatesConversion = m_data->coordinatesConversion();
        if (!coordinatesConversion.isNull()){
            PositionPixels imagePosition(scenePosition.x(), scenePosition.y());
            PositionMeters worldPosition = coordinatesConversion->imageToWorldPosition(imagePosition);
            emit mousePosition(imagePosition, worldPosition);
        }
    }
}
