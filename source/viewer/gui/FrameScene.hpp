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
    //! Informs that a right button is clicked.
    void rightButtonClicked(QPointF scenePosition);

protected:
    //! Triggered when the mouse is moved on the scene.
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
    //! Triggered when the mouse is clicked on the scene.
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
};

#endif // CATS2_FRAME_SCENE_HPP
