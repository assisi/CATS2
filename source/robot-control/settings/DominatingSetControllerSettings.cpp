#include "DominatingSetControllerSettings.hpp"

#include <settings/ReadSettingsHelper.hpp>

#include <QtCore/QFileInfo>
#include <QtCore/QDir>

/*!
 * Constructor.
 */
DominatingSetControllerSettings::DominatingSetControllerSettings() :
    ExperimentControllerSettings(ExperimentControllerType::DOMINATING_SET)
{

}

/*!
 * Destructor.
 */
DominatingSetControllerSettings::~DominatingSetControllerSettings()
{
    qDebug() << "Destroying the object";
}

/*!
 * Initialization of the parameters for this specific controller.
 */
bool DominatingSetControllerSettings::init(QString configurationFileName)
{
//    // get the path of the configuration file
//    QString configurationFolder = QFileInfo(configurationFileName).path();

//    ReadSettingsHelper settings(configurationFileName);

//    // if this controller is not set in settings then stop here
//    if (!settings.validPath(m_settingPathPrefix))
//        return false;

//    // read the path to the control areas
//    std::string controlAreasFilePath = "";
//    settings.readVariable(QString("%1/controlAreasPath").arg(m_settingPathPrefix),
//                          controlAreasFilePath, controlAreasFilePath);
//    m_data.setControlAreasFileName(configurationFolder +
//                                   QDir::separator() +
//                                   QString::fromStdString(controlAreasFilePath));

//    return (QFileInfo(m_data.controlAreasFileName()).exists());
    return true;
}
