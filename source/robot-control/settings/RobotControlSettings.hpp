#ifndef CATS2_ROBOT_CONTROL_SETTINGS_HPP
#define CATS2_ROBOT_CONTROL_SETTINGS_HPP

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

private:
    //! Robot's id.
    QString m_id;
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

    //! Returns the settings for given robot.
    RobotSettings robotSettings(QString id) const { return m_robotsSettings[id]; }
    //! Returns the list of all robot ids.
    QList<QString > ids() const { return m_robotsSettings.keys(); }

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
    //! Maps robot's id to individual robots settings.
    QMap<QString, RobotSettings> m_robotsSettings;
    // TODO : this class must keep the following data
    // (*) number of agents
    // (*) id of every agent
    // (*) LED colour of every agent
    // (*) parameters of control modes and of the navigation system
    // (*) the control map files

};

#endif // CATS2_ROBOT_CONTROL_SETTINGS_HPP
