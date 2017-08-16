#ifndef CATS2_FOLLOW_GROUP_BASED_HPP
#define CATS2_FOLLOW_GROUP_BASED_HPP

#include "ControlMode.hpp"
#include "SetupMap.hpp"
#include "navigation/GridBasedMethod.hpp"

#include <AgentState.hpp>
#include <Timer.hpp>

/*!
 * Makes the robots to follow the fish group centroid. It also inherits
 * GridBasedMethod to be able to make fast checks if the generated target is
 * inside of the setup.
 */
class FollowGroup : public ControlMode, public GridBasedMethod
{
    Q_OBJECT
public:
    //! Constructor.
    explicit FollowGroup(FishBot* robot);
    //! Destructor.
    virtual ~FollowGroup() override;

    //! Called when the control mode is activated. Used to reset mode's
    //! parameters.
    virtual void start() override;
    //! The step of the control mode.
    virtual ControlTargetPtr step() override;

    //! Informs on what kind of control targets this control mode generates.
    virtual QList<ControlTargetType> supportedTargets() override;

private:
    //! Computes the target position.
    PositionMeters computeTargetPosition();

private:
    //! The robot's target position.
    PositionMeters m_targetPosition;
};

#endif // CATS2_FOLLOW_GROUP_BASED_HPP
