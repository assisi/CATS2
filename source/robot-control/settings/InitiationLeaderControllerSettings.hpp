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
    //! Constructor.
    InitiationLeaderControllerSettingsData() :
        m_defaultControlAreasFileName(),
        m_fishNumberAroundOnDeparture(1),
        m_groupRadius(0.1),
        m_fishFollowCheckTimeOutSec(5.),
        m_maximalFishNumberAllowedToStay(0)
    {}

public:
    //! Returns the path to the file describine the control areas.
    QString defaultControlAreasFileName() const { return m_defaultControlAreasFileName; }
    //! Set the path to the file describine the control areas.
    void setDefaultControlAreasFileName(QString controlAreasFileName)
    {
        m_defaultControlAreasFileName = controlAreasFileName;
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

protected:
    //! The path to the file describing the control areas. In addition for every
    //! robot we can set a specific control areas file.
    QString m_defaultControlAreasFileName;
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
