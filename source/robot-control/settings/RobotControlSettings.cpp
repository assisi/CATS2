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
        settings.readVariable(QString("robots/fishBot_%1/ledColor/r")
                              .arg(index), red);
        int green;
        settings.readVariable(QString("robots/fishBot_%1/ledColor/g")
                              .arg(index), green);
        int blue;
        settings.readVariable(QString("robots/fishBot_%1/ledColor/b")
                              .arg(index), blue);
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
                                  .arg(ExperimentControllerType::
                                       toSettingsString(controllerType)),
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
    settings.readVariable("robots/navigation/fishMotionPattern/distanceCm",
                          distanceCm, distanceCm);
    m_fishMotionPatternSettings.setDistanceCm(distanceCm);
    int speedCmSec = 0;
    settings.readVariable("robots/navigation/fishMotionPattern/speedCmSec",
                          speedCmSec, speedCmSec);
    m_fishMotionPatternSettings.setSpeedCmSec(speedCmSec);
    settingsAccepted = settingsAccepted && m_fishMotionPatternSettings.isValid();

    // read the pid controller settings
    double kp = 0;
    QString settingsPath = "robots/navigation/pid/kp";
    settings.readVariable(settingsPath, kp, kp);
    m_pidControllerSettings.setKp(kp);
    m_parametersGetters[settingsPath.toStdString()] = [this](std::string) { return m_pidControllerSettings.kp(); };
    m_parametersSetters[settingsPath.toStdString()] =
            [this](double value, std::string) { m_pidControllerSettings.setKp(value); emit notifyPidControllerSettingsChanged(); };

    double ki = 0;
    settingsPath = "robots/navigation/pid/ki";
    settings.readVariable(settingsPath, ki, ki);
    m_pidControllerSettings.setKi(ki);
    m_parametersGetters[settingsPath.toStdString()] = [this](std::string) { return m_pidControllerSettings.ki(); };
    m_parametersSetters[settingsPath.toStdString()] =
            [this](double value, std::string) { m_pidControllerSettings.setKi(value); emit notifyPidControllerSettingsChanged(); };

    double kd = 0;
    settingsPath = "robots/navigation/pid/kd";
    settings.readVariable(settingsPath, kd, kd);
    m_pidControllerSettings.setKd(kd);
    m_parametersGetters[settingsPath.toStdString()] = [this](std::string) { return m_pidControllerSettings.kd(); };
    m_parametersSetters[settingsPath.toStdString()] =
            [this](double value, std::string) { m_pidControllerSettings.setKd(value); emit notifyPidControllerSettingsChanged(); };
    settingsAccepted = settingsAccepted && (!qFuzzyIsNull(m_pidControllerSettings.kp()));

    // read the pid controller settings
    double kpDist = 0;
    settingsPath = "robots/navigation/pid/kpDist";
    settings.readVariable(settingsPath, kpDist, kpDist);
    m_pidControllerSettings.setKpDist(kpDist);
    m_parametersGetters[settingsPath.toStdString()] = [this](std::string) { return m_pidControllerSettings.kpDist(); };
    m_parametersSetters[settingsPath.toStdString()] =
            [this](double value, std::string) { m_pidControllerSettings.setKpDist(value); emit notifyPidControllerSettingsChanged(); };

    double kiDist = 0;
    settingsPath = "robots/navigation/pid/kiDist";
    settings.readVariable(settingsPath, kiDist, kiDist);
    m_pidControllerSettings.setKiDist(kiDist);
    m_parametersGetters[settingsPath.toStdString()] = [this](std::string) { return m_pidControllerSettings.kiDist(); };
    m_parametersSetters[settingsPath.toStdString()] =
            [this](double value, std::string) { m_pidControllerSettings.setKiDist(value); emit notifyPidControllerSettingsChanged(); };

    double kdDist = 0;
    settingsPath = "robots/navigation/pid/kdDist";
    settings.readVariable(settingsPath, kdDist, kdDist);
    m_pidControllerSettings.setKdDist(kdDist);
    m_parametersGetters[settingsPath.toStdString()] = [this](std::string) { return m_pidControllerSettings.kdDist(); };
    m_parametersSetters[settingsPath.toStdString()] =
            [this](double value, std::string) { m_pidControllerSettings.setKdDist(value); emit notifyPidControllerSettingsChanged(); };
    settingsAccepted = settingsAccepted && (!qFuzzyIsNull(m_pidControllerSettings.kpDist()));

    // read the fish model
    readFishModelSettings(settings);

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
        return m_parametersGetters[path](path);
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
        m_parametersSetters[path](value, path);
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

/*!
 * Gets a path in the configuration file and tells to which zone it belongs, i.e.
 * it's index in the list.
 */
int FishModelSettings::indexByPath(std::string pathString)
{
    QString pathQString = QString::fromStdString(pathString);
    int index = pathQString.indexOf("zone_");
    index += 5;
    QString zoneQString;
    while ((index < pathQString.size()) && pathQString[index].isDigit()) {
        zoneQString.append(pathQString[index]);
        index++;
    }
    if (zoneQString.isEmpty())
        return -1;
    else
        return zoneQString.toInt() - 1; // zones start from 1
}


/*!
 * Reads the settings.
 */
void RobotControlSettings::readFishModelSettings(ReadSettingsHelper& reader)
{
    QString settingsPath;
    // read the agent settings
    {
        settingsPath = "robots/fishModel/agent/length";
        reader.readVariable(settingsPath,
                            m_fishModelSettings.agentParameters.length,
                            m_fishModelSettings.agentParameters.length);
        m_parametersGetters[settingsPath.toStdString()] = [this](std::string) { return m_fishModelSettings.agentParameters.length; };
        m_parametersSetters[settingsPath.toStdString()] =
                [this](double value, std::string) { m_fishModelSettings.agentParameters.length = value;
                                       emit notifyFishModelSettingsChanged(); };

        settingsPath = "robots/fishModel/agent/width";
        reader.readVariable(settingsPath,
                            m_fishModelSettings.agentParameters.width,
                            m_fishModelSettings.agentParameters.width);
        m_parametersGetters[settingsPath.toStdString()] = [this](std::string) { return m_fishModelSettings.agentParameters.width; };
        m_parametersSetters[settingsPath.toStdString()] =
                [this](double value, std::string) { m_fishModelSettings.agentParameters.width = value;
                                       emit notifyFishModelSettingsChanged(); };

        settingsPath = "robots/fishModel/agent/height";
        reader.readVariable(settingsPath,
                            m_fishModelSettings.agentParameters.height,
                            m_fishModelSettings.agentParameters.height);
        m_parametersGetters[settingsPath.toStdString()] = [this](std::string) { return m_fishModelSettings.agentParameters.height; };
        m_parametersSetters[settingsPath.toStdString()] =
                [this](double value, std::string) { m_fishModelSettings.agentParameters.height = value;
                                       emit notifyFishModelSettingsChanged(); };

        settingsPath = "robots/fishModel/agent/fov";
        reader.readVariable(settingsPath,
                            m_fishModelSettings.agentParameters.fov,
                            m_fishModelSettings.agentParameters.fov);
        m_parametersGetters[settingsPath.toStdString()] = [this](std::string) { return m_fishModelSettings.agentParameters.fov; };
        m_parametersSetters[settingsPath.toStdString()] =
                [this](double value, std::string) { m_fishModelSettings.agentParameters.fov = value;
                                       emit notifyFishModelSettingsChanged(); };

        settingsPath = "robots/fishModel/agent/meanSpeed";
        reader.readVariable(settingsPath,
                            m_fishModelSettings.agentParameters.meanSpeed,
                            m_fishModelSettings.agentParameters.meanSpeed);
        m_parametersGetters[settingsPath.toStdString()] = [this](std::string) { return m_fishModelSettings.agentParameters.meanSpeed; };
        m_parametersSetters[settingsPath.toStdString()] =
                [this](double value, std::string) { m_fishModelSettings.agentParameters.meanSpeed = value;
                                                    emit notifyFishModelSettingsChanged(); };

        settingsPath = "robots/fishModel/agent/varSpeed";
        reader.readVariable(settingsPath,
                            m_fishModelSettings.agentParameters.varSpeed,
                            m_fishModelSettings.agentParameters.varSpeed);
        m_parametersGetters[settingsPath.toStdString()] = [this](std::string) { return m_fishModelSettings.agentParameters.varSpeed; };
        m_parametersSetters[settingsPath.toStdString()] =
                [this](double value, std::string) { m_fishModelSettings.agentParameters.varSpeed = value;
                                       emit notifyFishModelSettingsChanged(); };

        settingsPath = "robots/fishModel/simulation/dt";
        reader.readVariable(settingsPath,
                            m_fishModelSettings.agentParameters.dt,
                            m_fishModelSettings.agentParameters.dt);
        m_parametersGetters[settingsPath.toStdString()] = [this](std::string) { return m_fishModelSettings.agentParameters.dt; };
        m_parametersSetters[settingsPath.toStdString()] =
                [this](double value, std::string) { m_fishModelSettings.agentParameters.dt = value;
                                   emit notifyFishModelSettingsChanged(); };
    }

    // read the basic model settings
    {
        settingsPath = "robots/fishModel/BM/kappaFishes";
        reader.readVariable(settingsPath,
                            m_fishModelSettings.basicFishModelSettings.kappaFishes,
                            m_fishModelSettings.basicFishModelSettings.kappaFishes);
        m_parametersGetters[settingsPath.toStdString()] = [this](std::string) { return m_fishModelSettings.basicFishModelSettings.kappaFishes; };
        m_parametersSetters[settingsPath.toStdString()] =
                [this](double value, std::string) { m_fishModelSettings.basicFishModelSettings.kappaFishes = value;
                                       emit notifyFishModelSettingsChanged(); };

        settingsPath = "robots/fishModel/BM/alpha";
        reader.readVariable(settingsPath,
                            m_fishModelSettings.basicFishModelSettings.alphasCenter,
                            m_fishModelSettings.basicFishModelSettings.alphasCenter);
        m_parametersGetters[settingsPath.toStdString()] = [this](std::string) { return m_fishModelSettings.basicFishModelSettings.alphasCenter; };
        m_parametersSetters[settingsPath.toStdString()] =
                [this](double value, std::string) { m_fishModelSettings.basicFishModelSettings.alphasCenter = value;
                                       emit notifyFishModelSettingsChanged(); };

        settingsPath = "robots/fishModel/BM/kappaNeutCenter";
        reader.readVariable(settingsPath,
                            m_fishModelSettings.basicFishModelSettings.kappaNeutCenter,
                            m_fishModelSettings.basicFishModelSettings.kappaNeutCenter);
        m_parametersGetters[settingsPath.toStdString()] = [this](std::string) { return m_fishModelSettings.basicFishModelSettings.kappaNeutCenter; };
        m_parametersSetters[settingsPath.toStdString()] =
                [this](double value, std::string) { m_fishModelSettings.basicFishModelSettings.kappaNeutCenter = value;
                                       emit notifyFishModelSettingsChanged(); };

        settingsPath = "robots/fishModel/BM/repulsionFromAgentsAtDist";
        reader.readVariable(settingsPath,
                            m_fishModelSettings.basicFishModelSettings.repulsionFromAgentsAtDist,
                            m_fishModelSettings.basicFishModelSettings.repulsionFromAgentsAtDist);
        m_parametersGetters[settingsPath.toStdString()] = [this](std::string) { return m_fishModelSettings.basicFishModelSettings.repulsionFromAgentsAtDist; };
        m_parametersSetters[settingsPath.toStdString()] =
                [this](double value, std::string) { m_fishModelSettings.basicFishModelSettings.repulsionFromAgentsAtDist = value;
                                       emit notifyFishModelSettingsChanged(); };
    }

    // read the model with walls settings
    {
        settingsPath = "robots/fishModel/BMWithWalls/kappaFishes";
        reader.readVariable(settingsPath,
                            m_fishModelSettings.fishModelWithWallsSettings.kappaFishes,
                            m_fishModelSettings.fishModelWithWallsSettings.kappaFishes);
        m_parametersGetters[settingsPath.toStdString()] = [this](std::string) { return m_fishModelSettings.fishModelWithWallsSettings.kappaFishes; };
        m_parametersSetters[settingsPath.toStdString()] =
                [this](double value, std::string) { m_fishModelSettings.fishModelWithWallsSettings.kappaFishes = value;
                                       emit notifyFishModelSettingsChanged(); };

        settingsPath = "robots/fishModel/BMWithWalls/alpha";
        reader.readVariable(settingsPath,
                            m_fishModelSettings.fishModelWithWallsSettings.alpha,
                            m_fishModelSettings.fishModelWithWallsSettings.alpha);
        m_parametersGetters[settingsPath.toStdString()] = [this](std::string) { return m_fishModelSettings.fishModelWithWallsSettings.alpha; };
        m_parametersSetters[settingsPath.toStdString()] =
                [this](double value, std::string) { m_fishModelSettings.fishModelWithWallsSettings.alpha = value;
                                       emit notifyFishModelSettingsChanged(); };

        settingsPath = "robots/fishModel/BMWithWalls/kappaNeutCenter";
        reader.readVariable(settingsPath,
                            m_fishModelSettings.fishModelWithWallsSettings.kappaNeutCenter,
                            m_fishModelSettings.fishModelWithWallsSettings.kappaNeutCenter);
        m_parametersGetters[settingsPath.toStdString()] = [this](std::string) { return m_fishModelSettings.fishModelWithWallsSettings.kappaNeutCenter; };
        m_parametersSetters[settingsPath.toStdString()] =
                [this](double value, std::string) { m_fishModelSettings.fishModelWithWallsSettings.kappaNeutCenter = value;
                                       emit notifyFishModelSettingsChanged(); };

        settingsPath = "robots/fishModel/BMWithWalls/repulsionFromAgentsAtDist";
        reader.readVariable(settingsPath,
                            m_fishModelSettings.fishModelWithWallsSettings.repulsionFromAgentsAtDist,
                            m_fishModelSettings.fishModelWithWallsSettings.repulsionFromAgentsAtDist);
        m_parametersGetters[settingsPath.toStdString()] = [this](std::string) { return m_fishModelSettings.fishModelWithWallsSettings.repulsionFromAgentsAtDist; };
        m_parametersSetters[settingsPath.toStdString()] =
                [this](double value, std::string) { m_fishModelSettings.fishModelWithWallsSettings.repulsionFromAgentsAtDist = value;
                                       emit notifyFishModelSettingsChanged(); };

        settingsPath = "robots/fishModel/BMWithWalls/wallDistanceThreshold";
        reader.readVariable(settingsPath,
                            m_fishModelSettings.fishModelWithWallsSettings.wallDistanceThreshold,
                            m_fishModelSettings.fishModelWithWallsSettings.wallDistanceThreshold);
        m_parametersGetters[settingsPath.toStdString()] = [this](std::string) { return m_fishModelSettings.fishModelWithWallsSettings.wallDistanceThreshold; };
        m_parametersSetters[settingsPath.toStdString()] =
                [this](double value, std::string) { m_fishModelSettings.fishModelWithWallsSettings.wallDistanceThreshold = value;
                                       emit notifyFishModelSettingsChanged(); };
    }

    // zoned model
    {
        settingsPath = "robots/fishModel/ZonedBM/numberOfZones";
        int numberOfZones = 0;
        reader.readVariable(settingsPath, numberOfZones, numberOfZones);
        if (numberOfZones > 0) {
            for (int zoneIndex = 1; zoneIndex <= numberOfZones; ++zoneIndex) {
                ZonedFishModelSettings settings;
                // read kappaFishes
                settingsPath = QString("robots/fishModel/ZonedBM/zone_%1/kappaFishes").arg(zoneIndex);
                reader.readVariable(settingsPath, settings.kappaFishes, settings.kappaFishes);
                m_parametersGetters[settingsPath.toStdString()] =
                        [this](std::string path) {
                            int index = FishModelSettings::indexByPath(path);
                            if ((index < m_fishModelSettings.zonedFishModelSettings.size()) && (index > 0))
                                return m_fishModelSettings.zonedFishModelSettings[index].kappaFishes;
                            else
                                return static_cast<float>(0.);
                        };
                m_parametersSetters[settingsPath.toStdString()] =
                        [this](double value, std::string path) {
                            int index = FishModelSettings::indexByPath(path);
                            if ((index < m_fishModelSettings.zonedFishModelSettings.size()) && (index > 0)) {
                                m_fishModelSettings.zonedFishModelSettings[index].kappaFishes = value;
                                emit notifyFishModelSettingsChanged();
                            };
                        };
                // read alphasCenter
                settingsPath = QString("robots/fishModel/ZonedBM/zone_%1/alphasCenter").arg(zoneIndex);
                reader.readVariable(settingsPath, settings.alphasCenter, settings.alphasCenter);
                m_parametersGetters[settingsPath.toStdString()] =
                        [this](std::string path) {
                            int index = FishModelSettings::indexByPath(path);
                            if ((index < m_fishModelSettings.zonedFishModelSettings.size()) && (index > 0))
                                return m_fishModelSettings.zonedFishModelSettings[index].alphasCenter;
                            else
                                return static_cast<float>(0.);
                        };
                m_parametersSetters[settingsPath.toStdString()] =
                        [this](double value, std::string path) {
                            int index = FishModelSettings::indexByPath(path);
                            if ((index < m_fishModelSettings.zonedFishModelSettings.size()) && (index > 0)) {
                                m_fishModelSettings.zonedFishModelSettings[index].alphasCenter = value;
                                emit notifyFishModelSettingsChanged();
                            };
                        };
                // read kappaNeutCenter
                settingsPath = QString("robots/fishModel/ZonedBM/zone_%1/kappaNeutCenter").arg(zoneIndex);
                reader.readVariable(settingsPath, settings.kappaNeutCenter, settings.kappaNeutCenter);
                m_parametersGetters[settingsPath.toStdString()] =
                        [this](std::string path) {
                            int index = FishModelSettings::indexByPath(path);
                            if ((index < m_fishModelSettings.zonedFishModelSettings.size()) && (index > 0))
                                return m_fishModelSettings.zonedFishModelSettings[index].kappaNeutCenter;
                            else
                                return static_cast<float>(0.);
                        };
                m_parametersSetters[settingsPath.toStdString()] =
                        [this](double value, std::string path) {
                            int index = FishModelSettings::indexByPath(path);
                            if ((index < m_fishModelSettings.zonedFishModelSettings.size()) && (index > 0)) {
                                m_fishModelSettings.zonedFishModelSettings[index].kappaNeutCenter = value;
                                emit notifyFishModelSettingsChanged();
                            };
                        };
                // read repulsionFromAgentsAtDist
                settingsPath = QString("robots/fishModel/ZonedBM/zone_%1/repulsionFromAgentsAtDist").arg(zoneIndex);
                reader.readVariable(settingsPath, settings.repulsionFromAgentsAtDist, settings.repulsionFromAgentsAtDist);
                m_parametersGetters[settingsPath.toStdString()] =
                        [this](std::string path) {
                            int index = FishModelSettings::indexByPath(path);
                            if ((index < m_fishModelSettings.zonedFishModelSettings.size()) && (index > 0))
                                return m_fishModelSettings.zonedFishModelSettings[index].repulsionFromAgentsAtDist;
                            else
                                return static_cast<float>(0.);
                        };
                m_parametersSetters[settingsPath.toStdString()] =
                        [this](double value, std::string path) {
                            int index = FishModelSettings::indexByPath(path);
                            if ((index < m_fishModelSettings.zonedFishModelSettings.size()) && (index > 0)) {
                                m_fishModelSettings.zonedFishModelSettings[index].repulsionFromAgentsAtDist = value;
                                emit notifyFishModelSettingsChanged();
                            };
                        };
                // read gammaZone
                settingsPath = QString("robots/fishModel/ZonedBM/zone_%1/gammaZone").arg(zoneIndex);
                reader.readVariable(settingsPath, settings.gammaZone, settings.gammaZone);
                m_parametersGetters[settingsPath.toStdString()] =
                        [this](std::string path) {
                            int index = FishModelSettings::indexByPath(path);
                            if ((index < m_fishModelSettings.zonedFishModelSettings.size()) && (index > 0))
                                return m_fishModelSettings.zonedFishModelSettings[index].gammaZone;
                            else
                                return static_cast<float>(0.);
                        };
                m_parametersSetters[settingsPath.toStdString()] =
                        [this](double value, std::string path) {
                            int index = FishModelSettings::indexByPath(path);
                            if ((index < m_fishModelSettings.zonedFishModelSettings.size()) && (index > 0)) {
                                m_fishModelSettings.zonedFishModelSettings[index].gammaZone = value;
                                emit notifyFishModelSettingsChanged();
                            };
                        };
                // read beta
                settingsPath = QString("robots/fishModel/ZonedBM/zone_%1/beta").arg(zoneIndex);
                reader.readVariable(settingsPath, settings.beta, settings.beta);
                m_parametersGetters[settingsPath.toStdString()] =
                        [this](std::string path) {
                            int index = FishModelSettings::indexByPath(path);
                            if ((index < m_fishModelSettings.zonedFishModelSettings.size()) && (index > 0))
                                return m_fishModelSettings.zonedFishModelSettings[index].beta;
                            else
                                return static_cast<float>(0.);
                        };
                m_parametersSetters[settingsPath.toStdString()] =
                        [this](double value, std::string path) {
                            int index = FishModelSettings::indexByPath(path);
                            if ((index < m_fishModelSettings.zonedFishModelSettings.size()) && (index > 0)) {
                                m_fishModelSettings.zonedFishModelSettings[index].beta = value;
                                emit notifyFishModelSettingsChanged();
                            };
                        };
                // read kappaWalls
                settingsPath = QString("robots/fishModel/ZonedBM/zone_%1/kappaWalls").arg(zoneIndex);
                reader.readVariable(settingsPath, settings.kappaWalls, settings.kappaWalls);
                m_parametersGetters[settingsPath.toStdString()] =
                        [this](std::string path) {
                            int index = FishModelSettings::indexByPath(path);
                            if ((index < m_fishModelSettings.zonedFishModelSettings.size()) && (index > 0))
                                return m_fishModelSettings.zonedFishModelSettings[index].kappaWalls;
                            else
                                return static_cast<float>(0.);
                        };
                m_parametersSetters[settingsPath.toStdString()] =
                        [this](double value, std::string path) {
                            int index = FishModelSettings::indexByPath(path);
                            if ((index < m_fishModelSettings.zonedFishModelSettings.size()) && (index > 0)) {
                                m_fishModelSettings.zonedFishModelSettings[index].kappaWalls = value;
                                emit notifyFishModelSettingsChanged();
                            };
                        };
                // read minSpeed
                settingsPath = QString("robots/fishModel/ZonedBM/zone_%1/minSpeed").arg(zoneIndex);
                reader.readVariable(settingsPath, settings.minSpeed, settings.minSpeed);
                m_parametersGetters[settingsPath.toStdString()] =
                        [this](std::string path) {
                            int index = FishModelSettings::indexByPath(path);
                            if ((index < m_fishModelSettings.zonedFishModelSettings.size()) && (index > 0))
                                return m_fishModelSettings.zonedFishModelSettings[index].minSpeed;
                            else
                                return static_cast<float>(0.);
                        };
                m_parametersSetters[settingsPath.toStdString()] =
                        [this](double value, std::string path) {
                            int index = FishModelSettings::indexByPath(path);
                            if ((index < m_fishModelSettings.zonedFishModelSettings.size()) && (index > 0)) {
                                m_fishModelSettings.zonedFishModelSettings[index].minSpeed = value;
                                emit notifyFishModelSettingsChanged();
                            };
                        };
                // read maxSpeed
                settingsPath = QString("robots/fishModel/ZonedBM/zone_%1/maxSpeed").arg(zoneIndex);
                reader.readVariable(settingsPath, settings.maxSpeed, settings.maxSpeed);
                m_parametersGetters[settingsPath.toStdString()] =
                        [this](std::string path) {
                            int index = FishModelSettings::indexByPath(path);
                            if ((index < m_fishModelSettings.zonedFishModelSettings.size()) && (index > 0))
                                return m_fishModelSettings.zonedFishModelSettings[index].maxSpeed;
                            else
                                return static_cast<float>(0.);
                        };
                m_parametersSetters[settingsPath.toStdString()] =
                        [this](double value, std::string path) {
                            int index = FishModelSettings::indexByPath(path);
                            if ((index < m_fishModelSettings.zonedFishModelSettings.size()) && (index > 0)) {
                                m_fishModelSettings.zonedFishModelSettings[index].maxSpeed = value;
                                emit notifyFishModelSettingsChanged();
                            };
                        };
                // read followWalls
                settingsPath = QString("robots/fishModel/ZonedBM/zone_%1/followWalls").arg(zoneIndex);
                reader.readVariable(settingsPath, settings.followWalls, settings.followWalls);
                m_parametersGetters[settingsPath.toStdString()] =
                        [this](std::string path) {
                            int index = FishModelSettings::indexByPath(path);
                            if ((index < m_fishModelSettings.zonedFishModelSettings.size()) && (index > 0))
                                return static_cast<float>(m_fishModelSettings.zonedFishModelSettings[index].followWalls);
                            else
                                return static_cast<float>(0.);
                        };
                m_parametersSetters[settingsPath.toStdString()] =
                        [this](double value, std::string path) {
                            int index = FishModelSettings::indexByPath(path);
                            if ((index < m_fishModelSettings.zonedFishModelSettings.size()) && (index > 0)) {
                                m_fishModelSettings.zonedFishModelSettings[index].followWalls = static_cast<bool>(value);
                                emit notifyFishModelSettingsChanged();
                            };
                        };

                // read polygons
                settingsPath = QString("robots/fishModel/ZonedBM/zone_%1/polygons/numberOfPolygons").arg(zoneIndex);
                int numberOfPolygons;
                reader.readVariable(settingsPath, numberOfPolygons);
                for (int polygonIndex = 1; polygonIndex <= numberOfPolygons; polygonIndex++) {
                    std::vector<cv::Point2f> cvPolygon;
                    settingsPath = QString("robots/fishModel/ZonedBM/zone_%1/polygons/polygon_%2").arg(zoneIndex).arg(polygonIndex);
                    reader.readVariable(settingsPath, cvPolygon);
                    WorldPolygon polygon;
                    for (const auto& point : cvPolygon)
                        polygon.append(PositionMeters(point.x, point.y));
                    settings.zone.append(polygon);
                }

                // read speed histograms
                settingsPath = QString("robots/fishModel/ZonedBM/zone_%1/speedHistogram").arg(zoneIndex);
                reader.readVariable(settingsPath, settings.speedHistogram);

                m_fishModelSettings.zonedFishModelSettings.append(settings);
            }
        }
    }
}
