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
    //! Destructor.
    ~GoStraight();

    //! The step of the control mode, generates the constant speed target.
    virtual ControlTargetPtr step() override;

    //! Informs on what kind of control targets this control mode generates.
    virtual QList<ControlTargetType> supportedTargets() override;
};

#endif // CATS2_GO_STRAIGHT_HPP
