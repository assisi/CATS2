#include "RobotsWidget.hpp"
#include "ui_RobotsWidget.h"

#include "FishBot.hpp"
#include "gui/RobotControlWidget.hpp"

#include <QtCore/QDebug>

/*!
 * Constructor.
 */
RobotsWidget::RobotsWidget(QList<FishBotPtr> robots, QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::RobotsWidget)
{
    m_ui->setupUi(this);

    // populate the tabs with the robots' information
    foreach (FishBotPtr robot, robots) {
        m_ui->robotsTabWidget->addTab(new RobotControlWidget(robot), robot->name());
    }
    // select a robot when a tab is changed
    connect(m_ui->robotsTabWidget, &QTabWidget::currentChanged,
            [=](int index) {
                emit selectRobot(m_ui->robotsTabWidget->tabText(index));
            });
    // select the first tab
    if (m_ui->robotsTabWidget->count() > 0)
        m_ui->robotsTabWidget->setCurrentIndex(0);
}

/*!
 * Destructor.
 */
RobotsWidget::~RobotsWidget()
{
    qDebug() << Q_FUNC_INFO << "Destroying the object";
    delete m_ui;
}
