#ifndef CATS2_INITIATION_LEADER_CONTROLLER_HPP
#define CATS2_INITIATION_LEADER_CONTROLLER_HPP

#include "ExperimentController.hpp"
#include "settings/InitiationLeaderControllerSettings.hpp"

#include <Timer.hpp>

#include <chrono>

/*!
 * The controller that implements the initiation procedure. The goal of the
 * robot is to either make the fish to change the room from time to time, or to
 * make the stay in one room defined in the settings.
 * Here is the outline of the robot's behavior:
 * (1) The robot follows the fish model (not restricted to specific room) and
 * does the fish-motion pattern.
 * (2) IF the robot arrives to a room, where most of the fish are not with him,
 * then it joins them (go-to-target).
 * (3) IF the robot arrives to the Other-Room AND the most of the fish is with
 * him AND there are fish close to the robot THEN the robot starts the
 * initiation, it goes to the center of the Target-Room, first for one second in
 * the fish-motion, and then in PID. Once it arrives to the Target-Room it
 * switches again to the fish-motion, but still makes circles around the center
 * of the room.
 * (4) When the robot starts the initiation (by going to the target room) a
 * timer is started, on its timeout the robot checks if the fish follow, IF they
 * don’t follow THEN the robot returns back to the Other-Room in PID, once on
 * the target it switches to (1).
 * (5) IF the robot is in the Target-Room AND the fish follow it THEN it waits
 * until there is at least one fish in the proximity, in this case it switches
 * to (1).
 */
class InitiationLeaderController : public ExperimentController
{
    Q_OBJECT
public:
    //! Constructor. Gets the robot and the settings.
    InitiationLeaderController(FishBot* robot,
                               ExperimentControllerSettingsPtr settings);

public:
    //! Called when the controller is activated. Used to reset parameters.
    virtual void start() override;
    //! Returns the control values for given position.
    virtual ControlData step() override;

private:
    //! The controller's state machine
    enum State {
        SWIMMING_WITH_FISH, // fish model based
        CHANGING_ROOM,      // try to bring the fish to another room
        GOING_BACK,          // return back to the original room if fail
        UNDEFINED
    };

    //! Converts the state to the string for the output.
    QString stateToString(State state);

private:
    //! Updates the current state.
    void updateState(State state);
    //! Checks if the conditions are met to start the inititaion procedure.
    bool needToChangeRoom();
    //! Runs the initiation state machine.
    ControlData changeRoom();
    //! Gets the control data that corresponds to the current state.
    ControlData stateControlData();
    //! Checks if the robot is already changing the room.
    bool changingRoom() const;
    //! Checks the departure timer.
    bool timeToDepart();
    //! Checks that the fish follow.
    bool fishFollow();
    //! Counts the number of fish around the robot.
    int fishAroundRobot();

private:
    //! The settings for this controller.
    InitiationLeaderControllerSettingsData m_settings;

    //! The current state of the robot.
    State m_state;

//    //! The flag that defines that we need to limit the model area to the
//    //! current room occupied by the robot. The goal is to prevent the robot
//    //! from leaving the room when in the model-based control mode.
//    bool m_limitModelArea;

    //! The departure timer.
    Timer m_departureTimer;
    //! The check-that-fish-follow timer. After departing from the original room
    //! the robot will start this timer to know when it needs to check that the
    //! fish follow it.
    Timer m_fishFollowCheckTimer;
    //! When the robot changes the room, it first goes in fish motion pattern
    //! and then one second later switches to PID
    Timer m_fishToPIDTimer;
    //! The flag to check that we have entered to the target room. 
    bool m_inTargetRoom;

    //! The room to bring fish.
    QString m_targetAreaId;
    //! The room from where the robot departs.
    QString m_departureAreaId;
};

#endif // CATS2_INITIATION_LEADER_CONTROLLER_HPP
