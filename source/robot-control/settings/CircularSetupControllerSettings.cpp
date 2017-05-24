#include "CircularSetupControllerSettings.hpp"

#include <settings/ReadSettingsHelper.hpp>

#include <QtCore/QFileInfo>
#include <QtCore/QDir>

/*!
 * Constructor.
 */
CircularSetupControllerSettings::CircularSetupControllerSettings(ExperimentControllerType::Enum type):
    ExperimentControllerSettings(type)
{

}

/*!
 * Destructor.
 */
CircularSetupControllerSettings::~CircularSetupControllerSettings()
{
    qDebug() << "Destroying the object";
}

/*!
 * Initialization of the parameters for this specific controller.
 */
bool CircularSetupControllerSettings::init(QString configurationFileName)
{
    // get the path of the configuration file
    QString configurationFolder = QFileInfo(configurationFileName).path();

    ReadSettingsHelper settings(configurationFileName);

    // if this controller is not set in settings then stop here
    if (!settings.validPath(m_settingPathPrefix))
        return false;

    // read the path to the control areas
    std::string controlAreasFilePath = "";
    settings.readVariable(QString("%1/controlAreasPath").arg(m_settingPathPrefix),
                          controlAreasFilePath, controlAreasFilePath);
    m_data.setControlAreasFileName(configurationFolder +
                                   QDir::separator() +
                                   QString::fromStdString(controlAreasFilePath));

    // read the delta angle
    double angleDeg = 0;
    settings.readVariable(QString("%1/targetDeltaAngleDeg").arg(m_settingPathPrefix),
                          angleDeg, angleDeg);
    m_data.setTargetDeltaAngleRad(angleDeg * M_PI / 180.);

    return (QFileInfo(m_data.controlAreasFileName()).exists());
}
