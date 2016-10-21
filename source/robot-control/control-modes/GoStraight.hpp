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
    GoStraight(FishBot* robot);

    //! The step of the control mode, generates the constant speed target.
    virtual ControlTargetPtr step() override;

    //! Informs on what kind of control targets this control mode generates.
    virtual QList<ControlTargetType> supportedTargets() override;

private:
    //! The constant speed to be used for this behavior.
    // FIXME : define units
    // TODO : to check if it's to be moved to the configuration file
    static constexpr float Speed = 2.0;
};

#endif // CATS2_GO_STRAIGHT_HPP
