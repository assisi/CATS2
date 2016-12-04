#ifndef CATS2_ROBOT_CONTROL_SETTINGS_HPP
#define CATS2_ROBOT_CONTROL_SETTINGS_HPP

#include "SetupMap.hpp"

#include <QtCore/QString>
#include <QtCore/QMap>

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
    void setControlMapPath(QString controlMapPath) { m_controlMapPath = controlMapPath; }
    //! Return control map file path.
    QString controlMapFilePath() const { return m_controlMapPath; }

private:
    //! Robot's id.
    QString m_id;
    //! Control map file path.
    QString m_controlMapPath;
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
};

#endif // CATS2_ROBOT_CONTROL_SETTINGS_HPP
