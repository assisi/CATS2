#include "RobotsWidget.hpp"
#include "ui_RobotsWidget.h"

#include "FishBot.hpp"
#include "ControlLoop.hpp"
#include "gui/RobotControlWidget.hpp"

#include <QtCore/QDebug>
#include <QtCore/QEvent>
#include <QtGui/QKeyEvent>
#include <QtGui/QPixmap>

/*!
 * Constructor.
 */
RobotsWidget::RobotsWidget(ControlLoopPtr contolLoop, QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::RobotsWidget)
{
    m_ui->setupUi(this);

    // fill the icons map
    m_connectionIcons.insert(ConnectionStatus::CONNECTED, QIcon(":/connected.png"));
    m_connectionIcons.insert(ConnectionStatus::PENDING, QIcon(":/pending-connection.png"));
    m_connectionIcons.insert(ConnectionStatus::DISCONNECTED, QIcon(":/not-connected.png"));

    // connect tab change with the robot selection
    connect(this, &RobotsWidget::notifySelectedRobotChanged,
            contolLoop.data(), &ControlLoop::selectRobot);

    // select a robot when a tab is changed
    connect(m_ui->robotsTabWidget, &QTabWidget::currentChanged,
            [=](int index) {
                emit notifySelectedRobotChanged(m_ui->robotsTabWidget->tabText(index));
            });
    // populate the tabs with the robots' information
    for (auto& robot : contolLoop->robots()) {
        int newTabIndex = m_ui->robotsTabWidget->addTab(new RobotControlWidget(robot), robot->name());

        // set the current connection status
        ConnectionStatus status = ConnectionStatus::DISCONNECTED;
        if (robot->isConnected())
            status = ConnectionStatus::CONNECTED;
        m_ui->robotsTabWidget->setTabIcon(newTabIndex, m_connectionIcons[status]);
        // update the connection status
        connect(robot.data(), &FishBot::notifyConnectionStatusChanged,
                [=](QString name, ConnectionStatus newStatus) {
                    for (int index = 0; index < m_ui->robotsTabWidget->count(); ++index)
                        if (m_ui->robotsTabWidget->tabText(index) == name) {
                            m_ui->robotsTabWidget->setTabIcon(index,
                                                              m_connectionIcons[newStatus]);
                        }
                });
    }
    // select the first tab
    if (m_ui->robotsTabWidget->count() > 0) {
        m_ui->robotsTabWidget->setCurrentIndex(0);
    }
    // install the event filter
    qApp->installEventFilter(this);
}

/*!
 * Destructor.
 */
RobotsWidget::~RobotsWidget()
{
    qDebug() << "Destroying the object";
    delete m_ui;
}

/*!
 * Intercepts all events. If it's a "number" key pressed than
 * the corresponding robot is selected.
 */
bool RobotsWidget::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        bool isNumber;
        int index = keyEvent->text().toInt(&isNumber) - 1; // map to [0,...]
        if (isNumber && keyEvent->modifiers().testFlag(Qt::AltModifier) &&
                (index >= 0) && (index < m_ui->robotsTabWidget->count()))
        {
            m_ui->robotsTabWidget->setCurrentIndex(index);
            return true;
        } else
            return false;
    }
    return false;
}
