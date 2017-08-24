#include "Trajectory.hpp"
#include "FishBot.hpp"

#include <QtCore/QDebug>

/*!
 * Constructor.
 */
Trajectory::Trajectory(FishBot* robot) :
    ControlMode(robot, ControlModeType::TRAJECTORY),
    m_trajectory(RobotControlSettings::get().trajectory()),
    m_currentIndex(0),
    m_loopTrajectory(RobotControlSettings::get().loopTrajectory()),
    m_providePointsOnTimer(RobotControlSettings::get().providePointsOnTimer())
{
    connect(&m_updateTimer, &QTimer::timeout, this, &Trajectory::updateCurrentIndex);
}

/*!
 * Destructor.
 */
Trajectory::~Trajectory()
{
    qDebug() << "Destroying the object";
}

/*!
 * The step of the control mode, generates the target based on the predefined
 * trajectory.
 */
ControlTargetPtr Trajectory::step()
{
    if (!m_trajectory.isEmpty()) {
        // if the points are updated upon arrival
        if (!m_providePointsOnTimer && m_robot->state().position().isValid() &&
            m_robot->state().position().closeTo(m_trajectory.at(m_currentIndex)))
        {
            // the robot approaches the current waypoint, hence it needs to be
            // updated
            updateCurrentIndex();
        }

        // returns the current position
        if ((m_currentIndex >= 0) && (m_currentIndex < m_trajectory.size()))
            return ControlTargetPtr(new TargetPosition(m_trajectory.at(m_currentIndex)));
    }
    // if the trajectory is not defined then don't move
    return ControlTargetPtr(new TargetSpeed(0, 0));
}

/*!
 * Switches to the next waypoint.
 */
void Trajectory::updateCurrentIndex()
{
    if (m_trajectory.size() > 0) {
        if ((m_currentIndex != m_trajectory.size()) || m_loopTrajectory)
            m_currentIndex = (m_currentIndex + 1) % m_trajectory.size();
    } else {
        m_currentIndex = 0;
    }
}

/*!
 * Informs on what kind of control targets this control mode generates.
 */
QList<ControlTargetType> Trajectory::supportedTargets()
{
    return QList<ControlTargetType>({ControlTargetType::SPEED,
                                     ControlTargetType::POSITION});
}

/*!
 * Called when the control mode is activated.
 */
void Trajectory::start()
{
    if (m_providePointsOnTimer) {
        double intervalMs = 1000. / RobotControlSettings::get().controlFrequencyHz();
        m_updateTimer.start(intervalMs);
    }
}

/*!
 * Called when the control mode is disactivated.
 */
void Trajectory::finish()
{
    if (m_providePointsOnTimer)
        m_updateTimer.stop();

    m_currentIndex = 0;
}

