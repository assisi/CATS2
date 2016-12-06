#ifndef CATS2_ROBOT_CONTROL_SETTINGS_HPP
#define CATS2_ROBOT_CONTROL_SETTINGS_HPP

#include "SetupMap.hpp"

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
    FishModelSettings() {}

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

    //! Sets control map file path.
    void setControlAreasPath(QString controlAreasPath) { m_controlAreasPath = controlAreasPath; }
    //! Return control map file path.
    QString controlAreasFilePath() const { return m_controlAreasPath; }

private:
    //! Robot's id.
    QString m_id;
    //! Control map file path.
    QString m_controlAreasPath;
};

/*!
 * Stores settings of fish motion pattern.
 */
class FishMotionPatternSettings
{
public:
    //! Constructor.
    explicit FishMotionPatternSettings() { }

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
    explicit PidControllerSettings() { }

    //! Sets proportional coefficient.
    void setKp(double kp) { m_kp = kp; }
    //! Returns proportional coefficient.
    int kp() const { return m_kp; }

    //! Sets integral coefficient.
    void setKi(double ki) { m_ki = ki; }
    //! Returns integral coefficient.
    int ki() const { return m_ki; }

    //! Sets derivative coefficient.
    void setKd(double kd) { m_kd = kd; }
    //! Returns derivative coefficient.
    int kd() const { return m_kd; }

private:
    //! Proportional coefficient.
    double m_kp;
    //! Integral coefficient.
    double m_ki;
    //! Derivative coefficient.
    double m_kd;
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

    //! Gives the const reference to the experimental setup map.
    const SetupMap& setupMap() const { return m_setupMap; }

    //! Return the number of animals used in the experiment.
    int numberOfAnimals() const { return m_numberOfAnimals; }

    //! Gives the const reference to the fish model parameters.
    const FishModelSettings& fishModelSettings() const { return m_fishModelSettings; }

    //! Gives the reference to the path planning settngs.
    const PathPlanningSettings& pathPlanningSettings() const { return m_pathPlanningSettings; }

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
    //! The map of the setup, used in path planning and modelling.
    SetupMap m_setupMap;
    //! The path planning settings.
    PathPlanningSettings m_pathPlanningSettings;
    //! The number of animals used in the experiment.
    int m_numberOfAnimals;
    //! The fish model parameters.
    FishModelSettings m_fishModelSettings;
};

#endif // CATS2_ROBOT_CONTROL_SETTINGS_HPP
