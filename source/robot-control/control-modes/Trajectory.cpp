#include "Trajectory.hpp"
#include "FishBot.hpp"

#include <QtCore/QDebug>

/*!
 * Constructor.
 */
Trajectory::Trajectory(FishBot* robot) :
    ControlMode(robot, ControlModeType::TRAJECTORY),
    m_trajectory(RobotControlSettings::get().trajectory()),
    m_currentIndex(0)
{
}

/*!
 * Destructor.
 */
Trajectory::~Trajectory()
{
    qDebug() << Q_FUNC_INFO << "Destroying the object";
}

/*!
 * The step of the control mode, generates the target based on the predefined
 * trajectory.
 */
ControlTargetPtr Trajectory::step()
{
    if (!m_trajectory.isEmpty()) {
        if (m_robot->state().position().isValid() &&
            m_robot->state().position().closeTo(m_trajectory.at(m_currentIndex)))
        {
            // the robot approaches the current waypoint, hence it needs to be
            // updated
            m_currentIndex = (m_currentIndex + 1) % m_trajectory.size();
        }
        if ((m_currentIndex >= 0) && (m_currentIndex < m_trajectory.size()))
            return ControlTargetPtr(new TargetPosition(m_trajectory.at(m_currentIndex)));
    }
    // if the trajectory is not defined then don't move
    return ControlTargetPtr(new TargetSpeed(0, 0));
}

/*!
 * Informs on what kind of control targets this control mode generates.
 */
QList<ControlTargetType> Trajectory::supportedTargets()
{
    return QList<ControlTargetType>({ControlTargetType::SPEED,
                                     ControlTargetType::POSITION});
}
