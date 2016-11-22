#ifndef CATS2_FRAME_SCENE_HPP
#define CATS2_FRAME_SCENE_HPP

#include <QtWidgets/QGraphicsScene>

/*!
 * The scene class to show the camera frames and agent's positions.
 * It tracks the mouse's position and sends it out.
 */
class FrameScene : public QGraphicsScene
{
    Q_OBJECT
public:
    //! Constructor.
    explicit FrameScene(QObject *parent = nullptr);

signals:
    //! Sends out the mouse position.
    void mouseMoved(QPointF scenePosition);
    //! Sends out the mouse position on click.
    void mouseClicked(QPointF scenePosition);

protected:
    //! Triggered when the mouse is moved on the scene.
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
    //! Triggered when the mouse is clickied on the scene.
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) override;

};

#endif // CATS2_FRAME_SCENE_HPP
