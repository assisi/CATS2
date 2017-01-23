#ifndef CAT2_MAP_CONTROLLER_SETTINGS_HPP
#define CAT2_MAP_CONTROLLER_SETTINGS_HPP

#include "ExperimentControllerSettings.hpp"

/*!
 * The actual data stored in the settings. It's separated in a class to be
 * easily trasferable to the corresponding controller.
 */
class MapControllerSettingsData
{
public:
    //! Constructor.
    MapControllerSettingsData() :
        m_controlAreasFileName()
    {}

public:
    //! Returns the path to the file describine the control areas.
    QString controlAreasFileName() const { return m_controlAreasFileName; }
    //! Set the path to the file describine the control areas.
    void setControlAreasFileName(QString controlAreasFileName)
    {
        m_controlAreasFileName = controlAreasFileName;
    }

protected:
    //! The path to the file describine the control areas.
    QString m_controlAreasFileName;
};

class MapControllerSettings : public ExperimentControllerSettings
{
public:
    //! Constructor.
    MapControllerSettings();
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

#endif // CAT2_MAP_CONTROLLER_SETTINGS_HPP
