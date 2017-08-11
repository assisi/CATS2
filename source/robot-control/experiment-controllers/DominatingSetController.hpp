#ifndef CATS2_DOMINATING_SET_CONTROLLER_HPP
#define CATS2_DOMINATING_SET_CONTROLLER_HPP

#include "ExperimentController.hpp"
#include "settings/DominatingSetControllerSettings.hpp"

#include <Timer.hpp>

/*!
 * The controller that implements the dominating set experiment. The goal of the
 * experiment is to ...
 * This controller makes the robot to follow the fish group.
 */
class DominatingSetController : public ExperimentController
{
    Q_OBJECT
public:
    //! Constructor. Gets the robot and the settings.
    DominatingSetController(FishBot* robot,
                            ExperimentControllerSettingsPtr settings);

public:
    //! Called when the controller is activated. Used to reset parameters.
    virtual void start() override;
    //! Returns the control values for given position.
    virtual ControlData step() override;
    //! Called when the controller is disactivated.
    virtual void finish() override;

signals:
    //! Informs the interspecies-module on the fish and robots room occupation.
    void notifyRoomsOccupation(QString areaId,
                               QString fishRoomId,
                               QString robotRoomId);

private:
    //! Computes the occupation of the rooms (left/right, up/down) by fish and
    //! robots. Emits a signal notifing about these statistics values.
    void updateRoomsStatistics();
    //! Converts the room id to the string (0 -> lelf, 1 -> right).
    QString roomIdToString(int id) const;

private:
    //! The settings for this controller.
    DominatingSetControllerSettingsData m_settings;

    //! The flag that says that the robot's operation area was defined
    bool m_robotAreaDefined;
    //! The robot's control area.
    ControlAreaPtr m_robotArea;
    //! The fish turning angle update timer.
    Timer m_roomsStatisticsUpdateTimer;
    //! The constant that define how often we update the rooms occupation in the
    //! robot's area.
    static constexpr double RoomsStatisticsUpdateTimeout = 1.0;
    //! Rooms occupations statistics (0 ~ lelf/up, 1 ~ right/down) by the fish
    //! group, defines how many times the group (majority of fish) was detected
    //! in the left/up or right/down room of the section (area); updated every
    //! RoomsOccupationUpdateTimeout.
    int m_fishRoomsOccupationCounter[2];
    //! The id of the last room with the majority of fish.
    int m_fishRoomId;
    //! Rooms occupations statistics (0 ~ lelf/up, 1 ~ right/down) by the robot,
    //! updated every RoomsOccupationUpdateTimeout.
    int m_robotRoomsOccupationCounter[2];
    //! The id of the last room with the robot.
    int m_robotRoomId;
};

#endif // CATS2_DOMINATING_SET_CONTROLLER_HPP
