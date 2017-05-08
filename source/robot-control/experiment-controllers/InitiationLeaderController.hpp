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
    inline bool changingRoom() const { return m_state != SWIMMING_WITH_FISH; }
    //! Checks the departure timer.
    bool timeToDepart();
    //! Checks that the fish follow.
    bool fishFollow();

private:
    //! The settings for this controller.
    InitiationLeaderControllerSettingsData m_settings;

    //! The current state of the robot.
    State m_state;

    //! The flag that defines that we need to limit the model area to the
    //! current room occupied by the robot. The goal is to prevent the robot
    //! from leaving the room when in the model-based control mode.
    bool m_limitModelArea;

    //! The departure timer.
    Timer m_departureTimer;
    //! The check-that-fish-follow timer. After departing from the original room
    //! the robot will start this timer to know when it needs to check that the
    //! fish follow it.
    Timer m_fishFollowCheckTimer;
    //! The flag to check that we have entered to the target room. 
    bool m_inTargetRoom;

    //! The room to bring fish.
    QString m_targetAreaId;
    //! The room from where the robot departs.
    QString m_departureAreaId;
};

#endif // CATS2_INITIATION_LEADER_CONTROLLER_HPP
