#ifndef CATS2_CIRCULAR_SETUP_FOLLOWER_CONTROLLER_HPP
#define CATS2_CIRCULAR_SETUP_FOLLOWER_CONTROLLER_HPP

#include "CircularSetupController.hpp"

#include <Timer.hpp>

class CircularSetupFollowerController : public CircularSetupController
{
    Q_OBJECT
public:
    //! Constructor. Gets the robot and the settings.
    CircularSetupFollowerController(FishBot* robot,
                                    ExperimentControllerSettingsPtr settings);

public:
    //! Called when the controller is activated. Used to reset parameters.
    virtual void start() override;
    //! Returns the control values for given position.
    virtual ControlData step() override;

private:
    //! The fish turning angle update timer.
    Timer m_fishTurningAngleUpdateTimer;
    //! The constant that define how often we update the fish turning direction.
    static constexpr double FishTurningDirectionUpdateTimeout = 1.0;

    //! The timer to give the robot to turn efficiently when changing direction.
    Timer m_changingDirectionTimer;
    //! The constant that define the sufficient time to change the direction.
    static constexpr double RobotChangingDirectionTimeout = 1.0;

};

#endif // CATS2_CIRCULAR_SETUP_FOLLOWER_CONTROLLER_HPP
