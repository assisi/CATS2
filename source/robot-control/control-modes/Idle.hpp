#ifndef CATS2_IDLE_HPP
#define CATS2_IDLE_HPP

#include "ControlMode.hpp"

/*!
 * Generates the zero speed to make the robot to stop.
 */
class Idle : public ControlMode
{
public:
    //! Constructor.
    Idle(FishBot* robot);

    //! The step of the control mode, generates the zero speed control target.
    virtual ControlTargetPtr step() override;
};

#endif // CATS2_IDLE_HPP
