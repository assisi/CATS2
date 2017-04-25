#ifndef CATS2_IDLE_HPP
#define CATS2_IDLE_HPP

#include "ControlMode.hpp"

/*!
 * Generates the zero speed to make the robot to stop.
 */
class Idle : public ControlMode
{
    Q_OBJECT
public:
    //! Constructor.
    Idle(FishBot* robot);
    //! Destructor.
    ~Idle();

    //! The step of the control mode, generates the zero speed control target.
    virtual ControlTargetPtr step() override;

    //! Informs on what kind of control targets this control mode generates.
    virtual QList<ControlTargetType> supportedTargets() override;
};

#endif // CATS2_IDLE_HPP
