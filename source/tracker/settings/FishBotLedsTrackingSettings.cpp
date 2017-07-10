#include "FishBotLedsTrackingSettings.hpp"

#include <settings/ReadSettingsHelper.hpp>

#include <QtCore/QDir>

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
    // get the path of the configuration file
    QString configurationFolder = QFileInfo(configurationFileName).path();

    // read the settings
    ReadSettingsHelper settings(configurationFileName);

    // read the number of robots to track
    int numberOfRobots;
    settings.readVariable("robots/numberOfRobots", numberOfRobots);
    // read the mask file path
    std::string maskFilePath;
    settings.readVariable(QString("%1/tracking/maskFile").arg(m_settingPathPrefix), maskFilePath);
    m_data.setMaskFilePath(configurationFolder.toStdString() + QDir::separator().toLatin1() + maskFilePath);

    // read the robot specific settings
    for (int robotIndex = 1; robotIndex <= numberOfRobots; ++robotIndex) {
        FishBotLedsTrackingSettingsData::FishBotDescription robotDescription;
        // read the robot's id
        std::string id;
        settings.readVariable(QString("robots/fishBot_%1/id").arg(robotIndex), id);
        robotDescription.id = QString::fromStdString(id);
        // read the robot led's color
        int red;
        settings.readVariable(QString("robots/fishBot_%1/ledColor/r").arg(robotIndex), red);
        int green;
        settings.readVariable(QString("robots/fishBot_%1/ledColor/g").arg(robotIndex), green);
        int blue;
        settings.readVariable(QString("robots/fishBot_%1/ledColor/b").arg(robotIndex), blue);
        robotDescription.ledColor = QColor(red, green, blue);
        // read the color threshold
        settings.readVariable(QString("%1/tracking/fishBotLedsTracking/fishBot_%2/threshold")
                              .arg(m_settingPathPrefix)
                              .arg(id.data()),
                              robotDescription.colorThreshold);
        // read the area mask file path
        std::string areaMaskFilePath;
        settings.readVariable(QString("%1/tracking/fishBotLedsTracking/fishBot_%2/areaMaskFile")
                              .arg(m_settingPathPrefix)
                              .arg(id.data()),
                              areaMaskFilePath);
        robotDescription.areaMaskFilePath = configurationFolder.toStdString() +
                QDir::separator().toLatin1() + areaMaskFilePath;
        m_data.addRobotDescription(robotDescription);
    }

    return true;
}
