#ifndef CATS2_MANUAL_HPP
#define CATS2_MANUAL_HPP

#include "ControlMode.hpp"

/*!
 * Controlling the robot's displacement by a robot.
 * NOTE : at the moment works only under linux
 * FIXME : this code comes from CATS, it's not flexible (doesn't check where the
 * joystick is, doesn't detect when it's connected/disconnected, etc) and needs
 *  to be redone (by using SDL for instance).
 */
class Manual : public ControlMode
{
    Q_OBJECT
public:
    //! Constructor.
    explicit Manual(FishBot* robot);
    //! Destructor.
    virtual ~Manual() override;

    //! The step of the control mode, generates the zero speed control target.
    virtual ControlTargetPtr step() override;

    //! Informs on what kind of control targets this control mode generates.
    virtual QList<ControlTargetType> supportedTargets() override;

private:
    //! Joystick's descriptor.
    int m_descriptor;
    //!
    static constexpr float JoystickThetaCorrection = 2.0;
};

#endif // CATS2_MANUAL_HPP
