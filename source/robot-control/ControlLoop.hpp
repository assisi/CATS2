#ifndef CATS2_CONTROL_LOOP_HPP
#define CATS2_CONTROL_LOOP_HPP

#include "RobotControlPointerTypes.hpp"

#include <AgentData.hpp>

#include <QtCore/QObject>
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
    void onTrackingResultsReceived(QList<AgentDataWorld> agentsData);
    //! The target position received from the viewer; it's set as a target
    //! to the selected robots.
    void goToPosition(PositionMeters position);
    //! Asks to send control maps for the currently selected robot.
    void sendNavigationData(bool sendMaps);
    //! Asks to send the current robot id.
    void requestSelectedRobot();
    //! Asks to send the colors of all robots.
    void requestRobotsLedColors();

signals:
    //! Sends the control map areas' polygons of the robot.
    void notifyRobotControlAreasPolygons(QString agentId, QList<AnnotatedPolygons> polygons);
    //! Sends the trajectory of the robot.
    void notifyRobotTrajectoryChanged(QString agentId, QQueue<PositionMeters> trajectory);
    //! Sends the target of the robot.
    void notifyRobotTargetPositionChanged(QString agentId, PositionMeters position);
    //! Sends the current robot.
    void notifySelectedRobotChanged(QString agentId);
    //! Sends the leds' color of the robot.
    void notifyRobotLedColor(QString agentId, QColor ledColor);

private:
    //! Loads and initialized the robots' firmware scripts.
    void initializeRobotsInterfaces();

private:
    //! An inferface with the robots' Aseba firmware. It's shared by all
    //! robots, like this they have a direct access to set parameters.
    Aseba::DBusInterfacePtr m_robotsInterface;
    //! A list of all connected robots.
    QList<FishBotPtr> m_robots;
    //! The robot selected in the GUI.
    FishBotPtr m_selectedRobot;

    //! The control loop timer.
    QTimer m_controlLoopTimer;

    //! The flag that defines if the navigation data of robots are to be submitted.
    bool m_sendData;
};

#endif // CATS2_CONTROL_LOOP_HPP
