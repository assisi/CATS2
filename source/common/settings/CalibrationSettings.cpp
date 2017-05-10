#include "CalibrationSettings.hpp"

#include "settings/ReadSettingsHelper.hpp"

#include <QtCore/QFileInfo>
#include <QtCore/QDir>

/*!
 * The singleton getter. Provides an instance of the settings.
 */
CalibrationSettings& CalibrationSettings::get()
{
    static CalibrationSettings instance;
    return instance;
}

/*!
 * Initializes the parameters from the configuration file.
 */
bool CalibrationSettings::init(QString configurationFileName, SetupType::Enum setupType)
{
    bool settingsAccepted = true;

    // get the path of the configuration file
    QString configurationFolder = QFileInfo(configurationFileName).path();

    // get the prefix in the path in the configuration file
    QString prefix = SetupType::toSettingsString(setupType);

    std::string calibrationFilePath;
    ReadSettingsHelper settings(configurationFileName);
    settings.readVariable(QString("%1/cameraCalibrationFile").arg(prefix), calibrationFilePath);

    m_calibrationFilePaths[setupType] = configurationFolder + QDir::separator() + QString::fromStdString(calibrationFilePath);
    settingsAccepted = settingsAccepted && QFileInfo(m_calibrationFilePaths[setupType]).exists();

    // read the target image size
    int width;
    settings.readVariable(QString("%1/imageSize/width").arg(prefix), width, -1); // default value to guarantee an
                                                                                    // invalid size if the correct valus is not read
    int height;
    settings.readVariable(QString("%1/imageSize/height").arg(prefix), height, -1); // default value to guarantee an
                                                                                        // invalid size if the correct valus is not read
    m_videoFrameSizes[setupType] = QSize(width, height);
    settingsAccepted = settingsAccepted && m_videoFrameSizes[setupType].isValid();

    return settingsAccepted;
}
