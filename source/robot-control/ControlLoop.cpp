#include "ControlLoop.hpp"
#include "settings/RobotControlSettings.hpp"
#include "FishBot.hpp"

/*!
 * Constructor.
 */
ControlLoop::ControlLoop() :
    QObject(nullptr),
    m_robotsInterface(new Aseba::DBusInterface(), &QObject::deleteLater),
    m_selectedRobot()
{
    // create the robots
    foreach (QString id, RobotControlSettings::get().ids()) {
        m_robots.append(FishBotPtr(new FishBot(id), &QObject::deleteLater));
        m_robots.last()->setRobotInterface(m_robotsInterface);
    }

    // conect the robots
    if (m_robotsInterface->checkConnection()) {
        // TODO : to make this initialization logics more robust, to allow the
        // (re)connection when medula is (re)launched after CATS2.
        initializeRobotsInterfaces();
    } else {
        qDebug() << Q_FUNC_INFO << "The connection with the dbus could not be established.";
    }

    // start the control timer
    int stepMsec = static_cast<int>(1000. / RobotControlSettings::get().controlFrequencyHz());
    connect(&m_controlLoopTimer, &QTimer::timeout, [=](){ step(); });
    m_controlLoopTimer.start(stepMsec);
}

/*!
 * Destructor.
 */
ControlLoop::~ControlLoop()
{
    // stop the timer
    m_controlLoopTimer.stop();

    // stop the robots before shutting down
    for (auto& robot : m_robots) {
        robot->setControlMode(ControlModeType::IDLE);
    }
    // step the control
    step();
}

/*!
 * The main step of the control system.
 */
void ControlLoop::step()
{
    for (auto& robot : m_robots) {
        robot->stepControl();
    }
}

/*!
 * Loads and initialized the robots' firmware scripts.
 */
void ControlLoop::initializeRobotsInterfaces()
{
    for (int index = 0; index < m_robots.size(); ++index) {
        m_robots[index]->setupConnection(index);
    }
}

/*!
 * Receives the resutls from the tracking system and transfers it further to
 * the robots.
 */
void ControlLoop::onTrackingResultsReceived(QList<AgentDataWorld> agentsData)
{
    // the data of robots
    QList<AgentDataWorld> robotsData;
    // the states of fish
    QList<StateWorld> fishStates;

    foreach (AgentDataWorld agentData, agentsData) {
        if (agentData.type() == AgentType::FISH_CASU) {
            robotsData.append(agentData);
        } else if (agentData.type() == AgentType::FISH) {
            fishStates.append(agentData.state());
        }
    }
    // transfers the data to all robots
    for (auto& robot : m_robots) {
        robot->setRobotsData(robotsData);
        robot->setFishStates(fishStates);
    }
}

/*!
 * Set the selected robot from the name.
 */
void ControlLoop::selectRobot(QString name)
{
    for (auto& robot : m_robots) {
        if (robot->name() == name) {
            qDebug() << Q_FUNC_INFO << name << "is selected";
            m_selectedRobot = robot;
            break;
        }
    }
}

/*!
 * The target position received from the viewer; it's set as a target
 * to the selected robots.
 */
 void ControlLoop::goToPosition(PositionMeters position)
 {
     if (m_selectedRobot.data()) {
         m_selectedRobot->goToPosition(position);
     }
 }
