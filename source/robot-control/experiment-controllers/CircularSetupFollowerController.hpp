#ifndef CATS2_CIRCULAR_SETUP_FOLLOWER_CONTROLLER_HPP
#define CATS2_CIRCULAR_SETUP_FOLLOWER_CONTROLLER_HPP

#include "CircularSetupController.hpp"

class CircularSetupFollowerController : public CircularSetupController
{
    Q_OBJECT
public:
    //! Constructor. Gets the robot and the settings.
    CircularSetupFollowerController(FishBot* robot,
                                    ExperimentControllerSettingsPtr settings);

public:
    //! Returns the control values for given position.
    virtual ControlData step() override;
};

#endif // CATS2_CIRCULAR_SETUP_FOLLOWER_CONTROLLER_HPP
