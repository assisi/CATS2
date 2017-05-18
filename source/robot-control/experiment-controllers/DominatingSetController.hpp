#ifndef CATS2_DOMINATING_SET_CONTROLLER_HPP
#define CATS2_DOMINATING_SET_CONTROLLER_HPP

#include "ExperimentController.hpp"
#include "settings/DominatingSetControllerSettings.hpp"

/*!
 * The controller that implements the dominating set experiment. The goal of the
 * experiment is to ...
 * This controller makes the robot to follow the fish group.
 */
class DominatingSetController : public ExperimentController
{
    Q_OBJECT
public:
    //! Constructor. Gets the robot and the settings.
    DominatingSetController(FishBot* robot,
                            ExperimentControllerSettingsPtr settings);

public:
    //! Called when the controller is activated. Used to reset parameters.
    virtual void start() override;
    //! Returns the control values for given position.
    virtual ControlData step() override;

private:
    //! The settings for this controller.
    DominatingSetControllerSettingsData m_settings;

    //! The flag that says that the robot's operation area was defined
    bool m_robotAreaDefined;
};

#endif // CATS2_DOMINATING_SET_CONTROLLER_HPP
