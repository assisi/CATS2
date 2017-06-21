#ifndef CATS2_INITIATION_CONTROLLER_SETTINGS_HPP
#define CATS2_INITIATION_CONTROLLER_SETTINGS_HPP

#include "ExperimentControllerSettings.hpp"

/*!
 * The actual data stored in the settings. It's separated in a class to be
 * easily trasferable to the corresponding controller.
 */
class InitiationLureControllerSettingsData
{
public:
    //! Constructor.
    InitiationLureControllerSettingsData() :
        m_defaultControlAreasFileName()
    {}

public:
    //! Returns the path to the file describine the control areas.
    QString defaultControlAreasFileName() const { return m_defaultControlAreasFileName; }
    //! Set the path to the file describine the control areas.
    void setDefaultControlAreasFileName(QString controlAreasFileName)
    {
        m_defaultControlAreasFileName = controlAreasFileName;
    }

protected:
    //! The path to the file describine the control areas.
    QString m_defaultControlAreasFileName;
};

class InitiationLureControllerSettings : public ExperimentControllerSettings
{
public:
    //! Constructor.
    InitiationLureControllerSettings();
    //! Destructor.
    virtual ~InitiationLureControllerSettings();

public:
    //! Initialization of the parameters for this specific controller.
    //! Returns true if the parameters are considered valid.
    virtual bool init(QString configurationFileName) override;

    //! Provides a copy of the settings data.
    InitiationLureControllerSettingsData data() { return m_data; }

private:
    //! The settings data.
    InitiationLureControllerSettingsData m_data;
};


#endif // CATS2_INITIATION_CONTROLLER_SETTINGS_HPP
