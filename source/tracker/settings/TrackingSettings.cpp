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

    // FIXME : these variables are read several times (every time the setup is initialized), consider moving this code
    // to a separated function
    // first read the generic settings
    ReadSettingsHelper settings(configurationFileName);
    std::string experimentName;
    settings.readVariable("experiment/name", experimentName, std::string("Experiment"));
    m_experimentName = QString::fromUtf8(experimentName.c_str());

    settings.readVariable("robots/numberOfRobots", m_numberOfRobots, 0);
    settings.readVariable("experiment/agents/numberOfAnimals", m_numberOfAnimals, 0);

    // and now read the settings specific for given setup type
    // get the tracking routine type
    TrackingRoutineType::Enum trackingRoutineType = readTrackingRoutineType(configurationFileName, setupType);
    if (trackingRoutineType == TrackingRoutineType::UNDEFINED)
        return false;

    // create corresponding settings
    TrackingRoutineSettingsPtr routineSettings = TrackerFactory::createTrackingRoutineSettings(trackingRoutineType, setupType);
    m_trackingRoutineSettings.insert(setupType, routineSettings);

    // initialize settings
    settingsAccepted = routineSettings->init(configurationFileName);

    return settingsAccepted;
}

/*!
 * Reads from the configuration file the tracking routine type corresponding to the setup
 * type of this instance.
 */
TrackingRoutineType::Enum TrackingSettings::readTrackingRoutineType(QString configurationFileName, SetupType::Enum setupType)
{
    // get the prefix in the path in the configuration file
    QString prefix = SetupType::toSettingsString(setupType);

    std::string trackingRoutineName;
    ReadSettingsHelper settings(configurationFileName);
    settings.readVariable(QString("%1/tracking/trackingMethod").arg(prefix), trackingRoutineName);

    return TrackingRoutineType::fromSettingsString(QString::fromUtf8(trackingRoutineName.c_str()));
}
