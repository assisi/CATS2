#include "RobotsWidget.hpp"
#include "ui_RobotsWidget.h"

#include "FishBot.hpp"
#include "ControlLoop.hpp"
#include "gui/RobotControlWidget.hpp"

#include <QtCore/QDebug>

/*!
 * Constructor.
 */
RobotsWidget::RobotsWidget(ControlLoopPtr contolLoop, QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::RobotsWidget)
{
    m_ui->setupUi(this);

    // connect tab change with the robot selection
    connect(this, &RobotsWidget::notifySelectedRobotChanged, contolLoop.data(), &ControlLoop::selectRobot);

    // select a robot when a tab is changed
    connect(m_ui->robotsTabWidget, &QTabWidget::currentChanged,
            [=](int index) {
                emit notifySelectedRobotChanged(m_ui->robotsTabWidget->tabText(index));
            });
    // populate the tabs with the robots' information
    for (auto& robot : contolLoop->robots()) {
        m_ui->robotsTabWidget->addTab(new RobotControlWidget(robot), robot->name());
    }
    // select the first tab
    if (m_ui->robotsTabWidget->count() > 0) {
        m_ui->robotsTabWidget->setCurrentIndex(0);
    }
}

/*!
 * Destructor.
 */
RobotsWidget::~RobotsWidget()
{
    qDebug() << Q_FUNC_INFO << "Destroying the object";
    delete m_ui;
}
