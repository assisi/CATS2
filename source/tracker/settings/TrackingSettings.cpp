#include "TrackingSettings.hpp"
#include "TrackingFactory.hpp"

#include <settings/ReadSettingsHelper.hpp>

/*!
 * The singleton getter. Provides an instance of the settings.
 */
TrackingSettings& TrackingSettings::get()
{
    static TrackingSettings instance;
    return instance;
}

/*!
 * Initializes the parameters from the configuration file.
 */
bool TrackingSettings::init(QString configurationFileName, SetupType::Enum setupType)
{
    bool settingsAccepted = true;

    // get the tracking routine type
    TrackingRoutineType trackingRoutineType = readTrackingRoutineType(configurationFileName, setupType);
    if (trackingRoutineType == TrackingRoutineType::UNDEFINED)
        return false;

    // create corresponding settings
    TrackingRoutineSettingsPtr settings = TrackerFactory::createTrackingRoutineSettings(trackingRoutineType, setupType);
    m_trackingRoutineSettings.insert(setupType, settings);

   // initialize settings
   settingsAccepted = settings->init(configurationFileName);

    return settingsAccepted;
}

/*!
 * Reads from the configuration file the tracking routine type corresponding to the setup
 * type of this instance.
 */
TrackingRoutineType TrackingSettings::readTrackingRoutineType(QString configurationFileName, SetupType::Enum setupType)
{
    // get the prefix in the path in the configuration file
    QString prefix = SetupType::toString(setupType);

    std::string trackingRoutineName;
    ReadSettingsHelper settings(configurationFileName);
    settings.readVariable(QString("%1/tracking/trackingMethod").arg(prefix), trackingRoutineName);

    return TrackerFactory::trackingRoutineTypeByName(QString::fromUtf8(trackingRoutineName.c_str()));
}
