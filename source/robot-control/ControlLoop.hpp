#ifndef CATS2_CONTROL_LOOP_HPP
#define CATS2_CONTROL_LOOP_HPP

#include "RobotControlPointerTypes.hpp"
#include "experiment-controllers/ExperimentControllerType.hpp"

#include <AgentData.hpp>

#include <QtCore/QObject>
#include <QtCore/QMap>
#include <QtCore/QTimer>

/*!
 * The main control class. Manages the interfaces to the robots, robot classes,
 * and runs their control step.
 */
class ControlLoop : public QObject
{
    Q_OBJECT
public:
    //! Constructor.
    explicit ControlLoop();
    //! Destructor.
    virtual ~ControlLoop() final;

    //! The main step of the control system.
    void step();

    //! Return the list of all robots. Used to initialize the gui.
    QList<FishBotPtr> robots() { return m_robots; }

public slots:
    //! Set the selected robot from the name.
    void selectRobot(QString name);
    //! Receives the resutls from the tracking system and transfers it
    //! further to the robots.
    void onTrackingResultsReceived(QList<AgentDataWorld> agentsData,
                                   std::chrono::milliseconds timestamp);
    //! The target position received from the viewer; it's set as a target
    //! to the selected robots.
    void goToPosition(PositionMeters position, bool fromNetwork = false);
    //! Asks the robots to send their navigation data (trajectories, targets, etc).
    void sendNavigationData(bool sendData);
    //! Asks to send the control areas for the selected robot.
    void sendControlAreas(bool sendAreas);
    //! Asks to send the selected robot id.
    void requestSelectedRobot();
    //! Asks to send the colors of all robots.
    void requestRobotsLedColors();
    //! Reconnect the robot's to the aseba interface.
    void reconnectRobots();
    //! Stops all the robots.
    void stopAllRobots();
    //! Sets the experimental controller for all robots.
    void setController(ExperimentControllerType::Enum controllerType);

public slots: // interspecies
    //! Passes further to the robot the message from the bee setup (CW/CCW).
    void setCircularSetupTurningDirection(QString message);
    //! Change robot mode in 2Rooms Inter-species experiments
    void changeInterspecies2RoomsMode(int robotId, QString targetMode);

signals:
    //! Sends the control map areas' polygons of the robot.
    void notifyRobotControlAreasPolygons(QString agentId,
                                         QList<AnnotatedPolygons> polygons);
    //!  Sends the current robot's control areas' occupation by fish information.
    void notifyFishNumberByRobotControlAreas(QString agentId,
                                             QMap<QString, int> fishNumberByArea);
    //! Sends the trajectory of the robot.
    void notifyRobotTrajectoryChanged(QString agentId, QQueue<PositionMeters> trajectory);
    //! Sends the target of the robot.
    void notifyRobotTargetPositionChanged(QString agentId, PositionMeters position);
    //! Sends the current robot.
    void notifySelectedRobotChanged(QString agentId);
    //! Sends the leds' color of the robot.
    void notifyRobotLedColor(QString agentId, QColor ledColor);
    //! Notifies that the obstacle avoidance status has changed.
    void notifyObstacleDetectedStatusChanged(QString agentId, bool obstacleDetected);
    //! Sends the data from the circular experiment.
    void notifyCircularSetupTurningDirections(QString agentId,
                                              QString fishTurningDirection,
                                              QString robotTurningDirection);
    //! Sends the statistics from the circular experiment.
    void notifyCircularSetupStatistics(QString agentId,
                                       double fishClockWisePercent,
                                       double fishCounterClockWisePercent,
                                       QString fishTurningDirection,
                                       double robotClockWisePercent,
                                       double robotCounterClockWisePercent,
                                       QString robotTurningDirection);

private:
    //! Loads and initializes the robots' firmware scripts for the shared
    //! interface.
    void reinitializeSharedRobotInterface();
    //! Asks robots to setup unique connections with the hardware, to load and
    //! initialize the firmware scripts.
    void reinitializeUniqueRobotInterface();

private: // statistics related code
    //! Registers the statistics data available at the control loop level at the
    //! statistics module.
    void registerStatistics() const;
    //! Updates the statistics.
    void updateStatistics(const QList<AgentDataWorld>& agentsData,
                          std::chrono::milliseconds timestamp) const;
    //! Computes the statistics id for the position x.
    QString agentStatisticsPoxitionXId(AgentType type, QString agentId) const;
    //! Computes the statistics id for the position y.
    QString agentStatisticsPoxitionYId(AgentType type, QString agentId) const;
    //! Computes the statistics id for the orientation.
    QString agentStatisticsDirectionId(AgentType type, QString agentId) const;
    //! Computes the statistics id.
    QString agentStatisticsId(AgentType type, QString agentId, QString postfix) const;
    //! Gives the timestamp's statistics id.
    QString agentStatisticsTimestampId() const;

private:
    //! An inferface with the robots' Aseba firmware. It's shared by all
    //! robots, like this they have a direct access to set parameters.
    DBusInterfacePtr m_sharedRobotInterface;
    //! A list of all connected robots.
    QList<FishBotPtr> m_robots;
    //! The robot selected in the GUI.
    FishBotPtr m_selectedRobot;

    //! The control loop timer.
    QTimer m_controlLoopTimer;

    //! The flag that defines if the navigation data of robots are to be submitted.
    bool m_sendNavigationData;
    //! The flag that defines if the control areas for the current robot are to be submitted.
    bool m_sendControlAreas;
};

#endif // CATS2_CONTROL_LOOP_HPP
