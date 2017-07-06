#ifndef CATS2_CIRCULAR_SETUP_LEADER_CONTROLLER_HPP
#define CATS2_CIRCULAR_SETUP_LEADER_CONTROLLER_HPP

#include "CircularSetupController.hpp"

#include <Timer.hpp>

/*!
 * The controller for the circular experiment where the robot plays the leader's
 * role and tries to make the fish to swim in the clock-wise or counter-clock-wise
 * direction, the direction depends on the ExperimentControllerType value provided.
 */
class CircularSetupLeaderController : public CircularSetupController
{
    Q_OBJECT
public:
    //! Constructor. Gets the robot and the settings.
    CircularSetupLeaderController(FishBot* robot,
                                  ExperimentControllerSettingsPtr settings,
                                  TurningDirection::Enum turningDirection);

public:
    //! Returns the control values for given position.
    virtual ControlData step() override;

//public slots:
//    //! Sets the direction to keep, expected values are "CW" or "CCW".
//    void setTurningDirection(QString directionString);

private:
    //! The fish turning angle update timer.
    Timer m_fishTurningAngleUpdateTimer;
    //! The constant that define how often we update the fish turning direction.
    static constexpr double FishTurningDirectionUpdateTimeout = 1.0;
};

#endif // CATS2_CIRCULAR_SETUP_LEADER_CONTROLLER_HPP
