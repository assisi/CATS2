#ifndef CATS2_FISH_BOT_HPP
#define CATS2_FISH_BOT_HPP

#include "RobotControlPointerTypes.hpp"
#include "ControlModeStateMachine.hpp"
#include "control-modes/ControlTarget.hpp"
#include "MotionPatternType.hpp"
#include "navigation/Navigation.hpp"
#include "experiment-controllers/ExperimentControllerType.hpp"
#include "experiment-controllers/ExperimentManager.hpp"

#include <AgentState.hpp>

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
    explicit FishBot(QString id, QString controlAreasPath);
    //! Destructor.
    virtual ~FishBot() final;

    //! Return the robot's name.
    QString name() const { return m_name; }
    //! Return the robot's id.
    QString id() const { return m_id; }

    //! Sets the robot's color
    void setLedColor(QColor color) { m_ledColor = color; }

    //! Sets the robot's interface.
    void setRobotInterface(Aseba::DBusInterfacePtr robotInterface);
    //! Returns the robot's interface.
    Aseba::DBusInterfacePtr robotInterface() { return m_robotInterface; }
    //! Inititialises the robot's firmware. The robot's index is used to
    //! initilize the robot's id in its firmware.
    void setupConnection(int robotIndex);

public:
    //! Returns the supported controllers.
    QList<ExperimentControllerType::Enum> supportedControllers() const { return m_experimentManager.supportedControllers(); }
    //! Sets the controller.
    void setController(ExperimentControllerType::Enum type) { m_experimentManager.setController(type); }
    //! Return the type of the current controller.
    ExperimentControllerType::Enum currentController() const { return m_experimentManager.currentController(); }

    //! Returns the supported control modes.
    QList<ControlModeType::Enum> supportedControlModes() const { return m_controlStateMachine.supportedControlModes(); }
    //! Sets the control mode.
    void setControlMode(ControlModeType::Enum type);
    //! Return the type of the current control mode.
    ControlModeType::Enum currentControlMode() const { return m_controlStateMachine.currentControlMode(); }

    //! Checks that the current control modes can generate targets with
    //! different motion patterns.
    bool supportsMotionPatterns();
    //! Sets the motion pattern.
    void setMotionPattern(MotionPatternType::Enum type);
    //! Return the motion pattern.
    MotionPatternType::Enum currentMotionPattern() const { return m_navigation.motionPattern(); }
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

    //! Steps the control for the robot.
    void stepControl();

public:
    //! The target position received from the viewer; it's transfered further
    //! to the state machine.
    void goToPosition(PositionMeters position);

public:
    //! Sets the robot's state.
    void setState(StateWorld state) {m_state = state; }
    //! Returns the robot's state.
    StateWorld state() const { return m_state; }

    //! Receives data of all tracked robots, finds and sets the one corresponding
    //! to this robot and keeps the rest in case it's needed by the control mode.
    void setRobotsData(QList<AgentDataWorld> robotsPositions);
    //! Received states of all tracked fish, keeps them in case it's needed by
    //! the control mode.
    void setFishStates(QList<StateWorld> fishStates);
    //! Returns the states of all tracked fish.
    QList<StateWorld> fishStates() const { return m_fishStates; }

public slots:
    //! Requests to sends the control map areas' polygons.
    void requestControlAreasPolygons() { m_experimentManager.requestPolygons(); }
    //! Requests to sends the control map areas' polygons.
    void requestTrajectory() { /* TODO : to implement */}
    //! Requests to sends the control map areas' polygons.
    void requestCurrentTarget() { m_navigation.requestTargetPosition(); }
    //! Requests leds color.
    void requestLedColor() { emit notifyLedColor(m_id, m_ledColor); }

signals: // control states
    //! Informs that the robot's experiment controller was modified.
    void notifyControllerChanged(ExperimentControllerType::Enum type);
    //! Informs that the robot's control mode was modified.
    void notifyControlModeChanged(ControlModeType::Enum type);
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

signals: // navigation
    //! Sends the control map areas' polygons.
    void notifyControlAreasPolygons(QString agentId, QList<AnnotatedPolygons> polygons);
    //! Sends the current target.
    void notifyTargetPositionChanged(QString agentId, PositionMeters position);
    //! Sends the current trajectory.
    void notifyTrajectoryChanged(QString agentId, QQueue<PositionMeters> trajectory);

signals: // other
    //! Informs about the leds color.
    void notifyLedColor(QString agentId, QColor color);

public:
    //! Distance between robot's wheels.
    static constexpr double InterWheelsDistanceCm = 1.8;

private:
    //! Sets the control parameters based on the control map.
    void stepExperimentManager();

private:
    //! The robot's id.
    QString m_id;
    //! The robot's name.
    QString m_name;
    //! The color of the robot's LEDs.
    QColor m_ledColor;
    //! The robot's state.
    StateWorld m_state;
    //! The interface to communicate with the robot. Shared by all robots.
    Aseba::DBusInterfacePtr m_robotInterface;

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
};

#endif // CATS2_FISH_BOT_HPP
