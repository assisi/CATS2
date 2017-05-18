#ifndef CATS2_CIRCULAR_SETUP_CONTROLLER_SETTINGS_HPP
#define CATS2_CIRCULAR_SETUP_CONTROLLER_SETTINGS_HPP

#include "ExperimentControllerSettings.hpp"

/*!
 * The actual data stored in the settings. It's separated in a class to be
 * easily trasferable to the corresponding controller.
 */
class CircularSetupControllerSettingsData
{
public:
    //! Constructor.
    CircularSetupControllerSettingsData()
    {}

public:

protected:
};

class CircularSetupControllerSettings : public ExperimentControllerSettings
{
public:
    //! Constructor.
    CircularSetupControllerSettings(ExperimentControllerType::Enum type);
    //! Destructor.
    virtual ~CircularSetupControllerSettings();

public:
    //! Initialization of the parameters for this specific controller.
    //! Returns true if the parameters are considered valid.
    virtual bool init(QString configurationFileName) override;

    //! Provides a copy of the settings data.
    CircularSetupControllerSettingsData data() { return m_data; }

private:
    //! The settings data.
    CircularSetupControllerSettingsData m_data;
};

#endif // CATS2_CIRCULAR_SETUP_CONTROLLER_SETTINGS_HPP
