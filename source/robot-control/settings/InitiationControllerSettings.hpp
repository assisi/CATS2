#ifndef CATS2_INITIATION_CONTROLLER_SETTINGS_HPP
#define CATS2_INITIATION_CONTROLLER_SETTINGS_HPP

#include "ExperimentControllerSettings.hpp"

/*!
 * The actual data stored in the settings. It's separated in a class to be
 * easily trasferable to the corresponding controller.
 */
class InitiationControllerSettingsData
{
public:
    //! Constructor.
    InitiationControllerSettingsData() :
        m_controlAreasFileName(),
        m_departureTimeOutSec(5.)
    {}

public:
    //! Returns the path to the file describine the control areas.
    QString controlAreasFileName() const { return m_controlAreasFileName; }
    //! Set the path to the file describine the control areas.
    void setControlAreasFileName(QString controlAreasFileName)
    {
        m_controlAreasFileName = controlAreasFileName;
    }

    //! Returns the time to wait before trying the initiation procedure.
    double departureTimeOutSec() const { return m_departureTimeOutSec; }
    //! Set the time to wait before trying the initiation procedure.
    void setDepartureTimeOutSec(double departureTimeOutSec)
    {
        m_departureTimeOutSec = departureTimeOutSec;
    }

    //! Returns the time to wait before trying the initiation procedure.
    double fishFollowCheckTimeOutSec() const { return m_fishFollowCheckTimeOutSec; }
    //! Set the time to wait before trying the initiation procedure.
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
    //! The path to the file describine the control areas.
    QString m_controlAreasFileName;
    //! The time the robot waits before trying the initiation procedure.
    double m_departureTimeOutSec;
    //! The time the robot waits before checking if the fish follow it.
    double m_fishFollowCheckTimeOutSec;
    //! The maximal number of fish that is allowed to stay behind the robot.
    int m_maximalFishNumberAllowedToStay;
};

class InitiationControllerSettings : public ExperimentControllerSettings
{
public:
    //! Constructor.
    InitiationControllerSettings();
    //! Destructor.
    virtual ~InitiationControllerSettings();

public:
    //! Initialization of the parameters for this specific controller.
    //! Returns true if the parameters are considered valid.
    virtual bool init(QString configurationFileName) override;

    //! Provides a copy of the settings data.
    InitiationControllerSettingsData data() { return m_data; }

private:
    //! The settings data.
    InitiationControllerSettingsData m_data;
};


#endif // CATS2_INITIATION_CONTROLLER_SETTINGS_HPP
