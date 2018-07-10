#ifndef CATS2_FISH_BOT_HPP
#define CATS2_FISH_BOT_HPP

#include "RobotControlPointerTypes.hpp"
#include "ConnectionStatusType.hpp"
#include "ControlModeStateMachine.hpp"
#include "control-modes/ControlTarget.hpp"
#include "control-modes/ModelParameters.hpp"
#include "MotionPatternType.hpp"
#include "navigation/Navigation.hpp"
#include "experiment-controllers/ExperimentControllerType.hpp"
#include "experiment-controllers/ExperimentManager.hpp"

#include "interfaces/DBusInterface.hpp"

#include <AgentState.hpp>
#include <Timer.hpp>

#include <QtCore/QObject>

class AgentDataWorld;
class StateWorld;

/*!
 * The main class for the FishBot robot. It stores the robot's state, the
 * current control mode and locomotion pattern, and also provides an interface
 * to communicate with the physical robot.
 */
class FishBot : public QObject
{
    Q_OBJECT
public:
    //! Constructor.
    explicit FishBot(QString id);
    //! Destructor.
    virtual ~FishBot() final;

    //! Return the robot's name.
    QString name() const { return m_name; }
    //! Return the robot's id.
    QString id() const { return m_id; }
    //! Sets the robot's color
    void setLedColor(QColor color) { m_ledColor = color; }

public:
    //! Sets the robot's interface.
    void setSharedRobotInterface(DBusInterfacePtr robotInterface);
    //! Inititialises the robot's firmware. The robot's index is used to
    //! initilize the robot's id in its firmware.
    void setupSharedConnection(int robotIndex);
    //! Convenience method. Inititialises the robot's firmware, it uses the
    //! stored robot's index if available.
    void setupSharedConnection();
    //! Connects to the robot via its own interface.
    void setupUniqueConnection();
    //! Returns the connection status.
    bool isConnected() const;
    //! Sends an aseba event to the robot.
    void sendEvent(const QString& eventName, const Values& value);

public:
    //! Returns the supported controllers.
    QList<ExperimentControllerType::Enum> supportedControllers() const;
    //! Sets the controller.
    void setController(ExperimentControllerType::Enum type);
    //! Return the type of the current controller.
    ExperimentControllerType::Enum currentController() const;

    // interspecies
    //! Passes further to the experiments manager the message from the bee setup
    //! (CW/CCW).
    void setCircularSetupTurningDirection(QString message);
    //! Sets preferedAreaId
    void setInitiationLurePreferedAreaId(Qstring preferedAreaId);

    //! Returns the supported control modes.
    QList<ControlModeType::Enum> supportedControlModes() const;
    //! Sets the control mode.
    void setControlMode(ControlModeType::Enum type);
    //! Return the type of the current control mode.
    ControlModeType::Enum currentControlMode() const;

    //! Checks that the current control modes can generate targets with
    //! different motion patterns.
    bool supportsMotionPatterns();
    //! Sets the motion pattern.
    void setMotionPattern(MotionPatternType::Enum type);
    //! Return the motion pattern.
    MotionPatternType::Enum currentMotionPattern() const;
    //! Sets the motion pattern frequency divider. The goal is to send commands
    //! less often to keep the network load low.
    void setMotionPatternFrequencyDivider(MotionPatternType::Enum type,
                                          int frequencyDivider);
    //! Return the motion pattern frequency divider.
    int motionPatternFrequencyDivider(MotionPatternType::Enum type);
    //! Sets the path planning usage flag in the navigation.
    void setUsePathPlanning(bool usePathPlanning);
    //! Returns the path planning usage from from the navigation.
    bool usePathPlanning() const { return m_navigation.usePathPlanning(); }

    //! Sets the obstacle avoidance usage flag in the navigation.
    void setUseObstacleAvoidance(bool useObstacleAvoidance);
    //! Returns the obstacle avoidance usage from from the navigation.
    bool useObstacleAvoidance() const { return m_navigation.useObstacleAvoidance(); }

    //! Steps the control for the robot.
    void stepControl();

public:
    // FIXME : this is a temporary code, to be removed once the parameters of the
    // obstacle avoidance are tuned
    //! Returns a pointer to the potential field obstacle avoidance module.
    PotentialFieldPtr potentialField() { return m_navigation.potentialField(); }

public:
    //! The target position received from the viewer; it's transfered further
    //! to the state machine.
    void goToPosition(PositionMeters position, bool fromNetwork = false);

public:
    //! Sets the robot's state.
    void setState(StateWorld state) {m_state = state; }
    //! Returns the robot's state.
    StateWorld state() const { return m_state; }

    //! Receives data of all tracked robots, finds and sets the one corresponding
    //! to this robot and keeps the rest in case it's needed by the control mode.
    void setRobotsData(QList<AgentDataWorld> robotsPositions);
    //! Returns the data of other robots.
    const QList<AgentDataWorld>& otherRobotsData() const { return m_otherRobotsData; }
    //! Received states of all tracked fish, keeps them in case it's needed by
    //! the control mode.
    void setFishStates(QList<StateWorld> fishStates);
    //! Returns the states of all tracked fish.
    QList<StateWorld> fishStates() const { return m_fishStates; }

public slots:
    //! Requests to sends the control map areas' polygons.
    void requestControlAreasPolygons() { m_experimentManager.requestPolygons(); }
    //! Requests to sends the control map areas' polygons.
    void requestTrajectory() { m_navigation.requestTrajectory(); }
    //! Requests to sends the control map areas' polygons.
    void requestCurrentTarget() { m_navigation.requestTargetPosition(); }
    //! Requests leds color.
    void requestLedColor() { emit notifyLedColor(m_id, m_ledColor); }

signals: // control states
    //! Informs that the robot's experiment controller was modified.
    void notifyControllerChanged(ExperimentControllerType::Enum type);
    //! Sends out the current controller status.
    void notifyControllerStatus(QString status);

    //! Sends the data from the circular experiment.
    void notifyCircularSetupTurningDirections(QString agentId,
                                                    QString fishTurningDirection,
                                                    QString robotTurningDirection);

    //! Sends the statistics from the circular experiment.
    void notifyCircularSetupStatistics(QString agentId,
                                       double fishClockWisePercent,
                                       double fishCounterClockWisePercent,
                                       QString fishTurningDirection,
                                       double robotClockWisePercent,
                                       double robotCounterClockWisePercent,
                                       QString robotTurningDirection);

    //! Informs that the robot's control mode was modified.
    void notifyControlModeChanged(ControlModeType::Enum type);
    //! Sends out the current control mode status.
    void notifyControlModeStatus(QString status);
    //! Informs that the robot's motion pattern was changed.
    void notifyMotionPatternChanged(MotionPatternType::Enum type);
    //! Informs that the value of the frequency divider of the robot's motion
    //! pattern has been changed.
    void notifyMotionPatternFrequencyDividerChanged(MotionPatternType::Enum type,
                                                    int frequencyDivider);
    //! Informs that the robot is in manual control mode.
    void notifyInManualMode(QString id);
    //! Informs that the path planning is on/off in the navigation.
    void notifyUsePathPlanningChanged(bool value);
    //! Informs that the obstacle avoidance is on/off in the navigation.
    void notifyUseObstacleAvoidanceChanged(bool value);

signals: // robot on-board data
    //! Notifies that the connection status has changed.
    void notifyConnectionStatusChanged(QString name, ConnectionStatus status);
    //! Notifies that the obstacle avoidance status has changed.
    void notifyObstacleDetectedStatusChanged(QString agentId, bool obstacleDetected);

signals: // navigation
    //! Sends the control map areas' polygons.
    void notifyControlAreasPolygons(QString agentId, QList<AnnotatedPolygons> polygons);
    //! Sends the areas' occupation by fish information.
    void notifyFishNumberByAreas(QString agentId, QMap<QString, int> fishNumberByArea);
    //! Sends the current target.
    void notifyTargetPositionChanged(QString agentId, PositionMeters position);
    //! Sends the current trajectory.
    void notifyTrajectoryChanged(QString agentId, QQueue<PositionMeters> trajectory);

signals: // statistics
    //! Updates statistics.
    void updateStatistics(QString id, double value);

signals: // other
    //! Informs about the leds color.
    void notifyLedColor(QString agentId, QColor color);

public:
    //! Distance between robot's wheels.
    static constexpr double InterWheelsDistanceCm = 1.8;

private:
    //! Sets the control parameters based on the control map.
    void stepExperimentManager();

    //! Updates the parameters of the model.
    void setModelParameters(ControlModeType::Enum type,
                            ModelParameters parameters);
    //! Requests the state machine to limit the arena matrix of the model-based
    //! control mode by a mask. The reason is to prevent the model's target to
    //! leave the control area defined by the experiment. The mask is defined by
    //! a set of polygons and is labeled with an id.
    void limitModelArea(ControlModeType::Enum type,
                        QString maskId,
                        QList<WorldPolygon> allowedArea);
    //! Requests the state machine to remove the limitations on the model area
    //! that were applied by a experiment controller.
    void releaseModelArea();

private:
    //! Closes the unique connection.
    void closeUniqueConnection();

    //! A service method that makes the code to wait for a certatin time by printing
    //! the count down.
    void countDown(double timeOut);

private: // safety logics
    //! Runs the emergency logics for the safety issues.
    void stepSafetyLogics();

    //! Implements the reaction of the robot on the power-down event.
    void processPowerDownEvent();
    //! Implements the reaction of the robot on obstacle-detected event.
    void processObstacleEvent();

private:
    //! The robot's id.
    QString m_id;
    //! The robot's index used by the firmware, provided by the control loop.
    int m_firmwareId;
    //! The robot's name.
    QString m_name;
    //! The color of the robot's LEDs.
    QColor m_ledColor;
    //! The robot's state.
    StateWorld m_state;
    // TODO : this interfaces should be placed to a separated Connection class
    //! The interface to communicate with the robot. Shared by all robots.
    DBusInterfacePtr m_sharedRobotInterface;
    //! The interface to communicate with the robot. Unique for this robot. We
    //! use either this one or the shared one.
    DashelInterfacePtr m_uniqueRobotInterface;

    // TODO : to make this class members scopedpointers and use forward declaration
    // for efficiency
    //! The "super" controller that manages specific experiments.
    ExperimentManager m_experimentManager;
    //! The control loop state machine that generates the targets for the navigation.
    ControlModeStateMachine m_controlStateMachine;

    //! The data of other robots.
    QList<AgentDataWorld> m_otherRobotsData;
    //! The states of fish.
    QList<StateWorld> m_fishStates;

    //! Navigates the robot to a target.
    Navigation m_navigation;

    // TODO : to add the interface with the RiBot lure

private: // to manage the power down events
    //! Counts the time from the first power-donw message in a sequence.
    Timer m_powerDownStartTimer;
    //! Counts the time from the last power-down message in a sequence.
    Timer m_powerDownUpdateTimer;
    //! If the power-down message is not received for at least this value
    //! then we consider that it is not valid anymore.
    static constexpr double PowerDownUpdateTimeoutSec = 1.;
    //! If the power-down message is received for longer than this period
    //! then we consider this as emergency.
    static constexpr double ToleratedPowerDownDurationSec = 3.;

private: // to manage the obstacle events
    //! Counts the time from the last obstacle-detected message in a sequence.
    Timer m_obstacleDetectedUpdateTimer;
    //! If the obstacle-detected message is not received for at least this value
    //! then we consider that it is not valid anymore.
    static constexpr double ObstacleDetectedUpdateTimeoutSec = 0.5;

private:
    //! Defines if the statistics are to be computed and updated.
    bool m_computeStatistics;
    //! Computes and updates statistics.
    void computeStatistics();
    //! Provides the id for the robot-fish-group distance statistics id.
    QString robotFishGroupStatisticsId();
};

#endif // CATS2_FISH_BOT_HPP
