#ifndef CATS2_ROBOTS_WIDGET_HPP
#define CATS2_ROBOTS_WIDGET_HPP

#include "RobotControlPointerTypes.hpp"

#include <QtWidgets/QWidget>

namespace Ui {
class RobotsWidget;
}

/*!
 * Shows all present robots.
 */
class RobotsWidget : public QWidget
{
    Q_OBJECT

public:
    //! Constructor.
    explicit RobotsWidget(ControlLoopPtr contolLoop, QWidget *parent = nullptr);
    //! Destructor.
    virtual ~RobotsWidget() final;

signals:
    //! When a tab is changed the corresponding robot is considered as
    //! selected.
    void notifySelectedRobotChanged(QString robotName);

protected:
    //! Intercepts all events.
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    //! The gui form.
    Ui::RobotsWidget *m_ui;
};

#endif // CATS2_ROBOTS_WIDGET_HPP
