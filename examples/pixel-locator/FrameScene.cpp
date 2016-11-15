#include "FrameScene.hpp"

#include <QtCore/QtDebug>
#include <QtWidgets/QGraphicsSceneMouseEvent>

/*!
 * Constructor.
 */
FrameScene::FrameScene(QObject *parent) : QGraphicsScene(parent)
{

}

/*!
 * Triggered when the mouse is moved on the scene.
 */
void FrameScene::mouseMoveEvent( QGraphicsSceneMouseEvent * mouseEvent )
{
    QGraphicsScene::mouseMoveEvent(mouseEvent);
    emit mouseMoved(mouseEvent->scenePos());
}

/*!
 * Triggered when the mouse is clickied on the scene.
 */
void FrameScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    QGraphicsScene::mouseMoveEvent(mouseEvent);
    emit mouseClicked(mouseEvent->scenePos());
}
