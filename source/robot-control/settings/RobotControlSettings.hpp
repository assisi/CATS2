#ifndef CATS2_ROBOT_CONTROL_SETTINGS_HPP
#define CATS2_ROBOT_CONTROL_SETTINGS_HPP

#include "SetupMap.hpp"
#include "experiment-controllers/ExperimentControllerType.hpp"
#include "RobotControlPointerTypes.hpp"

#include <QtCore/QString>
#include <QtCore/QMap>

/*!
 * Stores settings of the fish model.
 */
// TODO : read from the configuration file.
// TODO : make the class and members private.
struct FishModelSettings {
public:
    //! Constructor.
    explicit FishModelSettings() {}

    //! Agents parameters.
    float length = 0.02;
    float width = 0.02;
    float height = 0.01;
    int fov = 169;
    float meanSpeed = -2.65; // Log-normal distribution (so negative mean is normal)
    float varSpeed = 0.51;

    //! BM parameters.
    float kappaFishes = 20.0;  //! \kappa_f
    float alphasCenter = 55.0;   //! \alpha_0
    float kappaNeutCenter = 6.3; //! \kappa_0
    float repulsionFromAgentsAtDist = 0.02; //! Repulsion from agent if other is too close

    //! Simulation parameters.
    float dt = 1.;
};

/*!
 * Stores settings of one robot.
 */
class RobotSettings
{
public:
    //! Constructor.
    explicit RobotSettings() { }

    //! Sets id.
    void setId(QString id) { m_id = id; }
    //! Return id.
    QString id() const { return m_id; }

    //! Set color.
    void setLedColor(QColor color) { m_ledColor = color; }
    //! Get color.
    QColor ledColor() const { return m_ledColor; }

    //! Set the target.
    void setConnectionTarget(QString target) { m_connectionTarget = target; }
    //! Get the target.
    QString connectionTarget() const { return m_connectionTarget; }

    //! Set the control areas file for the experiment.
    void setControlAreasFile(ExperimentControllerType::Enum type, QString path)
    {
        m_controlAreasFileNames[type] = path;
    }
    //! Get the control areas file for the experiment.
    QString controlAreasFile(ExperimentControllerType::Enum type)
    {
        if (m_controlAreasFileNames.contains(type))
            return m_controlAreasFileNames[type];
        else
            return "";
    }

private:
    //! Robot's id.
    QString m_id;
    //! Robot's color.
    QColor m_ledColor;
    //! The target to connect directly to the robot.
    QString m_connectionTarget;
    //! The path to the file describing the control areas. Normally it is setup
    //! par a controller, but for some experiments we might need to have a
    //! robot specific control area.
    QMap<ExperimentControllerType::Enum, QString> m_controlAreasFileNames;
};

/*!
 * Stores settings of fish motion pattern.
 */
class FishMotionPatternSettings
{
public:
    //! Constructor.
    explicit FishMotionPatternSettings():
        m_distanceCm(10.),
        m_speedCmSec(15.)
    { }

    //! Sets distance to accelerate.
    void setDistanceCm(int distance) { m_distanceCm = distance; }
    //! Returns distance to accelerate.
    int distanceCm() const { return m_distanceCm; }

    //! Sets the speed to go after acceleration.
    void setSpeedCmSec(int speed) { m_speedCmSec = speed; }
    //! Returns distance to accelerate.
    int speedCmSec() const { return m_speedCmSec; }

    //! Checks the validity.
    bool isValid() const { return (m_distanceCm > 0) && (m_speedCmSec > 0); }

private:
    //! Distance to accelerate.
    int m_distanceCm; // [cm]
    //! Speed to keep after accelerating.
    int m_speedCmSec; // [cm/sec]
};

/*!
 * Stores settings of PID controller.
 */
class PidControllerSettings
{
public:
    //! Constructor.
    explicit PidControllerSettings():
        m_kp(1),
        m_ki(0),
        m_kd(0),
        m_kpDist(100),
        m_kiDist(0),
        m_kdDist(0)
    { }

    //! Sets proportional coefficient.
    void setKp(double kp) { m_kp = kp; }
    //! Returns proportional coefficient.
    double kp() const { return m_kp; }

    //! Sets integral coefficient.
    void setKi(double ki) { m_ki = ki; }
    //! Returns integral coefficient.
    double ki() const { return m_ki; }

    //! Sets derivative coefficient.
    void setKd(double kd) { m_kd = kd; }
    //! Returns derivative coefficient.
    double kd() const { return m_kd; }

    //! Sets proportional coefficient.
    void setKpDist(double kpDist) { m_kpDist = kpDist; }
    //! Returns proportional coefficient.
    double kpDist() const { return m_kpDist; }

    //! Sets integral coefficient.
    void setKiDist(double kiDist) { m_kiDist = kiDist; }
    //! Returns integral coefficient.
    double kiDist() const { return m_kiDist; }

    //! Sets derivative coefficient.
    void setKdDist(double kdDist) { m_kdDist = kdDist; }
    //! Returns derivative coefficient.
    double kdDist() const { return m_kdDist; }

private:
    //! Proportional coefficient.
    double m_kp;
    //! Integral coefficient.
    double m_ki;
    //! Derivative coefficient.
    double m_kd;
    //! Proportional coefficient.
    double m_kpDist;
    //! Integral coefficient.
    double m_kiDist;
    //! Derivative coefficient.
    double m_kdDist;
};

/*!
 * Stores settings of path planning.
 */
class PathPlanningSettings
{
public:
    //! Constructor.
    explicit PathPlanningSettings() : m_gridSizeMeters(0.0)
    { }

    //! Sets the grid size.
    void setGridSizeMeters(double gridSize) { m_gridSizeMeters = gridSize; }
    //! Returns the grid size.
    double gridSizeMeters() const { return m_gridSizeMeters; }

private:
    //! The size of the grid square for the grid based path planning.
    double m_gridSizeMeters; // meters
};

/*!
 * Stores settings for the potential field obstacle avoidance
 */
struct PotentialFieldSettings
{
    //! Constructor.
    explicit PotentialFieldSettings() :
        influenceDistanceArenaMeters(0.03),
        influenceStrengthArena(10),
        influenceDistanceRobotsMeters(0.09),
        influenceStrengthRobots(20),
        influenceDistanceTargetMeters(0.03),
        influenceStrengthTarget(2),
        maxForce(1000),
        maxAngleDeg(60),
        obstacleAvoidanceAreaDiameterMeters(0.1)
    { }

    //! Repulsive parameters rho0 being the distance of influence and nu the
    //! "strength" of the repulsion.
    float influenceDistanceArenaMeters;
    float influenceStrengthArena;
    float influenceDistanceRobotsMeters;
    float influenceStrengthRobots;
    //! Attractive parameters.
    float influenceDistanceTargetMeters;
    float influenceStrengthTarget;
    //! Maximal acceptable force, an empirical parameter,
    float maxForce;
    //! A cone where we check the presence of other robots to avoid.
    float maxAngleDeg;
    //! The local window size around the robot.
    float obstacleAvoidanceAreaDiameterMeters;
};

/*!
 * Class-signleton that is used to store parameters of the robot control system.
 * Their values are loaded from the configuration file.
 * NOTE : All the settings are made as singltons to simplify the access to them;
 * the drawback is that initialization of many objects becomes obscure because of this.
 * NOTE : All the settings must be initialized on the program startup.
*/
class RobotControlSettings
{
public:
    //! The singleton getter.
    static RobotControlSettings& get();

    //! Initializes the parameters from the configuration file.
    bool init(QString configurationFileName);

    // delete copy and move constructors and assign operators
    //! Copy constructor.
    RobotControlSettings(RobotControlSettings const&) = delete;
    //! Move constructor.
    RobotControlSettings(RobotControlSettings&&) = delete;
    //! Copy assignment.
    RobotControlSettings& operator=(RobotControlSettings const&) = delete;
    //! Move assignment.
    RobotControlSettings& operator=(RobotControlSettings &&) = delete;

public:
    //! Returns the number of robots.
    int numberOfRobots() const { return m_numberOfRobots; }
    //! Returns the contol loop frequency.
    int controlFrequencyHz() const { return m_controlFrequencyHz; }
    //! Gives the reference to the fish motion pattern settngs.
    const FishMotionPatternSettings& fishMotionPatternSettings() const { return m_fishMotionPatternSettings; }
    //! Returns the frequency divider for the navigation commands
    //! for fish motion pattern.
    int fishMotionPatternFrequencyDivider() const { return m_fishMotionPatternFrequencyDivider; }
    //! Gives the reference to the PID controller settngs.
    const PidControllerSettings& pidControllerSettings() const { return m_pidControllerSettings; }

    //! Returns the settings for given robot.
    RobotSettings robotSettings(QString id) const { return m_robotsSettings[id]; }
    //! Returns the list of all robot ids.
    QList<QString > ids() const { return m_robotsSettings.keys(); }

    //! Returns the default linear speed.
    int defaultLinearSpeedCmSec() const { return m_defaultLinearSpeedCmSec; }

    //! Returns the flag that defines if the robot needs an orientation to
    //! navigate.
    int needOrientationToNavigate() const { return m_needOrientationToNavigate; }

    //! Gives the const reference to the experimental setup map.
    const SetupMap& setupMap() const { return m_setupMap; }

    //! Return the number of animals used in the experiment.
    int numberOfAnimals() const { return m_numberOfAnimals; }

    //! Gives the const reference to the fish model parameters.
    const FishModelSettings& fishModelSettings() const { return m_fishModelSettings; }

    //! Gives the settings for the given controller type.
    ExperimentControllerSettingsPtr controllerSettings(ExperimentControllerType::Enum type);
    //! Returns the list of available controllers.
    QList<ExperimentControllerType::Enum> availableControllers() const
    {
        return m_controllerSettings.keys();
    }

    //! Gives the reference to the path planning settings.
    const PathPlanningSettings& pathPlanningSettings() const { return m_pathPlanningSettings; }

    //! Gives the reference to the potential field obstacle avoidance settings.
    const PotentialFieldSettings& potentialFieldSettings() const { return m_potentialFieldSettings; }

    //! Returns the predefined trajectory for the Trajectory control mode.
    QList<PositionMeters> trajectory() const { return m_trajectory; }
    //! Returns the flag defining if the trajectory should be restarted once the
    //! last point is reached.
    bool loopTrajectory() { return m_loopTrajectory; }

    //! Returns the flag specifing if the next point of the trajectory is to be
    //! provided on timer or once the previous is reached.
    bool providePointsOnTimer() const { return m_providePointsOnTimer; }
    //! Returns the update rate for the trajectory points when they are provided
    //! on a timeout.
    int trajectoryUpdateRateHz() const { return m_trajectoryUpdateRateHz; }

private:
    //! Constructor. Defining it here prevents construction.
    RobotControlSettings() {}
    //! Destructor. Defining it here prevents unwanted destruction.
    ~RobotControlSettings() {}

private:
    //! The number of robots.
    int m_numberOfRobots;
    //! The contol loop frequency.
    int m_controlFrequencyHz;
    //! The frequency divider for the navigation commands for fish motion pattern.
    int m_fishMotionPatternFrequencyDivider;
    //! Maps robot's id to individual robots settings.
    QMap<QString, RobotSettings> m_robotsSettings;
    //! Fish motion pattern settings; at the moment they are shared
    //! by all robots.
    FishMotionPatternSettings m_fishMotionPatternSettings;
    //! PID controller settings.
    PidControllerSettings m_pidControllerSettings;
    //! Default linear speed of the robot.
    int m_defaultLinearSpeedCmSec;
    //! If the robot needs to have a valid orientation to navigate.
    bool m_needOrientationToNavigate;
    //! The map of the setup, used in path planning and modelling.
    SetupMap m_setupMap;
    //! The path planning settings.
    PathPlanningSettings m_pathPlanningSettings;
    //! The potential field obstacle avoidance settings.
    PotentialFieldSettings m_potentialFieldSettings;
    //! The number of animals used in the experiment.
    int m_numberOfAnimals;
    //! The fish model parameters.
    FishModelSettings m_fishModelSettings;

    //! The settings for specific experiment controllers.
    QMap<ExperimentControllerType::Enum,
         ExperimentControllerSettingsPtr> m_controllerSettings;

    // TODO : to make a map of usefull settings for every control mode
    //! The predefined trajectory for the Trajectory control mode.
    QList<PositionMeters> m_trajectory;
    //! Defines if the trajectory should be restarted once the last point is
    //! reached.
    bool m_loopTrajectory;
    //! Specifies if the next point of the trajectory is to be provided on
    //! timer or once the previous is reached.
    bool m_providePointsOnTimer;
    //! The update rate for the trajectory points when they are provided on a
    //! timeout.
    int m_trajectoryUpdateRateHz;
};

#endif // CATS2_ROBOT_CONTROL_SETTINGS_HPP
