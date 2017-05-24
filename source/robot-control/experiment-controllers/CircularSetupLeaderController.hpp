#ifndef CATS2_CIRCULAR_SETUP_LEADER_CONTROLLER_HPP
#define CATS2_CIRCULAR_SETUP_LEADER_CONTROLLER_HPP

#include "CircularSetupController.hpp"

class CircularSetupLeaderController : public CircularSetupController
{
    Q_OBJECT
public:
    //! Constructor. Gets the robot and the settings.
    CircularSetupLeaderController(FishBot* robot,
                                  ExperimentControllerSettingsPtr settings);

public:
    //! Returns the control values for given position.
    virtual ControlData step() override;

public slots:
    //! Sets the direction to keep, expected values are "CW" or "CCW".
    void setTurningDirection(QString directionString);
};

#endif // CATS2_CIRCULAR_SETUP_LEADER_CONTROLLER_HPP
