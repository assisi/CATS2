#include "RobotControlSettings.hpp"

#include <QtCore/QFileInfo>
#include <QtCore/QDir>

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

    // get the path of the configuration file
    QString configurationFolder = QFileInfo(configurationFileName).path();

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
    settings.readVariable("robots/navigation/fishMotionPatternFrequencyDivider",
                          m_fishMotionPatternFrequencyDivider);
    settingsAccepted = settingsAccepted && (m_fishMotionPatternFrequencyDivider > 0);

    // read settings for every robot.
    for (int index = 1; index <= m_numberOfRobots; index++) {
        RobotSettings robotSettings;
        std::string id = "";
        settings.readVariable(QString("robots/fishBot_%1/id").arg(index), id, id);
        robotSettings.setId(QString::fromStdString(id));

        // read the robot led's color
        int red;
        settings.readVariable(QString("robots/fishBot_%1/ledColor/r").arg(index), red);
        int green;
        settings.readVariable(QString("robots/fishBot_%1/ledColor/g").arg(index), green);
        int blue;
        settings.readVariable(QString("robots/fishBot_%1/ledColor/b").arg(index), blue);
        robotSettings.setLedColor(QColor(red, green, blue));

        std::string controlAreasFilePath = "";
        settings.readVariable(QString("robots/fishBot_%1/controlAreasPath").arg(index), controlAreasFilePath, controlAreasFilePath);
        robotSettings.setControlAreasFilePath(configurationFolder + QDir::separator() + QString::fromStdString(controlAreasFilePath));

        m_robotsSettings.insert(robotSettings.id(), robotSettings);
        settingsAccepted = settingsAccepted && (id.size() > 0);
    }

    // read the fish motion pattern settings
    int distanceCm = 0;
    settings.readVariable("robots/navigation/fishMotionPattern/distanceCm", distanceCm, distanceCm);
    m_fishMotionPatternSettings.setDistanceCm(distanceCm);
    int speedCmSec = 0;
    settings.readVariable("robots/navigation/fishMotionPattern/speedCmSec", speedCmSec, speedCmSec);
    m_fishMotionPatternSettings.setSpeedCmSec(speedCmSec);
    settingsAccepted = settingsAccepted && m_fishMotionPatternSettings.isValid();

    // read the pid controller settings
    double kp = 0;
    settings.readVariable("robots/navigation/pid/kp", kp, kp);
    m_pidControllerSettings.setKp(kp);
    double ki = 0;
    settings.readVariable("robots/navigation/pid/ki", ki, ki);
    m_pidControllerSettings.setKi(ki);
    double kd = 0;
    settings.readVariable("robots/navigation/pid/kd", kd, kd);
    m_pidControllerSettings.setKd(kd);
    settingsAccepted = settingsAccepted && (m_pidControllerSettings.kp() != 0);

    // read the default linear speed
    m_defaultLinearSpeedCmSec = 0;
    settings.readVariable("robots/navigation/defaultLinearSpeedCmSec", m_defaultLinearSpeedCmSec);
    settingsAccepted = settingsAccepted && (m_defaultLinearSpeedCmSec > 0);

    // read the navigation flag
    m_needOrientationToNavigate = false;
    settings.readVariable("robots/navigation/needOrientationToNavigate", m_needOrientationToNavigate);

    // read the setup map
    std::string setupMap = "";
    settings.readVariable(QString("experiment/setupMapPath"), setupMap, setupMap);
    m_setupMap.init(configurationFolder + QDir::separator() + QString::fromStdString(setupMap));

    // read the number of animals used in experimetns
    settings.readVariable("experiment/agents/numberOfAnimals", m_numberOfAnimals, 0);

    // read the path planning settings
    double gridSizeMeters = 0;
    settings.readVariable("robots/pathPlanning/gridSizeM", gridSizeMeters, gridSizeMeters);
    m_pathPlanningSettings.setGridSizeMeters(gridSizeMeters);

    // read the potential field settings
    settings.readVariable("robots/obstacleAvoidance/potentialField/influenceDistanceArenaM",
                          m_potentialFieldSettings.influenceDistanceArenaMeters,
                          m_potentialFieldSettings.influenceDistanceArenaMeters);
    settings.readVariable("robots/obstacleAvoidance/potentialField/influenceStrengthArena",
                          m_potentialFieldSettings.influenceStrengthArena,
                          m_potentialFieldSettings.influenceStrengthArena);
    settings.readVariable("robots/obstacleAvoidance/potentialField/influenceDistanceRobotsM",
                          m_potentialFieldSettings.influenceDistanceRobotsMeters,
                          m_potentialFieldSettings.influenceDistanceRobotsMeters);
    settings.readVariable("robots/obstacleAvoidance/potentialField/influenceStrengthRobots",
                          m_potentialFieldSettings.influenceStrengthRobots,
                          m_potentialFieldSettings.influenceStrengthRobots);
    settings.readVariable("robots/obstacleAvoidance/potentialField/influenceDistanceTargetM",
                          m_potentialFieldSettings.influenceDistanceTargetMeters,
                          m_potentialFieldSettings.influenceDistanceTargetMeters);
    settings.readVariable("robots/obstacleAvoidance/potentialField/influenceStrengthTarget",
                          m_potentialFieldSettings.influenceStrengthTarget,
                          m_potentialFieldSettings.influenceStrengthTarget);
    settings.readVariable("robots/obstacleAvoidance/potentialField/maxForce",
                          m_potentialFieldSettings.maxForce,
                          m_potentialFieldSettings.maxForce);
    settings.readVariable("robots/obstacleAvoidance/potentialField/maxAngleDeg",
                          m_potentialFieldSettings.maxAngleDeg,
                          m_potentialFieldSettings.maxAngleDeg);
    settings.readVariable("robots/obstacleAvoidance/potentialField/obstacleAvoidanceAreaDiameterM",
                          m_potentialFieldSettings.obstacleAvoidanceAreaDiameterMeters,
                          m_potentialFieldSettings.obstacleAvoidanceAreaDiameterMeters);

    return settingsAccepted;
}
