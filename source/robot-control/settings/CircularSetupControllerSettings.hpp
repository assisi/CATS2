#ifndef CATS2_CIRCULAR_SETUP_CONTROLLER_SETTINGS_HPP
#define CATS2_CIRCULAR_SETUP_CONTROLLER_SETTINGS_HPP

#include <CoordinatesConversion.hpp>
#include <AgentState.hpp>
#include "ExperimentControllerSettings.hpp"

/*!
 * The actual data stored in the settings. It's separated in a class to be
 * easily trasferable to the corresponding controller.
 */
class CircularSetupControllerSettingsData
{
public:
    //! Constructor.
    explicit CircularSetupControllerSettingsData() :
        m_controlAreasFileName(),
        m_targetRadiusM(0),
        m_targetDeltaAngleRad(0)
    {}

public:
    //! Returns the path to the file describine the control areas.
    QString controlAreasFileName() const { return m_controlAreasFileName; }
    //! Set the path to the file describine the control areas.
    void setControlAreasFileName(QString controlAreasFileName)
    {
        m_controlAreasFileName = controlAreasFileName;
    }

    //! Returns the radius to set the target.
    double targetRadiusM() const { return m_targetRadiusM; }
    //! Set the radius to set the target.
    void setTargetRadiusM(double targetRadiusM)
    {
        m_targetRadiusM = targetRadiusM;
    }

    //! Returns the delta angle to set the target.
    double targetDeltaAngleRad() const { return m_targetDeltaAngleRad; }
    //! Set the delta angle to set the target.
    void setTargetDeltaAngleRad(double targetDeltaAngleRad)
    {
        m_targetDeltaAngleRad = targetDeltaAngleRad;
    }

    //! Returns the file name of trajectory configuration file.
    QString trajectoryFileName() const { return m_trajectoryFileName; }
    //! Set the trajectory file name.
    void setTrajectoryFileName(QString trajectoryFileName)
    {
        m_trajectoryFileName = trajectoryFileName;
    }

    //! Returns trajectory
    QList<PositionMeters> trajectory() const { return m_trajectory; }
    //! Set the trajectory
    void setTrajectory(QList<PositionMeters>& trajectory)
    {
        m_trajectory = trajectory;
    }

    //! Returns trajectory current index
    size_t trajectoryCurrentIndex() const { return m_trajectoryCurrentIndex; }
    //! Set the trajectory curent index
    void setTrajectoryCurrentIndex(size_t trajectoryCurrentIndex)
    {
        m_trajectoryCurrentIndex = trajectoryCurrentIndex;
    }


protected:
    //! The path to the file describine the control areas.
    QString m_controlAreasFileName;
    //! The radius of trajectory in meters to set the target position with
    //! respect to the robot.
    double m_targetRadiusM;
    //! The delta angle to set the target position with respect to the robot.
    double m_targetDeltaAngleRad;
    //! The path of the file containing the trajectory. If not provided, use m_targetRadiusM and m_targetDeltaAngleRad
    //  to define robot trajectories.
    QString m_trajectoryFileName;
    //! The predefined trajectory.
    QList<PositionMeters> m_trajectory;
    //! Current index in the trajectory.
    size_t m_trajectoryCurrentIndex;
};

class CircularSetupControllerSettings : public ExperimentControllerSettings
{
public:
    //! Constructor.
    explicit CircularSetupControllerSettings(ExperimentControllerType::Enum type);
    //! Destructor.
    virtual ~CircularSetupControllerSettings();

public:
    //! Initialization of the parameters for this specific controller.
    //! Returns true if the parameters are considered valid.
    virtual bool init(QString configurationFileName) override;

    //! Provides a copy of the settings data.
    CircularSetupControllerSettingsData data() { return m_data; }

private:
    //! The settings data.
    CircularSetupControllerSettingsData m_data;
};

#endif // CATS2_CIRCULAR_SETUP_CONTROLLER_SETTINGS_HPP
