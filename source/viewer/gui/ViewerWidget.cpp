#include "ui_ViewerWidget.h"
#include "ViewerWidget.hpp"
#include "ViewerData.hpp"
#include "FrameScene.hpp"
#include "AgentTextItem.hpp"
#include "AgentItem.hpp"
#include "AnnotatedPolygonItem.hpp"
#include "TrajectoryItem.hpp"
#include "TargetItem.hpp"

#include <CoordinatesConversion.hpp>

#include <QtWidgets/QGraphicsPixmapItem>
#include <QtWidgets/QGraphicsTextItem>
#include <QtCore/QDir>
#include <QtCore/QDebug>
#include <QtCore/QQueue>
#include <QtCore/QtMath>
#include <QtCore/QStandardPaths>
#include <QtCore/QDateTime>
#include <QtWidgets/QMenu>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>
#include <QtGui/QContextMenuEvent>

/*!
 * Constructor.
 */
ViewerWidget::ViewerWidget(ViewerDataPtr viewerData, QSize frameSize, QWidget *parent) :
    QWidget(parent),
    m_data(viewerData),
    m_frameSize(frameSize),
    m_uiViewer(new Ui::ViewerWidget),
    m_averageFps(0.),
    m_showAgents(false),
    m_autoAdjust(false),
    m_currentAgentId(),
    m_setupPolygon(nullptr)
{
    m_uiViewer->setupUi(this);
    // always receive mouse events
    m_uiViewer->view->setMouseTracking(true);
    // set the size of the view
    m_uiViewer->view->resize(frameSize.width(), frameSize.height());

    // create the scene
    m_scene = new FrameScene(this);
    connect(m_scene, &FrameScene::mouseMoved, this, &ViewerWidget::onMouseMoved);
    connect(m_scene, &FrameScene::rightButtonClicked, this, &ViewerWidget::onRightButtonClicked);
    m_uiViewer->view->setScene(m_scene);

    // create the video frame item
    m_videoFrame = new QGraphicsPixmapItem();
    m_videoFrame->setZValue(0);
    m_videoFrame->setPos(0,0);
    m_scene->addItem(m_videoFrame);

    // create the frame rate item
    m_frameRate = new QGraphicsTextItem();
    m_frameRate->setFont(QFont("Times", 12, QFont::Bold));
    m_frameRate->setDefaultTextColor(Qt::white);
    m_scene->addItem(m_frameRate);
    m_frameRate->setPos(20,30);

    // connect to the data class
    qRegisterMetaType<QSharedPointer<QPixmap>>("QSharedPointer<QPixmap>");
    connect(m_data.data(), &ViewerData::newFrame, this, &ViewerWidget::onNewFrame);

    // create the context menu actions
    m_adjustAction = new QAction(tr("&Adjust"), this);
    connect(m_adjustAction, &QAction::triggered, this, &ViewerWidget::adjust);
}

/*!
 * Destructor.
 */
ViewerWidget::~ViewerWidget()
{
    qDebug() << Q_FUNC_INFO << "Destroying the object";
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

/*!
 * A new frame arrived.
 */
void ViewerWidget::onNewFrame(QSharedPointer<QPixmap> pixmap, int fps)
{
    if (parent()) { // doesn't make sense to update the widget that belongs nowhere
        if (!pixmap.isNull()) {
            m_videoFrame->setPixmap(*pixmap.data());
            updateFrameRate(fps);
        }
    }
}

/*!
 * Computes new average frame rate value as exponential moving average.
 * https://en.wikipedia.org/wiki/Moving_average#Exponential_moving_average
 */
void ViewerWidget::updateFrameRate(int fps)
{
    if (m_averageFps > 0)
    {
        // set the smoothing factor
        double alpha = 0.25;
        m_averageFps = alpha * fps + (1 - alpha) * m_averageFps;
    }
    else {
        // initialize the filter
        m_averageFps = fps;
    }
    m_frameRate->setPlainText(QString::number(qFloor(m_averageFps + 0.5)) +" fps");
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
    PositionPixels imagePosition(scenePosition.x(), scenePosition.y());
    PositionMeters worldPosition;
    if (convertScenePosition(imagePosition, worldPosition))
        emit mousePosition(imagePosition, worldPosition);
}

/*!
 * Scale to the available area.
 */
void ViewerWidget::adjust()
{
    if (m_frameSize.isValid()) {
        QSize availableArea = m_uiViewer->view->size();
        float width = m_uiViewer->view->transform().m11() * m_frameSize.width();
        float height = m_uiViewer->view->transform().m22() * m_frameSize.height();
        double scaleCoefficient = qMin(availableArea.width() / width,
                                       availableArea.height() / height);
        m_uiViewer->view->scale(scaleCoefficient, scaleCoefficient);
    } else {
        qDebug() << Q_FUNC_INFO << "The target frame size is invalid, can not resize";
    }
}

/*!
 * Triggered on arrival of the new data.
 */
void ViewerWidget::updateAgentLabels(QList<AgentDataWorld> agentDataList)
{
    // first hide all the items on the scene
    foreach (AgentTextItem* agentText, m_agentLabels.values())
        agentText->hide();

    // update the item's positions, the map itself by adding new
    // items
    foreach (AgentDataWorld agentData, agentDataList) {
        QString id = agentData.id();
        // if an item corresponding to the agent is not yet in the list
        // then add it
        if (! m_agentLabels.contains(id)) {
            m_agentLabels[id] = new AgentTextItem(agentData.label());
            if (m_agentColors.contains(id))
                m_agentLabels[id]->setColor(m_agentColors[id]);
            m_scene->addItem(m_agentLabels[id]);
        }
        // set the position
        if (agentData.state().position().isValid()) {
            // convert the position
            CoordinatesConversionPtr coordinatesConversion = m_data->coordinatesConversion();
            if (!coordinatesConversion.isNull()) {
                PositionPixels imagePosition = coordinatesConversion->worldToImagePosition(agentData.state().position());
                m_agentLabels[id]->setPos(imagePosition.x(), imagePosition.y());
                m_agentLabels[id]->setVisible(true);
            } else {
                qDebug() << Q_FUNC_INFO << "Unable to convert agent's world position to the image position";
            }
        } else {
            qDebug() << Q_FUNC_INFO << QString("The postion of %1 is invalid").arg(agentData.label());
        }
    }
}

/*!
 * Triggered on arrival of the new data.
 */
void ViewerWidget::updateAgents(QList<AgentDataWorld> agentDataList)
{
    if (!m_showAgents)
        return;

    // first hide all the items on the scene
    foreach (AgentItem* agentItem, m_agents.values())
        agentItem->hide();

    // update the item's positions, the map itself by adding new
    // items
    foreach (AgentDataWorld agentData, agentDataList) {
        QString id = agentData.id();
        // if an item corresponding to the agent is not yet in the list
        // then add it
        if (! m_agents.contains(id)) {
            m_agents[id] = new AgentItem();
            if (m_agentColors.contains(id))
                m_agents[id]->setColor(m_agentColors[id]);
            m_scene->addItem(m_agents[id]);
        }
        // set the position
        if (agentData.state().position().isValid()) {
            // convert the position
            CoordinatesConversionPtr coordinatesConversion = m_data->coordinatesConversion();
            if (!coordinatesConversion.isNull()) {
                PositionPixels imagePosition = coordinatesConversion->worldToImagePosition(agentData.state().position());
                m_agents[id]->setPos(imagePosition.x(), imagePosition.y());

                OrientationRad orientation = coordinatesConversion->worldToImageOrientation(agentData.state().position(),
                                                                                               agentData.state().orientation());
                m_agents[id]->setVisible(true);
                if (orientation.isValid()) {
                    m_agents[id]->setHasOrientation(true);
                    m_agents[id]->setRotation(orientation.angleDeg() );
                } else {
                    // we hide the item without known orientation to prevent unpleasant visual effects
                    m_agents[id]->setHasOrientation(false);
                    m_agents[id]->hide();
                }
            } else {
                qDebug() << Q_FUNC_INFO << "Unable to convert agent's world position to the image position";
            }
        } else {
            qDebug() << Q_FUNC_INFO << QString("The postion of %1 is invalid").arg(agentData.label());
        }
    }
}

/*!
 * Triggered when a right button click is received from the scene.
 */
void ViewerWidget::onRightButtonClicked(QPointF scenePosition)
{
    PositionPixels imagePosition(scenePosition.x(), scenePosition.y());
    PositionMeters worldPosition;
    if (convertScenePosition(imagePosition, worldPosition))
        emit notifyRightButtonClick(worldPosition);
}

/*!
 * Converts the scene position to PositionPixels and PositionMeters.
 */
bool ViewerWidget::convertScenePosition(const PositionPixels& imagePosition,
                                        PositionMeters& worldPosition)
{
    if (!m_data.isNull()) {
        CoordinatesConversionPtr coordinatesConversion = m_data->coordinatesConversion();
        if (!coordinatesConversion.isNull()){
            worldPosition = coordinatesConversion->imageToWorldPosition(imagePosition);
            return true;
        }
    }
    return false;
}

/*!
 * Converts the scene position to PositionPixels and PositionMeters.
 */
bool ViewerWidget::convertWorldPosition(const PositionMeters& worldPosition,
                                             PositionPixels& imagePosition)
{
    if (!m_data.isNull()) {
        CoordinatesConversionPtr coordinatesConversion = m_data->coordinatesConversion();
        if (!coordinatesConversion.isNull()){
            imagePosition = coordinatesConversion->worldToImagePosition(worldPosition);
            return true;
        }
    }
    return false;
}

/*!
 * Set the flag that defines if the agents must be shown.
 */
void ViewerWidget::setShowAgents(bool shownAgents)
{
    m_showAgents = shownAgents;
    // hide agents if shown
    foreach (AgentItem* agentItem, m_agents.values())
        agentItem->setVisible(m_showAgents);
}

/*!
 * Request to update areas on the scene.
 */
void ViewerWidget::updateAreas(QString agentId, QList<AnnotatedPolygons> polygonsToDraw)
{
    if (!m_showNavigationData)
        return;

    // add a group if doesn't exist yet
    if (!m_navigationData.contains(agentId)) {
        m_navigationData.insert(agentId, AgentDataItemGroup());
    }
    AgentDataItemGroup& itemsGroup = m_navigationData[agentId];

    // first remove previous polygons
    for (AnnotatedPolygonItem* polygonItem : itemsGroup.m_areas) {
        m_scene->removeItem(polygonItem);
        delete polygonItem;
    }
    itemsGroup.m_areas.clear();

    // add new items
    for (AnnotatedPolygons annotatedPolygons : polygonsToDraw) {
        for (WorldPolygon worldPolygon : annotatedPolygons.polygons) {
            // convert points
            QPolygonF imagePolygon;
            for (PositionMeters worldPosition : worldPolygon) {
                PositionPixels imagePosition;
                if (convertWorldPosition(worldPosition, imagePosition) && imagePosition.isValid()) {
                    imagePolygon.append(QPointF(imagePosition.x(), imagePosition.y()));
                } else {
                    qDebug() << Q_FUNC_INFO << QString("Not able to convert %1 to image coordinates").arg(worldPosition.toString());
                    imagePolygon.clear();
                    break;
                }
            }
            // draw the polygon
            if (imagePolygon.size() > 0) {
                AnnotatedPolygonItem* item = new AnnotatedPolygonItem(imagePolygon, annotatedPolygons.label);
                item->setColor(annotatedPolygons.color);
                m_scene->addItem(item);
                // store the polygon
                itemsGroup.m_areas.append(item);
                // hide if it's not the current agent
                itemsGroup.m_areas.last()->setVisible(m_currentAgentId == agentId);
                // need to position the item to (0,0) in order to the polygon was placed correctly
                item->setPos(0, 0);
            }
        }
    }
}

/*!
 * Request to update the trajectory on the scene.
 */
void ViewerWidget::updateTrajectory(QString agentId, QQueue<PositionMeters> worldPolygon)
{
    if (!m_showNavigationData)
        return;

    // add a group if doesn't exist yet
    if (!m_navigationData.contains(agentId)) {
        m_navigationData.insert(agentId, AgentDataItemGroup());
    }
    AgentDataItemGroup& itemsGroup = m_navigationData[agentId];

    // convert to image polygon
    QPolygonF imagePolygon;
    for (PositionMeters worldPosition : worldPolygon) {
        PositionPixels imagePosition;
        if (convertWorldPosition(worldPosition, imagePosition) && imagePosition.isValid()) {
            imagePolygon.append(QPointF(imagePosition.x(), imagePosition.y()));
        } else {
            qDebug() << Q_FUNC_INFO << QString("Not able to convert %1 to image coordinates").arg(worldPosition.toString());
            imagePolygon.clear();
            break;
        }
    }

    // add a trajectory item is doesn't exist yet
    if (!itemsGroup.m_trajectory) {
        itemsGroup.m_trajectory = new TrajectoryItem(imagePolygon);
        if (m_agentColors.contains(agentId))
            itemsGroup.m_trajectory->setColor(m_agentColors[agentId]);
        m_scene->addItem(itemsGroup.m_trajectory);
        // hide if it's not the current agent
        itemsGroup.m_trajectory->setVisible(m_currentAgentId == agentId);
        // need to position the item to (0,0) in order to the polygon was placed correctly
        itemsGroup.m_trajectory->setPos(0, 0);
    } else {
        itemsGroup.m_trajectory->setTrajectory(imagePolygon);
    }
}

/*!
 * Request to update the target on the scene.
 */
void ViewerWidget::updateTarget(QString agentId, PositionMeters worldPosition)
{
    if (!m_showNavigationData)
        return;

    // add a group if doesn't exist yet
    if (!m_navigationData.contains(agentId)) {
        m_navigationData.insert(agentId, AgentDataItemGroup());
    }
    AgentDataItemGroup& itemsGroup = m_navigationData[agentId];

    // convert to image position
    PositionPixels imagePosition;
    if (convertWorldPosition(worldPosition, imagePosition) && imagePosition.isValid()) {
        // add a target item if doesn't exist yet
        if (!itemsGroup.m_target) {
            itemsGroup.m_target = new TargetItem();
            if (m_agentColors.contains(agentId))
                itemsGroup.m_target->setColor(m_agentColors[agentId]);
            m_scene->addItem(itemsGroup.m_target);
            // hide if it's not the current agent
            itemsGroup.m_target->setVisible(m_currentAgentId == agentId);
        }
        itemsGroup.m_target->setPos(QPointF(imagePosition.x(), imagePosition.y()));
    } else {
        qDebug() << Q_FUNC_INFO << QString("Not able to convert %1 to image coordinates").arg(worldPosition.toString());
        // clean up
        if (itemsGroup.m_target) {
            m_scene->removeItem(itemsGroup.m_target);
            delete itemsGroup.m_target;
            itemsGroup.m_target = nullptr;
        }
    }
}

/*!
 * Updates the agent's color.
 */
void ViewerWidget::updateColor(QString agentId, QColor color)
{
    m_agentColors[agentId] = color;
    // update the agents
    if (m_agentLabels.contains(agentId))
        m_agentLabels[agentId]->setColor(color);
    if (m_agents.contains(agentId))
        m_agents[agentId]->setColor(color);
    // update the navigation data
    if (m_navigationData.contains(agentId)) {
        if (m_navigationData[agentId].m_target)
            m_navigationData[agentId].m_target->setColor(color);
        if (m_navigationData[agentId].m_trajectory)
            m_navigationData[agentId].m_trajectory->setColor(color);
    }
}

/*!
 * Show/hides the navigation data.
 */
void ViewerWidget::showNavigationData(bool showData)
{
    if (showData != m_showNavigationData) {
        m_showNavigationData = showData;
        setNavigationDataVisible(m_currentAgentId, m_showNavigationData);
    }
}

/*!
 * Only one agent data can be shown in a time.
 */
void ViewerWidget::updateCurrentAgent(QString id)
{
    if (id != m_currentAgentId) {
        // if the data is to be show
        if (m_showNavigationData) {
            // hide the navigation data for previous current item
            setNavigationDataVisible(m_currentAgentId, false);
            // show the navigation data for new item
            setNavigationDataVisible(id, true);
        }
        // set the new current agent
        m_currentAgentId = id;
    }
}

/*!
 * Shows/hide the navigation data for given agent.
 */
void ViewerWidget::setNavigationDataVisible(QString agentId, bool visible)
{
    if (m_navigationData.contains(agentId)) {
        if (m_navigationData[agentId].m_target)
            m_navigationData[agentId].m_target->setVisible(visible);
        if (m_navigationData[agentId].m_trajectory)
            m_navigationData[agentId].m_trajectory->setVisible(visible);
        for (auto& item : m_navigationData[agentId].m_areas) {
            item->setVisible(visible);
        }
    }
}

/*!
 * Context menu.
 */
void ViewerWidget::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);
    menu.addAction(m_adjustAction);

    menu.exec(event->globalPos());
}

/*!
 * Sets the flag that defines if the viwer is adjusted to the view size
 * automatically.
 */
void ViewerWidget::setAutoAdjust(bool value)
{
    m_autoAdjust = value;
    if (m_autoAdjust)
        adjust();
}

/*!
 * Resize event.
 */
void ViewerWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if (m_autoAdjust)
        adjust();
}

/*!
 * Updates the setup outline polygon.
 */
void ViewerWidget::updateSetup(AnnotatedPolygons annotatedPolygon)
{
    if (! m_showSetup)
        return;

    // the setup map never changes during the program lifetime
    // thus if it was already drawn once no need to re-compute
    if (m_setupPolygon)
        return;

    // otherwise create the item
    for (WorldPolygon worldPolygon : annotatedPolygon.polygons) {
        // convert points
        QPolygonF imagePolygon;
        for (PositionMeters worldPosition : worldPolygon) {
            PositionPixels imagePosition;
            if (convertWorldPosition(worldPosition, imagePosition) && imagePosition.isValid()) {
                imagePolygon.append(QPointF(imagePosition.x(), imagePosition.y()));
            } else {
                qDebug() << Q_FUNC_INFO << QString("Not able to convert %1 to image coordinates").arg(worldPosition.toString());
                imagePolygon.clear();
                break;
            }
        }

        // draw the polygon
        if (imagePolygon.size() > 0) {
            if (! m_setupPolygon) {
                m_setupPolygon = new AnnotatedPolygonItem(imagePolygon, annotatedPolygon.label);
                m_scene->addItem(m_setupPolygon);
                m_setupPolygon->setVisible(m_showSetup);
                // need to position the item to (0,0) in order to the polygon was placed correctly
                m_setupPolygon->setPos(0, 0);
            }
            m_setupPolygon->setColor(annotatedPolygon.color);
        }
    }
}

/*!
 * Set the flag that defines if the setup must be shown.
 */
void ViewerWidget::setShowSetup(bool showSetup)
{
    m_showSetup = showSetup;
    // hide setup if necessary if shown
    if (m_setupPolygon)
        m_setupPolygon->setVisible(m_showSetup);
}
