#ifndef CATS2_CIRCULAR_SETUP_CONTROLLER_HPP
#define CATS2_CIRCULAR_SETUP_CONTROLLER_HPP

#include "ExperimentController.hpp"
#include "settings/CircularSetupControllerSettings.hpp"

/*!
 * The parent class for controllers that implements various behavior on the
 * circular setup.
 */
class CircularSetupController : public ExperimentController
{
    Q_OBJECT
public:
    //! Constructor. Gets the robot and the settings.
    CircularSetupController(FishBot* robot,
                            ExperimentControllerType::Enum type,
                            ExperimentControllerSettingsPtr settings);

protected:
    //! The settings for this controller.
    CircularSetupControllerSettingsData m_settings;
};

#endif // CATS2_CIRCULAR_SETUP_CONTROLLER_HPP
