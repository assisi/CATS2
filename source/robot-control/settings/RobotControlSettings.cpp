#include "RobotControlSettings.hpp"

#include <QtCore/QFileInfo>

#include <settings/ReadSettingsHelper.hpp>

/*!
 * The singleton getter. Provides an instance of the settings.
 */
RobotControlSettings& RobotControlSettings::get()
{
    static RobotControlSettings instance;
    return instance;
}

/*!
 * Initializes the parameters from the configuration file.
 */
bool RobotControlSettings::init(QString configurationFileName)
{
    bool settingsAccepted = true;

    ReadSettingsHelper settings(configurationFileName);

    // read the number of robots
    settings.readVariable("robots/numberOfRobots", m_numberOfRobots);
    settingsAccepted = settingsAccepted && (m_numberOfRobots > 0);

    // read the control loop frequency
    settings.readVariable("robots/controlFrequencyHz", m_controlFrequencyHz);
    bool validControlFrequency = (m_controlFrequencyHz > 0) && (m_controlFrequencyHz <= 30);
    settingsAccepted = settingsAccepted && validControlFrequency;
    if (!validControlFrequency) {
        qDebug() << Q_FUNC_INFO << "The control frequency is invalid" << m_controlFrequencyHz;
    }

    // read the frequency divider for the fish motion pattern
    settings.readVariable("robots/fishMotionPatternFrequencyDivider", m_fishMotionPatternFrequencyDivider);
    settingsAccepted = settingsAccepted && (m_fishMotionPatternFrequencyDivider > 0);

    // read settings for every robot.
    for (int i = 1; i <= m_numberOfRobots; i++) {
        RobotSettings robotSettings;
        std::string id = "";
        settings.readVariable(QString("robots/fishBot_%1/id").arg(i), id, id);
        robotSettings.setId(QString::fromStdString(id));

        std::string controlAreasFilePath = "";
        settings.readVariable(QString("robots/fishBot_%1/controlAreasPath").arg(i), controlAreasFilePath, controlAreasFilePath);
        robotSettings.setControlAreasPath(QString::fromStdString(controlAreasFilePath));

        m_robotsSettings.insert(robotSettings.id(), robotSettings);
        settingsAccepted = settingsAccepted && (id.size() > 0);
    }

    // read the fish motion pattern settings
    int distanceCm = 0;
    settings.readVariable("robots/fishMotionPattern/distanceCm", distanceCm, distanceCm);
    m_fishMotionPatternSettings.setDistanceCm(distanceCm);
    int speedCmSec = 0;
    settings.readVariable("robots/fishMotionPattern/speedCmSec", speedCmSec, speedCmSec);
    m_fishMotionPatternSettings.setSpeedCmSec(speedCmSec);
    settingsAccepted = settingsAccepted && m_fishMotionPatternSettings.isValid();

    // read the pid controller settings
    double kp = 0;
    settings.readVariable("robots/pid/kp", kp, kp);
    m_pidControllerSettings.setKp(kp);
    double ki = 0;
    settings.readVariable("robots/pid/ki", ki, ki);
    m_pidControllerSettings.setKi(ki);
    double kd = 0;
    settings.readVariable("robots/pid/kd", kd, kd);
    m_pidControllerSettings.setKd(kd);
    settingsAccepted = settingsAccepted && (m_pidControllerSettings.kp() != 0);

    // read the default linear speed
    m_defaultLinearSpeedCmSec = 0;
    settings.readVariable("robots/defaultLinearSpeedCmSec", m_defaultLinearSpeedCmSec);
    settingsAccepted = settingsAccepted && (m_defaultLinearSpeedCmSec > 0);

    // read the setup map
    std::string setupMap = "";
    settings.readVariable(QString("experiment/setupMapPath"), setupMap, setupMap);
    m_setupMap.init(QString::fromStdString(setupMap));

    return settingsAccepted;
}
