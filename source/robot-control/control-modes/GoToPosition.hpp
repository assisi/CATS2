#ifndef CATS2_GO_TO_POSITION_HPP
#define CATS2_GO_TO_POSITION_HPP

#include "ControlMode.hpp"
#include <AgentState.hpp>

/*!
 * Sends the robot to requested position.
 */
class GoToPosition : public ControlMode
{
    Q_OBJECT
public:
    //! Constructor.
    GoToPosition(FishBot* robot);

    //! The step of the control mode.
    virtual ControlTargetPtr step() override;

    //! Called when the control mode is disactivated. Invalidates the target
    //! position to prevent to robot from going there the next time this mode
    //! is activated
    virtual void finish() override { m_targetPosition.setValid(false); }

    //! Sets the target position.
    void setTargetPosition(PositionMeters position);

private:
    //! The target position.
    PositionMeters m_targetPosition;
};

#endif // CATS2_GO_TO_POSITION_HPP
