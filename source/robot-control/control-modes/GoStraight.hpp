#ifndef CATS2_GO_STRAIGHT_HPP
#define CATS2_GO_STRAIGHT_HPP

#include "ControlMode.hpp"

/*!
 * Generates a constant speed output.
 */
class GoStraight : public ControlMode
{
    Q_OBJECT
public:
    //! Constructor.
    explicit GoStraight(FishBot* robot);
    //! Destructor.
    virtual ~GoStraight() override;

    //! The step of the control mode, generates the constant speed target.
    virtual ControlTargetPtr step() override;

    //! Informs on what kind of control targets this control mode generates.
    virtual QList<ControlTargetType> supportedTargets() override;

private:
    //! The speed to be sent to the robot.
    int m_linearSpeed; // [cm/sec]
};

#endif // CATS2_GO_STRAIGHT_HPP
