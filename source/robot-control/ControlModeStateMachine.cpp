#include "ControlModeStateMachine.hpp"

#include "control-modes/ControlMode.hpp"
#include "control-modes/ControlModeType.hpp"
#include "control-modes/GoStraight.hpp"
#include "control-modes/GoToPosition.hpp"
#include "control-modes/Idle.hpp"
#include "control-modes/Manual.hpp"
#include "control-modes/ModelBased.hpp"
#include "control-modes/Trajectory.hpp"
#include "FishBot.hpp"

#include <QtCore/QDebug>

/*!
 * Constructor.
 */
ControlModeStateMachine::ControlModeStateMachine(FishBot* robot, QObject *parent) :
    QObject(parent),
    m_currentControlMode(ControlModeType::IDLE),
    m_robot(robot)
{
    // fill the map will all control modes
    m_controlModes.insert(ControlModeType::IDLE, ControlModePtr(new Idle(m_robot)));
    m_controlModes.insert(ControlModeType::MANUAL, ControlModePtr(new Manual(m_robot)));
    m_controlModes.insert(ControlModeType::GO_STRAIGHT, ControlModePtr(new GoStraight(m_robot)));
    m_controlModes.insert(ControlModeType::GO_TO_POSITION, ControlModePtr(new GoToPosition(m_robot)));
    m_controlModes.insert(ControlModeType::MODEL_BASED, ControlModePtr(new ModelBased(m_robot)));
    m_controlModes.insert(ControlModeType::TRAJECTORY, ControlModePtr(new Trajectory(m_robot)));

    // make necessary connections
    foreach (ControlModePtr controlMode, m_controlModes.values()) {
       connect(controlMode.data(), &ControlMode::requestControlModeChange, this, &ControlModeStateMachine::setControlMode);
       connect(controlMode.data(), &ControlMode::notifyControlModeStatus, this, &ControlModeStateMachine::notifyControlModeStatus);
    }
}

/*!
 * Sets the requested control mode. Returns true is the mode changes.
 */
bool ControlModeStateMachine::setControlMode(ControlModeType::Enum type)
{
    if (!m_controlModes.contains(type)) {
        qDebug() << "Requested control mode is not available";
        return false;
    }

    if (!isTransitionAuthorized(type)) {
        qDebug() << QString("Impossible to change the control mode from %1 to %2")
                    .arg(ControlModeType::toString(m_currentControlMode))
                    .arg(ControlModeType::toString(type));
        return false;
    }

    if (type != m_currentControlMode) {
        qDebug() << QString("Changing the control mode from %1 to %2 for %3")
                    .arg(ControlModeType::toString(m_currentControlMode))
                    .arg(ControlModeType::toString(type))
                    .arg(m_robot->name());

        // first stop the current control mode
        m_controlModes[m_currentControlMode]->finish();

        // then start the new one
        m_currentControlMode = type;
        m_controlModes[m_currentControlMode]->start();

        emit notifyControlModeChanged(type);
    }
    return true;
}

/*!
 * Sets the position for the GO_TO_POSITION control mode.
 */
void ControlModeStateMachine::setUserDefinedTargetPosition(PositionMeters position)
{
    if (m_controlModes.contains(ControlModeType::GO_TO_POSITION)) {
        GoToPosition* controlMode = qobject_cast<GoToPosition*>(m_controlModes[ControlModeType::GO_TO_POSITION].data());
        if (controlMode)
            controlMode->setTargetPosition(position);
    }
}

/*!
 * The state machine step. Returns the control target values.
 */
ControlTargetPtr ControlModeStateMachine::step()
{
    return m_controlModes[m_currentControlMode]->step();
}

/*!
 * Checks if the control mode transition is authorized.
 */
bool ControlModeStateMachine::isTransitionAuthorized(ControlModeType::Enum stateToTransit) const
{
    if (stateToTransit != ControlModeType::UNDEFINED)
        return true;
    else
        return false;
}

/*!
 * Sets the target position for the go-to-position control mode.
 */
void ControlModeStateMachine::setTargetPosition(PositionMeters position)
{
    if (m_controlModes.contains(ControlModeType::GO_TO_POSITION))
        dynamic_cast<GoToPosition*>(m_controlModes[ControlModeType::GO_TO_POSITION].data())->setTargetPosition(position);
}

/*!
 * Sets the parameters of the fish model.
 */
void ControlModeStateMachine::setModelParameters(ModelParameters parameters)
{
    if (m_controlModes.contains(ControlModeType::MODEL_BASED)) {
        ControlMode* mode = m_controlModes[ControlModeType::MODEL_BASED].data();
        dynamic_cast<ModelBased*>(mode)->setParameters(parameters);
    }
}

/*!
 * Limits the arena matrix of the model-based control mode by a mask. The mask
 * is defined by a set of polygons and is labeled with an id.
 */
void ControlModeStateMachine::limitModelArea(QString maskId,
                                             QList<WorldPolygon> allowedArea)
{
    if (m_controlModes.contains(ControlModeType::MODEL_BASED)) {
        ControlMode* mode = m_controlModes[ControlModeType::MODEL_BASED].data();
        dynamic_cast<ModelBased*>(mode)->restrictModelArea(maskId, allowedArea);
    }
}

/*! Removes the applied limitations on the model area. After this the model
 * will be applied on the whole setup.
 */
void ControlModeStateMachine::releaseModelArea()
{
    if (m_controlModes.contains(ControlModeType::MODEL_BASED)) {
        ControlMode* mode = m_controlModes[ControlModeType::MODEL_BASED].data();
        dynamic_cast<ModelBased*>(mode)->clearAreaMask();
    }
}

/*!
 * Checks that the current control modes can generate targets with different
 *  motion patterns. At the moment the only such target is the position target.
 */
bool ControlModeStateMachine::supportsMotionPatterns()
{
    return m_controlModes[m_currentControlMode]->supportedTargets().contains(ControlTargetType::POSITION);
}
