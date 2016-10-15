#include "FishBotLedsTrackingSettings.hpp"

#include <settings/ReadSettingsHelper.hpp>

/*!
 * Constructor.
 */
FishBotLedsTrackingSettings::FishBotLedsTrackingSettings(SetupType::Enum setupType) :
    TrackingRoutineSettings(setupType)
{
    m_trackingRoutineType = TrackingRoutineType::FISHBOT_LEDS_TRACKING;
}

/*!
 * Initialization of the parameters for this specific method of tracking.
 * A part of the parameters is read from the robots' related section of the
 * settings file.
 */
bool FishBotLedsTrackingSettings::init(QString configurationFileName)
{
    // read the settings
    ReadSettingsHelper settings(configurationFileName);

    // read the number of robots to track
    int numberOfRobots;
    settings.readVariable("robots/numberOfRobots", numberOfRobots);
    // read the mask file path
    std::string maskFilePath;
    settings.readVariable(QString("%1/tracking/maskFile").arg(m_settingPathPrefix), maskFilePath);
    m_data.setMaskFilePath(maskFilePath);

    // read the robot specific settings
    for (int robotIndex = 1; robotIndex <= numberOfRobots; ++robotIndex) {
        FishBotLedsTrackingSettingsData::FishBotDescription robotDescription;
        // read the robot's id
        std::string id;
        settings.readVariable(QString("robots/fishBot_%2/id").arg(robotIndex), id);
        robotDescription.id = QString::fromStdString(id);
        // read the robot led's color
        int red;
        settings.readVariable(QString("%1/tracking/fishBotLedsTracking/fishBot_%2/color/r").arg(m_settingPathPrefix).arg(id.data()), red);
        int green;
        settings.readVariable(QString("%1/tracking/fishBotLedsTracking/fishBot_%2/color/g").arg(m_settingPathPrefix).arg(id.data()), green);
        int blue;
        settings.readVariable(QString("%1/tracking/fishBotLedsTracking/fishBot_%2/color/b").arg(m_settingPathPrefix).arg(id.data()), blue);
        robotDescription.ledColor = QColor(red, green, blue);
        // read the color threshold
        settings.readVariable(QString("%1/tracking/fishBotLedsTracking/fishBot_%2/threshold").arg(m_settingPathPrefix).arg(id.data()), robotDescription.colorThreshold);
        m_data.addRobotDescription(robotDescription);
    }

    return (numberOfRobots > 0);
}
