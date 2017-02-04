#ifndef CATS2_TRAJECTORY_HPP
#define CATS2_TRAJECTORY_HPP

#include "ControlMode.hpp"

/*!
 * Makes the robots to follow the predefined trajectory. When it arrives to the
 * last point it restarts it from the first point.
 */
// TODO : to show the trajectory on the viewer
class Trajectory : public ControlMode
{
    Q_OBJECT
public:
    //! Constructor.
    Trajectory(FishBot* robot);
    //! Destructor.
    ~Trajectory();

    //! The step of the control mode.
    virtual ControlTargetPtr step() override;

    //! Called when the control mode is disactivated.
    virtual void finish() override { m_currentIndex = 0; }

    //! Informs on what kind of control targets this control mode generates.
    virtual QList<ControlTargetType> supportedTargets() override;

private:
    //! The trajectory to follow.
    QList<PositionMeters> m_trajectory;
    //! The current point index.
    int m_currentIndex;
    // TODO : to add a flag to loop the trajectory
};

#endif // CATS2_TRAJECTORY_HPP
