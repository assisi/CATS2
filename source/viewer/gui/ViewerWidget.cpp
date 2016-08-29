#include "ui_ViewerWidget.h"
#include "ViewerWidget.hpp"
#include "ViewerData.hpp"
#include "FrameScene.hpp"
#include "AgentItem.hpp"

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
ViewerWidget::ViewerWidget(ViewerDataPtr viewerData, QSize frameSize, QWidget *parent) :
    QWidget(parent),
    m_data(viewerData),
    m_frameSize(frameSize),
    m_uiViewer(new Ui::ViewerWidget)
{
    m_uiViewer->setupUi(this);
    // always receive mouse events
    m_uiViewer->view->setMouseTracking(true);
    // set the size of the view
    m_uiViewer->view->resize(frameSize.width(), frameSize.height());

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

/*!
 * Scale to the available area.
 */
void ViewerWidget::adjust()
{
    QSize availableArea = m_uiViewer->view->size();
    float width = m_uiViewer->view->transform().m11() * m_frameSize.width();
    float height = m_uiViewer->view->transform().m22() * m_frameSize.height();
    double scaleCoefficient = qMin(availableArea.width() / width,
                                   availableArea.height() / height);
    m_uiViewer->view->scale(scaleCoefficient, scaleCoefficient);
}

/*!
 * Triggered on arrival of the new data.
 */
void ViewerWidget::showAgents(QList<AgentDataWorld> agentDataList)
{
    // first hide all the items on the scene
    foreach (AgentItem* agentItem, m_agents.values()) {
        agentItem->hide();
    }
    // update the item's positions, the map itself by adding new
    // items
    foreach (AgentDataWorld agentData, agentDataList) {
        QString id = agentData.id();
        // if an item corresponding to the agent is not yet in the list
        // then add it
        if (! m_agents.contains(id)) {
            m_agents[id] = new AgentItem(agentData.label());
            m_scene->addItem(m_agents[id]);
        }
        // set the position
        if (agentData.state().position().isValid()) {
            // convert the position
            CoordinatesConversionPtr coordinatesConversion = m_data->coordinatesConversion();
            if (!coordinatesConversion.isNull()) {
                PositionPixels imagePosition = coordinatesConversion->worldToImagePosition(agentData.state().position());
                m_agents[id]->setPos(imagePosition.x(), imagePosition.y());
                m_agents[id]->setVisible(true);
            } else {
                qDebug() << Q_FUNC_INFO << "Unable to convert agent's world position to the image position";
            }
        } else {
            qDebug() << Q_FUNC_INFO << QString("The postion of %1 is invalid").arg(agentData.label());
        }
    }
}

