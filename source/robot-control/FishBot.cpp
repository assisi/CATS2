#include "FishBot.hpp"
#include "control-modes/ControlMode.hpp"
#include "control-modes/ControlTarget.hpp"

#include "interfaces/DashelInterface.hpp"
#include "statistics/StatisticsPublisher.hpp"

#include <settings/CommandLineParameters.hpp>

#include <AgentData.hpp>
#include <Timer.hpp>

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>

/*!
 * Constructor.
 */
FishBot::FishBot(QString id) :
    QObject(nullptr),
    m_id(id),
    m_firmwareId(-1),
    m_name(QString("Fish_bot_%1").arg(m_id)), // NOTE : don't change this name as the .aesl files are searched by it
    m_ledColor(Qt::black),
    m_state(),
    m_sharedRobotInterface(nullptr),
    m_uniqueRobotInterface(nullptr),
    m_experimentManager(this),
    m_controlStateMachine(this),
    m_navigation(this),
    m_computeStatistics(CommandLineParameters::get().publishRobotsStatistics())
{
    // control areas
    connect(&m_experimentManager, &ExperimentManager::notifyPolygons,
            [=](QList<AnnotatedPolygons> polygons)
            {
                emit notifyControlAreasPolygons(m_id, polygons);
            });

    // fish number by area
    connect(&m_experimentManager, &ExperimentManager::notifyFishNumberByAreas,
            [=](QMap<QString, int> fishNumberByArea)
            {
                emit notifyFishNumberByAreas(m_id, fishNumberByArea);
            });
    // navigation data
    connect(&m_navigation, &Navigation::notifyTargetPositionChanged,
            [=](PositionMeters position)
            {
                emit notifyTargetPositionChanged(m_id, position);
            });
    connect(&m_navigation, &Navigation::notifyTrajectoryChanged,
            [=](QQueue<PositionMeters> trajectory)
            {
                emit notifyTrajectoryChanged(m_id, trajectory);
            });

    // controller data
    connect(&m_experimentManager, &ExperimentManager::notifyControllerChanged,
            this, &FishBot::notifyControllerChanged);
    connect(&m_experimentManager, &ExperimentManager::notifyControllerChanged,
            [=]() { releaseModelArea(); });
    connect(&m_experimentManager, &ExperimentManager::notifyControllerStatus,
            this, &FishBot::notifyControllerStatus);
    connect(&m_experimentManager, &ExperimentManager::notifyCircularSetupTurningDirections,
            [=](QString fishTurningDirection, QString robotTurningDirection)
            {
                emit notifyCircularSetupTurningDirections(m_id,
                                                          fishTurningDirection,
                                                          robotTurningDirection);
            });
    connect(&m_experimentManager, &ExperimentManager::notifyCircularSetupStatistics,
            [=](double fishClockWisePercent, double fishCounterClockWisePercent, double robotClockWisePercent, double robotCounterClockWisePercent)
            {
                emit notifyCircularSetupStatistics(m_id,
                                                   fishClockWisePercent,
                                                   fishCounterClockWisePercent,
                                                   robotClockWisePercent,
                                                   robotCounterClockWisePercent);
            });

    // control modes
    connect(&m_controlStateMachine, &ControlModeStateMachine::notifyControlModeChanged,
            this, &FishBot::notifyControlModeChanged);
    connect(&m_controlStateMachine, &ControlModeStateMachine::notifyControlModeStatus,
            this, &FishBot::notifyControlModeStatus);
    connect(&m_navigation, &Navigation::notifyMotionPatternChanged,
            this, &FishBot::notifyMotionPatternChanged);
    connect(&m_navigation, &Navigation::notifyMotionPatternFrequencyDividerChanged,
            this, &FishBot::notifyMotionPatternFrequencyDividerChanged);
    connect(&m_navigation, &Navigation::notifyMotionPatternFrequencyDividerChanged,
            this, &FishBot::notifyMotionPatternFrequencyDividerChanged);
    connect(&m_navigation, &Navigation::notifyUsePathPlanningChanged,
            this, &FishBot::notifyUsePathPlanningChanged);
    connect(&m_navigation, &Navigation::notifyUseObstacleAvoidanceChanged,
            this, &FishBot::notifyUseObstacleAvoidanceChanged);

    // connect to the statistics module
    if (m_computeStatistics) {
        StatisticsPublisher::get().addStatistics(robotFishGroupStatisticsId());
        connect(this, &FishBot::updateStatistics,
                &StatisticsPublisher::get(), &StatisticsPublisher::updateStatistics);
    }
}

/*!
 * Destructor.
 */
FishBot::~FishBot()
{
    qDebug() << "Destroying the object";
    // close the connection if necessary
    closeUniqueConnection();
    // TODO : to remove the callback, dbus interface must be modified for this.
}

/*!
 * Sets the robot's interface.
 */
void FishBot::setSharedRobotInterface(DBusInterfacePtr sharedRobotInterface)
{
    m_sharedRobotInterface = sharedRobotInterface;
    // add a callback to process the incoming messages from the robot
    if (m_sharedRobotInterface) {
        m_sharedRobotInterface->
                connectEvent("PowerDown",
                             [this](const Values& data) {
                                // get the firmware's id
                                if ((data.size() > 0) && (data[0] == m_firmwareId))
                                    processPowerDownEvent();
                            });

        m_sharedRobotInterface->
                connectEvent("Obstacle",
                             [this](const Values& data) {
                                // get the firmware's id
                                if ((data.size() > 0) && (data[0] == m_firmwareId))
                                    processObstacleEvent();
                            });
    }
}

/*!
 * Inititialises the robot's firmaware.
 */
void FishBot::setupSharedConnection(int robotIndex)
{
    if (m_sharedRobotInterface) {
        if (m_sharedRobotInterface->isConnected()) {
            emit notifyConnectionStatusChanged(name(), ConnectionStatus::CONNECTED);
            // FIXME : in the multi-robot/node mode aseba doesn't provide the node list correctly
            // if (m_robotInterface->nodeList.contains(m_name)) {
            m_firmwareId = robotIndex;
            QString scriptDirPath = QCoreApplication::applicationDirPath() +
                    QDir::separator() + "aesl";
            QString scriptPath = scriptDirPath + QDir::separator() +
                    m_name + ".aesl";
            if (QFileInfo(scriptPath).exists()) {
                m_sharedRobotInterface->loadScript(scriptPath);
                // set the robots id
                Values data;
                data.append(m_firmwareId);
                m_sharedRobotInterface->setVariable(m_name, "IDControl", data);
                // set the obstacle avoidance on the robot
                m_navigation.updateLocalObstacleAvoidance();
            } else {
                qDebug() << QString("Script %1 could not be found.").arg(scriptPath);
            }
            // }
        } else {
            emit notifyConnectionStatusChanged(name(), ConnectionStatus::DISCONNECTED);
        }
    } else {
        qDebug() << QString("The %1 interface is not set").arg(m_name);
        emit notifyConnectionStatusChanged(name(), ConnectionStatus::DISCONNECTED);
    }
}

/*!
 * Convenience method. Inititialises the robot's firmware, it uses the stored
 * robot's index if available.
 */
void FishBot::setupSharedConnection()
{
    if (m_firmwareId >= 0) // if the index is valid
        setupSharedConnection(m_firmwareId);
    else
        qDebug() << "Can not reconnect to the robot, the firmware id is not "
                    "defined";
}

/*!
 * Connects to the robot via its own interface.
 */
void FishBot::setupUniqueConnection()
{
    // if the connection is open then close it
    closeUniqueConnection();

    // make new connection
    QString target = RobotControlSettings::get().robotSettings(m_id).connectionTarget();
    m_uniqueRobotInterface.reset(new DashelInterface());

    // FIXME : how to get a feedback if the connection is established
    qDebug() << QString("Connecting to %1").arg(m_name);
    m_uniqueRobotInterface->connectAseba(target);

    // wait until connected
    countDown(5.);

    if (m_uniqueRobotInterface->isConnected()) {
        emit notifyConnectionStatusChanged(name(), ConnectionStatus::CONNECTED);
        qDebug() << QString("Requesting the node description for %1").arg(m_name);
        // request the node's description
        m_uniqueRobotInterface->pingNetwork();
        // wait until the description received
        countDown(2.);

        qDebug() << QString("Loading the script on %1").arg(m_name);
        // load the script
        QString scriptDirPath = QCoreApplication::applicationDirPath() +
                QDir::separator() + "aesl";
        QString scriptPath = scriptDirPath + QDir::separator() +
                m_name + ".aesl";
        if (QFileInfo(scriptPath).exists()) {
            m_uniqueRobotInterface->loadScript(scriptPath);
            // set the obstacle avoidance on the robot
            m_navigation.updateLocalObstacleAvoidance();

            // add a callback to process the incoming messages from the robot
            m_uniqueRobotInterface->connectEvent("PowerDown",
                                           [this](const Values& data) {
                // no need to check the id for the unique connection, process
                // directly
                processPowerDownEvent();
            });

            // add a callback to process the incoming messages from the robot
            m_uniqueRobotInterface->connectEvent("Obstacle",
                                           [this](const Values& data) {
                // no need to check the id for the unique connection, process
                // directly
                processObstacleEvent();
            });
        } else {
            qDebug() << QString("Script %1 could not be found.").arg(scriptPath);
            // FIXME : should we disconnect here
        }
    } else  {
        qDebug() << QString("Could not connect to %1").arg(m_name);
    }
}

/*!
 * Closes the unique connection if it's open.
 */
void FishBot::closeUniqueConnection()
{
    // if the connection is open then close it
    if (m_uniqueRobotInterface.data() && m_uniqueRobotInterface->isConnected()) {
        m_uniqueRobotInterface->disconnectAseba();
        emit notifyConnectionStatusChanged(name(), ConnectionStatus::DISCONNECTED);
        m_uniqueRobotInterface.reset(nullptr);
    }
}

/*!
 * Steps the control for the robot.
 */
void FishBot::stepControl()
{
    // check the incoming events to see if the control mode is to be changed
    // due to the low-power or the obstacle-avoidance routine - THIS CAN BE DONE
    // IN THE CALLBACK AND MANAGED BY THE PRIORITY LOGICS OR BY A FLAG ON THE
    // CONTROL MODE "ACCEPTS CONTROL MODE CHANGE"
    stepSafetyLogics();

    // check the experiment controller to see if the control mode is to be changed
    if (m_experimentManager.isActive()) {
        stepExperimentManager();
    }

    // step the control mode state machine with the robot's position and
    // other agents positions.
    ControlTargetPtr controlTarget = m_controlStateMachine.step();

    // step the navigation with the resulted target values
    // it's the navigation that sends commands to robots via the dbus interface
    if (!controlTarget.isNull())
        m_navigation.step(controlTarget);
}

/*!
 * Sets the control mode.
 */
void FishBot::setControlMode(ControlModeType::Enum type)
{
    if (m_controlStateMachine.currentControlMode() != type) {
        // NOTE : this is disabled because
        // (1) the fish never stop
        // (2) it's better to manage the safety on the level of control modes
//        // stop the robot for safety reason
//        m_navigation.stop();

        // change the control mode
        m_controlStateMachine.setControlMode(type);

        // a check for a special case - joystick controlled manual mode, only one
        // robot can be controlled, hence other robots in manual mode switch to idle
        if (m_controlStateMachine.currentControlMode() == ControlModeType::MANUAL)
            emit notifyInManualMode(m_id);
    }
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
            setState(agentData.state());
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

    // updates the statistics
    if (m_computeStatistics)
        computeStatistics();
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
 * Updates the parameters of the model.
 */
void FishBot::setModelParameters(ControlModeType::Enum type, ModelParameters parameters)
{
    if (m_controlStateMachine.currentControlMode() == type) {
        m_controlStateMachine.setModelParameters(type, parameters);
    }
}

/*!
 * Limits the arena matrix of the model-based control mode by a mask. The mask
 * is defined by a set of polygons and is labeled with an id.
 */
void FishBot::limitModelArea(ControlModeType::Enum type,
                             QString maskId,
                             QList<WorldPolygon> allowedArea)
{
    if (m_controlStateMachine.currentControlMode() == type) {
        m_controlStateMachine.limitModelArea(type, maskId, allowedArea);
    }
}

/*!
 * Requests the state machine to remove the limitations on the model area that
 * were applied by a experiment controller.
 */
void FishBot::releaseModelArea()
{
    // no check is done on the current control mode as the model might be
    // released even when it's not active
    m_controlStateMachine.releaseModelArea(ControlModeType::FISH_MODEL);
    m_controlStateMachine.releaseModelArea(ControlModeType::FISH_MODEL_WITH_WALLS);
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
 * Sets the path planning usage flag in the navigation.
 */
void FishBot::setUsePathPlanning(bool usePathPlanning)
{
    m_navigation.setUsePathPlanning(usePathPlanning);
}

/*!
 * Sets the obstacle avoidance usage flag in the navigation.
 */
void FishBot::setUseObstacleAvoidance(bool useObstacleAvoidance)
{
    m_navigation.setUseObstacleAvoidance(useObstacleAvoidance);
}

/*!
 * Sets the control parameters based on the control map.
 */
void FishBot::stepExperimentManager()
{
    ExperimentController::ControlData controlData = m_experimentManager.step();
    if (controlData.controlMode != ControlModeType::UNDEFINED) {
        setControlMode(controlData.controlMode);

        // if the control map is set 
        if ((currentControlMode() == controlData.controlMode)) {
            // and if we can set motion patterns
            if (supportsMotionPatterns() && (controlData.motionPattern != MotionPatternType::UNDEFINED))
                setMotionPattern(controlData.motionPattern);
        
            // if the control mode uses the extra control data
            switch (controlData.controlMode) {
            case ControlModeType::GO_TO_POSITION:
            {
                if (controlData.data.canConvert<PositionMeters>()) {
                    PositionMeters targetPosition(controlData.data.value<PositionMeters>());
                    goToPosition(targetPosition);
                }
                break;
            }
            case ControlModeType::FISH_MODEL:
            case ControlModeType::FISH_MODEL_WITH_WALLS:
            {
                if (controlData.data.canConvert<AnnotatedPolygons>()) {
                    // the polygons that define the limits of the model
                    AnnotatedPolygons annotatedPolygons(controlData.data.value<AnnotatedPolygons>());
                    QString areaId = annotatedPolygons.label;
                    // the id of the limits as they will be reused
                    QString id = QString("%1%2")
                            .arg(ExperimentControllerType::toSettingsString(
                                     m_experimentManager.currentController()))
                            .arg(areaId.left(1).toUpper() + areaId.mid(1));
                    limitModelArea(controlData.controlMode, id, annotatedPolygons.polygons);
                } else if (controlData.data.canConvert<ModelParameters>()) {
                    // define if the model should follow or ignore the fish
                    ModelParameters parameters(controlData.data.value<ModelParameters>());
                    setModelParameters(controlData.controlMode, parameters);
                }
                break;
            }
            case ControlModeType::ZONE_BASED_FISH_MODEL:
            {
                if (controlData.data.canConvert<ModelParameters>()) {
                    // define if the model should follow or ignore the fish
                    ModelParameters parameters(controlData.data.value<ModelParameters>());
                    setModelParameters(controlData.controlMode, parameters);
                }
                break;
            }
            case ControlModeType::GO_STRAIGHT:
            case ControlModeType::IDLE:
            case ControlModeType::MANUAL:
            case ControlModeType::UNDEFINED:
            default:
                break;
            }
        }
    }
}

/*!
 * Sends an aseba event to the robot.
 */
void FishBot::sendEvent(const QString& eventName, const Values& data)
{
    if (m_sharedRobotInterface.data() && m_sharedRobotInterface->isConnected()) {
        m_sharedRobotInterface->sendEventName(eventName, data);
    } else if (m_uniqueRobotInterface.data() && m_uniqueRobotInterface->isConnected()) {
        m_uniqueRobotInterface->sendEventName(eventName, data);
    }
}

/*!
 * Returns the connection status of the robot.
 */
bool FishBot::isConnected() const
{
    if (m_sharedRobotInterface.data())
        return m_sharedRobotInterface->isConnected();
    else if (m_uniqueRobotInterface.data())
        return m_uniqueRobotInterface->isConnected();
    else
        return false;
}

/*!
 * A service method that makes the code to wait for a certatin time
 * by printing the count down.
 */
void FishBot::countDown(double timeOut)
{
    Timer connectionTimer;
    connectionTimer.reset();

    int elapsed = 1;
    std::cout << timeOut;
    while (!connectionTimer.isTimedOutSec(timeOut)) {
        if (connectionTimer.runTimeSec() >= elapsed) {
            std::cout << "..." << qRound(timeOut - elapsed) << std::flush;
            elapsed += 1;
        }
        continue;
    }
    std::cout << std::endl;
}

/*!
 * Implements the reaction of the robot on the power-down event.
 */
void FishBot::processPowerDownEvent()
{
    // if the power down arrives first time then the set the corresponding timer
    if (!m_powerDownStartTimer.isSet()) {
        qDebug() << QString("Power-down detected on %1, the connection will be "
                            "closed in %2 seconds")
                    .arg(m_name)
                    .arg(ToleratedPowerDownDurationSec);
        m_powerDownStartTimer.reset();
        // power down arriving, meaning that we risk to disconnect soon
        emit notifyConnectionStatusChanged(name(), ConnectionStatus::PENDING);
    }
    // in any case reset the last-power-down timer
    m_powerDownUpdateTimer.reset();
}

/*!
 * Implements the reaction of the robot on obstacle-detected event.
 */
void FishBot::processObstacleEvent()
{
    // if the obstacle-detected arrives first time then the set the
    // corresponding timer
    if (!m_obstacleDetectedUpdateTimer.isSet()) {
        qDebug() << QString("Obstacle-event detected on %1").arg(m_name);
    }
    // notify about the obstacle detection
    emit notifyObstacleDetectedStatusChanged(id(), true);
    // in any case reset the obstacle-detected timer
    m_obstacleDetectedUpdateTimer.reset();
}

/*!
 * Runs the emergency logics for the safety issues.
 */
void FishBot::stepSafetyLogics()
{
    // if the power down issue detected
    if (m_powerDownStartTimer.isSet()) {
        // if the power down message was not received recently, then stop tracking it
        if (m_powerDownUpdateTimer.isTimedOutSec(PowerDownUpdateTimeoutSec)) {
            qDebug() << QString("Power is restored on %1").arg(m_name);
            // clear timers
            m_powerDownStartTimer.clear();
            m_powerDownUpdateTimer.clear();
            emit notifyConnectionStatusChanged(name(), ConnectionStatus::CONNECTED);
        } else {
            // if we have a power down for too long then we disconnect the robot
            if (m_powerDownStartTimer.isTimedOutSec(ToleratedPowerDownDurationSec)) {
                qDebug() << QString("Disconnecting %1 due to power-down").arg(m_name);
                // if the connection is open then close it
                closeUniqueConnection();
                // clear timers
                m_powerDownStartTimer.clear();
                m_powerDownUpdateTimer.clear();
            }
        }
    }

    // if the obstacle detected
    if (m_obstacleDetectedUpdateTimer.isSet()) {
        // if the obstacle-detected message was not received recently, then stop
        // tracking it
        if (m_obstacleDetectedUpdateTimer.isTimedOutSec(ObstacleDetectedUpdateTimeoutSec)) {
            qDebug() << QString("Obstacle is not detected anymore by %1").arg(m_name);
            // clear timers
            m_obstacleDetectedUpdateTimer.clear();
            emit notifyObstacleDetectedStatusChanged(id(), false);
        }
    }
}

/*!
 * Returns the supported controllers.
 */
QList<ExperimentControllerType::Enum> FishBot::supportedControllers() const
{
    return m_experimentManager.supportedControllers();
}

/*!
 * Sets the controller.
 */
void FishBot::setController(ExperimentControllerType::Enum type)
{
    m_experimentManager.setController(type);

    // a check for the special case when the controller is disactivated, in this
    // case the stop the robot until the control mode is set. Before we left the
    // control mode that was set the last to run
    if (type == ExperimentControllerType::NONE)
        setControlMode(ControlModeType::IDLE);
}

/*!
 * Return the type of the current controller.
 */
ExperimentControllerType::Enum FishBot::currentController() const
{
    return m_experimentManager.currentController();
}

/*!
 * Returns the supported control modes.
 */
QList<ControlModeType::Enum> FishBot::supportedControlModes() const
{
    return m_controlStateMachine.supportedControlModes();
}

/*!
 * Return the type of the current control mode.
 */
ControlModeType::Enum FishBot::currentControlMode() const
{
    return m_controlStateMachine.currentControlMode();
}

/*!
 * Return the motion pattern.
 */
MotionPatternType::Enum FishBot::currentMotionPattern() const
{
    return m_navigation.motionPattern();
}

/*!
 * Passes further to the experiments manager the message from the bee setup (CW/CCW).
 */
void FishBot::setCircularSetupTurningDirection(QString message)
{
    m_experimentManager.setCircularSetupTurningDirection(message);
}

/*!
 * Computes and updates statistics.
 */
void FishBot::computeStatistics()
{
    // the distance between the robot and the center 
    if (m_fishStates.size() > 0) {
        WorldPolygon fishPositions;
        for (auto& state : m_fishStates) {
            fishPositions.append(state.position());
        }
        double distance = m_state.position().distance2dTo(fishPositions.center());
        emit updateStatistics(robotFishGroupStatisticsId(), distance);
    }

}

/*!
 * Provides the id for the robot-fish-group distance statistics id.
 */
QString FishBot::robotFishGroupStatisticsId()
{
    return QString("fishbot-%1-fish-group-distance").arg(m_id);
}
