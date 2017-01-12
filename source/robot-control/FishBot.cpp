#include "FishBot.hpp"
#include "control-modes/ControlMode.hpp"
#include "control-modes/ControlTarget.hpp"

#include "dbusinterface.h"

#include <AgentData.hpp>

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>

/*!
 * Constructor.
 */
FishBot::FishBot(QString id) :
    QObject(nullptr),
    m_id(id),
    m_name(QString("Fish_bot_%1").arg(m_id)),
    m_ledColor(Qt::black),
    m_state(),
    m_robotInterface(nullptr),
    m_experimentManager(this),
    m_controlStateMachine(this),
    m_navigation(this)
{
    // control areas
    connect(&m_experimentManager, &ExperimentManager::notifyPolygons,
            [=](QList<AnnotatedPolygons> polygons)
            {
                emit notifyControlAreasPolygons(m_id, polygons);
            });
    // navigation data
    connect(&m_navigation, &Navigation::notifyTargetPositionChanged,
            [=](PositionMeters position)
            {
                emit notifyTargetPositionChanged(m_id, position);
            });
    // TODO : add the trajectory here
    // controller data
    connect(&m_experimentManager, &ExperimentManager::notifyControllerChanged,
            this, &FishBot::notifyControllerChanged);
    // control modes
    connect(&m_controlStateMachine, &ControlModeStateMachine::notifyControlModeChanged,
            this, &FishBot::notifyControlModeChanged);
    connect(&m_navigation, &Navigation::notifyMotionPatternChanged,
            this, &FishBot::notifyMotionPatternChanged);
    connect(&m_navigation, &Navigation::notifyMotionPatternFrequencyDividerChanged,
            this, &FishBot::notifyMotionPatternFrequencyDividerChanged);
}

/*!
 * Destructor.
 */
FishBot::~FishBot()
{
    qDebug() << Q_FUNC_INFO << "Destroying the object";
    // TODO : to remove the callback, dbus interface must be modified for this.
}

/*!
 * Sets the robot's interface.
 */
void FishBot::setRobotInterface(Aseba::DBusInterfacePtr robotInterface)
{
    m_robotInterface = robotInterface;
    // TODO : to add a callback that sets the robot's state from the event
}

/*!
 * Inititialises the robot's firmaware.
 */
void FishBot::setupConnection(int robotIndex)
{
    if (m_robotInterface.data()) {
        // FIXME : in the multi-robot/node mode aseba doesn't provide the node list correctly
//        if (m_robotInterface->nodeList.contains(m_name)) {
            QString scriptDirPath = QCoreApplication::applicationDirPath() + QDir::separator() + "aesl";
            QString scriptPath = scriptDirPath + QDir::separator() + m_name + ".aesl";
            if (QFileInfo(scriptPath).exists()) {
                m_robotInterface->loadScript(scriptPath);
                // set the robots id
                Values data;
                data.append(robotIndex);
                m_robotInterface->setVariable(m_name, "IDControl", data);
            } else {
                qDebug() << Q_FUNC_INFO << QString("Script %1 could not be found.").arg(scriptPath);
            }
//        }
    } else {
        qDebug() << Q_FUNC_INFO << "The robot's interface is not set";
    }
}

/*!
 * Steps the control for the robot.
 */
void FishBot::stepControl()
{
    // check the experiment controller to see if the control mode is to be changed
    if (m_experimentManager.isActive()) {
        stepExperimentManager();
    }

    // check the incoming events to see if the control mode is to be changed
    // due to the low-power or the obstacle-avoidance routine - THIS CAN BE DONE
    // IN THE CALLBACK AND MANAGED BY THE PRIORITY LOGICS OR BY A FLAG ON THE
    // CONTROL MODE "ACCEPTS CONTROL MODE CHANGE"

    // step the control mode state machine with the robot's position and
    // other agents positions.
    // TODO : to define how to pass the other agents positions
    ControlTargetPtr controlTarget = m_controlStateMachine.step();

    // step the navigation with the resulted target values
    // it's the navigation that sends commands to robots via the dbus interface
    m_navigation.step(controlTarget);
}

/*!
 * Sets the control mode.
 */
void FishBot::setControlMode(ControlModeType::Enum type)
{
    m_controlStateMachine.setControlMode(type);

    // a check for a special case - joystick controlled manual mode, only one
    // robot can be controlled, hence other robots in manual mode switch to idle
    if (m_controlStateMachine.currentControlMode() == ControlModeType::MANUAL)
        emit notifyInManualMode(m_id);
}

/*! Received positions of all tracked robots, finds and sets the one
 * corresponding to this robot and keeps the rest in case it's needed
 * by the control mode.
 */
void FishBot::setRobotsData(QList<AgentDataWorld> robotsData)
{
    m_otherRobotsData.clear();
    foreach (AgentDataWorld agentData, robotsData) {
        if (agentData.id() == m_id)
            this->setState(agentData.state());
        else
            m_otherRobotsData.append(agentData);
    }
}

/*!
 * Received positions of all tracked fish, keeps them in case it's
 * needed by the control mode.
 */
void FishBot::setFishStates(QList<StateWorld> fishStates)
{
    m_fishStates = fishStates;
}

/*!
 * The target position received from the viewer; it's transfered further
 * to the state machine.
 */
void FishBot::goToPosition(PositionMeters position)
{
    if (m_controlStateMachine.currentControlMode() == ControlModeType::GO_TO_POSITION) {
        m_controlStateMachine.setTargetPosition(position);
    }
}

/*!
 * Checks that the current control modes can generate targets with
 * different motion patterns.
 */
bool FishBot::supportsMotionPatterns()
{
    return m_controlStateMachine.supportsMotionPatterns();
}

/*!
 * Sets the motion pattern.
 */
void FishBot::setMotionPattern(MotionPatternType::Enum type)
{
    m_navigation.setMotionPattern(type);
}

/*!
 * Sets the motion pattern frequency divider. The goal is to send commands
 * less often to keep the network load low.
 */
void FishBot::setMotionPatternFrequencyDivider(MotionPatternType::Enum type,
                                               int frequencyDivider)
{
    m_navigation.setMotionPatternFrequencyDivider(type, frequencyDivider);
}

/*!
 * Return the motion pattern frequency divider.
 */
int FishBot::motionPatternFrequencyDivider(MotionPatternType::Enum type)
{
    return m_navigation.motionPatternFrequencyDivider(type);
}

/*!
 * Sets the control parameters based on the control map.
 */
void FishBot::stepExperimentManager()
{
    ExperimentController::ControlData controlData = m_experimentManager.step();
    if (controlData.controlMode != ControlModeType::UNDEFINED) {
        setControlMode(controlData.controlMode);

        // if the control map is set and we can set motion patterns
        if ((currentControlMode() == controlData.controlMode) && supportsMotionPatterns())
            if (controlData.motionPattern != MotionPatternType::UNDEFINED)
                setMotionPattern(controlData.motionPattern);
    }
}
