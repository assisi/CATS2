#ifndef CATS2_CONTROL_LOOP_HPP
#define CATS2_CONTROL_LOOP_HPP

#include "RobotControlPointerTypes.hpp"
#include "dbusinterface.h"

#include <AgentData.hpp>

#include <QtCore/QObject>

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
};

#endif // CATS2_CONTROL_LOOP_HPP