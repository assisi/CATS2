#ifndef CATS2_ROBOT_CONTROL_WIDGET_HPP
#define CATS2_ROBOT_CONTROL_WIDGET_HPP

#include "RobotControlPointerTypes.hpp"

#include <QWidget>

namespace Ui {
class RobotControlWidget;
}

/*!
 * A widget to set the behavior of given robot.
 */
class RobotControlWidget : public QWidget
{
    Q_OBJECT

public:
    //! Constructor.
    explicit RobotControlWidget(FishBotPtr robot, QWidget *parent = 0);
    //! Destructor
    virtual ~RobotControlWidget() final;

private:
    //! The gui form.
    Ui::RobotControlWidget *m_ui;
    //! The robot to which corresponds this widget.
    FishBotPtr m_robot;
};

#endif // CATS2_ROBOT_CONTROL_WIDGET_HPP
