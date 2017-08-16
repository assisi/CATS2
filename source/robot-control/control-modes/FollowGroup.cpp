#include "FollowGroup.hpp"
#include "FishBot.hpp"

#include <QtCore/QDebug>
#include <QtCore/QtMath>

/*!
 * Constructor.
 */
FollowGroup::FollowGroup(FishBot* robot) :
    ControlMode(robot, ControlModeType::FOLLOW_GROUP),
    GridBasedMethod(),
    m_targetPosition(PositionMeters::invalidPosition())
{

}

/*!
 * Destructor.
 */
FollowGroup::~FollowGroup()
{
    qDebug() << "Destroying the object";
}

/*!
 * Called when the control mode is activated. Used to reset mode's parameters.
 */
void FollowGroup::start()
{
    // compute the first target position
    m_targetPosition = computeTargetPosition();
}

/*!
 * The step of the control mode.
 */
ControlTargetPtr FollowGroup::step()
{
    PositionMeters previousTargetPosition = m_targetPosition;
    m_targetPosition = computeTargetPosition();
    // check if the target position is inside the model area
    if (!containsPoint(m_targetPosition)) {
        // switch to the previous target
        m_targetPosition = previousTargetPosition;
    }

    if (m_targetPosition.isValid()) {
        PositionMeters robotPosition = m_robot->state().position();
        QString status;
        if (robotPosition.isValid()) {
            status = QString("group distance %1 m")
                    .arg(robotPosition.distance2dTo(m_targetPosition), 0, 'f', 3);
        } else {
            status = QString("group distance unknown");
        }
        emit notifyControlModeStatus(status);
        return ControlTargetPtr(new TargetPosition(m_targetPosition));
    } else {
        // otherwise the robot doesn't move
        emit notifyControlModeStatus("target undefined");
        return ControlTargetPtr(new TargetSpeed(0, 0));
    }
}

/*!
 * Informs on what kind of control targets this control mode generates.
 */
QList<ControlTargetType> FollowGroup::supportedTargets()
{
    return QList<ControlTargetType>({ControlTargetType::SPEED,
                                     ControlTargetType::POSITION});
}

/*!
 * Computes the target position as a centroid of the fish group.
 */
PositionMeters FollowGroup::computeTargetPosition()
{
    QList<StateWorld> fishStates = m_robot->fishStates();
    if (fishStates.size() > 0) {
        WorldPolygon fishPositions;
        for (const auto& state : fishStates) {
            fishPositions.append(state.position());
        }
        return fishPositions.center();
    } else {
        return PositionMeters::invalidPosition();
    }
}
