#include "ui_ViewerWidget.h"
#include "ViewerWidget.hpp"
#include "ViewerData.hpp"

#include <QtWidgets/QGraphicsPixmapItem>
#include <QtCore/QDir>
#include <QtCore/QStandardPaths>
#include <QtCore/QDateTime>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>

/*!
 * Constructor.
 */
ViewerWidget::ViewerWidget(QSharedPointer<ViewerData> viewerData, QWidget *parent) :
    QWidget(parent),
    m_data(viewerData),
    m_uiViewer(new Ui::ViewerWidget)
{
    m_uiViewer->setupUi(this);

    // create the scene
    m_scene = new QGraphicsScene(this);
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
    if (!frame.isNull())
        m_videoFrame->setPixmap(QPixmap::fromImage(*frame.data()));
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
