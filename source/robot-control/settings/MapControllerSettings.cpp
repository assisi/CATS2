#include "MapControllerSettings.hpp"

#include <settings/ReadSettingsHelper.hpp>

#include <QtCore/QFileInfo>
#include <QtCore/QDir>

/*!
 * Constructor.
 */
MapControllerSettings::MapControllerSettings() :
    ExperimentControllerSettings(ExperimentControllerType::CONTROL_MAP)
{

}

/*!
 * Destructor.
 */
MapControllerSettings::~MapControllerSettings()
{
    qDebug() << "Destroying the object";
}

/*!
 * Initialization of the parameters for this specific controller.
 */
bool MapControllerSettings::init(QString configurationFileName)
{
    // get the path of the configuration file
    QString configurationFolder = QFileInfo(configurationFileName).path();

    ReadSettingsHelper settings(configurationFileName);

    // if this controller is not set in settings then stop here
    if (!settings.validPath(m_settingPathPrefix))
        return false;

    std::string controlAreasFilePath = "";
    settings.readVariable(QString("%1/controlAreasPath").arg(m_settingPathPrefix),
                          controlAreasFilePath, controlAreasFilePath);
    m_data.setDefaultControlAreasFileName(configurationFolder +
                                   QDir::separator() +
                                   QString::fromStdString(controlAreasFilePath));

    return (QFileInfo(m_data.defaultControlAreasFileName()).exists());
}


