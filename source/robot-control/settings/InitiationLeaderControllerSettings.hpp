#ifndef CATS2_INITIATION_LEADER_CONTROLLER_SETTINGS_HPP
#define CATS2_INITIATION_LEADER_CONTROLLER_SETTINGS_HPP

#include "ExperimentControllerSettings.hpp"

/*!
 * The actual data stored in the settings. It's separated in a class to be
 * easily trasferable to the corresponding controller.
 */
class InitiationLeaderControllerSettingsData
{
public:
    //! The trigger event to leave the room.
    enum DepartureTrigger
    {
        ON_TIME_OUT,
        WHEN_IN_GROUP
    };

    //! Constructor.
    InitiationLeaderControllerSettingsData() :
        m_controlAreasFileName(),
        m_depatureTrigger(ON_TIME_OUT),
        m_departureTimeOutSec(5.),
        m_fishNumberAroundOnDeparture(1),
        m_groupRadius(0.1),
        m_fishFollowCheckTimeOutSec(5.),
        m_maximalFishNumberAllowedToStay(0)
    {}

public:
    //! Gets the type of the departure trigger from the settings' string.
    static DepartureTrigger depatureTriggerFromSettingsString(QString name)
    {
        if (name.toLower() == "timeout")
            return ON_TIME_OUT;
        else if (name.toLower() == "ingroup")
            return WHEN_IN_GROUP;
        else
            return ON_TIME_OUT;
    }

public:
    //! Returns the path to the file describine the control areas.
    QString controlAreasFileName() const { return m_controlAreasFileName; }
    //! Set the path to the file describine the control areas.
    void setControlAreasFileName(QString controlAreasFileName)
    {
        m_controlAreasFileName = controlAreasFileName;
    }

    //! Sets the departure trigger.
    void setDepartureTrigger(QString name)
    {
        m_depatureTrigger = depatureTriggerFromSettingsString(name);
    }

    //! Returns the number of fish that should be around the robot when it leaves.
    int fishNumberAroundOnDeparture() const
    {
        return m_fishNumberAroundOnDeparture;
    }
    //! Set the number of fish that should be around the robot when it leaves.
    void setFishNumberAroundOnDeparture(int fishNumberAroundOnDeparture)
    {
        m_fishNumberAroundOnDeparture = fishNumberAroundOnDeparture;
    }

    //! Returns the radius around the robot where we search for fish.
    double groupRadius() const { return m_groupRadius; }
    //! Set the radius around the robot where we search for fish.
    void setGroupRadius(double groupRadius)
    {
        m_groupRadius = groupRadius;
    }

    //! Returns the time to wait before trying the initiation procedure.
    double departureTimeOutSec() const { return m_departureTimeOutSec; }
    //! Set the time to wait before trying the initiation procedure.
    void setDepartureTimeOutSec(double departureTimeOutSec)
    {
        m_departureTimeOutSec = departureTimeOutSec;
    }

    //! Returns the time to wait before checking that the fish follow.
    double fishFollowCheckTimeOutSec() const { return m_fishFollowCheckTimeOutSec; }
    //! Set the time to wait before checking that the fihs follow.
    void setFishFollowCheckTimeOutSec(double fishFollowCheckTimeOutSec)
    {
        m_fishFollowCheckTimeOutSec = fishFollowCheckTimeOutSec;
    }

    //! Returns the maximal number of fish that is allowed to stay behind the robot.
    int maximalFishNumberAllowedToStay() const
    {
        return m_maximalFishNumberAllowedToStay;
    }
    //! Set the maximal number of fish that is allowed to stay behind the robot.
    void setMaximalFishNumberAllowedToStay(int maximalFishNumberAllowedToStay)
    {
        m_maximalFishNumberAllowedToStay = maximalFishNumberAllowedToStay;
    }

    //! Does the robot try initiation on the timeout?
    bool departureOnTimeOut() const { return (m_depatureTrigger == ON_TIME_OUT);}
    //! Does the robot try initiation when it's in a group?
    bool departureWhenInGroup() const { return (m_depatureTrigger == WHEN_IN_GROUP);}

protected:
    //! The path to the file describing the control areas.
    QString m_controlAreasFileName;
    //! The trigger event to leave the room.
    DepartureTrigger m_depatureTrigger;
    //! The time the robot waits before trying the initiation procedure.
    double m_departureTimeOutSec;
    //! The number of fish that should be around the robot when it leaves.
    int m_fishNumberAroundOnDeparture;
    //! The radius around the robot where we search for fish.
    double m_groupRadius;
    //! The time the robot waits before checking if the fish follow it.
    double m_fishFollowCheckTimeOutSec;
    //! The maximal number of fish that is allowed to stay behind the robot.
    int m_maximalFishNumberAllowedToStay;
};

class InitiationLeaderControllerSettings : public ExperimentControllerSettings
{
public:
    //! Constructor.
    InitiationLeaderControllerSettings();
    //! Destructor.
    virtual ~InitiationLeaderControllerSettings();

public:
    //! Initialization of the parameters for this specific controller.
    //! Returns true if the parameters are considered valid.
    virtual bool init(QString configurationFileName) override;

    //! Provides a copy of the settings data.
    InitiationLeaderControllerSettingsData data() { return m_data; }

private:
    //! The settings data.
    InitiationLeaderControllerSettingsData m_data;
};


#endif // CATS2_INITIATION_LEADER_CONTROLLER_SETTINGS_HPP
