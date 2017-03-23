#include "ViewerSettings.hpp"

#include "settings/ReadSettingsHelper.hpp"

#include <QtCore/QFileInfo>

/*!
 * The singleton getter. Provides an instance of the settings.
 */
ViewerSettings& ViewerSettings::get()
{
    static ViewerSettings instance;
    return instance;
}

/*!
 * Initializes the parameters from the configuration file.
 */
bool ViewerSettings::init(QString configurationFileName, SetupType::Enum setupType, bool needTargetFrameSize)
{
    bool settingsAccepted = true;

    // get the prefix in the path in the configuration file
    QString prefix = SetupType::toSettingsString(setupType);

    ReadSettingsHelper settings(configurationFileName);

    // read the target image size
    int width;
    settings.readVariable(QString("%1/imageSize/width").arg(prefix), width, -1); // default value to guarantee an
                                                                                 // invalid size if the correct valus is not read
    int height;
    settings.readVariable(QString("%1/imageSize/height").arg(prefix), height, -1); // default value to guarantee an
                                                                                   // invalid size if the correct valus is not read
    m_targetFrameSizes[setupType] = QSize(width, height);

    // check that the settings are valid
    bool foundTargetFrameSize = m_targetFrameSizes[setupType].isValid();
    if (!foundTargetFrameSize) {
        qDebug() << "The target frame size in not set for setup" << SetupType::toString(setupType);
    }
    settingsAccepted = (foundTargetFrameSize || (!needTargetFrameSize));

    return settingsAccepted;
}
