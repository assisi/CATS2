#ifndef CATS2_CONTROL_MODE_STATE_MACHINE_HPP
#define CATS2_CONTROL_MODE_STATE_MACHINE_HPP

#include "RobotControlPointerTypes.hpp"
#include <control-modes/ControlModeType.hpp>

#include <AgentState.hpp>

#include <QtCore/QObject>
#include <QtCore/QMap>

class FishBot;

class ControlModeStateMachine : public QObject
{
    Q_OBJECT
public:
    //! Constructor.
    explicit ControlModeStateMachine(FishBot* robot, QObject *parent = 0);

    //! The state machine step. Returns the control target values.
    ControlTargetPtr step();

    //! Return a list of all supported control modes.
    QList<ControlModeType::Enum> supportedControlModes() const { return m_controlModes.keys(); }
    //! Return the current control mode.
    ControlModeType::Enum currentControlMode() const { return m_currentControlMode; }

    //! Sets the target position for the go-to-position control mode.
    void setTargetPosition(PositionMeters targetPosition);

signals:
    //! Informs that the robot's control mode was modified.
    void notifyControlModeChanged(ControlModeType::Enum type);

public slots:
    //! Sets the requested control mode.
    void setControlMode(ControlModeType::Enum type);
    //! Sets the position for the GO_TO_POSITION control mode.
    void setUserDefinedTargetPosition(PositionMeters position);

private:
    //! Checks if the control mode transition is authorized. For instance when
    //! a robot executes an emergency behavior in a case of a low battery or
    //! a collision avoidance, the change of its behavior is undesired.
    bool isTransitionAuthorized(ControlModeType::Enum) const;

    //! The list of control modes available to this state machine.
    QMap<ControlModeType::Enum, ControlModePtr> m_controlModes;

    //! The current control mode.
    ControlModeType::Enum m_currentControlMode;

    //! A pointer to the robot that is controlled by this state machine.
    FishBot* m_robot;
};

#endif // CATS2_CONTROL_MODE_STATE_MACHINE_HPP
