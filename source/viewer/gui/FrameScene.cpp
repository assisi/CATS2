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
 * Triggered when the mouse is clicked on the scene. If it's a right click then
 * it's transfered futher.
 */
void FrameScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (mouseEvent->button() == Qt::RightButton || mouseEvent->button() == Qt::LeftButton) {
        emit buttonClicked(mouseEvent->button(), mouseEvent->scenePos());
    }
    QGraphicsScene::mousePressEvent(mouseEvent);
}
