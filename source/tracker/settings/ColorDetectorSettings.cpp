#include "ColorDetectorSettings.hpp"

#include <settings/ReadSettingsHelper.hpp>

#include <QtCore/QDir>

/*!
 * Constructor.
 */
ColorDetectorSettings::ColorDetectorSettings(SetupType::Enum setupType) :
    TrackingRoutineSettings(setupType)
{
    m_trackingRoutineType = TrackingRoutineType::COLOR_DETECTOR;
}

/*!
 * Initialization of the parameters for this specific method of tracking.
 */
bool ColorDetectorSettings::init(QString configurationFileName)
{
    // get the path of the configuration file
    QString configurationFolder = QFileInfo(configurationFileName).path();

    // read the settings
    ReadSettingsHelper settings(configurationFileName);

    // read the mask file path
    std::string maskFilePath;
    settings.readVariable(QString("%1/tracking/maskFile").arg(m_settingPathPrefix), maskFilePath);
    m_data.setMaskFilePath(configurationFolder.toStdString() + QDir::separator().toLatin1() + maskFilePath);

    int value;
    settings.readVariable(QString("%1/tracking/numberOfAgents").arg(m_settingPathPrefix), value, m_data.numberOfAgents());
    m_data.setNumberOfAgents(value);

    // read the robot led's color
    int red;
    settings.readVariable(QString("%1/tracking/colorDetector/color/r").arg(m_settingPathPrefix), red);
    int green;
    settings.readVariable(QString("%1/tracking/colorDetector/color/g").arg(m_settingPathPrefix), green);
    int blue;
    settings.readVariable(QString("%1/tracking/colorDetector/color/b").arg(m_settingPathPrefix), blue);
    m_data.setColor(QColor(red, green, blue));

    // read the color threshold
    int threshold;
    settings.readVariable(QString("%1/tracking/colorDetector/threshold").arg(m_settingPathPrefix), threshold);
    m_data.setThreshold(threshold);


    return true;
}


