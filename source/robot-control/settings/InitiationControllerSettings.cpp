#include "InitiationControllerSettings.hpp"

#include <settings/ReadSettingsHelper.hpp>

#include <QtCore/QFileInfo>
#include <QtCore/QDir>

/*!
 * Constructor.
 */
InitiationControllerSettings::InitiationControllerSettings() :
    ExperimentControllerSettings(ExperimentControllerType::INITIATION)
{

}

/*!
 * Destructor.
 */
InitiationControllerSettings::~InitiationControllerSettings()
{
    qDebug() << "Destroying the object";
}

/*!
 * Initialization of the parameters for this specific controller.
 */
bool InitiationControllerSettings::init(QString configurationFileName)
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

    // read the departure trigger
    std::string departureTrigger = "";
    settings.readVariable(QString("%1/departureTrigger").arg(m_settingPathPrefix),
                          departureTrigger, departureTrigger);
    m_data.setDepartureTrigger(QString::fromStdString(departureTrigger));

    // read the departure timeout
    double value;
    settings.readVariable(QString("%1/departureOnTimeOut/departureTimeOutSec")
                          .arg(m_settingPathPrefix),
                          value, m_data.departureTimeOutSec());
    m_data.setDepartureTimeOutSec(value);

    // read the departure number of fish
    int fishNumber;
    settings.readVariable(QString("%1/departureWhenInGroup/fishNumberAround").arg(m_settingPathPrefix),
                          fishNumber, m_data.fishNumberAroundOnDeparture());
    m_data.setFishNumberAroundOnDeparture(fishNumber);

    // read the radius around the robot where we search for fish.
    settings.readVariable(QString("%1/departureWhenInGroup/groupRadius").arg(m_settingPathPrefix),
                          value, m_data.groupRadius());
    m_data.setGroupRadius(value);

    // read the following check timeout
    settings.readVariable(QString("%1/fishFollowCheckTimeOutSec").arg(m_settingPathPrefix),
                          value, m_data.fishFollowCheckTimeOutSec());
    m_data.setFishFollowCheckTimeOutSec(value);

    // read the fish staying limit number
    settings.readVariable(QString("%1/maximalFishNumberAllowedToStay").arg(m_settingPathPrefix),
                          fishNumber, m_data.maximalFishNumberAllowedToStay());
    m_data.setMaximalFishNumberAllowedToStay(fishNumber);

    return (QFileInfo(m_data.controlAreasFileName()).exists());
}
