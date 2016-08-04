#include "CalibrationSettings.hpp"

#include "settings/ReadSettingsHelper.hpp"

#include <QtCore/QFileInfo>

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

    // get the prefix in the path in the configuration file
    QString prefix = SetupType::toSettingsString(setupType);

    std::string calibrationFilePath;
    ReadSettingsHelper settings(configurationFileName);
    settings.readVariable(QString("%1/cameraCalibrationFile").arg(prefix), calibrationFilePath);

    m_calibrationFilePaths[setupType] = QString::fromStdString(calibrationFilePath);
    settingsAccepted = QFileInfo(m_calibrationFilePaths[setupType]).exists();

    return settingsAccepted;
}
