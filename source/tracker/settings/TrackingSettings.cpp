#include "TrackingSettings.hpp"
#include "TrackingFactory.hpp"

#include <ReadSettingsHelper.hpp>

/*!
 * The singleton getter. Provides an instance of the settings corresponding
 * to the type of setup : main camera or the below camera.
 */
TrackingSettings& TrackingSettings::get(SetupType setupType)
{
    if (setupType == SetupType::CAMERA_BELOW){
        static TrackingSettings cameraBelowTrackingInstance;
        cameraBelowTrackingInstance.setSetupType(setupType);
        return cameraBelowTrackingInstance;
    } else if (setupType == SetupType::MAIN_CAMERA) {
        static TrackingSettings mainCameraTrackingInstance;
        mainCameraTrackingInstance.setSetupType(SetupType::MAIN_CAMERA);
        return mainCameraTrackingInstance;
    }

    // security for a case if the provided setup type is not supported by this method:
    // by default we return the main camera instance
    static TrackingSettings mainCameraTrackingInstance;
    mainCameraTrackingInstance.setSetupType(SetupType::MAIN_CAMERA);
    return mainCameraTrackingInstance;
}

/*!
 * Initializes the parameters from the configuration file.
 */
bool TrackingSettings::init(QString configurationFileName)
{
    bool settingsAccepted = true;

    // get the tracking routine type
    TrackingRoutineType trackingRoutineType = readTrackingRoutineType(configurationFileName);
    if (trackingRoutineType == TrackingRoutineType::UNDEFINED)
        return false;

    // create corresponding settings
   _trackingRoutineSettings = TrackerFactory::createTrackingRoutineSettings(trackingRoutineType, TrackingSetup::setupSettingsNameByType(_setupType));

   // initialize settings
   settingsAccepted = _trackingRoutineSettings.data()->init(configurationFileName);

    return settingsAccepted;
}

/*!
 * Reads from the configuration file the tracking routine type corresponding to the setup
 * type of this instance.
 */
TrackingRoutineType TrackingSettings::readTrackingRoutineType(QString configurationFileName)
{
    // get the prefix in the path in the configuration file
    QString prefix = TrackingSetup::setupSettingsNameByType(_setupType);

    std::string trackingRoutineName;
    ReadSettingsHelper settings(configurationFileName);
    settings.readVariable(QString("%1/tracking/trackingMethod").arg(prefix), trackingRoutineName);

    return TrackerFactory::trackingRoutineTypeByName(QString::fromUtf8(trackingRoutineName.c_str()));
}
