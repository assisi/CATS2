#include "ControlLoop.hpp"
#include "settings/RobotControlSettings.hpp"
#include "FishBot.hpp"
#include "dbusinterface.h"

/*!
 * Constructor.
 */
ControlLoop::ControlLoop() :
    QObject(nullptr),
    m_robotsInterface(new Aseba::DBusInterface()),
    m_selectedRobot(),
    m_sendNavigationData(false),
    m_sendControlAreas(false)
{
    // create the robots
    for (QString id : RobotControlSettings::get().ids()) {
        m_robots.append(FishBotPtr(new FishBot(id)));
        m_robots.last()->setLedColor(RobotControlSettings::get().robotSettings(id).ledColor());
        m_robots.last()->setRobotInterface(m_robotsInterface);

        // ensure that only one robot can be in manual mode
        connect(m_robots.last().data(), &FishBot::notifyInManualMode,
                [=](QString senderId)
                {
                    for (auto& robot : m_robots)
                        if ((robot->id() != senderId) && (robot->currentControlMode() == ControlModeType::MANUAL))
                            robot->setControlMode(ControlModeType::IDLE);
                });

        // send the cotrol areas for the _selected_ robot if the corresponding flag is set
        connect(m_robots.last().data(), &FishBot::notifyControlAreasPolygons,
                [=](QString agentId, QList<AnnotatedPolygons> polygons)
                {
                    if (m_sendControlAreas && (m_selectedRobot->id() == agentId))
                        emit notifyRobotControlAreasPolygons(agentId, polygons);
                });

        // send the robot trajectory for all robots if the corresponding flag is set
        connect(m_robots.last().data(), &FishBot::notifyTrajectoryChanged,
                [=](QString agentId, QQueue<PositionMeters> trajectory)
                {
                    if (m_sendNavigationData)
                        emit notifyRobotTrajectoryChanged(agentId, trajectory);
                });

        // send the robot target for all robots if the corresponding flag is set
        connect(m_robots.last().data(), &FishBot::notifyTargetPositionChanged,
                [=](QString agentId, PositionMeters position)
                {
                    if (m_sendNavigationData)
                        emit notifyRobotTargetPositionChanged(agentId, position);
                });
        // send the robot trajectory for all robots if the corresponding flag is set
        connect(m_robots.last().data(), &FishBot::notifyTrajectoryChanged,
                [=](QString agentId, QQueue<PositionMeters> trajectory)
                {
                    if (m_sendNavigationData)
                        emit notifyRobotTrajectoryChanged(agentId, trajectory);
                });

        // send the robot color
        connect(m_robots.last().data(), &FishBot::notifyLedColor, this, &ControlLoop::notifyRobotLedColor);
    }

    // conect the robots
    initializeRobotsInterfaces();

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
    qDebug() << Q_FUNC_INFO << "Destroying the object";

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
    if (m_robotsInterface->checkConnection()) {
        // TODO : to make this initialization logics more robust, to allow the
        // (re)connection when medula is (re)launched after CATS2.
        for (int index = 0; index < m_robots.size(); ++index) {
            m_robots[index]->setupConnection(index);
        }
    } else {
        qDebug() << Q_FUNC_INFO << "The connection with the dbus could not be established.";
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

//    qDebug() << Q_FUNC_INFO << robotsData.size() << fishStates.size();

    // transfers the data to all robots
    for (auto& robot : m_robots) {
        robot->setRobotsData(robotsData);
        // HACK : update only when any fish found, it's done to prevent setting
        // zero fish in a case when fish tracker is slower than the the robot
        // tracker and thus we don't receive its data in time; as a result in
        // this case the robot will be using the positions of fish previously
        // detected
        if (fishStates.size() > 0)
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
            if (m_selectedRobot != robot) {
                qDebug() << Q_FUNC_INFO << name << "is selected";
                m_selectedRobot = robot;
                // inform about the change
                emit notifySelectedRobotChanged(m_selectedRobot->id());
                // update the navigation data
                sendNavigationData(m_sendNavigationData);
            }
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

 /*!
  * Asks the robots to send their navigation data (trajectories, targets, etc).
  */
 void ControlLoop::sendNavigationData(bool sendData)
 {
     m_sendNavigationData = sendData;
     if (m_sendNavigationData) {
         for(auto& robot : m_robots) {
             robot->requestCurrentTarget();
             robot->requestTrajectory();
         }
     }
 }

 /*!
  * Asks to send the control areas for the selected robot.
  */
 void ControlLoop::sendControlAreas(bool sendAreas)
 {
     m_sendControlAreas = sendAreas;
     if (m_sendControlAreas && m_selectedRobot.data()) {
         m_selectedRobot->requestControlAreasPolygons();
     }
 }

 /*!
  * Asks to send the current robot id.
  */
 void ControlLoop::requestSelectedRobot()
 {
     emit notifySelectedRobotChanged(m_selectedRobot->id());
 }

 /*!
  * Asks to send the colors of all robots.
  */
 void ControlLoop::requestRobotsLedColors()
 {
     for(auto& robot : m_robots) {
         robot->requestLedColor();
     }
 }
