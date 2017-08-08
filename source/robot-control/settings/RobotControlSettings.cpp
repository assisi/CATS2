#include "RobotControlSettings.hpp"
#include "experiment-controllers/ExperimentControllerType.hpp"
#include "experiment-controllers/ExperimentControllerFactory.hpp"

#include <settings/CommandLineParameters.hpp>
#include <statistics/StatisticsPublisher.hpp>

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
    bool validControlFrequency = (m_controlFrequencyHz > 0) &&
            (m_controlFrequencyHz <= 30);
    settingsAccepted = settingsAccepted && validControlFrequency;
    if (!validControlFrequency) {
        qDebug() << "The control frequency is invalid"
                 << m_controlFrequencyHz;
    }

    // read the frequency divider for the fish motion pattern
    settings.readVariable("robots/navigation/fishMotionPatternFrequencyDivider",
                          m_fishMotionPatternFrequencyDivider);
    settingsAccepted = settingsAccepted &&
            (m_fishMotionPatternFrequencyDivider > 0);

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

        // read the connection target line
        std::string target = "";
        settings.readVariable(QString("robots/fishBot_%1/connectionTarget")
                              .arg(index),
                              target, target);
        robotSettings.setConnectionTarget(QString::fromStdString(target));

        // read the control areas for all available controllers
        for (int type = ExperimentControllerType::CONTROL_MAP;
             type <= ExperimentControllerType::INITIATION_LURE; type++ )
        {
            ExperimentControllerType::Enum controllerType =
                    static_cast<ExperimentControllerType::Enum>(type);
            std::string controlAreaFile = "";
            settings.readVariable(QString("robots/fishBot_%1/robotControlMaps/%2/path")
                                  .arg(index)
                                  .arg(ExperimentControllerType::toSettingsString(controllerType)),
                                  controlAreaFile);
            if (!controlAreaFile.empty())
                robotSettings.setControlAreasFile(controllerType,
                                                  configurationFolder +
                                                  QDir::separator() +
                                                  QString::fromStdString(controlAreaFile));
        }

        // add settings
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
    QString settingsPath = "robots/navigation/pid/kp";
    settings.readVariable(settingsPath, kp, kp);
    m_pidControllerSettings.setKp(kp);
    m_parametersGetters[settingsPath.toStdString()] = [this]() { return m_pidControllerSettings.kp(); };
    m_parametersSetters[settingsPath.toStdString()] =
            [this](double value) { m_pidControllerSettings.setKp(value); emit notifyPidControllerSettingsChanged(); };

    double ki = 0;
    settingsPath = "robots/navigation/pid/ki";
    settings.readVariable(settingsPath, ki, ki);
    m_pidControllerSettings.setKi(ki);
    m_parametersGetters[settingsPath.toStdString()] = [this]() { return m_pidControllerSettings.ki(); };
    m_parametersSetters[settingsPath.toStdString()] =
            [this](double value) { m_pidControllerSettings.setKi(value); emit notifyPidControllerSettingsChanged(); };

    double kd = 0;
    settingsPath = "robots/navigation/pid/kd";
    settings.readVariable(settingsPath, kd, kd);
    m_pidControllerSettings.setKd(kd);
    m_parametersGetters[settingsPath.toStdString()] = [this]() { return m_pidControllerSettings.kd(); };
    m_parametersSetters[settingsPath.toStdString()] =
            [this](double value) { m_pidControllerSettings.setKd(value); emit notifyPidControllerSettingsChanged(); };
    settingsAccepted = settingsAccepted && (!qFuzzyIsNull(m_pidControllerSettings.kp()));

    // read the pid controller settings
    double kpDist = 0;
    settingsPath = "robots/navigation/pid/kpDist";
    settings.readVariable(settingsPath, kpDist, kpDist);
    m_pidControllerSettings.setKpDist(kpDist);
    m_parametersGetters[settingsPath.toStdString()] = [this]() { return m_pidControllerSettings.kpDist(); };
    m_parametersSetters[settingsPath.toStdString()] =
            [this](double value) { m_pidControllerSettings.setKpDist(value); emit notifyPidControllerSettingsChanged(); };

    double kiDist = 0;
    settingsPath = "robots/navigation/pid/kiDist";
    settings.readVariable(settingsPath, kiDist, kiDist);
    m_pidControllerSettings.setKiDist(kiDist);
    m_parametersGetters[settingsPath.toStdString()] = [this]() { return m_pidControllerSettings.kiDist(); };
    m_parametersSetters[settingsPath.toStdString()] =
            [this](double value) { m_pidControllerSettings.setKiDist(value); emit notifyPidControllerSettingsChanged(); };

    double kdDist = 0;
    settingsPath = "robots/navigation/pid/kdDist";
    settings.readVariable(settingsPath, kdDist, kdDist);
    m_pidControllerSettings.setKdDist(kdDist);
    m_parametersGetters[settingsPath.toStdString()] = [this]() { return m_pidControllerSettings.kdDist(); };
    m_parametersSetters[settingsPath.toStdString()] =
            [this](double value) { m_pidControllerSettings.setKdDist(value); emit notifyPidControllerSettingsChanged(); };
    settingsAccepted = settingsAccepted && (!qFuzzyIsNull(m_pidControllerSettings.kpDist()));

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

    // read the model settings
    settingsPath = "robots/fishModel/agent/length";
    settings.readVariable(settingsPath,
                          m_fishModelSettings.length,
                          m_fishModelSettings.length);
    m_parametersGetters[settingsPath.toStdString()] = [this]() { return m_fishModelSettings.length; };
    m_parametersSetters[settingsPath.toStdString()] =
            [this](double value) { m_fishModelSettings.length = value;
                                   emit notifyFishModelSettingsChanged(); };

    settingsPath = "robots/fishModel/agent/width";
    settings.readVariable(settingsPath,
                          m_fishModelSettings.width,
                          m_fishModelSettings.width);
    m_parametersGetters[settingsPath.toStdString()] = [this]() { return m_fishModelSettings.width; };
    m_parametersSetters[settingsPath.toStdString()] =
            [this](double value) { m_fishModelSettings.width = value;
                                   emit notifyFishModelSettingsChanged(); };

    settingsPath = "robots/fishModel/agent/height";
    settings.readVariable(settingsPath,
                          m_fishModelSettings.height,
                          m_fishModelSettings.height);
    m_parametersGetters[settingsPath.toStdString()] = [this]() { return m_fishModelSettings.height; };
    m_parametersSetters[settingsPath.toStdString()] =
            [this](double value) { m_fishModelSettings.height = value;
                                   emit notifyFishModelSettingsChanged(); };

    settingsPath = "robots/fishModel/agent/fov";
    settings.readVariable(settingsPath,
                          m_fishModelSettings.fov,
                          m_fishModelSettings.fov);
    m_parametersGetters[settingsPath.toStdString()] = [this]() { return m_fishModelSettings.fov; };
    m_parametersSetters[settingsPath.toStdString()] =
            [this](double value) { m_fishModelSettings.fov = value;
                                   emit notifyFishModelSettingsChanged(); };

    settingsPath = "robots/fishModel/agent/meanSpeed";
    settings.readVariable(settingsPath,
                          m_fishModelSettings.meanSpeed,
                          m_fishModelSettings.meanSpeed);
    m_parametersGetters[settingsPath.toStdString()] = [this]() { return m_fishModelSettings.meanSpeed; };
    m_parametersSetters[settingsPath.toStdString()] =
            [this](double value) { m_fishModelSettings.meanSpeed = value;
                                   emit notifyFishModelSettingsChanged(); };

    settingsPath = "robots/fishModel/agent/varSpeed";
    settings.readVariable(settingsPath,
                          m_fishModelSettings.varSpeed,
                          m_fishModelSettings.varSpeed);
    m_parametersGetters[settingsPath.toStdString()] = [this]() { return m_fishModelSettings.varSpeed; };
    m_parametersSetters[settingsPath.toStdString()] =
            [this](double value) { m_fishModelSettings.varSpeed = value;
                                   emit notifyFishModelSettingsChanged(); };

    settingsPath = "robots/fishModel/BM/kappaFishes";
    settings.readVariable(settingsPath,
                          m_fishModelSettings.kappaFishes,
                          m_fishModelSettings.kappaFishes);
    m_parametersGetters[settingsPath.toStdString()] = [this]() { return m_fishModelSettings.kappaFishes; };
    m_parametersSetters[settingsPath.toStdString()] =
            [this](double value) { m_fishModelSettings.kappaFishes = value;
                                   emit notifyFishModelSettingsChanged(); };

    settingsPath = "robots/fishModel/BM/alphasCenter";
    settings.readVariable(settingsPath,
                          m_fishModelSettings.alphasCenter,
                          m_fishModelSettings.alphasCenter);
    m_parametersGetters[settingsPath.toStdString()] = [this]() { return m_fishModelSettings.alphasCenter; };
    m_parametersSetters[settingsPath.toStdString()] =
            [this](double value) { m_fishModelSettings.alphasCenter = value;
                                   emit notifyFishModelSettingsChanged(); };

    settingsPath = "robots/fishModel/BM/kappaNeutCenter";
    settings.readVariable(settingsPath,
                          m_fishModelSettings.kappaNeutCenter,
                          m_fishModelSettings.kappaNeutCenter);
    m_parametersGetters[settingsPath.toStdString()] = [this]() { return m_fishModelSettings.kappaNeutCenter; };
    m_parametersSetters[settingsPath.toStdString()] =
            [this](double value) { m_fishModelSettings.kappaNeutCenter = value;
                                   emit notifyFishModelSettingsChanged(); };

    settingsPath = "robots/fishModel/BM/repulsionFromAgentsAtDist";
    settings.readVariable(settingsPath,
                          m_fishModelSettings.repulsionFromAgentsAtDist,
                          m_fishModelSettings.repulsionFromAgentsAtDist);
    m_parametersGetters[settingsPath.toStdString()] = [this]() { return m_fishModelSettings.repulsionFromAgentsAtDist; };
    m_parametersSetters[settingsPath.toStdString()] =
            [this](double value) { m_fishModelSettings.repulsionFromAgentsAtDist = value;
                                   emit notifyFishModelSettingsChanged(); };

    settingsPath = "robots/fishModel/simulation/dt";
    settings.readVariable(settingsPath,
                          m_fishModelSettings.dt,
                          m_fishModelSettings.dt);
    m_parametersGetters[settingsPath.toStdString()] = [this]() { return m_fishModelSettings.dt; };
    m_parametersSetters[settingsPath.toStdString()] =
            [this](double value) { m_fishModelSettings.dt = value;
                                   emit notifyFishModelSettingsChanged(); };

    // read a trajectory for the Trajectory control mode
    std::string relativeTrajectoryPath = "";
    settings.readVariable("robots/controlModes/trajectory/points",
                          relativeTrajectoryPath,
                          relativeTrajectoryPath);
    QString trajectoryPath = configurationFolder +
                                QDir::separator() +
                                QString::fromStdString(relativeTrajectoryPath);
    QFileInfo trajectoryFile(trajectoryPath);
    if (trajectoryFile.exists() && trajectoryFile.isFile()) {
        ReadSettingsHelper trajectorySettings(trajectoryPath);
        std::vector<cv::Point2f> polygon;
        trajectorySettings.readVariable(QString("polygon"), polygon);
        if (polygon.size() > 0) {
            for (auto& point : polygon)
                m_trajectory << PositionMeters(point);
            qDebug() << QString("Loaded trajectory of %1 points, shared by all "
                                "robots").arg(m_trajectory.size());
        } else {
            qDebug() << "The trajectory is empty";
        }
    } else {
        qDebug() << "Could not find the trajectory file";
    }
    // read the corresponding flags
    m_loopTrajectory = true;
    settings.readVariable("robots/controlModes/trajectory/loopTrajectory",
                          m_loopTrajectory,
                          m_loopTrajectory);
    m_providePointsOnTimer = false;
    settings.readVariable("robots/controlModes/trajectory/providePointsOnTimer",
                          m_providePointsOnTimer,
                          m_providePointsOnTimer);
    if (m_providePointsOnTimer) {
        m_trajectoryUpdateRateHz = 0;
        settings.readVariable("robots/controlModes/trajectory/updateRateHz",
                              m_trajectoryUpdateRateHz,
                              m_trajectoryUpdateRateHz);
    }

    settings.close();

    // read the settings for all available controllers
    for (int type = ExperimentControllerType::CONTROL_MAP;
         type < ExperimentControllerType::UNDEFINED; type++ )
    {
        ExperimentControllerType::Enum controllerType =
                static_cast<ExperimentControllerType::Enum>(type);
        ExperimentControllerSettingsPtr controllerSettings =
                    ExperimentControllerFactory::createSettings(controllerType);
        if (!controllerSettings.isNull() &&
                controllerSettings->init(configurationFileName))
        {
            m_controllerSettings.insert(controllerType, controllerSettings);
        }
    }

    return settingsAccepted;
}

/*!
 * Gives the settings for the given controller type.
 */
ExperimentControllerSettingsPtr RobotControlSettings::controllerSettings(ExperimentControllerType::Enum type)
{
    if (m_controllerSettings.contains(type))
        return m_controllerSettings.value(type);
    else
        return ExperimentControllerSettingsPtr();
}

/*!
 * Provides the settings value by its path in the configuration file. Only
 * numerical values are supported.
 */
double RobotControlSettings::valueByPath(std::string path)
{
    if (m_parametersGetters.count(path) > 0) {
        return m_parametersGetters[path]();
    } else {
        qDebug() << QString("Path %1 is not supported.").arg(QString::fromStdString(path));
        return 0;
    }
}

/*!
 * Sets the settings value by its path in the configuration file. Only numerical
 * values are supported.
 */
void RobotControlSettings::setValueByPath(std::string path, double value)
{
    if (m_parametersSetters.count(path) > 0) {
        m_parametersSetters[path](value);
    } else {
        qDebug() << QString("Path %1 is not supported.").arg(QString::fromStdString(path));
    }
}

/*!
 * Constructor. Defining it here prevents construction.
 */
RobotControlSettings::RobotControlSettings() :
    QObject(nullptr)
{
    // starts the robot statistics publisher
    // TODO : move it somewhere else
    if (CommandLineParameters::get().publishRobotsStatistics())
        StatisticsPublisher::get();
}
