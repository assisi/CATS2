#ifndef CATS2_FISHBOT_LEDS_TRACKING_SETTINGS_HPP
#define CATS2_FISHBOT_LEDS_TRACKING_SETTINGS_HPP

#include "TrackingRoutineSettings.hpp"

#include <QtCore/QDebug>
#include <QtGui/QColor>

/*!
 * The actual data stored in the settings. It's separated in a class to be easily trasferable
 * to the corresponding tracking routine.
 */
class FishBotLedsTrackingSettingsData
{
public:
    //! Constructor.
    explicit FishBotLedsTrackingSettingsData()
    {
    }

public:
    //! Returns the number of agents to be tracked.
    size_t numberOfAgents() const { return m_robotsDescriptions.size(); }

    //! The structure that stores the tracking parameters for one robot.
    struct FishBotDescription
    {
        //! Initialization.
        FishBotDescription() : id('Z'), ledColor(0,0,0), colorThreshold(100) {}
        //! The FishBot's id.
        QString id;
        //! The robot LEDs' color.
        QColor ledColor;
        //! The color detection threshold.
        int colorThreshold;
    };

    //! Returns the robot's description.
    FishBotDescription robotDescription(int robotIndex) const
    {
        if ((robotIndex < m_robotsDescriptions.size()) && (robotIndex >=0))
            return m_robotsDescriptions[robotIndex];
        else {
            qDebug() << "Requesting the robot description out of bounds";
            return FishBotDescription();
        }
    }

    //! Sets the robot's description.
    void setRobotDescription(int robotIndex, FishBotDescription description)
    {
        if ((robotIndex < m_robotsDescriptions.size()) && (robotIndex >=0))
            m_robotsDescriptions[robotIndex] = description;
        else {
            qDebug() << "Requesting the robot description out of bounds";
        }
    }

    void addRobotDescription(FishBotDescription description)
    {
        m_robotsDescriptions.append(description);
    }

    std::string maskFilePath() const { return m_maskFilePath; }
    void setMaskFilePath(std::string maskFilePath) { m_maskFilePath = maskFilePath; }

protected:
    //! The parameters of all robots to track.
    QList<FishBotDescription> m_robotsDescriptions;
    //! The arena mask file, an optional parameter.
    std::string m_maskFilePath;
    // TODO : add the rest of parameters
};

/*!
 * The settings for the FishBots leds tracking routine.
 */
class FishBotLedsTrackingSettings : public TrackingRoutineSettings
{
public:
    //! Constructor.
    explicit FishBotLedsTrackingSettings(SetupType::Enum setupType);

public:
    //! Initialization of the parameters for this specific method of tracking.
    //! Returns true if the parameters are considered valid.
    virtual bool init(QString configurationFileName) override;

    //! Provides a copy of the settings data.
    FishBotLedsTrackingSettingsData data() { return m_data; }

private:
    //! The settings data.
    FishBotLedsTrackingSettingsData m_data;
};

#endif // CATS2_FISHBOT_LEDS_TRACKING_SETTINGS_HPP
