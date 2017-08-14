#ifndef CATS2_MAP_CONTROLLER_SETTINGS_HPP
#define CATS2_MAP_CONTROLLER_SETTINGS_HPP

#include "ExperimentControllerSettings.hpp"

/*!
 * The actual data stored in the settings. It's separated in a class to be
 * easily trasferable to the corresponding controller.
 */
class MapControllerSettingsData
{
public:
    //! Constructor.
    explicit MapControllerSettingsData() :
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

class MapControllerSettings : public ExperimentControllerSettings
{
public:
    //! Constructor.
    explicit MapControllerSettings();
    //! Destructor.
    virtual ~MapControllerSettings();

public:
    //! Initialization of the parameters for this specific controller.
    //! Returns true if the parameters are considered valid.
    virtual bool init(QString configurationFileName) override;

    //! Provides a copy of the settings data.
    MapControllerSettingsData data() { return m_data; }

private:
    //! The settings data.
    MapControllerSettingsData m_data;
};

#endif // CATS2_MAP_CONTROLLER_SETTINGS_HPP
